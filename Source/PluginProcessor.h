/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/DelayJuceDSP.h"
#include "DSP/DelayCircBuffer.h"
#include "DSP/DryWet.h"
#include "DSP/PitchShift.h"

//==============================================================================
/**
*/
class MeltDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MeltDelayAudioProcessor();
    ~MeltDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void updateParameters();


private:
    DelayJuceDSP delayJuceDSP;
    DryWet dryWet;
    juce::AudioBuffer<float> dryBuffer;
    DelayCircBuffer delayCircBuffer;

    //FOR TEST, ENREALDIAD VA ADENTRO DEL DELAYCIRC
    PitchShift pitchShift;


    // Params para PitchShift
    juce::StringArray fftSizeItemsUI = {
        "32",
        "64",
        "128",
        "256",
        "512",
        "1024",
        "2048",
        "4096",
        "8192"
    };

    enum fftSizeIndex {
        fftSize32 = 0,
        fftSize64,
        fftSize128,
        fftSize256,
        fftSize512,
        fftSize1024,
        fftSize2048,
        fftSize4096,
        fftSize8192,
    };

    juce::StringArray hopSizeItemsUI = {
        "1/2 Window",
        "1/4 Window",
        "1/8 Window",
    };

    enum hopSizeIndex {
        hopSize2 = 0,
        hopSize4,
        hopSize8,
    };

    //======================================

    juce::StringArray windowTypeItemsUI = {
        "Bartlett",
        "Hann",
        "Hamming",
    };

    enum windowTypeIndex {
        windowTypeBartlett = 0,
        windowTypeHann,
        windowTypeHamming,
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeltDelayAudioProcessor)
};
