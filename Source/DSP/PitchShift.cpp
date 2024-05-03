/*
  ==============================================================================

    PitchShift.cpp
    Created: 20 Apr 2024 1:48:56pm
    Author:  Iggy-casa

  ==============================================================================
*/

#include "PitchShift.h"

// Copiado de https://github.com/juandagilc/Audio-Effects/blob/master/Pitch%20Shift/Source/PluginProcessor.cpp

PitchShift::PitchShift() {}

PitchShift::~PitchShift() {}

void PitchShift::prepare(juce::dsp::ProcessSpec& spec)
{
    const double smoothTime = 1e-13;
    paramShift.reset(spec.sampleRate, smoothTime);
    /* paramFftSize.reset(sampleRate, smoothTime);
     paramHopSize.reset(sampleRate, smoothTime);
     paramWindowType.reset(sampleRate, smoothTime);*/

     //======================================

    updateFftSize(spec.numChannels);
    updateHopSize();
    updateAnalysisWindow();
    updateWindowScaleFactor();

    needToResetPhases = true;
}

void PitchShift::onSmoothMeltChange(float inSmoothMelt, double sampleRate)
{
    const double smoothTime = static_cast<double>(inSmoothMelt);
    paramShift.reset(sampleRate, smoothTime);
}

void PitchShift::process(juce::AudioBuffer<float>& buffer, float inSemitones)
{
    paramShift.setTargetValue(getScaleSemitone(inSemitones));

    int currentInputBufferWritePosition = 0;
    int currentOutputBufferWritePosition = 0;
    int currentOutputBufferReadPosition = 0;
    int currentSamplesSinceLastFFT = 0;

    /*const juce::ScopedLock sl(lock);

    juce::ScopedNoDenormals noDenormals;

    const int numInputChannels = buffer.getNumChannels(); //getTotalNumInputChannels();
    const int numOutputChannels = numOutputChannelsIn; // buffer.getNumChannels(); // getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    //======================================

    int currentInputBufferWritePosition;
    int currentOutputBufferWritePosition;
    int currentOutputBufferReadPosition;
    int currentSamplesSinceLastFFT;*/

    float shift = paramShift.getNextValue();
    float ratio = roundf(shift * (float)hopSize) / (float)hopSize;
    int resampledLength = floorf((float)fftSize / ratio);
    juce::HeapBlock<float> resampledOutput(resampledLength, true);
    juce::HeapBlock<float> synthesisWindow(resampledLength, true);
    updateWindow(synthesisWindow, resampledLength);

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        float* channelData = buffer.getWritePointer(channel);

        currentInputBufferWritePosition = inputBufferWritePosition;
        currentOutputBufferWritePosition = outputBufferWritePosition;
        currentOutputBufferReadPosition = outputBufferReadPosition;
        currentSamplesSinceLastFFT = samplesSinceLastFFT;

        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            //======================================
            const float in = channelData[sample];
            channelData[sample] = outputBuffer.getSample(channel, currentOutputBufferReadPosition);

            //======================================

            outputBuffer.setSample(channel, currentOutputBufferReadPosition, 0.0f);
            if (++currentOutputBufferReadPosition >= outputBufferLength)
                currentOutputBufferReadPosition = 0;

            //======================================

            inputBuffer.setSample(channel, currentInputBufferWritePosition, in);
            if (++currentInputBufferWritePosition >= inputBufferLength)
                currentInputBufferWritePosition = 0;

            //======================================

            if (++currentSamplesSinceLastFFT >= hopSize)
            {
                currentSamplesSinceLastFFT = 0;

                //======================================

                int inputBufferIndex = currentInputBufferWritePosition;
                for (int index = 0; index < fftSize; ++index)
                {
                    fftTimeDomain[index].real(sqrtf(fftWindow[index]) * inputBuffer.getSample(channel, inputBufferIndex));
                    fftTimeDomain[index].imag(0.0f);

                    if (++inputBufferIndex >= inputBufferLength)
                        inputBufferIndex = 0;
                }

                //======================================

                fft->perform(fftTimeDomain, fftFrequencyDomain, false);

                if (paramShift.isSmoothing())
                    needToResetPhases = true;

                if (shift == paramShift.getTargetValue() && needToResetPhases)
                {
                    inputPhase.clear();
                    outputPhase.clear();
                    needToResetPhases = false;
                }

                for (int index = 0; index < fftSize; ++index)
                {
                    float magnitude = abs(fftFrequencyDomain[index]);
                    float phase = arg(fftFrequencyDomain[index]);

                    float phaseDeviation = phase - inputPhase.getSample(channel, index) - omega[index] * (float)hopSize;
                    float deltaPhi = omega[index] * hopSize + princArg(phaseDeviation);
                    float newPhase = princArg(outputPhase.getSample(channel, index) + deltaPhi * ratio);

                    inputPhase.setSample(channel, index, phase);
                    outputPhase.setSample(channel, index, newPhase);
                    fftFrequencyDomain[index] = std::polar(magnitude, newPhase);
                }

                fft->perform(fftFrequencyDomain, fftTimeDomain, true);

                for (int index = 0; index < resampledLength; ++index)
                {
                    float x = (float)index * (float)fftSize / (float)resampledLength;
                    int ix = (int)floorf(x);
                    float dx = x - (float)ix;

                    float sample1 = fftTimeDomain[ix].real();
                    float sample2 = fftTimeDomain[(ix + 1) % fftSize].real();
                    resampledOutput[index] = sample1 + dx * (sample2 - sample1);
                    resampledOutput[index] *= sqrtf(synthesisWindow[index]);
                }

                //======================================
                int outputBufferIndex = currentOutputBufferWritePosition;
                for (int index = 0; index < resampledLength; ++index)
                {
                    float out = outputBuffer.getSample(channel, outputBufferIndex);
                    out += resampledOutput[index] * windowScaleFactor;
                    outputBuffer.setSample(channel, outputBufferIndex, out);

                    if (++outputBufferIndex >= outputBufferLength)
                        outputBufferIndex = 0;
                }

                //======================================

                currentOutputBufferWritePosition += hopSize;
                if (currentOutputBufferWritePosition >= outputBufferLength)
                    currentOutputBufferWritePosition = 0;
            }

            //======================================
        }
    }

    inputBufferWritePosition = currentInputBufferWritePosition;
    outputBufferWritePosition = currentOutputBufferWritePosition;
    outputBufferReadPosition = currentOutputBufferReadPosition;
    samplesSinceLastFFT = currentSamplesSinceLastFFT;

    //======================================

    //for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
    //    buffer.clear(channel, 0, numSamples);
}

