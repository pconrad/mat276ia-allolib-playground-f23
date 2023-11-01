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

float globalHeight = 1.0;
float globalWidth = 1.0;

// This example shows how to use SynthVoice and SynthManagerto create an audio
// visual synthesizer. In a class that inherits from SynthVoice you will
// define the synth's voice parameters and the sound and graphic generation
// processes in the onProcess() functions.

std::vector<std::vector<Vec3f>> points;

const float xScale = 180.0;
const float yScale = 90.0;

class SineEnv : public SynthVoice
{
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
  void init() override
  {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued

    // We have the mesh be a sphere
    addDisc(mMesh, 0.1, 30);

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
    createInternalTriggerParameter("x", 0.0, -180.0, 180.0);
    createInternalTriggerParameter("y", 0.0, -90.0, 90.0);
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
    mOsc.freq(getInternalParameterValue("frequency"));
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io())
    {
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
  void onProcess(Graphics &g) override
  {
    // Get the paramter values on every video frame, to apply changes to the
    // current instance
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");
    float x = getInternalParameterValue("x");
    float y = getInternalParameterValue("y");

    // std::cout << "x=" << x << " y=" << y << std::endl;

    // Now draw
    g.pushMatrix();

    g.camera(Viewpoint::IDENTITY);
    // g.scale(1.0 / xScale, 1.0 / yScale, 1.0);

    // Move x according to frequency, y according to amplitude
    g.translate(x/xScale, y/yScale, 0);
    // Scale in the x and y directions according to amplitude
    
    

    float scale = amplitude * 10;
    g.scale(scale * globalHeight / globalWidth, scale, 1.0);

    // Set the color. Red and Blue according to sound amplitude and Green
    // according to frequency. Alpha fixed to 0.4
    g.depthTesting(false);
    g.color(mEnvFollow.value(), frequency / 1000, mEnvFollow.value() * 10, 1.0);
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
class MyApp : public App
{
public:
  Mesh xAxis;
  Mesh yAxis;

 // this font is for rendering text labels on the axes
    FontRenderer fontRender;


  // GUI manager for SineEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<SineEnv> synthManager{"SineEnv"};

  // This function is called right after the window is created
  // It provides a grphics context to initialize ParameterGUI
  // It's also a good place to put things that should
  // happen once at startup.
  void onCreate() override
  {

  

    fontRender.load(Font::defaultFont().c_str(), 60, 1024);

    navControl().active(false); // Disable navigation via keyboard, since we
                                // will be using keyboard for note triggering

    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());

    imguiInit();

    // Play example sequence. Comment this line to start from scratch
    synthManager.synthSequencer().playSequence("earthquakes.synthSequence");
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
    // synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  // The graphics callback function.
  void onDraw(Graphics &g) override
  {

    globalHeight = height();
    globalWidth = width();

    g.clear();
    // Render the synth's graphics
    synthManager.render(g);

    g.camera(Viewpoint::IDENTITY);

    g.scale(1.0 / xScale, 1.0 / yScale, 1.0);

    float fontSize = 10.0;
    float tickSize = 5.0;
    float labelOffset = 5.0;

    drawYAxis(g, -90, 90, 30.0, tickSize, labelOffset, fontSize);
    drawXAxis(g, -180, 180, 30.0, tickSize, labelOffset, fontSize);
    drawWorldMap(g);

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
    if (k.shift())
    {
      // If shift pressed then keyboard sets preset
      int presetNumber = asciiToIndex(k.key());
      synthManager.recallPreset(presetNumber);
    }
    else
    {
      // Otherwise trigger note for polyphonic synth
      int midiNote = asciiToMIDI(k.key());
      if (midiNote > 0)
      {
        synthManager.voice()->setInternalParameterValue(
            "frequency", ::pow(2.f, (midiNote - 69.f) / 12.f) * 432.f);
        synthManager.triggerOn(midiNote);
      }
    }
    return true;
  }

  // Whenever a key is released this function is called
  bool onKeyUp(Keyboard const &k) override
  {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0)
    {
      synthManager.triggerOff(midiNote);
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }

  void drawLabelX(Graphics &g, float x, float labelOffset, float fontSize)
  {
    std::ostringstream oss;
    oss << std::fixed << std::showpoint;
    oss << std::setprecision(1);
    oss << "x=" << x;
    fontRender.alignCenter(); // makes the text centered around the position
    fontRender.write(oss.str().c_str(), fontSize);
    fontRender.renderAt(g, {x, labelOffset, 0.0});
  }

  void drawLabelY(Graphics &g, float y, float labelOffset, float fontSize)
  {
    std::ostringstream oss;
    oss << std::fixed << std::showpoint;
    oss << std::setprecision(1);
    oss << "y=" << y;
    fontRender.alignLeft(); // makes the text centered around the position
    fontRender.write(oss.str().c_str(), fontSize);
    fontRender.renderAt(g, {labelOffset, y - (fontSize * 0.2), 0.0});
  }

  void drawWorldMap(Graphics &g)
  {
    for (auto shape : points)
    {
      Mesh m;
      m.primitive(Mesh::LINE_STRIP);
      for (auto p : shape)
      {
        m.vertex(p);
      }
      g.color(1, 1, 1);
      g.draw(m);
    }
  }

  void drawXAxis(Graphics &g,
                 float minX,
                 float maxX,
                 float tickIncrement,
                 float tickSize,
                 float labelOffset,
                 float fontSize)
  {
    xAxis.primitive(Mesh::LINE_STRIP);

    // Create x and y axes
    xAxis.vertex(minX, 0);
    xAxis.vertex(maxX, 0);

    // Create the X tick marks and labels

    for (float x = 0; x < maxX; x += tickIncrement)
    {
      xAxis.vertex(x, 0);
      xAxis.vertex(x, tickSize);
      xAxis.vertex(x, -tickSize);
      xAxis.vertex(x, 0);
      drawLabelX(g, x, labelOffset, fontSize);
    }

    for (float x = 0; x >= minX; x -= tickIncrement)
    {
      xAxis.vertex(x, 0);
      xAxis.vertex(x, tickSize);
      xAxis.vertex(x, -tickSize);
      xAxis.vertex(x, 0);
      drawLabelX(g, x, labelOffset, fontSize);
    }

    //  Draw the axis
    g.color(1, 1, 0);
    g.draw(xAxis);
  }

  void drawYAxis(Graphics &g,
                 float minY,
                 float maxY,
                 float tickIncrement,
                 float tickSize,
                 float labelOffset,
                 float fontSize)
  {
    yAxis.primitive(Mesh::LINE_STRIP);

    yAxis.vertex(0, minY);
    yAxis.vertex(0, maxY);

    // Create the Y tick marks and labels


    for (float y = 0; y <= maxY; y += tickIncrement)
    {

      yAxis.vertex(0, y);
      yAxis.vertex(tickSize, y);
      yAxis.vertex(-tickSize, y);
      yAxis.vertex(0, y);
      g.color(1, 1, 1);

      drawLabelY(g, y, labelOffset, fontSize);
    }


    for (float y = 0; y >= minY; y -= tickIncrement)
    {
      yAxis.vertex(0, y);
      yAxis.vertex(tickSize, y);
      yAxis.vertex(-tickSize, y);
      yAxis.vertex(0, y);
      g.color(1, 1, 1);
      drawLabelY(g, y, labelOffset, fontSize);
    }

    //  Draw the Y Axis

    g.color(0, 1, 1);
    g.draw(yAxis);

  }
};

std::vector<std::string> split(const std::string &s, char delim)
{
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;

  while (getline(ss, item, delim))
  {
    result.push_back(item);
  }

  return result;
}

void readPoints()
{
  // file is taken from KML file with only the <coordinates> sections pulled out
  // and then the <coordinates> open/close tags put on their own lines
  // and each pair of x,y put on a separate line

  std::ifstream infile("../world-administrative.dat");
  std::string line;
  std::vector<Vec3f> thisShape;

  while (std::getline(infile, line))
  {
    if (line == "<coordinates>")
      continue;
    if (line == "</coordinates>")
    {
      points.push_back(thisShape);
      thisShape.clear();
      continue;
    }

    auto parts = split(line, ',');
    float x = std::stof(parts[0]);
    float y = std::stof(parts[1]);
    thisShape.push_back(Vec3f(x, y, 0.0f));
  }
  infile.close();
}

int main()
{

  readPoints();

  // Create app instance
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();
  return 0;
}
