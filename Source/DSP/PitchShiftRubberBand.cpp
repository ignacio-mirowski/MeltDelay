/*
  ==============================================================================

    PitchShiftRubberBand.cpp
    Created: 29 May 2024 10:39:37pm
    Author:  Iggy-casa

  ==============================================================================
*/

#include "PitchShiftRubberBand.h"

PitchShiftRubberBand::PitchShiftRubberBand() {}

PitchShiftRubberBand::~PitchShiftRubberBand() {}

//void PitchShiftRubberBand::setSemitones(float inSemitones)
//{
//    semitones = inSemitones;
//}
//void PitchShiftRubberBand::setOctaves(float inOctaves)
//{
//    octaves = inOctaves;
//}
//void PitchShiftRubberBand::setCents(float inCents)
//{
//    cents = inCents;
//}
//void PitchShiftRubberBand::setDryWet(float inDryWet)
//{
//    wet_dry = inDryWet;
//}
//void PitchShiftRubberBand::setCrisp(float inCrisp)
//{
//    crispness = inCrisp;
//}
//void PitchShiftRubberBand::setFormat(bool inFormat)
//{
//    format = inFormat;
//}


void PitchShiftRubberBand::prepare(juce::dsp::ProcessSpec spec)
{
    pitchShifter = std::make_unique<RubberBandPitchShifter>(
        spec.sampleRate, spec.numChannels);

    const double smoothTime = 1e-13;
    semitonesParamShift.reset(spec.sampleRate, smoothTime);


    //pitchShifter2 = std::make_unique<RubberBand::RubberBandStretcher>(spec.sampleRate, spec.numChannels, 
    //    RubberBand::RubberBandStretcher::OptionProcessRealTime | RubberBand::RubberBandStretcher::OptionPitchHighConsistency | RubberBand::RubberBandStretcher::OptionFormantPreserved
    //    ,1.0, 0.5);
    //mTempBuffer.setSize(spec.numChannels, spec.maximumBlockSize);

    //pitchShifter2->reset();
   // pitchShifter2->DefaultOptions
    //pitchShifter2->setPitchOption(RubberBand::RubberBandStretcher::Option::OptionPitchHighConsistency);
    //pitchShifter2->setFormantOption(RubberBand::RubberBandStretcher::Option::OptionFormantPreserved);

}

void PitchShiftRubberBand::onSmoothMeltChange(float inSmoothMelt, double sampleRate)
{
    const double newSmoothTime = static_cast<double>(inSmoothMelt);
    semitonesParamShift.reset(sampleRate, newSmoothTime);
}


void PitchShiftRubberBand::process(juce::AudioBuffer<float>& buffer, float inSemitones)
{
    float semitones_value = 0.0f;
    float octaves_value = 0.0f;

    if (inSemitones < -12.0f) {
        octaves_value = (static_cast<int>(std::floor(inSemitones / -12.0f)) * -1);
        float remainder = std::fmod(inSemitones, -12.0f);
        semitones_value = remainder ;
    }
    else {
        semitones_value = inSemitones;
    }

    semitonesParamShift.setTargetValue(semitones_value);
    float shift = semitonesParamShift.getNextValue();


    //float semitones_value = inSemitones;
    //float cents_value = 0.0f;
    //float wet_dry_value = 0.0f;
    //float crispness_value = 0.0f;
    //float format_value = 0.0f;
    pitchShifter->m_octaves = &octaves_value;
    pitchShifter->m_semitones = &shift;
    //pitchShifter->m_cents = &cents_value;
    //pitchShifter->m_wetDry = &wet_dry_value;
    //pitchShifter->m_crispness = &crispness_value;
    //pitchShifter->m_formant = &format_value;

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        pitchShifter->m_input[channel] = buffer.getWritePointer(channel);
        pitchShifter->m_output[channel] = buffer.getWritePointer(channel);
    }

    pitchShifter->runImpl(buffer.getNumSamples());
}


void PitchShiftRubberBand::process2(juce::AudioBuffer<float>& buffer)
{
    //WAY LAPSDA
    float a = -0.5f;
    pitchShifter->m_semitones = &a;

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        pitchShifter->m_input[channel] = buffer.getWritePointer(channel);
        pitchShifter->m_output[channel] = buffer.getWritePointer(channel);
    }

    pitchShifter->runImpl(buffer.getNumSamples());

    // WAY JESUS

    //mTempBuffer.makeCopyOf(buffer);

    //pitchShifter2->process(mTempBuffer.getArrayOfReadPointers(), mTempBuffer.getNumSamples(), false);

    //pitchShifter2->retrieve(buffer.getArrayOfWritePointers(), buffer.getNumSamples());
    //
   

    // WAY THEAUDIOPROGRAMMER

    //mTempBuffer.makeCopyOf(buffer);

    //auto outputBufferSamples = buffer.getNumSamples();
    //auto writePointers = buffer.getArrayOfWritePointers();
    //auto available = pitchShifter2->available();

    //while (available < outputBufferSamples)
    //{
    //    auto readPointers = mTempBuffer.getArrayOfReadPointers();
    //    pitchShifter2->process(mTempBuffer.getArrayOfReadPointers(), mTempBuffer.getNumSamples(), false);
    //    available = pitchShifter2->available();
    //    //mTempBuffer.clear();
    //}

    //pitchShifter2->retrieve(writePointers, outputBufferSamples);
    //mTempBuffer.clear();

    // WAY 1
    
    
    //for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    //{
    //    float* channelData = buffer.getWritePointer(channel);

    //    // Procesar los datos de audio con RubberBand
    //    pitchShifter2->process(&channelData, buffer.getNumSamples(), false);

    //    // Recuperar los datos procesados
    //    pitchShifter2->retrieve(&channelData, buffer.getNumSamples());
    //}


    /////////////////

    // WAY 2
    
    //const float* inputData[2];
    //float* outputData[2];

    //for (int channel = 0; channel < buffer.getNumChannels() ; ++channel)
    //{
    //    inputData[channel] = buffer.getReadPointer(channel);
    //    outputData[channel] = buffer.getWritePointer(channel);
    //}

    //int numSamples = buffer.getNumSamples();

    //// Process the audio data
    //pitchShifter->process(inputData, numSamples, false);

    //// Retrieve the processed data
    //int available = pitchShifter->available();

    //if (available >= numSamples)
    //{
    //    pitchShifter->retrieve(outputData, numSamples);
    //}
    //else
    //{
    //    // Handle the case where there is not enough data processed yet
    //    // For simplicity, we can clear the output buffer or handle it as needed
    //    buffer.clear();
    //}
}