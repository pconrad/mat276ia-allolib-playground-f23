#include <cstdio> // for printing to stdout

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

using std::cout;
using std::endl;

// using namespace gam;
using namespace al;

// This example shows how to use SynthVoice and SynthManagerto create an audio
// visual synthesizer. In a class that inherits from SynthVoice you will
// define the synth's voice parameters and the sound and graphic generation
// processes in the onProcess() functions.

class SineWav : public SynthVoice
{
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc;
  gam::Env<4> mAmpEnv;

  // Initialize voice. This function will only be called once per voice when
  // it is created. Voices will be reused if they are idle.
  void init() override
  {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0.75, 0);
    mAmpEnv.sustainPoint(3); // Make point 3 sustain until a release is issued

    createInternalTriggerParameter("frequency", 440, 20, 5000);
    createInternalTriggerParameter("amplitude", 0.8, 0.0, 1.0);
    createInternalTriggerParameter("attackTime", 0.05, 0.01, 3.0);
    createInternalTriggerParameter("decayTime", 0.05, 0.01, 3.0);
    createInternalTriggerParameter("releaseTime", 0.5, 0.1, 10.0);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
    createInternalTriggerParameter("startPitch", 220.0, 10.0, 4000.0);
  }

  // The audio processing function
  void onProcess(AudioIOData &io) override
  {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    float f = getInternalParameterValue("frequency");
    mOsc.freq(f);

    float a = getInternalParameterValue("amplitude");
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[1] = getInternalParameterValue("decayTime");

    mAmpEnv.lengths()[3] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io())
    {
      float s1 = mAmpEnv() * (mOsc() * a);
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (mAmpEnv.done())
      free();
  }

  // The triggering functions just need to tell the envelope to start or release
  // The audio processing function checks when the envelope is done to remove
  // the voice from the processing chain.
  void onTriggerOn() override { mAmpEnv.reset(); }
  void onTriggerOff() override { mAmpEnv.release(); }
};

// We make an app.
class MyApp : public App
{
public:
  // GUI manager for SquareWave voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<SineWav> synthManager{"SineWav"};

  // This function is called right after the window is created
  // It provides a grphics context to initialize ParameterGUI
  // It's also a good place to put things that should
  // happen once at startup.
  void onCreate() override
  {
    navControl().active(false); // Disable navigation via keyboard, since we
                                // will be using keyboard for note triggering

    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());

    imguiInit();

    // Play example sequence. Comment this line to start from scratch
    // synthManager.synthSequencer().playSequence("synth1.synthSequence");
    synthManager.synthRecorder().verbose(true);
  }

  // The audio callback function. Called when audio hardware requires data
  void onSound(AudioIOData &io) override
  {
    synthManager.render(io); // Render audio
  }

  void onAnimate(double dt) override
  {
    // The GUI is prepared here
    imguiBeginFrame();
    // Draw a window that contains the synth control panel
    synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  // The graphics callback function.
  void onDraw(Graphics &g) override
  {
    g.clear();
    // Render the synth's graphics
    synthManager.render(g);

    // GUI is drawn here
    imguiDraw();
  }

  // Whenever a key is pressed, this function is called
  bool onKeyDown(Keyboard const &k) override
  {
    if (ParameterGUI::usingKeyboard())
    { // Ignore keys if GUI is using
      // keyboard
      return true;
    }

    switch (k.key())
    {
    case 'a':
      std::cout << "a pressed!" << std::endl;
      playSequenceA();
      return false;
    case 'b':
      std::cout << "b pressed!" << std::endl;
      playSequenceB();
      return false;
    }

    return true;
  }

  // Whenever a key is released this function is called
  bool
  onKeyUp(Keyboard const &k) override
  {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0)
    {
      synthManager.triggerOff(midiNote);
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }

  // New code: a function to play a note A

  void playNote(float freq, float time, float duration = 0.5, float amp = 0.2, float attack = 0.002, float decay = 0.002, float release = 1.0)
  {
    auto *voice = synthManager.synth().getVoice<SineWav>();
    // amp, freq, attack, release, pan
    std::vector<float> triggerParams = {freq, amp, 0.1, 0.1, 0.0};
    // voice->setTriggerParams(triggerParams);
    voice->setInternalParameterValue("frequency", freq);
    voice->setInternalParameterValue("amplitude", amp);
    // voice->setInternalParameterValue("attackTime", attack);
    // voice->setInternalParameterValue("decayTime", decay);
    // voice->setInternalParameterValue("releaseTime", release);

    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  void playSequenceA()
  {
    const float fundamental = 220;
    for (int i = 1; i <= 10; i++)
    {
      float pitch = fundamental * i;
      playNote(pitch, i / 2.0, 2.0, 0.02);
    }
  }

  void playSequenceB(float beat = 1.0)
  {

    auto *voice = synthManager.synth().getVoice<SineWav>();
    float startPitch = voice->getInternalParameterValue("startPitch");
    cout << "startPitch = " << startPitch << endl; 

    cout << "Playing sequence B" << endl;
    const double halfStep = 1.059463094359; // 2^(1/12)
    const double wholeStep = halfStep * halfStep;

    float attack = 0.002;
    float decay = 0.002;
    float release = 0.25;

    playNote(startPitch, 0, 2.0, 0.3, attack, decay, release * 0.1);
    playNote(startPitch * wholeStep, beat * 1.0, 2.0, 0.3, attack, decay, release*0.2);
    playNote(startPitch / halfStep, beat * 1.5, 2.0, 0.3, attack, decay, release*0.2);
    playNote(startPitch, beat * 2.0, 2.0, 0.3, attack, decay, release*0.1);
  }
};

int main()
{
  // Create app instance
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();

  return 0;
}
