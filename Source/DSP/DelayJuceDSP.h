/*
  ==============================================================================

    Delay.h
    Created: 7 Apr 2024 2:32:59pm
    Author:  Iggy-casa

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DelayJuceDSP
{
public:
    DelayJuceDSP();
    ~DelayJuceDSP();

    void setDelayTime(float inTimeParam);
    void setDelayFeedback(float inFeedbackParam);
    void process(juce::AudioBuffer<float>& buffer);
    void prepare(juce::dsp::ProcessSpec spec);

private:
    float timeValue{ 0.0f };
    float feedbackValue{ 0.9f };
    float sampleRate{ 0.0f };

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine{ 192000 };
};