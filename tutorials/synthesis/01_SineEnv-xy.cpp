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

#include "al/graphics/al_Font.hpp" // for text rendering


// using namespace gam;
using namespace al;

// This example shows how to use SynthVoice and SynthManagerto create an audio
// visual synthesizer. In a class that inherits from SynthVoice you will
// define the synth's voice parameters and the sound and graphic generation
// processes in the onProcess() functions.

class SineEnv : public SynthVoice {
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc;
  gam::Env<3> mAmpEnv;
  // envelope follower to connect audio output to graphics
  gam::EnvFollow<> mEnvFollow;

  // Additional members
  Mesh mMesh;

  // Initialize voice. This function will only be called once per voice when
  // it is created. Voices will be reused if they are idle.
  void init() override {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued

    // We have the mesh be a sphere
    int r = 1; // radius
    int n = 30; // number of slices; e.g. 5 gives a pentagon, 30 is pretty smooth
    addDisc(mMesh, r, n);

    // This is a quick way to create parameters for the voice. Trigger
    // parameters are meant to be set only when the voice starts, i.e. they
    // are expected to be constant within a voice instance. (You can actually
    // change them while you are prototyping, but their changes will only be
    // stored and aplied when a note is triggered.)

    createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
    createInternalTriggerParameter("attackTime", 1.0, 0.01, 3.0);
    createInternalTriggerParameter("releaseTime", 3.0, 0.1, 10.0);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
    createInternalTriggerParameter("x", 0.0, -100, 100.0);
    createInternalTriggerParameter("y", 0.0, -100.0, 100.0);

  }

  // The audio processing function
  void onProcess(AudioIOData &io) override {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    mOsc.freq(getInternalParameterValue("frequency"));
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io()) {
      float s1 = mOsc() * mAmpEnv() * getInternalParameterValue("amplitude");
      float s2;
      mEnvFollow(s1);
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (mAmpEnv.done() && (mEnvFollow.value() < 0.001f))
      free();
  }

  // The graphics processing function
  void onProcess(Graphics &g) override {
    // Get the paramter values on every video frame, to apply changes to the
    // current instance
    float x = getInternalParameterValue("x");
    float y = getInternalParameterValue("y");
    float amplitude = getInternalParameterValue("amplitude");
    
    // Now draw
    g.pushMatrix();
    // Move x according to frequency, y according to amplitude
    g.translate(x, y, -8);
    g.scale(0.1, 0.1, 1.0);
    // Scale in the x and y directions according to amplitude
    // g.scale(1 - amplitude, amplitude, 1);
    // Set the color. Red and Blue according to sound amplitude and Green
    // according to frequency. Alpha fixed to 0.4
    // g.color(mEnvFollow.value(), frequency / 1000, mEnvFollow.value() * 10, 0.4);
    g.draw(mMesh);
    g.popMatrix();
  }

  // The triggering functions just need to tell the envelope to start or release
  // The audio processing function checks when the envelope is done to remove
  // the voice from the processing chain.
  void onTriggerOn() override { mAmpEnv.reset(); }

  void onTriggerOff() override { mAmpEnv.release(); }
};

// We make an app.
class MyApp : public App {
public:
  // GUI manager for SineEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<SineEnv> synthManager{"SineEnv"};

  float fontSize = 20.0;

  Mesh meshXAxis;
  Mesh meshYAxis;

  Mesh meshTickx1;
  Mesh meshTickx2;
  Mesh meshTickx3;
  Mesh meshTickx4;
  Mesh meshTickx5;
  Mesh meshTickx6;
  Mesh meshTickx7;
  Mesh meshTickx8;

  Mesh meshTicky1;
  Mesh meshTicky2;
  Mesh meshTicky3;
  Mesh meshTicky4;
  Mesh meshTicky5;
  Mesh meshTicky6;
  Mesh meshTicky7;
  Mesh meshTicky8;
  
  Mesh meshTick;


  // Font renderder
  FontRenderer fontRender;


