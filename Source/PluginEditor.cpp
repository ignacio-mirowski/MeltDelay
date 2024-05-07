/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MeltDelayAudioProcessorEditor::MeltDelayAudioProcessorEditor (MeltDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    pattyMeltImage = juce::ImageCache::getFromMemory(BinaryData::PattyAsset3_png, BinaryData::PattyAsset3_pngSize);
    pattyMeltImageComponent.setImage(pattyMeltImage);
    addAndMakeVisible(pattyMeltImageComponent);

    prepareSliders();


    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(600, 400);
}

MeltDelayAudioProcessorEditor::~MeltDelayAudioProcessorEditor()
{
}

//==============================================================================
void MeltDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    g.fillAll(juce::Colours::white);

}

void MeltDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    pattyMeltImageComponent.setBounds(200, 25, 200, 200);

    timeSlider.setBounds(50, 42, 105, 105);
    feedbackSlider.setBounds(445, 42, 105, 105);
    stToSubSlider.setBounds(112.5, 245, 75, 75);
    stToStopSlider.setBounds(212.5, 245, 75, 75); // +100
    meltThreshSlider.setBounds(312.5, 245, 75, 75); // +100
    meltSmoothSlider.setBounds(412.5, 245, 75, 75); // +100
    dryWetSlider.setBounds(490, 320, 75, 75);
}

void MeltDelayAudioProcessorEditor::prepareSliders()
{
    // Time
    timeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    timeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(timeSlider);
    //timeSlider.addListener(this);

    timeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Time", timeSlider);

    timeLabel.setText("Time", juce::dontSendNotification);
    timeLabel.attachToComponent(&timeSlider, false);
    timeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(timeLabel);

    timeLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);


    // Feedback
    feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(feedbackSlider);

    feedbackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Feedback", feedbackSlider);

    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(feedbackLabel);

    feedbackLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);

    // St to substract
    stToSubSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    stToSubSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(stToSubSlider);

    stToSubAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SemitonesToSubtract", stToSubSlider);

    stToSubLabel.setText("St to Subtract", juce::dontSendNotification);
    stToSubLabel.attachToComponent(&stToSubSlider, false);
    stToSubLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stToSubLabel);

    stToSubLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);

    // St to stop
    stToStopSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    stToStopSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(stToStopSlider);

    stToStopAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SemitonesToStop", stToStopSlider);

    stToStopLabel.setText("St to Stop", juce::dontSendNotification);
    stToStopLabel.attachToComponent(&stToStopSlider, false);
    stToStopLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stToStopLabel);

    stToStopLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);

    // Melt threshold
    meltThreshSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    meltThreshSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(meltThreshSlider);

    meltThreshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MeltTreshold", meltThreshSlider);

    meltThreshLabel.setText("Melt Threshold", juce::dontSendNotification);
    meltThreshLabel.attachToComponent(&meltThreshSlider, false);
    meltThreshLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(meltThreshLabel);

    meltThreshLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);

    // Melt Smoother
    meltSmoothSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    meltSmoothSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(meltSmoothSlider);

    meltSmoothAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SmoothMelt", meltSmoothSlider);

    meltSmoothLabel.setText("Melt Smooth", juce::dontSendNotification);
    meltSmoothLabel.attachToComponent(&meltSmoothSlider, false);
    meltSmoothLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(meltSmoothLabel);

    meltSmoothLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);

    // DryWet
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(dryWetSlider);

    dryWetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DryWet", dryWetSlider);

    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.attachToComponent(&dryWetSlider, false);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryWetLabel);

    dryWetLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
}
