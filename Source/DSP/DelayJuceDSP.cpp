/*
  ==============================================================================

    Delay.cpp
    Created: 7 Apr 2024 2:32:59pm
    Author:  Iggy-casa

  ==============================================================================
*/

#include "DelayJuceDSP.h"

DelayJuceDSP::DelayJuceDSP() {}
DelayJuceDSP::~DelayJuceDSP() {}

void DelayJuceDSP::setDelayTime(float inTimeParam)
{
    timeValue = inTimeParam;
}

void DelayJuceDSP::setDelayFeedback(float inFeedbackParam)
{
    feedbackValue = inFeedbackParam;
}

void DelayJuceDSP::prepare(juce::dsp::ProcessSpec spec)
{
    delayLine.reset();
    delayLine.prepare(spec);
    delayLine.setDelay(24000);

    sampleRate = static_cast<float>(spec.sampleRate);
}

void DelayJuceDSP::process(juce::AudioBuffer<float>& buffer)
{
    int delayTimeInSamples = timeValue * sampleRate; // El tiempo lo medimos en samples, 41k es un segundo
    delayLine.setDelay(delayTimeInSamples);

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
       // auto* inSamples = buffer.getReadPointer(channel);
       // auto* outSamples = buffer.getWritePointer(channel);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float delayedSample = delayLine.popSample(channel);
            float currentSample = buffer.getSample(channel, i);
            //float inDelay = inSamples[i] + (feedbackValue * delayedSample);
            float inDelay = currentSample + (feedbackValue * delayedSample);
            delayLine.pushSample(channel, inDelay);
            //outSamples[i] = inSamples[i] + delayedSample;
            buffer.setSample(channel, i, currentSample + delayedSample);
        }
    }
}