/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MeltDelayAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::Button::Listener
{
public:
    MeltDelayAudioProcessorEditor (MeltDelayAudioProcessor&);
    ~MeltDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider*) override;
    void buttonClicked(juce::Button*) override;

    void prepareSliders();
    void prepareButtons();
    //void prepareCombobox();

    void onBpmButtonClick();
    void onMsButtonClick();


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MeltDelayAudioProcessor& audioProcessor;

    juce::Slider timeSlider, feedbackSlider, stToSubSlider, stToStopSlider, meltThreshSlider, meltSmoothSlider, dryWetSlider; 
    juce::Label timeLabel, timeValueLabel, feedbackLabel, feedbackValueLabel, stToSubLabel, stStoSubValueLabel, stToStopLabel, stToStopValueLabel, meltThreshLabel, meltThreshValueLabel, meltSmoothLabel, meltSmoothValueLabel, dryWetLabel, dryWetValueLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeAttach, feedbackAttach, stToSubAttach, stToStopAttach, meltThreshAttach, meltSmoothAttach, dryWetAttach;

    juce::ToggleButton msButton;
    juce::ToggleButton bpmButton;
    
    juce::Slider timeChoiceSlider; juce::Label timeChoiceLabel, timeChoiceValueLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeChoiceAttach;

    juce::Image pattyMeltImage;
    juce::ImageComponent pattyMeltImageComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeltDelayAudioProcessorEditor)
};
