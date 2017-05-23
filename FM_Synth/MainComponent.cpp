// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Starter code provided by JUCE
// Original modifications by: Romain Michon
// Additional modifications by: Ron Guglielmone
//
// Description: Simple JUCE FM synthesizer with
// depth and rate parameters to control modulations.
// "Color" presets determine the carrier frequency.
//
// This version does not support the "speed" controller.
// Nor does it support key-presses.  Both of these are
// yet to be added.


#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"
#include "Smooth.h"
#include "FaustReverb.h"

class MainContentComponent :
public AudioAppComponent,
public Colour,
private Slider::Listener,
private Button::Listener {
    
public:
    MainContentComponent() : carrierFrequency(440.0), index(0.0), gain (0.0), samplingRate(0.0) {
        
        // Rotary controller for "depth" param:
        dial1.setSliderStyle (Slider::Rotary);
        dial1.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
        addAndMakeVisible (dial1);
        dial1.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(109, 114, 120));
        dial1.setColour(juce::Slider::textBoxTextColourId, juce::Colours::lightgrey);
        dial1.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
        dial1.setColour(juce::Slider::rotarySliderFillColourId , juce::Colour(54, 64, 74));
        dial1.setRange (0.0, 1000.0);
        dial1.addListener(this);
        
        // Label for "depth" rotary:
        addAndMakeVisible(tremeloDepthLabel);
        tremeloDepthLabel.setText ("Depth", dontSendNotification);
        tremeloDepthLabel.attachToComponent (&dial1, false);
        tremeloDepthLabel.setJustificationType( Justification::centredBottom);
        tremeloDepthLabel.setColour(0x1000281, juce::Colours::lightgrey);
        
        
        // Rotary controller for "speed" param:
        dial2.setSliderStyle (Slider::Rotary);
        dial2.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
        addAndMakeVisible (dial2);
        dial2.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(109, 114, 120));
        dial2.setColour(juce::Slider::textBoxTextColourId, juce::Colours::lightgrey);
        dial2.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
        dial2.setColour(juce::Slider::rotarySliderFillColourId , juce::Colour(54, 64, 74));
        dial2.setRange(1,50);
        dial2.addListener(this);
        
        // Label for "speed" rotary:
        addAndMakeVisible(tremeloSpeedLabel);
        tremeloSpeedLabel.setText ("Speed", dontSendNotification);
        tremeloSpeedLabel.setJustificationType( Justification::centredBottom);
        tremeloSpeedLabel.attachToComponent (&dial2, false);
        tremeloSpeedLabel.setColour(0x1000281, juce::Colours::lightgrey);
        
        // White piano key buttons
        button1.setButtonText ("");
        addAndMakeVisible (button1);
        button1.addListener(this);
        button3.setButtonText ("");
        addAndMakeVisible (button3);
        button3.addListener(this);
        button5.setButtonText ("");
        addAndMakeVisible (button5);
        button5.addListener(this);
        button6.setButtonText ("");
        addAndMakeVisible (button6);
        button6.addListener(this);
        button8.setButtonText ("");
        addAndMakeVisible (button8);
        button8.addListener(this);
        button10.setButtonText ("");
        addAndMakeVisible (button10);
        button10.addListener(this);
        button12.setButtonText ("");
        addAndMakeVisible (button12);
        button12.addListener(this);
        button13.setButtonText ("");
        addAndMakeVisible (button13);
        button13.addListener(this);
        // Black piano key buttons
        button2.setButtonText ("");
        addAndMakeVisible (button2);
        button2.setColour(0x1000100, juce::Colours::black);
        button2.addListener(this);
        button4.setButtonText ("");
        addAndMakeVisible (button4);
        button4.setColour(0x1000100, juce::Colours::black);
        button4.addListener(this);
        button7.setButtonText ("");
        addAndMakeVisible (button7);
        button7.setColour(0x1000100, juce::Colours::black);
        button7.addListener(this);
        button9.setButtonText ("");
        addAndMakeVisible (button9);
        button9.setColour(0x1000100, juce::Colours::black);
        button9.addListener(this);
        button11.setButtonText ("");
        addAndMakeVisible (button11);
        button11.setColour(0x1000100, juce::Colours::black);
        button11.addListener(this);
        
        // Gain slider:
        addAndMakeVisible (gainSlider);
        gainSlider.setRange (0.0, 1.0);
        gainSlider.setValue(0.5);
        gainSlider.addListener (this);
        gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition(juce::Slider::NoTextBox), false, 0, 0);
        
        // Label for gain slider
        addAndMakeVisible(gainLabel);
        gainLabel.setText ("Gain", dontSendNotification);
        gainLabel.setJustificationType( Justification::bottom);
        gainLabel.attachToComponent (&gainSlider, false);
        gainLabel.setColour(0x1000281, juce::Colours::lightgrey);
        
        // Set background and buttons:
        backgroundColor = Colour(36, 37, 40);
        buttonOneColor = Colour(51,71,125);
        buttonTwoColor = Colour(75,95,155);
        buttonThreeColor = Colour(102,71,133);
        
        // Color buttons:
        addAndMakeVisible (colorOne);
        colorOne.setColour(0x1000100, buttonOneColor);
                colorOne.addListener(this);
        addAndMakeVisible (colorTwo);
        colorTwo.setColour(0x1000100, buttonTwoColor);
                colorTwo.addListener(this);
        addAndMakeVisible (colorThree);
        colorThree.setColour(0x1000100, buttonThreeColor);
                colorThree.addListener(this);
        
        // Default size for full window:
        setSize (260, 400);
        
        // Initialize main program params:
        nChans = 2;
        setAudioChannels (0, nChans);
        audioBuffer = new float*[nChans];
    }
    
    
    // Colors the background
    void paint (Graphics& g) override
    {
        g.fillAll(backgroundColor);
    }
    
    
    // Destructor:
    ~MainContentComponent()
    {
        shutdownAudio();
        delete [] audioBuffer;
    }
    
    
    // Gets called when the window resizes or loads:
    void resized() override
    {
        
        const int border = 20;
        Rectangle<int> area = getLocalBounds();
        
        {
            // Box containing the rotaries:
            Rectangle<int> dialArea(0,0,260,200);
            dial1.setBounds (dialArea.removeFromLeft (dialArea.getWidth() / 2).reduced (border));
            dial2.setBounds (dialArea.reduced (border));
        }
        
        // Piano key pixel constants:
        const int whiteKeyHeight = 100;
        const int blackKeyHeight = 60;
        const int yPositionKeys = 200;
        
        // Piano key locations:
        button1.setBounds (10,yPositionKeys,30,whiteKeyHeight);
        button2.setBounds (25,yPositionKeys,30,blackKeyHeight);
        button3.setBounds (40,yPositionKeys,30,whiteKeyHeight);
        button4.setBounds (55,yPositionKeys,30,blackKeyHeight);
        button5.setBounds (70,yPositionKeys,30,whiteKeyHeight);
        button6.setBounds (100,yPositionKeys,30,whiteKeyHeight);
        button7.setBounds (115,yPositionKeys,30,blackKeyHeight);
        button8.setBounds (130,yPositionKeys,30,whiteKeyHeight);
        button9.setBounds (145,yPositionKeys,30,blackKeyHeight);
        button10.setBounds (160,yPositionKeys,30,whiteKeyHeight);
        button11.setBounds (175,yPositionKeys,30,blackKeyHeight);
        button12.setBounds (190,yPositionKeys,30,whiteKeyHeight);
        button13.setBounds (220,yPositionKeys,30,whiteKeyHeight);
        
        // Gain slider:
        gainSlider.setBounds (10, 360, 245, 20);
        
        // Color buttons:
        colorOne.setBounds (65,yPositionKeys + 115,30,30);
        colorTwo.setBounds (130,yPositionKeys + 115,30,30);
        colorThree.setBounds (195,yPositionKeys + 115,30,30);
        
    }
    
    
    // Handles UI Interactions:
    void sliderValueChanged (Slider* slider) override
    {
        if (carrier.getSamplingRate() > 0.0){
            
            if (slider == &gainSlider){
                gain = gainSlider.getValue();
            }
            
            
            //if (slider == &carrierSlider)
            //{
            //    carrierFrequency = carrierSlider.getValue();
            //}
            //else if (slider == &modulatorSlider)
            //{
            //    modulator.setFrequency(modulatorSlider.getValue());
            //}
            else if (slider == &dial1) {
                index = dial1.getValue();
            }
            else if (slider == &dial2) {
                vibeSineFreq = dial2.getValue();
                vibeSine.setFrequency(vibeSineFreq);
            }
        }
    }
    
    // Handles UI Interactions:
    void buttonClicked (Button* button) override {
        // Frequency Buttons:
        if(button == &button1) {
            carrierFrequency = Cn3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button2) {
            carrierFrequency = Cs3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button3) {
            carrierFrequency = Dn3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button4) {
            carrierFrequency = Ds3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button5) {
            carrierFrequency = En3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button6) {
            carrierFrequency = Fn3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button7) {
            carrierFrequency = Fs3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button8) {
            carrierFrequency = Gn3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button9) {
            carrierFrequency = Gs3;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button10) {
            carrierFrequency = An4;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button11) {
            carrierFrequency = As4;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button12) {
            carrierFrequency = Bn4;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &button13) {
            carrierFrequency = Cn4;
            if(colorOneSelected) {
                modulatorFrequency = carrierFrequency*(0.999/2);
            }
            else if(colorTwoSelected) {
                modulatorFrequency = carrierFrequency*(0.999);
            }
            else if(colorThreeSelected) {
                modulatorFrequency = carrierFrequency*(1.999);
            }
            else {
                modulatorFrequency = 50;
            }
        }
        if(button == &colorOne) {
            colorOneSelected = true;
            colorTwoSelected = false;
            colorThreeSelected = false;
        }
        if(button == &colorTwo) {
            colorOneSelected = false;
            colorTwoSelected = true;
            colorThreeSelected = false;
        }
        if(button == &colorThree) {
            colorOneSelected = false;
            colorTwoSelected = false;
            colorThreeSelected = true;
        }
        
    }
    
    // Initializes audio functions:
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        carrier.setSamplingRate(sampleRate);
        carrier.setFrequency(carrierFrequency);
        modulator.setSamplingRate(sampleRate);
        modulator.setFrequency(modulatorFrequency);
        for(int i=0; i<4; i++){
            smooth[i].setSmooth(0.999);
        }
        
        reverb.init(sampleRate);
        reverb.buildUserInterface(&reverbControl);
    }
    
    // Destructor:
    void releaseResources() override
    {
    }
    
    // Main audio function:
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        const float level = gainSlider.getValue();
        
        for(int i=0; i<nChans; i++){
            audioBuffer[i] = bufferToFill.buffer->getWritePointer (i, bufferToFill.startSample);
        }
        
        // computing one block
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            modulator.setFrequency(modulatorFrequency);
            carrier.setFrequency(smooth[0].tick(carrierFrequency) + modulator.tick()*smooth[3].tick(index)); // modulating freq of the carrier
            audioBuffer[0][sample] = carrier.tick() * smooth[1].tick(level);
        }
        reverb.compute(bufferToFill.numSamples,audioBuffer,audioBuffer);
    }
    
    