//void PitchShift::onUpdateFftSizeParamChoice(int indexChoice)
//{
//    // Si no entiendo mal,  esto es -> " 2 elevado a la potencia del valor del parámetro más 5" Tiene sentido viendo el enum de selecciones, dado que el index 0 seria fftSize32 (2^5 = 32)
//    auto fftSizeValue = (float)(1 << ((int)indexChoice + 5));
//    // Aca llamaria a updateFftSize pasandole el fftSizeValue, pero harcodeamos 512 por simplicidad...
//}
//
//void PitchShift::onUpdateHopSizeParamChoice(int indexChoice)
//{
//    //por ahora no vamos a hacer nada, vamos a harcodear
//    auto hopSizeValue = (float)(1 << ((int)indexChoice + 1));
//}
//
//void PitchShift::onUpdateWindowTypeParamChoice(int indexChoice)
//{
//    //por ahora no vamos a hacer nada, vamos a harcodear
//}

//Este es de jesus -> para escala atemperada
float PitchShift::getScaleSemitone(float inValue)
{
    return powf(2.0f, inValue / 12.0f);
}

void PitchShift::updateFftSize(int inNumChannels)
{
    fftSize = 512; //Harcodeado en vez de param
    fft = std::make_unique<juce::dsp::FFT>(static_cast<int>(log2(fftSize)));

    inputBufferLength = fftSize;
    inputBufferWritePosition = 0;
    inputBuffer.clear();
    inputBuffer.setSize(inNumChannels, inputBufferLength);

    float maxRatio = getScaleSemitone(-12.0f);
    outputBufferLength = (int)floorf((float)fftSize / maxRatio);
    outputBufferWritePosition = 0;
    outputBufferReadPosition = 0;
    outputBuffer.clear();
    outputBuffer.setSize(inNumChannels, outputBufferLength);

    fftWindow.realloc(fftSize);
    fftWindow.clear(fftSize);

    fftTimeDomain.realloc(fftSize);
    fftTimeDomain.clear(fftSize);

    fftFrequencyDomain.realloc(fftSize);
    fftFrequencyDomain.clear(fftSize);

    samplesSinceLastFFT = 0;

    omega.realloc(fftSize);
    for (int index = 0; index < fftSize; ++index)
        omega[index] = 2.0f * static_cast<float>(juce::MathConstants<float>::pi) * index / (float)fftSize;

    inputPhase.clear();
    inputPhase.setSize(inNumChannels, outputBufferLength);

    outputPhase.clear();
    outputPhase.setSize(inNumChannels, outputBufferLength);
}

void PitchShift::updateHopSize()
{
    //overlap = (int)paramHopSize.getTargetValue();  ---> harcodeamos 
    overlap = hopSize8;

    if (overlap != 0)
    {
        hopSize = fftSize / overlap;
        outputBufferWritePosition = hopSize % outputBufferLength;
    }
}

void PitchShift::updateAnalysisWindow()
{
    updateWindow(fftWindow, fftSize);
}

void PitchShift::updateWindow(const juce::HeapBlock<float>& window, const int windowLength)
{
    //Voy a comentar y suponer que siempre uisamos windowTypeHann

    //switch ((int)paramWindowType.getTargetValue()) {
    //    case windowTypeBartlett: {
    //        for (int sample = 0; sample < windowLength; ++sample)
    //            window[sample] = 1.0f - fabs(2.0f * (float)sample / (float)(windowLength - 1) - 1.0f);
    //        break;
    //    }
    //    case windowTypeHann: {
    for (int sample = 0; sample < windowLength; ++sample)
        window[sample] = 0.5f - 0.5f * cosf(2.0f * juce::MathConstants<float>::pi * (float)sample / (float)(windowLength - 1));
    //    break;
    //}
    //case windowTypeHamming: {
    //    for (int sample = 0; sample < windowLength; ++sample)
    //        window[sample] = 0.54f - 0.46f * cosf(2.0f * juce::MathConstants<float>::pi * (float)sample / (float)(windowLength - 1));
    //    break;
    //}
//}
}
void PitchShift::updateWindowScaleFactor()
{
    float windowSum = 0.0f;
    for (int sample = 0; sample < fftSize; ++sample)
        windowSum += fftWindow[sample];

    windowScaleFactor = 0.0f;
    if (overlap != 0 && windowSum != 0.0f)
        windowScaleFactor = 1.0f / (float)overlap / windowSum * (float)fftSize;
}

float PitchShift::princArg(const float phase)
{
    if (phase >= 0.0f)
        return fmod(phase + juce::MathConstants<float>::pi, 2.0f * juce::MathConstants<float>::pi) - juce::MathConstants<float>::pi;
    else
        return fmod(phase + juce::MathConstants<float>::pi, -2.0f * juce::MathConstants<float>::pi) + juce::MathConstants<float>::pi;
}
