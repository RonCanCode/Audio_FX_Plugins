// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE sine wave synthesizer
//
// Updated by Ron Guglilemone
// New Description: Triad building synthesizer

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"

class MainContentComponent :
public AudioAppComponent,
private Slider::Listener,
private Button::Listener
{
public:
    MainContentComponent() : gain (0.0), samplingRate(0.0)
    {

        // button to add the "C" root note
        addAndMakeVisible(Cn3Button);
        Cn3Button.addListener(this);
        addAndMakeVisible(Cn3ButtonLabel);
        Cn3ButtonLabel.setText ("C", dontSendNotification);
        Cn3ButtonLabel.attachToComponent (&Cn3Button, true);
        Cn3Button.setRadioGroupId(5);
        
        // button to add the "C#" root note
        addAndMakeVisible(Cs3Button);
        Cs3Button.addListener(this);
        addAndMakeVisible(Cs3ButtonLabel);
        Cs3ButtonLabel.setText ("C#", dontSendNotification);
        Cs3ButtonLabel.attachToComponent (&Cs3Button, true);
        Cs3Button.setRadioGroupId(5);
        
        // button to add the "D" root note
        addAndMakeVisible(Dn3Button);
        Dn3Button.addListener(this);
        addAndMakeVisible(Dn3ButtonLabel);
        Dn3ButtonLabel.setText ("D", dontSendNotification);
        Dn3ButtonLabel.attachToComponent (&Dn3Button, true);
        Dn3Button.setRadioGroupId(5);
        
        // button to add the "D#" root note
        addAndMakeVisible(Ds3Button);
        Ds3Button.addListener(this);
        addAndMakeVisible(Ds3ButtonLabel);
        Ds3ButtonLabel.setText ("D#", dontSendNotification);
        Ds3ButtonLabel.attachToComponent (&Ds3Button, true);
        Ds3Button.setRadioGroupId(5);
        
        // button to add the "E" root note
        addAndMakeVisible(En3Button);
        En3Button.addListener(this);
        addAndMakeVisible(En3ButtonLabel);
        En3ButtonLabel.setText ("E", dontSendNotification);
        En3ButtonLabel.attachToComponent (&En3Button, true);
        En3Button.setRadioGroupId(5);
        
        // button to add the "F" root note
        addAndMakeVisible(Fn3Button);
        Fn3Button.addListener(this);
        addAndMakeVisible(Fn3ButtonLabel);
        Fn3ButtonLabel.setText ("F", dontSendNotification);
        Fn3ButtonLabel.attachToComponent (&Fn3Button, true);
        Fn3Button.setRadioGroupId(5);
        
        // button to add the "F#" root note
        addAndMakeVisible(Fs3Button);
        Fs3Button.addListener(this);
        addAndMakeVisible(Fs3ButtonLabel);
        Fs3ButtonLabel.setText ("F#", dontSendNotification);
        Fs3ButtonLabel.attachToComponent (&Fs3Button, true);
        Fs3Button.setRadioGroupId(5);
        
        // button to add the "G" root note
        addAndMakeVisible(Gn3Button);
        Gn3Button.addListener(this);
        addAndMakeVisible(Gn3ButtonLabel);
        Gn3ButtonLabel.setText ("G", dontSendNotification);
        Gn3ButtonLabel.attachToComponent (&Gn3Button, true);
        Gn3Button.setRadioGroupId(5);
        
        // button to add the "G#" root note
        addAndMakeVisible(Gs3Button);
        Gs3Button.addListener(this);
        addAndMakeVisible(Gs3ButtonLabel);
        Gs3ButtonLabel.setText ("G#", dontSendNotification);
        Gs3ButtonLabel.attachToComponent (&Gs3Button, true);
        Gs3Button.setRadioGroupId(5);
        
        // button to add the "A" root note
        addAndMakeVisible(An4Button);
        An4Button.addListener(this);
        addAndMakeVisible(An4ButtonLabel);
        An4ButtonLabel.setText ("A", dontSendNotification);
        An4ButtonLabel.attachToComponent (&An4Button, true);
        An4Button.setRadioGroupId(5);
        
        // button to add the "A#" root note
        addAndMakeVisible(As4Button);
        As4Button.addListener(this);
        addAndMakeVisible(As4ButtonLabel);
        As4ButtonLabel.setText ("A#", dontSendNotification);
        As4ButtonLabel.attachToComponent (&As4Button, true);
        As4Button.setRadioGroupId(5);
        
        // button to add the "B" root note
        addAndMakeVisible(Bn4Button);
        Bn4Button.addListener(this);
        addAndMakeVisible(Bn4ButtonLabel);
        Bn4ButtonLabel.setText ("B", dontSendNotification);
        Bn4ButtonLabel.attachToComponent (&Bn4Button, true);
        Bn4Button.setRadioGroupId(5);
        
        // button to add the "C+8va" root note
        addAndMakeVisible(Cn4Button);
        Cn4Button.addListener(this);
        addAndMakeVisible(Cn4ButtonLabel);
        Cn4ButtonLabel.setText ("C", dontSendNotification);
        Cn4ButtonLabel.attachToComponent (&Cn4Button, true);
        Cn4Button.setRadioGroupId(5);

        // configuring gain slider and adding it to the main window
        addAndMakeVisible (gainSlider);
        gainSlider.setRange (0.0, 1.0);
        gainSlider.setValue(0.5); // will alsi set the default gain of the sine osc
        gainSlider.addListener (this);
        
        
        // configuring gain label and adding it to the main window
        addAndMakeVisible(gainLabel);
        gainLabel.setText ("Gain", dontSendNotification);
        gainLabel.attachToComponent (&gainSlider, true);
        
        
        // button to play the "major" chord
        addAndMakeVisible(majorChordButton);
        majorChordButton.addListener(this);
        addAndMakeVisible(majorChordButtonLabel);
        majorChordButtonLabel.setText ("Major Triad", dontSendNotification);
        majorChordButtonLabel.attachToComponent (&majorChordButton, true);
        majorChordButton.setRadioGroupId(1);
        
        // button to play the "minor" chord
        addAndMakeVisible(minorChordButton);
        minorChordButton.addListener(this);
        addAndMakeVisible(minorChordButtonLabel);
        minorChordButtonLabel.setText ("Minor Triad", dontSendNotification);
        minorChordButtonLabel.attachToComponent (&minorChordButton, true);
        minorChordButton.setRadioGroupId(1);
        
        setSize (800, 170);
        nChans = 2;
        setAudioChannels (0, nChans); // no inputs, one output
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void resized() override
    {
        // placing the UI elements in the main window
        // getWidth has to be used in case the window is resized by the user
        const int sliderLeft = 30;
        const int sliderLeft2 = 80;
        const int increment = 60;
        
        // Scale Note Button Positions
        Cn3Button.setBounds (sliderLeft, 20, getWidth() - sliderLeft - 20, 20);
        Cs3Button.setBounds (sliderLeft + increment, 20, getWidth() - sliderLeft - 20, 20);
        Dn3Button.setBounds (sliderLeft + 2*increment, 20, getWidth() - sliderLeft - 20, 20);
        Ds3Button.setBounds (sliderLeft + 3*increment, 20, getWidth() - sliderLeft - 20, 20);
        En3Button.setBounds (sliderLeft + 4*increment, 20, getWidth() - sliderLeft - 20, 20);
        Fn3Button.setBounds (sliderLeft + 5*increment, 20, getWidth() - sliderLeft - 20, 20);
        Fs3Button.setBounds (sliderLeft + 6*increment, 20, getWidth() - sliderLeft - 20, 20);
        Gn3Button.setBounds (sliderLeft + 7*increment, 20, getWidth() - sliderLeft - 20, 20);
        Gs3Button.setBounds (sliderLeft + 8*increment, 20, getWidth() - sliderLeft - 20, 20);
        An4Button.setBounds (sliderLeft + 9*increment, 20, getWidth() - sliderLeft - 20, 20);
        As4Button.setBounds (sliderLeft + 10*increment, 20, getWidth() - sliderLeft - 20, 20);
        Bn4Button.setBounds (sliderLeft + 11*increment, 20, getWidth() - sliderLeft - 20, 20);
        Cn4Button.setBounds (sliderLeft + 12*increment, 20, getWidth() - sliderLeft - 20, 20);

        // Gain Slider Position
        gainSlider.setBounds (sliderLeft2, 60, getWidth() - sliderLeft2 - 20, 20);
        // Note Button Positions
        majorChordButton.setBounds (350, 100, getWidth() - sliderLeft2 - 20, 20);
        minorChordButton.setBounds (600, 100, getWidth() - sliderLeft2 - 20, 20);
    }
    
    void sliderValueChanged (Slider* slider) override {
        if (slider == &gainSlider) {
                gain = gainSlider.getValue();
            }
        }
    
    // Changes bool values when buttons are clicked
    void buttonClicked (Button* button) override
    {
        
        // Frequency Buttons:
        if(button == &Cn3Button && Cn3Button.getToggleState()) {
            sineLRoot.setFrequency(Cn3);
            sineRRoot.setFrequency(Cn3);
            sineLMinorThird.setFrequency(Cn3*6/5);
            sineRMinorThird.setFrequency(Cn3*6/5);
            sineLThird.setFrequency(Cn3*5/4);
            sineRThird.setFrequency(Cn3*5/4);
            sineLFifth.setFrequency(Cn3*3/2);
            sineRFifth.setFrequency(Cn3*3/2);
        }
        if(button == &Cs3Button && Cs3Button.getToggleState()) {
            sineLRoot.setFrequency(Cs3);
            sineRRoot.setFrequency(Cs3);
            sineLMinorThird.setFrequency(Cs3*6/5);
            sineRMinorThird.setFrequency(Cs3*6/5);
            sineLThird.setFrequency(Cs3*5/4);
            sineRThird.setFrequency(Cs3*5/4);
            sineLFifth.setFrequency(Cs3*3/2);
            sineRFifth.setFrequency(Cs3*3/2);
        }
        if(button == &Dn3Button && Dn3Button.getToggleState()) {
            sineLRoot.setFrequency(Dn3);
            sineRRoot.setFrequency(Dn3);
            sineLMinorThird.setFrequency(Dn3*6/5);
            sineRMinorThird.setFrequency(Dn3*6/5);
            sineLThird.setFrequency(Dn3*5/4);
            sineRThird.setFrequency(Dn3*5/4);
            sineLFifth.setFrequency(Dn3*3/2);
            sineRFifth.setFrequency(Dn3*3/2);
        }
        if(button == &Ds3Button && Ds3Button.getToggleState()) {
            sineLRoot.setFrequency(Ds3);
            sineRRoot.setFrequency(Ds3);
            sineLMinorThird.setFrequency(Ds3*6/5);
            sineRMinorThird.setFrequency(Ds3*6/5);
            sineLThird.setFrequency(Ds3*5/4);
            sineRThird.setFrequency(Ds3*5/4);
            sineLFifth.setFrequency(Ds3*3/2);
            sineRFifth.setFrequency(Ds3*3/2);
        }
        if(button == &En3Button && En3Button.getToggleState()) {
            sineLRoot.setFrequency(En3);
            sineRRoot.setFrequency(En3);
            sineLMinorThird.setFrequency(En3*6/5);
            sineRMinorThird.setFrequency(En3*6/5);
            sineLThird.setFrequency(En3*5/4);
            sineRThird.setFrequency(En3*5/4);
            sineLFifth.setFrequency(En3*3/2);
            sineRFifth.setFrequency(En3*3/2);
        }
        if(button == &Fn3Button && Fn3Button.getToggleState()) {
            sineLRoot.setFrequency(Fn3);
            sineRRoot.setFrequency(Fn3);
            sineLMinorThird.setFrequency(Fn3*6/5);
            sineRMinorThird.setFrequency(Fn3*6/5);
            sineLThird.setFrequency(Fn3*5/4);
            sineRThird.setFrequency(Fn3*5/4);
            sineLFifth.setFrequency(Fn3*3/2);
            sineRFifth.setFrequency(Fn3*3/2);
        }
        if(button == &Fs3Button && Fs3Button.getToggleState()) {
            sineLRoot.setFrequency(Fs3);
            sineRRoot.setFrequency(Fs3);
            sineLMinorThird.setFrequency(Fs3*6/5);
            sineRMinorThird.setFrequency(Fs3*6/5);
            sineLThird.setFrequency(Fs3*5/4);
            sineRThird.setFrequency(Fs3*5/4);
            sineLFifth.setFrequency(Fs3*3/2);
            sineRFifth.setFrequency(Fs3*3/2);
        }
        if(button == &Gn3Button && Gn3Button.getToggleState()) {
            sineLRoot.setFrequency(Gn3);
            sineRRoot.setFrequency(Gn3);
            sineLMinorThird.setFrequency(Gn3*6/5);
            sineRMinorThird.setFrequency(Gn3*6/5);
            sineLThird.setFrequency(Gn3*5/4);
            sineRThird.setFrequency(Gn3*5/4);
            sineLFifth.setFrequency(Gn3*3/2);
            sineRFifth.setFrequency(Gn3*3/2);
        }
        if(button == &Gs3Button && Gs3Button.getToggleState()) {
            sineLRoot.setFrequency(Gs3);
            sineRRoot.setFrequency(Gs3);
            sineLMinorThird.setFrequency(Gs3*6/5);
            sineRMinorThird.setFrequency(Gs3*6/5);
            sineLThird.setFrequency(Gs3*5/4);
            sineRThird.setFrequency(Gs3*5/4);
            sineLFifth.setFrequency(Gs3*3/2);
            sineRFifth.setFrequency(Gs3*3/2);
        }
        if(button == &An4Button && An4Button.getToggleState()) {
            sineLRoot.setFrequency(An4);
            sineRRoot.setFrequency(An4);
            sineLMinorThird.setFrequency(An4*6/5);
            sineRMinorThird.setFrequency(An4*6/5);
            sineLThird.setFrequency(An4*5/4);
            sineRThird.setFrequency(An4*5/4);
            sineLFifth.setFrequency(An4*3/2);
            sineRFifth.setFrequency(An4*3/2);
        }
        if(button == &As4Button && As4Button.getToggleState()) {
            sineLRoot.setFrequency(As4);
            sineRRoot.setFrequency(As4);
            sineLMinorThird.setFrequency(As4*6/5);
            sineRMinorThird.setFrequency(As4*6/5);
            sineLThird.setFrequency(As4*5/4);
            sineRThird.setFrequency(As4*5/4);
            sineLFifth.setFrequency(As4*3/2);
            sineRFifth.setFrequency(As4*3/2);
        }
        if(button == &Bn4Button && Bn4Button.getToggleState()) {
            sineLRoot.setFrequency(Bn4);
            sineRRoot.setFrequency(Bn4);
            sineLMinorThird.setFrequency(Bn4*6/5);
            sineRMinorThird.setFrequency(Bn4*6/5);
            sineLThird.setFrequency(Bn4*5/4);
            sineRThird.setFrequency(Bn4*5/4);
            sineLFifth.setFrequency(Bn4*3/2);
            sineRFifth.setFrequency(Bn4*3/2);
        }
        if(button == &Cn4Button && Cn4Button.getToggleState()) {
            sineLRoot.setFrequency(Cn4);
            sineRRoot.setFrequency(Cn4);
            sineLMinorThird.setFrequency(Cn4*6/5);
            sineRMinorThird.setFrequency(Cn4*6/5);
            sineLThird.setFrequency(Cn4*5/4);
            sineRThird.setFrequency(Cn4*5/4);
            sineLFifth.setFrequency(Cn4*3/2);
            sineRFifth.setFrequency(Cn4*3/2);
        }
        
        // Triad Buttons:
        if(button == &majorChordButton && majorChordButton.getToggleState()){
            majorChordOnOff = true;
            minorChordOnOff = false;
        }
        if(button == &minorChordButton && minorChordButton.getToggleState()){
            minorChordOnOff = true;
            majorChordOnOff = false;
        }
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        samplingRate = sampleRate;
        sineLRoot.setSamplingRate(sampleRate);
        sineRRoot.setSamplingRate(sampleRate);
        sineLMinorThird.setSamplingRate(sampleRate);
        sineRMinorThird.setSamplingRate(sampleRate);
        sineLThird.setSamplingRate(sampleRate);
        sineRThird.setSamplingRate(sampleRate);
        sineLFifth.setSamplingRate(sampleRate);
        sineRFifth.setSamplingRate(sampleRate);
    }
    
    void releaseResources() override
    {
    }
    
    // This is the main audio process function.  It is continuously called
    // to fill the output buffers.
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override {
        
        // getting the audio output buffer to be filled for Left Channel
        float* const bufferL = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        
        // computing one block for Left Channel
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            if(majorChordOnOff == true) {
                bufferL[sample] = ((sineLRoot.tick() + sineLThird.tick() + sineLFifth.tick()) * gain)/3;
            }
            else if(minorChordOnOff == true) {
                bufferL[sample] = ((sineLRoot.tick() + sineLMinorThird.tick() + sineLFifth.tick()) * gain)/3;
            }
            else bufferL[sample] = 0.0;
        }
        
        // getting the audio output buffer to be filled for Right Channel
        float* const bufferR = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);
        
        // computing one block for Right Channel
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            if(majorChordOnOff == true) {
                bufferR[sample] = ((sineRRoot.tick() + sineRThird.tick() + sineRFifth.tick()) * gain)/3;
            }
            else if(minorChordOnOff == true) {
                bufferR[sample] = ((sineRRoot.tick() + sineRMinorThird.tick() + sineRFifth.tick()) * gain)/3;
            }
            else bufferR[sample] = 0.0;
        }
    }
    
    