  // This function is called right after the window is created
  // It provides a grphics context to initialize ParameterGUI
  // It's also a good place to put things that should
  // happen once at startup.
  void onCreate() override {
    navControl().active(false); // Disable navigation via keyboard, since we
                                // will be using keyboard for note triggering

    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());

    imguiInit();

    float w = float(width());
    float h = float(height());

    std::cout << "Width " << w << " Height " << h << std::endl;

    addRect(meshXAxis, width() * 2, 16, 0, height()/2.0);


    addRect(meshYAxis, 16, height() * 2, width()/2.0, 0);

    //addRect(meshTick, 5, 30, (width()/2.0)+80, height()/2.0);
    addRect(meshTick, 5, 30, 320, height()/2.0);


    addRect(meshTickx1, 5, 30, (width()/2.0) - 320, height()/2.0);
    addRect(meshTickx2, 5, 30, (width()/2.0) - 240, height()/2.0);
    addRect(meshTickx3, 5, 30, (width()/2.0) - 160, height()/2.0);
    addRect(meshTickx4, 5, 30, (width()/2.0) - 80, height()/2.0);
    addRect(meshTickx5, 5, 30, (width()/2.0) + 80, height()/2.0);
    addRect(meshTickx6, 5, 30, (width()/2.0) + 160, height()/2.0);
    addRect(meshTickx7, 5, 30, (width()/2.0) + 240, height()/2.0);
    addRect(meshTickx8, 5, 30, (width()/2.0) + 320, height()/2.0);


    addRect(meshTicky1, 30, 5, (width()/2.0), height()/2.0 -240);
    addRect(meshTicky2, 30, 5, (width()/2.0), height()/2.0 -180);
    addRect(meshTicky3, 30, 5, (width()/2.0), height()/2.0 -120);
    addRect(meshTicky4, 30, 5, (width()/2.0), height()/2.0 -60);
    addRect(meshTicky5, 30, 5, (width()/2.0), height()/2.0 +60);
    addRect(meshTicky6, 30, 5, (width()/2.0), height()/2.0 +120);
    addRect(meshTicky7, 30, 5, (width()/2.0), height()/2.0 +180);
    addRect(meshTicky8, 30, 5, (width()/2.0), height()/2.0 +240);
    

   
    fontRender.load(Font::defaultFont().c_str(), 60, 1024);

