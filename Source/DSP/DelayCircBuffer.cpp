/*
  ==============================================================================

    DelayCircBuffer.cpp
    Created: 16 Apr 2024 10:55:24pm
    Author:  Iggy-casa

  ==============================================================================
*/

#include "DelayCircBuffer.h"

DelayCircBuffer::DelayCircBuffer() {}
DelayCircBuffer::~DelayCircBuffer() {}

void DelayCircBuffer::setDelayTime(float inTimeParam)
{
    timeValue = inTimeParam;
}

void DelayCircBuffer::setDelayFeedback(float inFeedbackParam)
{
    feedbackValue = inFeedbackParam;
}

void DelayCircBuffer::fftSizeUpdated(int indexChoice)
{
    //pitchShift.onUpdateFftSizeParamChoice(indexChoice);
}

void DelayCircBuffer::hopSizeUpdated(int indexChoice)
{
    //pitchShift.onUpdateHopSizeParamChoice(indexChoice);
}

void DelayCircBuffer::windowTypeUpdated(int indexChoice)
{
    //pitchShift.onUpdateWindowTypeParamChoice(indexChoice);
}

void DelayCircBuffer::prepare(double theSampleRate, juce::dsp::ProcessSpec spec)
{
    sampleRate = static_cast<float>(theSampleRate);
    circularAudioBuffer.clear();
    pitchShift.prepare(spec);
}

void DelayCircBuffer::process(juce::AudioBuffer<float>& buffer)
{
    auto outputCircularBuffer = 0.0f; // "Delayed Sample"

    auto SmoothCoefficient_Fine = 0.002f;

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            // Smooth del time para evitar el cambio abrupto de time y escuchar crujidos
            timeSmooth[channel] = (float)(timeSmooth[channel] - SmoothCoefficient_Fine * (timeSmooth[channel] - timeValue)); // Truco para que "timeValue" este "smootheado" en caso de mover rapido el slider 
            
            int delayInSamples = timeSmooth[channel] * sampleRate; // El tiempo lo medimos en samples, 41k (samplerate) es un segundo

            if (delayInSamples >= 1) 
            {
                // Sample de buffer actual
                float currentSample = buffer.getSample(channel, i);

                readerPointer = writerPointer[channel] - delayInSamples; // Usamos el retraso en samples en base al writer para saber el index del reader
                if (readerPointer < 0)
                {
                    readerPointer += circularBufferSize;
                }

                // Leer antes de escribir (valor al que apunta el reader pointer)
                    // --> LO QUE HAY EN LA POSICION DEL READER, ES LO QUE VAMOS A SUMAR A LA SALIDA, LA REPETICION
                    // Esta variable se podria llamar "delayedSample"
                outputCircularBuffer = circularBuffer[readerPointer][channel];

                // Calculo output para que suenen ambos y tambien suene el feedback!! (TODO: ver tema drywet)
                // Esto va al buffer circular en el writer pointer, asi suena de nuevo cuando el reader llegue ahi! 
                auto outputCurrentPlusDelayedPlusFeedback = currentSample + (outputCircularBuffer * feedbackValue);
                // Escribimos en el circular buffer, en la posiction del writer pointer el SAMPLE ACTUAL (+feedback!) de la iteracion
                    // --> EN EL CIRCULAR BUFFER, EN LA POSICION DEL WRITER, PONEMOS LO QUE SUENA AHORA (+feedback!), PORQUE QUEREMOS QUE SE REPITA DESPUES CUANDO EL READER LLEGUE AHI            
                circularBuffer[writerPointer[channel]][channel] = outputCurrentPlusDelayedPlusFeedback;

                // Avanzamos el writer de a uno, asi como avanza el for sample a sample
                writerPointer[channel]++;
                if (writerPointer[channel] >= circularBufferSize)
                {
                    writerPointer[channel] = 0;
                }

                // En la salida quiero lo que sea que este sonando ahora + lo que sea que a lo que este apuntando el reader pointer en el circular buffer (sin meter feedback porque ya lo puse cuando escribi en el circular buffer!)
                buffer.setSample(channel, i, currentSample + outputCircularBuffer);
            }
           
        }
    }
}


void DelayCircBuffer::process2(juce::AudioBuffer<float>& buffer)
{
    auto outputCircularBuffer = 0.0f; // "Delayed Sample"

    auto SmoothCoefficient_Fine = 0.002f;

    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            // Smooth del time para evitar el cambio abrupto de time y escuchar crujidos
            timeSmooth[channel] = (float)(timeSmooth[channel] - SmoothCoefficient_Fine * (timeSmooth[channel] - timeValue)); // Truco para que "timeValue" este "smootheado" en caso de mover rapido el slider 

            int delayInSamples = timeSmooth[channel] * sampleRate; // El tiempo lo medimos en samples, 41k (samplerate) es un segundo

            if (delayInSamples >= 1)
            {
                // Sample de buffer actual
                float currentSample = buffer.getSample(channel, i);

                readerPointer = writerPointer[channel] - delayInSamples; // Usamos el retraso en samples en base al writer para saber el index del reader
                if (readerPointer < 0)
                {
                    readerPointer += circularBufferSize;
                }

                //pitchShift.process(circularAudioBuffer, -12.0f);

                // Leer antes de escribir (valor al que apunta el reader pointer)
                    // --> LO QUE HAY EN LA POSICION DEL READER, ES LO QUE VAMOS A SUMAR A LA SALIDA, LA REPETICION
                    // Esta variable se podria llamar "delayedSample"
                outputCircularBuffer = circularAudioBuffer.getSample(channel, readerPointer);

                // Calculo output para que suenen ambos y tambien suene el feedback!! (TODO: ver tema drywet)
                // Esto va al buffer circular en el writer pointer, asi suena de nuevo cuando el reader llegue ahi! 
                auto outputCurrentPlusDelayedPlusFeedback = currentSample + (outputCircularBuffer * feedbackValue);
                // Escribimos en el circular buffer, en la posiction del writer pointer el SAMPLE ACTUAL (+feedback!) de la iteracion
                    // --> EN EL CIRCULAR BUFFER, EN LA POSICION DEL WRITER, PONEMOS LO QUE SUENA AHORA (+feedback!), PORQUE QUEREMOS QUE SE REPITA DESPUES CUANDO EL READER LLEGUE AHI            
                circularAudioBuffer.setSample(channel, writerPointer[channel], outputCurrentPlusDelayedPlusFeedback);

                // Avanzamos el writer de a uno, asi como avanza el for sample a sample
                writerPointer[channel]++;
                if (writerPointer[channel] >= circularBufferSize)
                {
                    writerPointer[channel] = 0;
                }

                // En la salida quiero lo que sea que este sonando ahora + lo que sea que a lo que este apuntando el reader pointer en el circular buffer (sin meter feedback porque ya lo puse cuando escribi en el circular buffer!)
                buffer.setSample(channel, i, currentSample + outputCircularBuffer);
            }

        }
    }
}