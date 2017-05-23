// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Starter code by: Romain Michon
// Updated by: Ron Guglielmone
// Description: A MIDI controlled polyphonic synth
//              built using the JUCE framework.

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "Sine.h"
#include "Smooth.h"
#include "FaustReverb.h"
#include "TitleComponent.h"


// Taboo global variables:
float gain;


// This needs to be here for...  Reasons...
struct FMSound : public SynthesiserSound {
    FMSound() {}
    bool appliesToNote (int /*midiNoteNumber*/) override        { return true; }
    bool appliesToChannel (int /*midiChannel*/) override        { return true; }
};

// FM synth voice:
struct FMVoice : public SynthesiserVoice {
    FMVoice():
    carrierFrequency(440.0),
    index(0.0),
    level(0.0),
    envelope(0.0),
    onOff (false),
    tailOff(true)
    {
        carrier.setSamplingRate(getSampleRate());
        modulator.setSamplingRate(getSampleRate());
    };
    
    bool canPlaySound (SynthesiserSound* sound) override {
        return dynamic_cast<FMSound*> (sound) != nullptr;
    }
    
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override {
        // converting MIDI note number into freq
        carrierFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        
        // we don't want an ugly sweep when the note begins...
        smooth[0].setSmooth(0);
        smooth[0].tick(carrierFrequency);
        
        // standard smoothing...
        for(int i=0; i<2; i++){
            smooth[i].setSmooth(0.999);
        }
        
        level = velocity;
        // level = pow(velocity,2); // if we want linear dynamic
        
        // tells the note to begin!
        onOff = true;
        
        // These parameters could be controlled with UI elements and could
        // be assigned to specific MIDI controllers. If you do so,
        // don't forget to smooth them!
        modulator.setFrequency(1000.0);
        index = 150;
    }
    
    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        onOff = false; // end the note
        level = 0; // ramp envelope to 0 if tail off is allowed
        
        tailOff = allowTailOff;
    }
    
    void pitchWheelMoved (int /*newValue*/) override
    {
        // Pitch wheel is an important standard MIDI feature that should be implemented
    }
    
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
        // Thats where you would change the value of the modulator index and frequency
        // if you wanted to control them with MIDI controllers
    }
    
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override {
        // only compute block if note is on!
        if(envelope != 0 || onOff){
            while (--numSamples >= 0){
                envelope = smooth[1].tick(level); // here we use a smoother as an envelope generator
                carrier.setFrequency(smooth[0].tick(carrierFrequency) + modulator.tick()*index);
                const float currentSample = (float) carrier.tick() * envelope * gain;
                for (int i = outputBuffer.getNumChannels(); --i >= 0;){
                    outputBuffer.addSample (i, startSample, currentSample);
                }
                ++startSample;
                
                // if tail off is disabled, we end the note right away, otherwise, we wait for envelope
                // to reach a safe value
                if(!onOff && (envelope < 0.001 || !tailOff)){
                    envelope = 0;
                    clearCurrentNote();
                }
            }
        }
    }
    
private:
    Sine carrier, modulator;
    Smooth smooth[2];
    double carrierFrequency, index, level, envelope;
    bool onOff, tailOff;
};

struct SynthAudioSource : public AudioSource {
    SynthAudioSource (MidiKeyboardState& keyState) : keyboardState (keyState) {
        // Add some voices to our synth, to play the sounds..
        int nVoices = 4;
        for (int i = nVoices; --i >= 0;)
        {
            synth.addVoice (new FMVoice());
        }
        
        synth.clearSounds();
        synth.addSound (new FMSound());
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        midiCollector.reset (sampleRate);
        synth.setCurrentPlaybackSampleRate (sampleRate);
    }
    
    void releaseResources() override
    {
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // the synth always adds its output to the audio buffer, so we have to clear it
        // first..
        bufferToFill.clearActiveBufferRegion();
        
        // fill a midi buffer with incoming messages from the midi input.
        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);
        //midiCollector.
        
        // pass these messages to the keyboard state so that it can update the component
        // to show on-screen which keys are being pressed on the physical midi keyboard.
        // This call will also add midi messages to the buffer which were generated by
        // the mouse-clicking on the on-screen keyboard.
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
        
        // and now get the synth to process the midi events and generate its output.
        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    }
    
    // this collects real-time midi messages from the midi input device, and
    // turns them into blocks that we can process in our audio callback
    MidiMessageCollector midiCollector;
    
    // this represents the state of which keys on our on-screen keyboard are held
    // down. When the mouse is clicked on the keyboard component, this object also
    // generates midi messages for this, which we can pass on to our synth.
    MidiKeyboardState& keyboardState;
    
    // the synth itself!
    Synthesiser synth;
};

