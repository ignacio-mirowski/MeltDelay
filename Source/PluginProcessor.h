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
#include "DSP/PitchShiftRubberBand.h"

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
    void joinBuffers(juce::AudioBuffer<float> outputBuffer, juce::AudioBuffer<float> buffer1, juce::AudioBuffer<float> buffer2);

private:
    DelayJuceDSP delayJuceDSP;
    DryWet dryWet;
    //Esta va para hacer un dry wet del efecto
    juce::AudioBuffer<float> dryBuffer;
    //Este va para SIEMPRE mandar la señal dry mezclada con todo el efecto, asi solo las repeticiones tiene pitchshift
    juce::AudioBuffer<float> dryBufferAlwaysOn;
    DelayCircBuffer delayCircBuffer;

    PitchShiftRubberBand pitchShiftRubberBand;    


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeltDelayAudioProcessor)
};
