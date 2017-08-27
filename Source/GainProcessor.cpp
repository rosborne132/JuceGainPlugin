/*
 ==============================================================================

 This file is part of the JUCE library.
 Copyright (c) 2015 - ROLI Ltd.

 Permission is granted to use this software under the terms of either:
 a) the GPL v2 (or any later version)
 b) the Affero GPL v3

 Details of these licenses can be found at: www.gnu.org/licenses

 JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

 ------------------------------------------------------------------------------

 To release a closed-source product which uses JUCE, commercial licenses are
 available: visit www.juce.com for more information.

 ==============================================================================
 */

#include "JuceHeader.h"
#include "GenericEditor.h"

//==============================================================================
/**
 */
class TutorialProcessor  : public AudioProcessor
{
public:

    //==============================================================================
    TutorialProcessor()
    {
        addParameter (gain = new AudioParameterFloat ("gain", // parameterID
                                                      "Gain", // parameter name
                                                      NormalisableRange<float> (0.0f,1.0f),
                                                      0.5f)); // default value
        
        addParameter (invertPhase = new AudioParameterBool ("invertPhase",
                                                            "Invert Phase",
                                                            false));
    }

    ~TutorialProcessor() {}

    //==============================================================================
    void prepareToPlay (double, int) override
    {
        const float phase = *invertPhase ? -1.0f : 1.0f;
        previousGain = *gain * phase;
    }
    
    void releaseResources() override {}
    
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer&) override
    {
        const float phase = *invertPhase ? -1.0f : 1.0f;
        const float currentGain = *gain * phase;
        if (currentGain == previousGain)
        {
            buffer.applyGain (currentGain);
        }
        else
        {
            buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);
            previousGain = currentGain;
        }
    }

    //==============================================================================
    AudioProcessorEditor* createEditor() override { return new GenericEditor (*this); }
    bool hasEditor() const override               { return true;   }

    //==============================================================================
    const String getName() const override               { return "Param Tutorial"; }
    bool acceptsMidi() const override                   { return false; }
    bool producesMidi() const override                  { return false; }
    double getTailLengthSeconds() const override        { return 0; }

    //==============================================================================
    int getNumPrograms() override                          { return 1; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const String getProgramName (int) override             { return String(); }
    void changeProgramName (int , const String& ) override { }

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override
    {
        ScopedPointer<XmlElement> xml (new XmlElement ("ParamTutorial"));
        xml->setAttribute ("gain", (double) *gain);
        xml->setAttribute ("invertPhase", *invertPhase);
        copyXmlToBinary (*xml, destData);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
        if (xmlState != nullptr)
        {
            if (xmlState->hasTagName ("ParamTutorial"))
            {
                *gain = (float) xmlState->getDoubleAttribute ("gain", 1.0);
                *invertPhase = xmlState->getBoolAttribute ("invertPhase", false); // [5]
            }
        }
    }
    
private:
    //==============================================================================
    AudioParameterFloat* gain;
    AudioParameterBool* invertPhase;
    float previousGain;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialProcessor)
};

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TutorialProcessor();
}
