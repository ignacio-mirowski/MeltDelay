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

void PitchShiftRubberBand::setSemitones(float inSemitones)
{
    semitones = inSemitones;
}
void PitchShiftRubberBand::setOctaves(float inOctaves)
{
    octaves = inOctaves;
}
void PitchShiftRubberBand::setCents(float inCents)
{
    cents = inCents;
}
void PitchShiftRubberBand::setDryWet(float inDryWet)
{
    wet_dry = inDryWet;
}
void PitchShiftRubberBand::setCrisp(float inCrisp)
{
    crispness = inCrisp;
}
void PitchShiftRubberBand::setFormat(bool inFormat)
{
    format = inFormat;
}


void PitchShiftRubberBand::prepare(double theSampleRate, juce::dsp::ProcessSpec spec)
{
    pitchShifter = std::make_unique<RubberBandPitchShifter>(
        spec.sampleRate, 2);
 /*   pitchShifter = std::make_unique<RubberBand::RubberBandStretcher>(spec.sampleRate, spec.numChannels, RubberBand::RubberBandStretcher::DefaultOptions
    ,(1.0), 0.5);*/

}

void PitchShiftRubberBand::process(juce::AudioBuffer<float>& buffer)
{
    //juce::ScopedNoDenormals noDenormals;
    //auto totalNumInputChannels = 2;
    //auto totalNumOutputChannels = 2;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    //    buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    float octaves_value = 0.0f;
    float semitones_value = -12.0f;
    float cents_value = 0.0f;
    float wet_dry_value = 0.0f;
    float crispness_value = 0.0f;
    float format_value = 0.0f;
    //pitchShifter->m_octaves = &octaves_value;
    pitchShifter->m_semitones = &semitones_value;
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


//void PitchShiftRubberBand::process(juce::AudioBuffer<float>& buffer)
//{
//    //for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
//    //{
//    //    float* channelData = buffer.getWritePointer(channel);
//
//    //    // Procesar los datos de audio con RubberBand
//    //    pitchShifter->process(&channelData, buffer.getNumSamples(), false);
//
//    //    // Recuperar los datos procesados
//    //    pitchShifter->retrieve(&channelData, buffer.getNumSamples());
//    //}
//
//    const float* inputData[2];
//    float* outputData[2];
//
//    for (int channel = 0; channel < buffer.getNumChannels() ; ++channel)
//    {
//        inputData[channel] = buffer.getReadPointer(channel);
//        outputData[channel] = buffer.getWritePointer(channel);
//    }
//
//    int numSamples = buffer.getNumSamples();
//
//    // Process the audio data
//    pitchShifter->process(inputData, numSamples, false);
//
//    // Retrieve the processed data
//    int available = pitchShifter->available();
//
//    if (available >= numSamples)
//    {
//        pitchShifter->retrieve(outputData, numSamples);
//    }
//    else
//    {
//        // Handle the case where there is not enough data processed yet
//        // For simplicity, we can clear the output buffer or handle it as needed
//        buffer.clear();
//    }
//}