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
    void setDelayTimeStyle(int inTimeStlye);
    void setDelayTimeChoice(int inTimeChoice);
    //void setInitialPitch(int inInitialPitch);
    void setSemitonesToSubtract(float inSemitonesToSubtract);
    void setSemitonesToStop(float inSemitonesToStop);
    void setMeltTreshold(float inMeltThreshold);
    void setSmoothMelt(float inSmoothMelt);

    //void process(juce::AudioBuffer<float>& buffer);
    void prepare(double theSampleRate, juce::dsp::ProcessSpec spec);

    void process(juce::AudioBuffer<float>& buffer, juce::AudioPlayHead* playHead);
    void calculateTimeValue(juce::AudioPlayHead* playHead);

    //Pitch shift parameters update (no me gusta que este esto aca...despues lo re-pienso!)
    //void fftSizeUpdated(int indexChoice);
    //void hopSizeUpdated(int indexChoice);
    //void windowTypeUpdated(int indexChoice);

    void EvaluateCurrentTime(bool isPlaying, float circBufferRmsLevel, float delayInSamples, float bufferRmsLevel);

private:
    float timeValue{ 0.25f };
    float feedbackValue{ 0.25f };
    float sampleRate{ 0.0f };
    int timeStyle{ 0 };
    int timeChoice{ 0 };

    static const int circularBufferSize = 132300; // Maximo delay en samples seria esto??
   // Comentado, porque cambie implementacion para que el circular buffer sea un AudioBuffer y no un array
    // float circularBuffer[circularBufferSize][2] = { 0.0f }; // BufferSize = 3*44100 = 132300
    int readerPointer = 0;
    int writerPointer[2] = { 0 };
    float timeSmooth[2] = { 0.0f };

    PitchShift pitchShift;

    juce::AudioBuffer<float> circularAudioBuffer{ 2, circularBufferSize };

    int sampleCount = 0;
    float currentPitchSemitones = 0.0f;   
    float initialPitch = 0.0f;
    bool localIsPlaying = false;
    float meltThreshold = 6.0f;
    float smoothMelt = 1e-13f;

    float semitonesToSubtract = 1.0f;
    float semitonesToStop = -12.0f;
};