    // Play example sequence. Comment this line to start from scratch
    synthManager.synthSequencer().playSequence("testxy.synthSequence");
    synthManager.synthRecorder().verbose(true);
  }

  // The audio callback function. Called when audio hardware requires data
  void onSound(AudioIOData &io) override {
    synthManager.render(io); // Render audio
  }

  void onAnimate(double dt) override {
    // The GUI is prepared here
    imguiBeginFrame();
    // Draw a window that contains the synth control panel
    // synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  // The graphics callback function.
  void onDraw(Graphics &g) override {
    g.clear();

     // This example uses only the orthogonal projection for 2D drawing
    g.camera(Viewpoint::ORTHO_FOR_2D);  // Ortho [0:width] x [0:height]


    // g.color(1, 0, 0, 0.4);
    // addRect(meshFrame, width(), 2, 0, 0);

   //std::cout << "height() is " << height() << std::endl;


    g.color(1, 0, 0, 0.4);
    g.draw(meshXAxis);

    g.color(0, 0, 1, 0.4);
    g.draw(meshYAxis);

    g.color(1, 1, 1, 0.4);
    g.draw(meshTick);

    g.draw(meshTickx1);
    g.draw(meshTickx2);
    g.draw(meshTickx3);
    g.draw(meshTickx4);
    g.draw(meshTickx5);
    g.draw(meshTickx6);
    g.draw(meshTickx7);
    g.draw(meshTickx8);

    g.draw(meshTicky1);
    g.draw(meshTicky2);
    g.draw(meshTicky3);
    g.draw(meshTicky4);
    g.draw(meshTicky5);
    g.draw(meshTicky6);
    g.draw(meshTicky7);
    g.draw(meshTicky8);


    float textX = width() * 0.5;
    float textY = height() * 0.5;

    fontRender.write("(320,240)", fontSize);
    fontRender.renderAt(g, {textX, textY, 0.0});


    fontRender.write("(320,0)", fontSize);
    fontRender.renderAt(g, {width()/2.0, height()/2.0 -240, 0.0});

    fontRender.write("(320,60)", fontSize);
    fontRender.renderAt(g, {width()/2.0, height()/2.0 -180, 0.0});
    
    fontRender.write("(320,120)", fontSize);
    fontRender.renderAt(g, {width()/2.0, height()/2.0 -120, 0.0});
    
    fontRender.write("(320,180)", fontSize);
    fontRender.renderAt(g, {width()/2.0, height()/2.0 -60, 0.0});

    fontRender.write("(320,300)", fontSize);
    fontRender.renderAt(g, {width()/2.0, height()/2.0  +60, 0.0});

    fontRender.write("(320,360)", fontSize);
    fontRender.renderAt(g, {width()/2.0, height()/2.0 +120, 0.0});

    fontRender.write("(320,420)", fontSize);
    fontRender.renderAt(g, {width()/2.0, height()/2.0 +180, 0.0});

    fontRender.write("(320,480)", fontSize);
    fontRender.renderAt(g, {width()/2.0, height()/2.0 +240, 0.0});


   
   
   
   
    fontRender.write("(0,240)", fontSize);
    fontRender.renderAt(g, {width()/2.0 - 320, height()/2.0 ,0.0});

    fontRender.write("(80,240)", fontSize);
    fontRender.renderAt(g, {width()/2.0 - 240, height()/2.0 ,0.0});

    fontRender.write("(160,240)", fontSize);
    fontRender.renderAt(g, {width()/2.0 - 160, height()/2.0 ,0.0});
    
    fontRender.write("(240,240)", fontSize);
    fontRender.renderAt(g, {width()/2.0 - 80, height()/2.0 ,0.0});

    fontRender.write("(400,240)", fontSize);
    fontRender.renderAt(g, {width()/2.0 + 80, height()/2.0 ,0.0});

    fontRender.write("(480,240)", fontSize);
    fontRender.renderAt(g, {width()/2.0 + 160, height()/2.0 ,0.0});

    fontRender.write("(560,240)", fontSize);
    fontRender.renderAt(g, {width()/2.0 + 240, height()/2.0 ,0.0});

    fontRender.write("(640,240)", fontSize);
    fontRender.renderAt(g, {width()/2.0 + 320, height()/2.0 ,0.0});



    






    

    g.color(1, 1, 1, 0.4);

   // fontRender.write("(100, 100)", fontSize);
   // fontRender.renderAt(g, {100, 100, 0.0});

    //fontRender.write("(100, 100)", fontSize);
    //fontRender.renderAt(g, {100, 100, 0.0});

    g.color(1, 1, 0, 0.4);

    fontRender.write("(0, 0)", fontSize);
    fontRender.renderAt(g, {0, 0, 0.0});

    //fontRender.write("(600, 600)", fontSize);
    //fontRender.renderAt(g, {600, 600, 0.0});



    // Render the synth's graphics
    synthManager.render(g);

    // GUI is drawn here
    imguiDraw();
  }

  // Whenever a key is pressed, this function is called
  bool onKeyDown(Keyboard const &k) override {
    if (ParameterGUI::usingKeyboard()) { // Ignore keys if GUI is using
                                         // keyboard
      return true;
    }
    if (k.shift()) {
      // If shift pressed then keyboard sets preset
      int presetNumber = asciiToIndex(k.key());
      synthManager.recallPreset(presetNumber);
    } else {
      // Otherwise trigger note for polyphonic synth
      int midiNote = asciiToMIDI(k.key());
      if (midiNote > 0) {
        synthManager.voice()->setInternalParameterValue(
            "frequency", ::pow(2.f, (midiNote - 69.f) / 12.f) * 432.f);
        synthManager.triggerOn(midiNote);
      }
    }
    return true;
  }

  // Whenever a key is released this function is called
  bool onKeyUp(Keyboard const &k) override {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0) {
      synthManager.triggerOff(midiNote);
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }
};

int main() {
  // Create app instance
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();
  return 0;
}
