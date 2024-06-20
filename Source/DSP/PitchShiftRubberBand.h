/*
  ==============================================================================

    PitchShiftRubberBand.h
    Created: 29 May 2024 10:39:37pm
    Author:  Iggy-casa

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <RubberBandStretcher.h>
#include "RubberBandPitchShifter.h"

class PitchShiftRubberBand {
public:
    PitchShiftRubberBand();
    ~PitchShiftRubberBand();

    //void setSemitones(float inSemitones);
    //void setOctaves(float inOctaves);
    //void setCents(float inCents);
    //void setDryWet(float inDryWet);
    //void setCrisp(float inCrisp);
    //void setFormat(bool inFormat);

    void onSmoothMeltChange(float inSmoothMelt, double sampleRate);

    void prepare(juce::dsp::ProcessSpec spec);

    void process(juce::AudioBuffer<float>& buffer, float inSemitones);
    void process2(juce::AudioBuffer<float>& buffer);


private:
    std::unique_ptr<RubberBand::RubberBandStretcher> pitchShifter2;
    juce::AudioBuffer<float> mTempBuffer;

    std::unique_ptr<RubberBandPitchShifter> pitchShifter;

    juce::LinearSmoothedValue<float> semitonesParamShift{ 0.0f };

    //float octaves = 0.0f;
    //float semitones = 0.0f;
    //float cents = 0.0f;
    //float wet_dry = 0.0f;
    //int crispness = 0.0f;
    //bool format = 0.0f;
};