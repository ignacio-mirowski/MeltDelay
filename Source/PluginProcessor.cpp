/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/PitchShift.h"

//==============================================================================
MeltDelayAudioProcessor::MeltDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

MeltDelayAudioProcessor::~MeltDelayAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MeltDelayAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameters;

    //auto pi = juce::MathConstants<float>::pi;
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "DryWet", 1 }, "DryWet", 0.0f, 100.0f, 50.0f));
    parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "TimeChoice", 1 }, "TimeChoice", juce::StringArray("1/64", "1/32", "1/16", "1/8", "1/4", "1/2", "1/1"), 0));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "Time", 1 }, "Time", 0.1f, 0.9f, 0.25f));
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "Feedback", 1 }, "Feedback", 0.1f, 0.9f, 0.25f));

    /** Pitch */
    parameters.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Pitch", 1), "Pitch",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f, 1.0f),
        -12.0f, "st", juce::AudioProcessorParameter::genericParameter, nullptr, nullptr));

    //parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "FftSizeChoice", 1 }, "FftSizeChoice", fftSizeItemsUI, fftSize512));
    //parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "HopSizeChoice", 1 }, "HopSizeChoice", hopSizeItemsUI, hopSize8));
    //parameters.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "WindowTypeChoice", 1 }, "WindowTypeChoice", windowTypeItemsUI, windowTypeHann));


    return parameters;
}

//==============================================================================
const juce::String MeltDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MeltDelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MeltDelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MeltDelayAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MeltDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MeltDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int MeltDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MeltDelayAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String MeltDelayAudioProcessor::getProgramName(int index)
{
    return {};
}

void MeltDelayAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void MeltDelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    delayJuceDSP.prepare(spec);
    delayCircBuffer.prepare(sampleRate, spec);

    //FOR TEST, ENREALDIAD VA ADENTRO DEL DELAYCIRC
    pitchShift.prepare(spec);
}

void MeltDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MeltDelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void MeltDelayAudioProcessor::updateParameters()
{
    float inFeedbackParameter = *apvts.getRawParameterValue("Feedback");
    float inDryWetParameter = *apvts.getRawParameterValue("DryWet");
    int inTimeChoice = *apvts.getRawParameterValue("TimeChoice");
    float inTime = *apvts.getRawParameterValue("Time");

    //// Params de pitch shifting POR AHORA HARCODEADOS
    //int inFftSizeChoice = *apvts.getRawParameterValue("FftSizeChoice");
    //int inHopSizeChoice = *apvts.getRawParameterValue("HopSizeChoice");
    //int windowsTypeChoice = *apvts.getRawParameterValue("WindowTypeChoice");
    //

    delayJuceDSP.setDelayFeedback(inFeedbackParameter);
    delayJuceDSP.setDelayTime(inTime);
    delayCircBuffer.setDelayFeedback(inFeedbackParameter);
    delayCircBuffer.setDelayTime(inTime);
    //Sets de params de pitch shift dentro de delayCircBuffer
        //delayCircBuffer.
    //
    dryWet.setDryWetValue(inDryWetParameter);
}

void MeltDelayAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    updateParameters();

    dryBuffer.makeCopyOf(buffer);

    //delayJuceDSP.process(buffer);
    delayCircBuffer.process2(buffer);

    //FOR TEST, ENREALDIAD VA ADENTRO DEL DELAYCIRC
    //pitchShift.process(buffer, *apvts.getRawParameterValue("Pitch"));

    dryWet.process(dryBuffer, buffer);
}

//==============================================================================
bool MeltDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MeltDelayAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
    //return new MeltDelayAudioProcessorEditor (*this);
}

//==============================================================================
void MeltDelayAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml(apvts.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MeltDelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            auto state = juce::ValueTree::fromXml(*xmlState);
            apvts.replaceState(state);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MeltDelayAudioProcessor();
}