private:
    
    // General UI Elements
    Slider gainSlider;
    
    // Chord Buttons
    ToggleButton minorChordButton;
    ToggleButton majorChordButton;
    
    // Frequency Buttons
    ToggleButton Cn3Button;
    ToggleButton Cs3Button;
    ToggleButton Dn3Button;
    ToggleButton Ds3Button;
    ToggleButton En3Button;
    ToggleButton Fn3Button;
    ToggleButton Fs3Button;
    ToggleButton Gn3Button;
    ToggleButton Gs3Button;
    ToggleButton An4Button;
    ToggleButton As4Button;
    ToggleButton Bn4Button;
    ToggleButton Cn4Button;
    
    // Labels
    Label gainLabel;
    Label minorChordButtonLabel;
    Label majorChordButtonLabel;
    Label Cn3ButtonLabel;
    Label Cs3ButtonLabel;
    Label Dn3ButtonLabel;
    Label Ds3ButtonLabel;
    Label En3ButtonLabel;
    Label Fn3ButtonLabel;
    Label Fs3ButtonLabel;
    Label Gn3ButtonLabel;
    Label Gs3ButtonLabel;
    Label An4ButtonLabel;
    Label As4ButtonLabel;
    Label Bn4ButtonLabel;
    Label Cn4ButtonLabel;
    
    // Since oscilators
    Sine sineLRoot;
    Sine sineRRoot;
    Sine sineLMinorThird;
    Sine sineRMinorThird;
    Sine sineLThird;
    Sine sineRThird;
    Sine sineLFifth;
    Sine sineRFifth;
    
    // Global Variables
    float gain;
    int samplingRate, nChans;
    bool majorChordOnOff = false;
    bool minorChordOnOff = false;
    
    // Frequencies
    double Cn3 = 261.63;
    double Cs3 = 277.18;
    double Dn3 = 293.66;
    double Ds3 = 311.13;
    double En3 = 329.63;
    double Fn3 = 349.23;
    double Fs3 = 369.99;
    double Gn3 = 392.00;
    double Gs3 = 415.30;
    double An4 = 440.00;
    double As4 = 466.16;
    double Bn4 = 493.88;
    double Cn4 = 523.25;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
