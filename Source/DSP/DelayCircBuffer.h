/*
  ==============================================================================

    DelayCircBuffer.h
    Created: 16 Apr 2024 10:55:24pm
    Author:  Iggy-casa

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PitchShift.h"

class DelayCircBuffer
{
public:
    DelayCircBuffer();
    ~DelayCircBuffer();

    void setDelayTime(float inTimeParam);
    void setDelayFeedback(float inFeedbackParam);
    void process(juce::AudioBuffer<float>& buffer);
    void prepare(double theSampleRate, juce::dsp::ProcessSpec spec);

    void process2(juce::AudioBuffer<float>& buffer);

    //Pitch shift parameters update (no me gusta que este esto aca...despues lo re-pienso!)
    void fftSizeUpdated(int indexChoice);
    void hopSizeUpdated(int indexChoice);
    void windowTypeUpdated(int indexChoice);

private:
    float timeValue{ 0.0f };
    float feedbackValue{ 0.9f };
    float sampleRate{ 0.0f };
    static const int circularBufferSize = 132300; // Maximo delay en samples seria esto??
    float circularBuffer[circularBufferSize][2] = { 0.0f }; // BufferSize = 3*44100 = 132300 (Quizas en vez de array de dos dimensiones puedo crear otra cosa? un juce::AudioBuffer o algo asi?)
    int readerPointer = 0;
    int writerPointer[2] = { 0 };
    float timeSmooth[2] = { 0.0f };
    PitchShift pitchShift;

    juce::AudioBuffer<float> circularAudioBuffer{ 2, circularBufferSize };
    //Circular buffer como audiobuffer
};