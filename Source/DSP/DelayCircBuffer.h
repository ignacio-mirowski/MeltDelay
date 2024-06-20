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
#include "PitchShiftRubberBand.h"

class DelayCircBuffer
{
public:
    DelayCircBuffer();
    ~DelayCircBuffer();

    enum class PitchShiftAlgorithm {
        Juandagilc,
        RubberBand
    };

    enum class DownScaleMode {
        Free,
        Chromatic,
        Major,
        NaturalMinor,
        HarmonicMinor,
        MelodicMinor,
        MajorPentatonic,
        MinorPentatonic,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        Locrian
    };

    void setDelayTime(float inTimeParam);
    void setDelayFeedback(float inFeedbackParam);
    void setDelayTimeStyle(int inTimeStlye);
    void setDelayTimeChoice(int inTimeChoice);
    void setSemitonesToSubtract(float inSemitonesToSubtract);
    void setSemitonesToStop(float inSemitonesToStop);
    void setMeltTreshold(float inMeltThreshold);
    void setSmoothMelt(float inSmoothMelt);
    void setMode(int modeChoice);

    void prepare(juce::dsp::ProcessSpec spec);

    void process(juce::AudioBuffer<float>& buffer, juce::AudioPlayHead* playHead, PitchShiftAlgorithm algorithm);

    void calculateTimeValue(juce::AudioPlayHead* playHead);

    void EvaluateCurrentTime(bool isPlaying, float circBufferRmsLevel, float delayInSamples, float bufferRmsLevel);

    std::vector<int> findScaleToUse();


private:
    float timeValue{ 0.25f };
    float feedbackValue{ 0.25f };
    float sampleRate{ 0.0f };
    int timeStyle{ 0 };
    int timeChoice{ 0 };
    int modeChoice{ 0 };

    static const int circularBufferSize = 132300; // Maximo delay en samples seria esto??
   // Comentado, porque cambie implementacion para que el circular buffer sea un AudioBuffer y no un array
    // float circularBuffer[circularBufferSize][2] = { 0.0f }; // BufferSize = 3*44100 = 132300
    int readerPointer = 0;
    int writerPointer[2] = { 0 };
    float timeSmooth[2] = { 0.0f };

    PitchShift pitchShift;
    PitchShiftRubberBand pitchShiftRubberBand;

    juce::AudioBuffer<float> circularAudioBuffer{ 2, circularBufferSize };

    int sampleCount = 0;
    float currentPitchSemitones = 0.0f;   
    float initialPitch = 0.0f;
    bool localIsPlaying = false;
    float meltThreshold = 6.0f;
    float smoothMelt = 1e-13f;

    float semitonesToSubtract = 1.0f;
    float semitonesToStop = -12.0f;

    int scaleIndex = 0;
    std::vector<int> inverseCromaticScaleIntervals = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    std::vector<int> inverseMajorScaleIntervals = { 1, 2, 2, 2, 1, 2, 2 };
    std::vector<int> inverseNaturalMinorScaleIntervals = { 1, 2, 2, 1, 2, 2, 2 };
    std::vector<int> inverseHarmonicMinorScaleIntervals = { 1, 3, 1, 2, 1, 2, 2 };
    std::vector<int> inverseMelodicMinorScaleIntervals = { 1, 2, 2, 1, 2, 2, 2 };
    std::vector<int> inverseMajorPentatonicScaleIntervals = { 3, 2, 2, 3, 2 };
    std::vector<int> inverseMinorPentatonicScaleIntervals = { 3, 2, 2, 3, 2 };
    std::vector<int> inverseDorianScaleIntervals = { 1, 2, 2, 2, 1, 2, 2 };
    std::vector<int> inversePhrygianScaleIntervals = { 1, 2, 2, 2, 2, 1, 2 };
    std::vector<int> inverseLydianScaleIntervals = { 1, 2, 2, 1, 2, 2, 2 };
    std::vector<int> inverseMixolydianScaleIntervals = { 1, 2, 2, 2, 1, 2, 2 };
    std::vector<int> inverseLocrianScaleIntervals = { 1, 2, 2, 2, 1, 2, 2 };
};