private:
    
    // Rotary controls:
    Slider dial1;
    Slider dial2;
    
    // Buttons for the notes:
    TextButton button1;
    TextButton button2;
    TextButton button3;
    TextButton button4;
    TextButton button5;
    TextButton button6;
    TextButton button7;
    TextButton button8;
    TextButton button9;
    TextButton button10;
    TextButton button11;
    TextButton button12;
    TextButton button13;
    
    // Buttons for the harmonics:
    TextButton colorOne;
    TextButton colorTwo;
    TextButton colorThree;
    
    // Labels:
    Label tremeloSpeedLabel;
    Label tremeloDepthLabel;
    
    Colour backgroundColor;
    Colour buttonOneColor;
    Colour buttonTwoColor;
    Colour buttonThreeColor;
    
    // Bottom UI Elements:
    Slider gainSlider;
    ToggleButton onOffButton;
    Label gainLabel, onOffLabel;
    
    // Effects:
    FaustReverb reverb;
    MapUI reverbControl;
    
    // Oscillators, etc:
    Sine carrier;
    Sine modulator;
    Sine vibeSine;
    Smooth smooth[4];
    
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
    
    // Globals:
    float** audioBuffer;
    double carrierFrequency;
    double modulatorFrequency;
    double index;
    double vibeSineFreq;
    float gain;
    int samplingRate;
    int nChans;
    
    // Color Button Trackers:
    bool colorOneSelected = false;
    bool colorTwoSelected = true;
    bool colorThreeSelected = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }

#endif