class MainContentComponent :
public Component,
public Colour,
private ComboBox::Listener,
public Button::Listener,
public Slider::Listener,
private MidiInputCallback {
    
public:
    MainContentComponent() :
    keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
    synthAudioSource (keyboardState),
    lastMidiInputIndex (0) {
        
        audioDeviceManager.initialise (0, 2, nullptr, true, String(), nullptr);
        
        audioSourcePlayer.setSource (&synthAudioSource);
        
        audioDeviceManager.addAudioCallback (&audioSourcePlayer);
        audioDeviceManager.addMidiInputCallback (String(), &(synthAudioSource.midiCollector));
        
        // MIDI Inputs
        addAndMakeVisible (midiInputListLabel);
        midiInputListLabel.setText ("MIDI Input:", dontSendNotification);
        midiInputListLabel.attachToComponent (&midiInputList, true);
        
        // menu listing all the available MIDI inputs
        addAndMakeVisible (midiInputList);
        midiInputList.setTextWhenNoChoicesAvailable ("Plug In Your MIDI Keyboard To Rock");
        const StringArray midiInputs (MidiInput::getDevices());
        midiInputList.addItemList (midiInputs, 1);
        midiInputList.setSelectedId(0); // default
        midiInputList.addListener (this);
        // Make it pretty:
        midiInputList.setColour(0x1000b00, Colour(25, 25, 25)); //BG
        midiInputList.setColour(0x1000a00, juce::Colours::lightgrey); //Text
        midiInputList.setColour(0x1000c00, Colour(50, 50, 50)); //Border
        midiInputList.setColour(0x1000d00, Colour(50, 50, 50)); //Border
        
        
        // Set default MIDI input:
        setMidiInput(0);
        
        // The MIDI keyboard:
        addAndMakeVisible(keyboardComponent);
        
        // Main window size:
        setSize (600, 300);
        
        // GUI Elements:
        
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
        
        
        // Gain slider:
        gainSlider.setSliderStyle (Slider::Rotary);
        gainSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
        addAndMakeVisible (gainSlider);
        gainSlider.setRange (0.0, 1.0);
        gainSlider.setValue(0.5);
        gainSlider.addListener (this);
        gainSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(109, 114, 120));
        gainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::lightgrey);
        gainSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
        gainSlider.setColour(juce::Slider::rotarySliderFillColourId , juce::Colour(54, 64, 74));
        
        // Label for gain slider
        addAndMakeVisible(gainLabel);
        gainLabel.setText ("Gain", dontSendNotification);
        gainLabel.setJustificationType( Justification::centredBottom);
        gainLabel.attachToComponent (&gainSlider, false);
        gainLabel.setColour(0x1000281, juce::Colours::lightgrey);
        
        // Set background and buttons:
        backgroundColor = Colour(36, 37, 40);
        buttonOneColor = Colour(51,71,125);
        buttonTwoColor = Colour(75,95,155);
        buttonThreeColor = Colour(66,80,133);
        buttonFourColor = Colour(57,73,142);
        buttonFiveColor = Colour(55,72,137);
        buttonSixColor = Colour(61,77,139);
        
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
        addAndMakeVisible (colorFour);
        colorFour.setColour(0x1000100, buttonFourColor);
        colorFour.addListener(this);
        addAndMakeVisible (colorFive);
        colorFive.setColour(0x1000100, buttonFiveColor);
        colorFive.addListener(this);
        addAndMakeVisible (colorSix);
        colorSix.setColour(0x1000100, buttonSixColor);
        colorSix.addListener(this);
        
        // Color Title:
        addAndMakeVisible(colorTitle);

    }
    
    
    // Destructor for MainContentComponent
    ~MainContentComponent() {
        audioSourcePlayer.setSource (nullptr);
        audioDeviceManager.removeMidiInputCallback (String(), &(synthAudioSource.midiCollector));
        audioDeviceManager.removeAudioCallback (&audioSourcePlayer);
        audioDeviceManager.removeMidiInputCallback (MidiInput::getDevices()[midiInputList.getSelectedItemIndex()], this);
        midiInputList.removeListener (this);
    }
    
    
    // Colors the background
    void paint (Graphics& g) override
    {
        g.fillAll(backgroundColor);
    }
    
    
    // Gets called when the window is resized
    // or when it is first loaded.  We set all
    // GUI locations here.
    void resized() override {
        const int labelWidth = 0; // Hacky way to get rid of it
        midiInputList.setBounds (labelWidth, 0, getWidth()-labelWidth, 20);
        keyboardComponent.setBounds (0, 226, getWidth(), 64);
        
        
        // Box containing the rotaries:
        const int border = 10;
        Rectangle<int> area = getLocalBounds(); {
            Rectangle<int> dialArea(0,50,400,150);
            const int third = (dialArea.getWidth() / 3);
            dial1.setBounds (dialArea.removeFromLeft(third).reduced(border));
            gainSlider.setBounds (dialArea.removeFromRight (third).reduced(border));
            dial2.setBounds (dialArea.reduced(border));
        }
        
        // Box containing the bottom tone pads:
        Rectangle<int> area2 = getLocalBounds(); {
            const int border2 = 5;
            Rectangle<int> toneArea(400,135,185,55);
            const int third = (toneArea.getWidth() / 3);
            colorOne.setBounds (toneArea.removeFromLeft(third).reduced(border2));;
            colorTwo.setBounds (toneArea.removeFromRight (third).reduced(border2));
            colorThree.setBounds (toneArea.reduced(border2));
        }
        
        // Box containing the top tone pads:
        Rectangle<int> area3 = getLocalBounds(); {
            const int border3 = 5;
            Rectangle<int> toneArea2(400,80,185,55);
            const int third = (toneArea2.getWidth() / 3);
            colorFour.setBounds (toneArea2.removeFromLeft(third).reduced(border3));;
            colorFive.setBounds (toneArea2.removeFromRight (third).reduced(border3));
            colorSix.setBounds (toneArea2.reduced(border3));
        }
        
        // Box containing the tone title:
        Rectangle<int> area4 = getLocalBounds(); {
            Rectangle<int> titleArea(418,43,200,55);
            colorTitle.setBounds(titleArea);
        }
        
    }
    
    
    // Gets called when the midi input list
    // is changed.  "ComboBox" is the drop down.
    void comboBoxChanged (ComboBox* box) override {
        if (box == &midiInputList) setMidiInput  (midiInputList.getSelectedItemIndex());
    }
    
    
    // Called when sliders are moved:
    void sliderValueChanged (Slider* slider) override
    {
        if (true){ //carrier.getSamplingRate() > 0.0
            
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
                //vibeSine.setFrequency(vibeSineFreq);
            }
        }
    }
    
    
    // Called when buttons are clicked.
    void buttonClicked (Button* button) override {
        
        if(button == &colorOne) {
            colorOneSelected = true;
            colorTwoSelected = false;
            colorThreeSelected = false;
            colorFourSelected = false;
            colorFiveSelected = false;
            colorSixSelected = false;
        }
        if(button == &colorTwo) {
            colorOneSelected = false;
            colorTwoSelected = true;
            colorThreeSelected = false;
            colorFourSelected = false;
            colorFiveSelected = false;
            colorSixSelected = false;
        }
        if(button == &colorThree) {
            colorOneSelected = false;
            colorTwoSelected = false;
            colorThreeSelected = true;
            colorFourSelected = false;
            colorFiveSelected = false;
            colorSixSelected = false;
        }
        if(button == &colorFour) {
            colorOneSelected = false;
            colorTwoSelected = false;
            colorThreeSelected = false;
            colorFourSelected = true;
            colorFiveSelected = false;
            colorSixSelected = false;
        }
        if(button == &colorFive) {
            colorOneSelected = false;
            colorTwoSelected = false;
            colorThreeSelected = false;
            colorFourSelected = false;
            colorFiveSelected = true;
            colorSixSelected = false;
        }
        if(button == &colorSix) {
            colorOneSelected = false;
            colorTwoSelected = false;
            colorThreeSelected = false;
            colorFourSelected = false;
            colorFiveSelected = false;
            colorSixSelected = true;
        }
        
    }

    
