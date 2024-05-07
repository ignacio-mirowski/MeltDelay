/*
  ==============================================================================

    DryWet.h
    Created: 16 Mar 2024 1:11:22pm
    Author:  Iggy-casa

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DryWet {
public:
    DryWet();
    ~DryWet();

    void setDryWetValue(float theDryWetValue);

    void process(juce::AudioBuffer<float>& dryBuffer, juce::AudioBuffer<float>& wetBuffer);

private:
    float dryWetValue = 50.0f;
};
