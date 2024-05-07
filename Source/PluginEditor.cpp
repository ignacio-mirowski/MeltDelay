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


void MeltDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* s)
{
    //if (s == &volumeSlider)
    //{
    //    //log o algo
    //    //DBG(volumeSlider.getValue());
    //}
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
    timeValueLabel.setBounds(77, 125, 60, 40);

    feedbackSlider.setBounds(445, 42, 105, 105);
    feedbackValueLabel.setBounds(467, 125, 60, 40);

    stToSubSlider.setBounds(112.5, 245, 75, 75);
    stStoSubValueLabel.setBounds(131, 299, 60, 40);

    stToStopSlider.setBounds(212.5, 245, 75, 75); // +100
    stToStopValueLabel.setBounds(227, 299, 60, 40);

    meltThreshSlider.setBounds(312.5, 245, 75, 75); // +100
    meltThreshValueLabel.setBounds(321, 299, 60, 40);

    meltSmoothSlider.setBounds(412.5, 245, 75, 75); // +100
    meltSmoothValueLabel.setBounds(427, 299, 60, 40);

    dryWetSlider.setBounds(490, 313, 75, 75);
    dryWetValueLabel.setBounds(510, 365, 60, 40);
}

void MeltDelayAudioProcessorEditor::prepareSliders()
{
    // Time
    timeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    timeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(timeSlider);
    timeSlider.addListener(this);

    timeValueLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(timeValueLabel);
    timeSlider.onValueChange = [this] { timeValueLabel.setText(juce::String(timeSlider.getValue()) + " ms", juce::NotificationType::dontSendNotification); };

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
    feedbackSlider.addListener(this);

    feedbackValueLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(feedbackValueLabel);
    feedbackSlider.onValueChange = [this] { feedbackValueLabel.setText(juce::String(feedbackSlider.getValue()) + " ms", juce::NotificationType::dontSendNotification); };

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
    stToSubSlider.addListener(this);

    stStoSubValueLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(stStoSubValueLabel);
    stToSubSlider.onValueChange = [this] { stStoSubValueLabel.setText(juce::String(stToSubSlider.getValue()) + " st", juce::NotificationType::dontSendNotification); };

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
    stToStopSlider.addListener(this);

    stToStopValueLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(stToStopValueLabel);
    stToStopSlider.onValueChange = [this] { stToStopValueLabel.setText(juce::String(stToStopSlider.getValue()) + " st", juce::NotificationType::dontSendNotification); };

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
    meltThreshSlider.addListener(this);

    meltThreshValueLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(meltThreshValueLabel);
    meltThreshSlider.onValueChange = [this] { meltThreshValueLabel.setText(juce::String(meltThreshSlider.getValue()) + " db", juce::NotificationType::dontSendNotification); };

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
    meltSmoothSlider.addListener(this);

    meltSmoothValueLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(meltSmoothValueLabel);
    meltSmoothSlider.onValueChange = [this] { meltSmoothValueLabel.setText(juce::String(meltSmoothSlider.getValue()) + "", juce::NotificationType::dontSendNotification); };

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
    dryWetSlider.addListener(this);

    dryWetValueLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(dryWetValueLabel);
    dryWetSlider.onValueChange = [this] { dryWetValueLabel.setText(juce::String(dryWetSlider.getValue()) + " %", juce::NotificationType::dontSendNotification); };

    dryWetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DryWet", dryWetSlider);

    dryWetLabel.setText("Dry/Wet", juce::dontSendNotification);
    dryWetLabel.attachToComponent(&dryWetSlider, false);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryWetLabel);

    dryWetLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
}
