/*
  ==============================================================================

    PitchShift.h
    Created: 20 Apr 2024 1:48:56pm
    Author:  Iggy-casa

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PitchShift      // Copiado de https://github.com/juandagilc/Audio-Effects/blob/master/Pitch%20Shift/Source/PluginProcessor.cpp
{
public:

    PitchShift();
    ~PitchShift();

    void onSmoothMeltChange(float inSmoothMelt, double sampleRate);

    void prepare(juce::dsp::ProcessSpec& spec);

    void process(juce::AudioBuffer<float>& buffer, float inSemitones);

    //======================================

    float getScaleSemitone(float inValue);

    void updateFftSize(int inNumChannels);

    void updateHopSize();

    void updateAnalysisWindow();

    void updateWindow(const juce::HeapBlock<float>& window, const int windowLength);

    void updateWindowScaleFactor();

    float princArg(const float phase);

private:

    enum windowTypeIndex
    {
        windowTypeBartlett = 0,
        windowTypeHann,
        windowTypeHamming,
    };

    enum hopSizeIndex
    {
        hopSize2 = 0,
        hopSize4,
        hopSize8,
    };

    juce::CriticalSection lock;

    int fftSize;
    std::unique_ptr<juce::dsp::FFT> fft;

    int inputBufferLength;
    int inputBufferWritePosition;
    juce::AudioSampleBuffer inputBuffer;

    int outputBufferLength;
    int outputBufferWritePosition;
    int outputBufferReadPosition;
    juce::AudioSampleBuffer outputBuffer;

    juce::HeapBlock<float> fftWindow;
    juce::HeapBlock<juce::dsp::Complex<float>> fftTimeDomain;
    juce::HeapBlock<juce::dsp::Complex<float>> fftFrequencyDomain;

    int samplesSinceLastFFT;
    int overlap;
    int hopSize;
    float windowScaleFactor;

    //======================================

    juce::HeapBlock<float> omega;
    juce::AudioSampleBuffer inputPhase;
    juce::AudioSampleBuffer outputPhase;
    bool needToResetPhases;

    //======================================

    juce::LinearSmoothedValue<float> paramShift{ 0.0f };
};
