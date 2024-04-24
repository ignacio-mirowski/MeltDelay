/*
  ==============================================================================

    DryWet.cpp
    Created: 16 Mar 2024 1:11:22pm
    Author:  Iggy-casa

  ==============================================================================
*/

#include "DryWet.h"

DryWet::DryWet() {}
DryWet::~DryWet() {}

void DryWet::setDryWetValue(float theDryWetValue) 
{
    dryWetValue = theDryWetValue / 100; //Para que sea de 0-1
}

//a diferencia de otros, vamos a recibir 2 buffers (De lo mismo) para processar wet y dry
void DryWet::process(juce::AudioBuffer<float>& dryBuffer, juce::AudioBuffer<float>& wetBuffer) 
{
    for (int channel = 0; channel < dryBuffer.getNumChannels(); channel++)
    {
        for (int i = 0; i < dryBuffer.getNumSamples(); i++)
        {
            float wetSample = wetBuffer.getSample(channel, i);
            auto drySample = dryBuffer.getSample(channel, i);

            //Procesamiento
            float out = (drySample * (1 - dryWetValue)) + (wetSample * dryWetValue);

            //Lo metemos en el wet porque es el que tiene la señal procesada
            wetBuffer.setSample(channel, i, out);
        }
    }
}