private:
    // Confusing MIDI Stuff:
    AudioDeviceManager audioDeviceManager;
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent keyboardComponent;
    AudioSourcePlayer audioSourcePlayer;
    SynthAudioSource synthAudioSource;
    ComboBox midiInputList;
    Label midiInputListLabel;
    int lastMidiInputIndex;
    
    // GUI Stuff:
    
    // Rotary controls:
    Slider dial1;
    Slider dial2;
    Slider gainSlider;
    
    // Buttons for the harmonics:
    TextButton colorOne;
    TextButton colorTwo;
    TextButton colorThree;
    TextButton colorFour;
    TextButton colorFive;
    TextButton colorSix;
    
    // Labels:
    Label tremeloSpeedLabel;
    Label tremeloDepthLabel;
    Label gainLabel;
    TitleComponent colorTitle;

    // Colors:
    Colour backgroundColor;
    Colour buttonOneColor;
    Colour buttonTwoColor;
    Colour buttonThreeColor;
    Colour buttonFourColor;
    Colour buttonFiveColor;
    Colour buttonSixColor;
    
    // Bottom UI Elements:
    ToggleButton onOffButton;
    Label onOffLabel;
    
    // Effects:
    FaustReverb reverb;
    MapUI reverbControl;
    
    // Globals:
    float** audioBuffer;
    double carrierFrequency;
    double modulatorFrequency;
    double index;
    double vibeSineFreq;
    int samplingRate;
    int nChans;
    
    // Color Button Trackers:
    bool colorOneSelected = false;
    bool colorTwoSelected = true;
    bool colorThreeSelected = false;
    bool colorFourSelected = false;
    bool colorFiveSelected = false;
    bool colorSixSelected = false;
    
    
    // Handles MIDI inputs in some mysterious
    // black magic way.
    void setMidiInput (int index) {
        const StringArray list (MidiInput::getDevices());
        audioDeviceManager.removeMidiInputCallback (list[lastMidiInputIndex], this);
        const String newInput (list[index]);
        if (! audioDeviceManager.isMidiInputEnabled (newInput))
            audioDeviceManager.setMidiInputEnabled (newInput, true);
        audioDeviceManager.addMidiInputCallback (newInput, this);
        midiInputList.setSelectedId (index + 1, dontSendNotification);
        lastMidiInputIndex = index;
    }
    
    // Needed virtual function
    void handleIncomingMidiMessage (MidiInput*, const MidiMessage& message) override {}
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED
