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

void DelayCircBuffer::setDelayTimeStyle(int inTimeStlye)
{
    timeStyle = inTimeStlye;
}

void DelayCircBuffer::setDelayTimeChoice(int inTimeChoice)
{
    timeChoice = inTimeChoice;
}

void DelayCircBuffer::setMeltTreshold(float inMeltThreshold)
{
    meltThreshold = inMeltThreshold;
}

//void DelayCircBuffer::setInitialPitch(int inInitialPitch)
//{
//    initialPitch = inInitialPitch;
//}

void DelayCircBuffer::setSemitonesToSubtract(float inSemitonesToSubtract)
{
    semitonesToSubtract = inSemitonesToSubtract;
}

void DelayCircBuffer::setSemitonesToStop(float inSemitonesToStop)
{
    semitonesToStop = inSemitonesToStop;
}

void DelayCircBuffer::setSmoothMelt(float inSmoothMelt)
{
    if (smoothMelt != inSmoothMelt) 
    {
        pitchShift.onSmoothMeltChange(inSmoothMelt, sampleRate);
        smoothMelt = inSmoothMelt;
    }
}


//void DelayCircBuffer::fftSizeUpdated(int indexChoice)
//{
//    //pitchShift.onUpdateFftSizeParamChoice(indexChoice);
//}
//
//void DelayCircBuffer::hopSizeUpdated(int indexChoice)
//{
//    //pitchShift.onUpdateHopSizeParamChoice(indexChoice);
//}
//
//void DelayCircBuffer::windowTypeUpdated(int indexChoice)
//{
//    //pitchShift.onUpdateWindowTypeParamChoice(indexChoice);
//}

void DelayCircBuffer::prepare(juce::dsp::ProcessSpec spec)
{
    sampleRate = static_cast<float>(spec.sampleRate);
    circularAudioBuffer.clear();
    pitchShift.prepare(spec);
    pitchShiftRubberBand.prepare(spec);
}

// Implementacion de process usando array de dos dimensiones como circular buffer, en vez de un AudioBuffer
//void DelayCircBuffer::process(juce::AudioBuffer<float>& buffer)
//{
//    auto outputCircularBuffer = 0.0f; // "Delayed Sample"
//
//    auto SmoothCoefficient_Fine = 0.002f;
//
//    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
//    {
//        for (int i = 0; i < buffer.getNumSamples(); i++)
//        {
//            // Smooth del time para evitar el cambio abrupto de time y escuchar crujidos
//            timeSmooth[channel] = (float)(timeSmooth[channel] - SmoothCoefficient_Fine * (timeSmooth[channel] - timeValue)); // Truco para que "timeValue" este "smootheado" en caso de mover rapido el slider 
//            
//            int delayInSamples = timeSmooth[channel] * sampleRate; // El tiempo lo medimos en samples, 41k (samplerate) es un segundo
//
//            if (delayInSamples >= 1) 
//            {
//                // Sample de buffer actual
//                float currentSample = buffer.getSample(channel, i);
//
//                readerPointer = writerPointer[channel] - delayInSamples; // Usamos el retraso en samples en base al writer para saber el index del reader
//                if (readerPointer < 0)
//                {
//                    readerPointer += circularBufferSize;
//                }
//
//                // Leer antes de escribir (valor al que apunta el reader pointer)
//                    // --> LO QUE HAY EN LA POSICION DEL READER, ES LO QUE VAMOS A SUMAR A LA SALIDA, LA REPETICION
//                    // Esta variable se podria llamar "delayedSample"
//                outputCircularBuffer = circularBuffer[readerPointer][channel];
//
//                // Calculo output para que suenen ambos y tambien suene el feedback!! (TODO: ver tema drywet)
//                // Esto va al buffer circular en el writer pointer, asi suena de nuevo cuando el reader llegue ahi! 
//                auto outputCurrentPlusDelayedPlusFeedback = currentSample + (outputCircularBuffer * feedbackValue);
//                // Escribimos en el circular buffer, en la posiction del writer pointer el SAMPLE ACTUAL (+feedback!) de la iteracion
//                    // --> EN EL CIRCULAR BUFFER, EN LA POSICION DEL WRITER, PONEMOS LO QUE SUENA AHORA (+feedback!), PORQUE QUEREMOS QUE SE REPITA DESPUES CUANDO EL READER LLEGUE AHI            
//                circularBuffer[writerPointer[channel]][channel] = outputCurrentPlusDelayedPlusFeedback;
//
//                // Avanzamos el writer de a uno, asi como avanza el for sample a sample
//                writerPointer[channel]++;
//                if (writerPointer[channel] >= circularBufferSize)
//                {
//                    writerPointer[channel] = 0;
//                }
//
//                // En la salida quiero lo que sea que este sonando ahora + lo que sea que a lo que este apuntando el reader pointer en el circular buffer (sin meter feedback porque ya lo puse cuando escribi en el circular buffer!)
//                buffer.setSample(channel, i, currentSample + outputCircularBuffer);
//            }
//           
//        }
//    }
//}

void DelayCircBuffer::process(juce::AudioBuffer<float>& buffer, juce::AudioPlayHead* playHead, PitchShiftAlgorithm algorithm)
{
    auto outputCircularBuffer = 0.0f; // "Delayed Sample"

    auto SmoothCoefficient_Fine = 0.002f;

    calculateTimeValue(playHead);

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
                outputCircularBuffer = circularAudioBuffer.getSample(channel, readerPointer);

                // Calculo output para que suenen ambos y tambien suene el feedback!! 
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

                //Esto es para solo meter las repeticiones en el buffer...capaz no hace falta pensarlo como drywet creo yo..y simplemente mandar al out el outputCircularBuffer
                //float outWithDryWet = (currentSample * (1 - 1.0f)) + (outputCircularBuffer * 1.0f);

                // [OUTDATED] - En la salida quiero lo que sea que este sonando ahora + lo que sea que a lo que este apuntando el reader pointer en el circular buffer (sin meter feedback porque ya lo puse cuando escribi en el circular buffer!)
                //  -> Esto lo cambie, porque quiero solo el output del circular buffer aca, el dryBufferAlwaysOn se va a encargar de que "suene lo que suena actualmente"
                buffer.setSample(channel, i, outputCircularBuffer);//currentSample + outputCircularBuffer);
            }
        }
    }

    // Sample count para saber en que punto cambiar el pitching
    sampleCount += buffer.getNumSamples();

    juce::AudioPlayHead::CurrentPositionInfo currentPosition;
    playHead->getCurrentPosition(currentPosition);

    EvaluateCurrentTime(currentPosition.isPlaying, circularAudioBuffer.getRMSLevel(0, 0, circularAudioBuffer.getNumSamples()), timeSmooth[0] * sampleRate, buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    //if (currentPitchSemitones < 0) // TODO: ESTO LOHAGO para que con st to subtract = 0 suene clean, PERO, esta generando que si vuelvo a 0 sin dejar de estar en "play" no vuelva nunca a dejar de pitchear. Tengo que fixear eso.
    //{
        if (algorithm == PitchShiftAlgorithm::RubberBand)
            pitchShiftRubberBand.process(buffer, currentPitchSemitones);
        else
            pitchShift.process(buffer, currentPitchSemitones);
    //}
}


void DelayCircBuffer::EvaluateCurrentTime(bool isPlaying, float circBufferRmsLevel, float delayInSamples, float bufferRmsLevel)
{
    // Primeros dos ifs para en play/pause se resettee conteo de semitones
    if (isPlaying != localIsPlaying)
    {
        localIsPlaying = isPlaying;
        if (localIsPlaying)
        {
            sampleCount = 0;
            currentPitchSemitones = 0.0f;
        }
    }

    if (!localIsPlaying && circBufferRmsLevel < 3.0e-10f)
    {
        sampleCount = 0;
        currentPitchSemitones = 0.0f;
    }

    // Solo quiero bajar semitonos si el nivel audio actual esta por encima del threshold
    float dbBufferLevel = 20.0f * std::log10(bufferRmsLevel);

    if (dbBufferLevel > meltThreshold)
    {
        // Si paso un X tiempo (sampleRate = 1 seg)
        if (sampleCount >= delayInSamples)//sampleRate) // >= sampleRate seria un segundo. delayInSamples el "time" del delay
        {
            sampleCount = 0;
            currentPitchSemitones = currentPitchSemitones - semitonesToSubtract;

            if (currentPitchSemitones < semitonesToStop) {
                currentPitchSemitones = 0.0f;
            }
        }
    }
    else 
    {
        sampleCount = 0;
        currentPitchSemitones = 0.0f;
    }

}

void DelayCircBuffer::calculateTimeValue(juce::AudioPlayHead* playHead) //en segundos!
{
    if (timeStyle == 0) //ms
    {
        //do nothing, I want to keep the timeValue as it is
        //return timeValue;
    }
    else //bpm 
    {
        //calculate time in ms based on time choice selection
        juce::AudioPlayHead::CurrentPositionInfo currentPosition;
        //auto bpm2 = playHead->getPosition()->getBpm();        
        playHead->getCurrentPosition(currentPosition);
        double bpm = currentPosition.bpm;
        
        double secondsPerBeat = 60.0 / bpm;

        switch (timeChoice)
        {
        case 0: //1/16
            timeValue = secondsPerBeat / 4;
            break;
        case 1: //1/8
            timeValue = secondsPerBeat / 2;
            break;
        case 2: //1/4
            timeValue = secondsPerBeat;
            break;
        case 3: //1/2
            timeValue = secondsPerBeat * 2;
            break;
        case 4: //1/1
            timeValue = secondsPerBeat * 4; 
            break;
        default:
            break;
        }
    }
}