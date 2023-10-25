/*
  MUS109IA / MAT276IA
  Fall 2023

  Help file for:
  AlloLib Playground, Instrument 04: FM Vib-Visual

  From starter code by Myungin Lee (Spring 2022).
  Annotations and additional code by Ryan Millett (Oct. 2023)
*/


// ---------------------------------------------------------------------------------------
// LIBRARIES -----------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// Libraries are collections of code that provide additional features and functionalities.
// We need to include the headers of the libraries we want to use in our code.

// --> `.h` or "header files" are files that contain the declarations of functions, classes, 
// and other objects.  They do not contain the actual definitions, rather, the header file
// tells the compiler that the definition exists somewhere else and that it should be *linked*
// to the *implementation* code (usually found in a corresponding `.cpp` file).

// --> `.hpp` files are similar to `.h` files, but are used for C++ code.  They differ from 
// `.cpp` files in that they are not compiled separately, but are included in the compilation
// process of the `.cpp` file that includes them.


// For standard input/output operations.
#include <cstdio>                        // we want to print to `stdout`

// Gamma is a library for sound synthesis, analysis, and processing.
// These headers provide various functionalities for sound synthesis and analysis.
#include "Gamma/Analysis.h"              // Tools for analyzing audio signals.
#include "Gamma/Effects.h"               // Audio effects and filters.
#include "Gamma/Envelope.h"              // Envelope generators (like ADSR).
#include "Gamma/Gamma.h"                 // Core functionalities of Gamma.
#include "Gamma/Oscillator.h"            // Oscillators for sound generation.
#include "Gamma/Types.h"                 // Basic types used in Gamma.
#include "Gamma/DFT.h"                   // Tools for Discrete Fourier Transform.

// AlloLib is a library for creating multimedia applications.
// These headers provide various functionalities related to graphics, audio, and UI.
#include "al/app/al_App.hpp"             // Base application class.
#include "al/graphics/al_Shapes.hpp"     // Tools for creating geometric shapes.
#include "al/scene/al_PolySynth.hpp"     // Polyphonic synthesizer class.
#include "al/scene/al_SynthSequencer.hpp"// Sequencer for the synthesizer.
#include "al/ui/al_ControlGUI.hpp"       // GUI for controlling parameters.
#include "al/ui/al_Parameter.hpp"        // Parameter class for live control.

/*
  NOTE:

  Gamma and AlloLib are designed to be compatible with each other.

  When integrated into the same environment, they can be used in concert to create interactive
  audiovisual applications where musical, sonic, graphic, and data elements can be cross-mapped
  and entangled in creative and meaningful ways.
*/


// ---------------------------------------------------------------------------------------
// NAMESPACES ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// Namespaces are a way in C++ to group related functionalities and types under a single name.
// This avoids naming collisions, especially when using multiple libraries that might have functions or types 
// with the same name (e.g., many libraries have functions for "print".  So, when you call the `print()` function,
// which one do you mean?).

// By declaring that we're "using" a namespace, we can refer to its elements directly without the namespace prefix.
using namespace std; // C++ Standard Library namespace.
using namespace gam; // Gamma namespace.
using namespace al;  // AlloLib namespace.

// --> if you dont use a namespace, you have to use the prefix along with the *scope resolution operator* (`::`) to 
// call its elements (e.g., `std::cout` instead of `cout`).


// ---------------------------------------------------------------------------------------
// PREPROCESSOR DIRECTIVES ---------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// Preprocessor directives, often prefixed with the '#' symbol, instruct the compiler to perform specific 
// tasks before the actual compilation process begins. 

// *Macros* are one kind of directive that allows the definition of constants or even inline functions.
// This macro defines a constant named `FFT_SIZE` with the value 4048.
// When the code is compiled, any macros are replaced with their defined values or operations.
#define FFT_SIZE 4048   // <-- This is a constant that can be used throughout the code, but note: 
                        // unlike typical (i.e., "typed") constants in C++, macros do *not* have a type
                        // and are purely a text substitution mechanism (i.e., copy-paste).


// ---------------------------------------------------------------------------------------
// GLOBAL VARIABLES ----------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// Global variables are variables that are declared outside of any function.  They can be accessed 
// from anywhere in the code.
// 
// Here, we are allocating memory for multiple arrays that will later be used to store wavetables.
// 
// There is some debate about the practice of using global variables.  We are using them here because
// we want to be able to access these arrays from multiple functions, both sonic and graphic, from 
// different scopes. 
// 
// --> note, this does *not* define the wavetables just yet, it just declares that they *will* exist and 
// allocates memory for each.  The actual definition of each wavetable happens in the `init()` function.
gam::ArrayPow2<float> tbSaw(2048),  // we're allocating memory for 2048 float values (i.e., audio samples)
                      tbSqr(2048),  
                      tbImp(2048),
                      tbSin(2048),
                      tbPls(2048),
                      tb__1(2048),  // e.g., reserving memory for 2048 samples for a wavetable named `tb__1`
                      tb__2(2048),
                      tb__3(2048),
                      tb__4(2048);
/*
  NOTE:

  But how can we know what this line of code is doing without the aid of comments?

  Right-click and select "Go to Definition" to see where the `ArrayPow2` class is defined.

  
  The definition of `gam::ArrayPow2<>`, for example, can be found in:
  
  .../allolib_playground/allolib/external/Gamma/Gamma/Containers.h


  Looking at the source code, we can see that `gam::ArrayPow2<>` is an example of a Template Class.
  
  Templates are a way to define a class that can be used with different types of data.  In this case, 
  `ArrayPow2` is a class that can be used to store arrays of data of any type.  The type of data is 
  specified by the template parameter (the `<>` ) which, in this case, is of type `float`.

  A brief aside on templates:
    ex: `ArrayPow2<float>`

    The `<>` symbol indicates that the class is a template class. When used, the template parameter is
    replaced with the specified type. This allows for generic programming, meaning the same code can 
    operate on different data types without having to be rewritten for each type. For example, `ArrayPow2<float>` 
    is a class that allocates memory for arrays of floats, whereas `ArrayPow2<char>` allocates memory for 
    arrays of characters.

  The default constructor, `explicit ArrayPow2(uint32_t size): Base(size){}`, calls the constructor of the
  base class, `Base(size)`, which allocates memory for an array of `size`-many elements of type `float`.

  So, the above line of code is allocating memory for 2048 samples, stored as floats, for each wavetable.
*/


// ------------------------------------------------------------------------------------
// CLASSES AND OBJECT-ORIENTED PROGRAMMING (OOP) --------------------------------------
// ------------------------------------------------------------------------------------
// A class in C++ is a blueprint for creating objects. Objects are instances of classes.
// Think of a class as a template and an object as a realization of that template.
// 
// For example, consider a base (or "parent") class `Animal` with a function `makeSound`.
// We can create a derived (or "child") class `Dog` that inherits the `makeSound` function
// from the `Animal` class and overrides it with its own implementation.  We can then create
// a `Cat` class that also inherits the `makeSound` function from the `Animal` class and
// overrides it with its own implementation, and so on...
// 
// The core concepts of OOP include:
// 
// - Encapsulation: the bundling of data and functions that operate on that data.
// - Inheritance:   the ability to create new classes that inherit the attributes and behaviors of
//                  existing classes.
// - Polymorphism:  the ability to redefine the behavior of a class by overriding its methods.
//
// The `SynthVoice` class is a base class for creating polyphonic synthesizers.  Our class, `FMWT`,
// inherets from the `SynthVoice` class.  This means that our class will have all the attributes and
// behaviors of the `SynthVoice` class, but we can also override any of its methods to create our own
// implementation.
class FMWT : public SynthVoice
{
public: // `public` means these member variables/methods can be accessed from outside the class.
        // `private` means they can only be accessed from within the class.
        // `protected` means they can be accessed from within the class *and* any derived classes.
        // --> if no access specifier is provided, members are `private` by default.

  // ---------------------------------------------------------------------------------------
  // UNIT GENERATORS (UGens) ---------------------------------------------------------------
  // ---------------------------------------------------------------------------------------
  // Unit Generators, commonly known as UGens, are fundamental components in many sound synthesis 
  // and processing systems. Each UGen performs a specific audio-related task, such as generating 
  // a waveform, modifying an audio signal, or analyzing it.
  //
  // UGens can be thought of as modular blocks in a larger audio signal chain. They can be 
  // interconnected in various ways to design complex sound synthesis architectures. For instance, 
  // an oscillator UGen might feed its signal into a filter UGen, which then sends its output to 
  // an amplitude envelope UGen, and so on.
  // By chaining and combining UGens, one can sculpt a wide range of sonic timbres and behaviors.
  // Below are some of the UGens used in this class:
  //
  gam::Pan<> mPan;                    // Stereo panner: used to position the sound in a stereo field.
  gam::ADSR<> mAmpEnv;                // Amplitude envelope for the carrier oscillator.
  gam::ADSR<> mModEnv;                // Modulator frequency envelope
  gam::EnvFollow<> mEnvFollow;        // Envelope follower: this tracks the amplitude of an audio 
                                      // signal and produces a control signal based on it. Useful 
                                      // for dynamically changing parameters based on the sound's amplitude.
  gam::ADSR<> mVibEnv;                // Envelope for vibrato
  gam::Sine<> mod, mVib;              // Modulator frequency and vibrato sine wave oscillators. 
  gam::Osc<> car;                     // Carrier frequency oscillator

  // SYNTHESIS PARAMETERS ---------------------------------------------------------------
  // These parameters determine various aspects of the sound synthesis process.

  float mVibFrq;                      // Vibrato frequency: how fast the pitch wobbles.
  float mVibDepth;                    // Vibrato depth: how much the pitch wobbles.
  float mVibRise;                     // Vibrato "rise" time: the duration it takes for the vibrato rate 
                                      // to change from its initial to its final frequency.

  // OTHER MEMBER VARIABLES -------------------------------------------------------------
  // These are miscellaneous variables used within the class for various purposes.

  int mtable;                         // index for the current wavetable being used.

  static const int numb_waveform = 9; // A constant indicating the number of waveforms available. 
                                      // `static const` means its value won't change and is shared 
                                      // among all instances of the class (i.e., `this` class)

  // VISUAL/GRAPHICS PARAMETERS --------------------------------------------------------
  // These parameters are related to the visual representation of the synthesis process.

  Mesh mMesh[numb_waveform];          // An array of Mesh objects for graphical representation. 
                                      // Each mesh corresponds to a waveform.

  // some "accumulator" variables
  double a = 0;                       // rotation parameter for the visual mesh.
  double b = 0;                       // another rotation parameter.
  double timepose = 10;               // a temporal position or duration parameter for animations.

  bool wireframe = false;             // a flag to determine if the mesh should be drawn as a wireframe or solid.
  bool vertexLight = false;           // a flag to determine if vertex-based lighting should be used on the mesh.
  // --> don't worry too much about these right now....

  // The init() function is used to set up initial configurations for our FMWT voice.
  // This is only called once, when (before) the voice is first created.
  void init() override
  {
    // The 'levels()' function sets the initial, peak, sustain, and final levels of the envelope.
    // For example, mModEnv.levels(0, 1, 1, 0) means the envelope starts at 0, goes up to 1, 
    // sustains at 1, and ends at 0.
    mModEnv.levels(0, 1, 1, 0);             // Envelope for modulator frequency 
    mVibEnv.levels(0, 1, 1, 0);             // Envelope for vibrato

    // The 'sustainPoint()' function sets which level should be sustained.
    // Here, we're saying that the third level (which is 1) should be sustained.
    mAmpEnv.sustainPoint(2);  // Sets the point at which the envelope holds its value until released

    // -----------------------------------------------------------------------------------
    // INTERNAL PARAMETERS ---------------------------------------------------------------
    // -----------------------------------------------------------------------------------
    // These are the synth parameters.  Here, we are connecting them to the GUI in order to
    // expose them to the user.  This allows the user to control the parameters in real-time.

    createInternalTriggerParameter("freq",  // parameter name
                                   440,     // initial value
                                   10,      // minimum value
                                   4000.0   // maximum value
    );
    createInternalTriggerParameter("amplitude", // etc...
                                   0.1,
                                   0.0,
                                   1.0
    );
    createInternalTriggerParameter("attackTime", 0.1, 0.01, 3.0);
    createInternalTriggerParameter("releaseTime", 0.3, 0.1, 10.0);
    createInternalTriggerParameter("sustain", 0.65, 0.1, 1.0);

    // FM index
    createInternalTriggerParameter("idx1", 0.01, 0.0, 10.0);
    createInternalTriggerParameter("idx2", 7, 0.0, 10.0);
    createInternalTriggerParameter("idx3", 5, 0.0, 10.0);

    createInternalTriggerParameter("carMul", 1, 0.0, 20.0);
    createInternalTriggerParameter("modMul", 1.0007, 0.0, 20.0);

    createInternalTriggerParameter("vibRate1", 0.01, 0.0, 10.0);
    createInternalTriggerParameter("vibRate2", 0.5, 0.0, 10.0);
    createInternalTriggerParameter("vibRise", 0, 0.0, 10.0);
    createInternalTriggerParameter("vibDepth", 0, 0.0, 10.0);

    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
    createInternalTriggerParameter("table", 0, 0, 8);

    // -----------------------------------------------------------------------------------
    // WAVETABLES and MESHES -------------------------------------------------------------
    // -----------------------------------------------------------------------------------
    // A wavetable is a pre-computed array of samples that can be used to generate a periodic waveform.
    // Here, we are taking the arrays we declared earlier and filling them with samples that correspond
    // to different waveforms.  
    //
    // The `addSinesPow()` function is used to fill the arrays with audio samples.  It takes a *reference* 
    // to an array, and adds a series of sine waves to it. 
    
    /*
      NOTE about `gam::addSinesPow<1>`:

      Each sine wave is calcuated using the *inverse power law* for amplitudes.  This means that the
      amplitude of each harmonic is calculated as `1 / (n^p)` where `n` is the harmonic number and `p` is
      the exponent of the power law.  For example, if `p = 1`, then the amplitude of each harmonic is
      `1 / n`.  If `p = 2`, then the amplitude of each harmonic is `1 / (n^2)`.  If `p = 0`, then the
      amplitude of each harmonic is `1` (i.e., all harmonics have the same amplitude).
    */
    gam::addSinesPow<1>(tbSaw,  // the array to be filled
                        9,      // the number of sine waves (harmonics) to add
                        1       // harmonic number multiplication factor (i.e., the "step" between each harmonic)
    ); // --> this creates a wavetable with 9 harmonic partials counted sequentially (i.e., 1, 2, 3, ..., 9)

    /*
      NOTE:

      Slices and stacks?

      A *stack* is a collection of slices that are stacked on top of each other.  A *slice* is a 
      collection of vertices that are connected to form a polygon.  The number of slices determines 
      the resolution of the mesh.  The more slices, the more vertices, and the higher the resolution
      of the mesh.
    */
    addCone(mMesh[0],           // reference to a mesh (here, the 0th mesh in the array of meshes)
            1,                  // radius of the cone
            Vec3f(0,0,5),       // position of the apex of the cone
            40,                 // number of slices
            1                   // number of stacks
    ); // --> this creates a cone mesh with 40 slices and 1 stack


    // etc...
    gam::addSinesPow<1>(tbSqr, 9, 2);
    addCube(mMesh[1]);  // tbSquare

    gam::addSinesPow<0>(tbImp, 9, 1);
    addPrism(mMesh[2],          // reference to a mesh (here, the 2nd mesh in the array of meshes)
            1,                  // bottom radius
            1,                  // top radius
            1,                  // height
            100                 // number of slices
    );

    gam::addSine(tbSin);
    addSphere(mMesh[3],         // reference to a mesh (here, the 3rd mesh in the array of meshes)
              0.3,              // radius
              16,               // number of slices
              100               // number of stacks
    ); // --> With a sphere, stacks are circles cut perpendicular to the z axis while slices are 
       // circles cut through the z axis.


    /*
      NOTE about `gam::addSines`:

      addSines (dst, amps, cycs, numh)
      -- dst		destination array
      -- amps		harmonic amplitudes of series, size must be numh - A[]
      -- cycs		harmonic numbers of series, size must be numh - C[]
      -- numh		total number of harmonics
    */
    float scaler = 0.15;
    float hscaler = 1;

    { //tbPls
      float A[] = {1, 1, 1, 1, 0.7, 0.5, 0.3, 0.1};
      gam::addSines(tbPls, A, 8); 
      addWireBox(mMesh[4],2);    // tbPls
    }
    { // tb__1 
      float A[] = {1, 0.4, 0.65, 0.3, 0.18, 0.08, 0, 0};
      float C[] = {1, 4, 7, 11, 15, 18, 0, 0 };
      gam::addSines(tb__1, A, C, 6);
      for (int i = 0; i < 7; i++){
        addWireBox(mMesh[5],                // reference to a mesh
                   scaler * A[i]*C[i],      // width
                   scaler * A[i+1]*C[i+1],  // height
                   1 + 0.3*i                // depth
        );
      }
    }
    { // inharmonic partials
      float A[] = {0.5, 0.8, 0.7, 1, 0.3, 0.4, 0.2, 0.12};
      float C[] = {3, 4, 7, 8, 11, 12, 15, 16}; 
      gam::addSines(tb__2, A, C, 8); // tb__2
      for (int i = 0; i < 7; i++){
        addWireBox(mMesh[6], scaler * A[i]*C[i], scaler * A[i+1]*C[i+1], 1 + 0.3*i);
      }
    }
    { // inharmonic partials
      float A[] = {1, 0.7, 0.45, 0.3, 0.15, 0.08, 0 , 0};
      float C[] = {10, 27, 54, 81, 108, 135, 0, 0};
      gam::addSines(tb__3, A, C, 6); // tb__3
      for (int i = 0; i < 7; i++){
        addWireBox(mMesh[7], scaler * A[i]*C[i], scaler * A[i+1]*C[i+1], 1 + 0.3*i);
      }
    }
  { // harmonics 20-27
      float A[] = {0.2, 0.4, 0.6, 1, 0.7, 0.5, 0.3, 0.1};
      gam::addSines(tb__4, A, 8, 20); // tb__4
      for (int i = 0; i < 7; i++){
        addWireBox(mMesh[8], hscaler * A[i], hscaler * A[i+1], 1 + 0.3*i);
      }
    }

    // -----------------------------------------------------------------------------------
    // COMPUTER GRAPHICS: NORMALS --------------------------------------------------------
    // -----------------------------------------------------------------------------------
    // Normals are vectors that are perpendicular to a surface.  They are used in computer graphics
    // to determine how light interacts with a surface.  For example, if a surface is flat, then the
    // normal vector is the same everywhere on the surface.  If the surface is curved, then the normal
    // vector changes depending on the curvature of the surface.
    // Scale and generate normals
    for (int i = 0; i < numb_waveform; ++i) {   // for each mesh in the array of meshes...
      mMesh[i].scale(0.4);                          // apply a scaling factor of 0.4 to each dimension in the current mesh

      int Nv = mMesh[i].vertices().size();          // get the number of vertices in the current mesh
      
      for (int k = 0; k < Nv; ++k) {                // for each vertex in the current mesh...
                                                             // color the mesh according to the HSV color space
        mMesh[i].color(HSV(float(k) / Nv, // hue
                           0.3,           // saturation
                           1              // value
                      )
        );
      }
      // don't worry about this for now...
      if (!vertexLight && mMesh[i].primitive() == Mesh::TRIANGLES) {
        mMesh[i].decompress();  // decompresses the mesh (i.e., convert this particular mesh from 
                                // indexed form to non-indexed (or "flat") form.
      }
      mMesh[i].generateNormals();                   // generate normals for the current mesh
    }
  }

  // ---------------------------------------------------------------------------------------
  // DIGITAL AUDIO:  CALLBACK FUNCTIONS ----------------------------------------------------
  // ---------------------------------------------------------------------------------------
  // Callback functions are functions that are called by the system when a certain event occurs.
  // For example, the `onProcess()` function is called by the system when the audio callback is
  // triggered.  
  // An audio callback is a function that is called by the system when it is time to process the
  // next *block* of audio samples.  A block is a collection of audio samples that are processed
  // together.  The size of the block is determined by the system and is usually a power of 2 (the
  // default for this app is 512 samples per block).  Increasing the block size can reduce the
  // number of times the callback is called per second, but it can also increase the *latency* of
  // the system (i.e., the delay between when a sound is triggered and when it is heard).
  void onProcess(AudioIOData &io) override
  {
    mVib.freq(mVibEnv()); // `mVib` is our vibrato oscillator. Here, we set its frequency based 
                          // on `mVibEnv()`, which gives the current value of the vibrato envelope.
    
    // set base parameters...
    float carBaseFreq =
        getInternalParameterValue("freq") * getInternalParameterValue("carMul");
    float modScale = getInternalParameterValue("freq") * getInternalParameterValue("modMul");
    float amp = getInternalParameterValue("amplitude") * 0.01;

    // while there are still samples to process 
    while (io())                                        // for each sample...
    {
      mVib.freq(mVibEnv());                                 // set the frequency of the vibrato oscillator
      car.freq((1 + mVib() * mVibDepth) * carBaseFreq +     // calculate FM
               mod() * mModEnv() * modScale);
      float s1 = car() * mAmpEnv() * amp;                   // scale the output of the carrier oscillator
      float s2;                                             // reserve memory for the right audio channel
      mEnvFollow(s1);                                       // pass the sample through an envelope follower
      mPan(s1, s1, s2);                                     // compute left and right channel samples
      
      io.out(0) += s1;                                      // add left (0) and
      io.out(1) += s2;                                      // right (1) samples to the output buffer
    }

    if (mAmpEnv.done() && (mEnvFollow.value() < 0.001))     // Check if the amplitude envelope has finished its release stage and if the signal is effectively silent.
      free();                                                   // If both conditions are true, free up resources associated with this voice.
  }

  // ---------------------------------------------------------------------------------------
  // COMPUTER GRAPHICS:  CALLBACK FUNCTIONS ------------------------------------------------
  // ---------------------------------------------------------------------------------------
  void onProcess(Graphics &g) override
  {
    // Update accumulation member variables
    a += 0.29;
    b += 0.23;
    timepose -= 0.06;

    int shape = getInternalParameterValue("table");  // get the index of the wavetable
        
    g.polygonMode(wireframe ? GL_LINE : GL_FILL);
    // light.pos(0, 0, 0);
    gl::depthTesting(true);
    
    /*
      A word about the `pushMatrix()` and `popMatrix()` functions.

      > `push`? `pop`?  `matrix`?  What's going on here?

      In computer graphics, a *matrix* is a mathematical construct that can be used to represent
      transformations in space (e.g., translation, rotation, scaling).  Before we apply any kind
      of transformation to an object, we first need to push the current matrix onto a *stack* (a 
      data structure that stores data in a "last-in-first-out" (LIFO) order, i.e., it can be 
      modeled as a "stack" of things).

      The reason for this will be explained later in the code.  For now, the important concept to
      internalize is this notion of a *stack*, an abstract data structure in which data (e.g., the 
      state of a matrix), is pushed onto (i.e., added to the top of the stack) and popped off 
      (i.e., removed from the top of the stack) in LIFO order.
    */ 

    // Push the current matrix onto the stack.
    // When you "push" a matrix onto the stack, you're saving the current transformation matrix on 
    // top of the stack. This allows you to return to this transformation state later. 
    // It's akin to saving your current position.
    g.pushMatrix();
    
    // this is for rendering lighting, don't worry about this for now
    g.depthTesting(true);
    g.lighting(true);

    // Applying Transformations:
    // Once a matrix is pushed, you can apply further transformations (translate, rotate, scale) to 
    // create a new matrix...

    // --------------------------------------------------------------------------------------
    // COMPUTER GRAPHICS: BASIC TRANSFORMATION OPERATIONS -----------------------------------
    // --------------------------------------------------------------------------------------
    // In computer graphics, operations that pertain to change in position, rotation, and scale are
    // computed by applying *matrix transformations* (see https://en.wikipedia.org/wiki/Transformation_matrix). 
    
    // 1. TRANSLATE
    // The *translate* operation, for example, applies a dimension-wise offset factor to the current
    // position (e.g., in 3D `(x,y,z)` coordinates).
    
    // Here, the *instance* of the `Graphics` Object (parameter `g`) is translated in (x,y,z)
    // space.
    // `g` is moved to position `timepose` on the x-axis...
    // ...to position `getInternalParameterValue("freq") / 200 - 3` on the y-axis...
    // ...and to position `-4` on the z-axis (i.e., it stays at a constant position of -4).
    g.translate(timepose,                                    // x - dimension
                getInternalParameterValue("freq") / 200 - 3, // y - dimension 
                -4                                           // z - dimension
    );
    
    // 2. ROTATE
    // The *rotate* operation rotates the object around a specified axis (represented as a `Vec3f`
    // type) and at an angle (expressed in degrees).
    
    // Here, the `g` object is rotated around the y-axis by an angle of `mVib() + a` degrees...
    g.rotate(mVib() + a,    // angle, in degrees
             Vec3f(0, 1, 0) // rotation axis
    );

    // ...and then again around the x-axis by an angle of `mVib() * mVibDepth + b` degrees.
    g.rotate(mVib() * mVibDepth + b, Vec3f(1));
    
    // 3. SCALE
    // The *scale* operation scales the object along each dimension by a specified coefficient.
    // If the same coefficient is used for each dimension, the object is scaled uniformly (i.e., its 
    // size will change but its relative proportions will remain the same).
    // Scaling by different coefficients along each dimension will result in a non-uniform scaling (i.e.,
    // the object's relative proportions *will* change, resulting in a "warping" effect).

    // Here, we first calculate a scaling factor based on the current amplitude of the signal multiplied by 
    // a constant factor of 10.
    float scaling = getInternalParameterValue("amplitude") * 10;  // try different values here (e.g., 1, 100, 0.667, ...)
    // --> or try creating a more dynamic scaling factor by multiplying the amplitude by, e.g., the envelope 
    // follower or the vibrato envelope or some function of frequency and color, etc...

    // Then, we scale the object along each dimension, applying the scaling factor.  The instance object `g` 
    // is scaled along each dimension by a factor of `scaling` +...
    // ...`getInternalParameterValue("modMul") / 2` in the x-dimension...
    // ...`getInternalParameterValue("carMul") / 20` in the y-dimension...
    // ...and `mEnvFollow.value() * 5` in the z-dimension.
    g.scale(scaling + getInternalParameterValue("modMul") / 2,  // x
            scaling + getInternalParameterValue("carMul") / 20, // y 
            scaling + mEnvFollow.value() * 5                    // z
    );
    // --> as before, try different values here.  Think about what parameters, across modalities, might be
    // interesting to map to each dimension of the scale operation.  You can change these values manually
    // or you could even write you own functions to compute each dimension's scaling factor based on some
    // internal cross-mapping of audio and visual parameters.

    // Speaking of which...

    // --------------------------------------------------------------------------------------
    // COMPUTER GRAPHICS: COLOR SPACES ------------------------------------------------------
    // --------------------------------------------------------------------------------------
    // Color can be encoded and represented in different ways.  A common representation is in
    // terms of the RGB color space, where each color is represented as a combination of red,
    // green, and blue values (each ranging from 0 to 255).  This (r,g,b) color space, when 
    // mapped to (x,y,z) position, results in a volumetric cube where coordinates in the 3-D 
    // space within the cube, when expressed as a color in RGB values, results in a "RGB 
    // Color Cube" (see: https://en.wikipedia.org/wiki/RGB_color_space).
    //    
    // Alternatively, the HSV color space is represented as a combination of hue, saturation, 
    // and value (or brightness).  Unlike RGB color space which is mapped to a 3-D volumetric 
    // cube, HSV color space is mapped to a 3-D volumetric cone or cylinder (depending on the 
    // implementation, see https://en.wikipedia.org/wiki/HSL_and_HSV).
    //
    // Here, we use the `HSV` class to represent color in HSV color space. The instance object `g` 
    // is colored according to a position in the HSV color space which is calculated as follows 
    // by...
    // ...setting the hue to `getInternalParameterValue("modMul") / 20`...
    // ...setting the saturation to `getInternalParameterValue("carMul") / 20`...
    // ...setting the value to `0.5 + getInternalParameterValue("attackTime")`.    
    g.color(HSV(getInternalParameterValue("modMul") / 20,       // hue
                getInternalParameterValue("carMul") / 20,       // saturation
                0.5 + getInternalParameterValue("attackTime"))  // value
    );
    
    // Draw the mesh at index `shape` in the array of meshes `mMesh`
    g.draw(mMesh[shape]);
    
    // Pop matrix off the top of the stack.
    // When you "pop" a matrix, you're removing the top matrix from the stack, reverting to 
    // the transformation matrix below. 
    // It's like going back to your previously saved position.
    g.popMatrix();
  }

  void onTriggerOn() override
  {
    timepose = 10;
    mAmpEnv.reset();
    mVibEnv.reset();
    mModEnv.reset();
    mVib.phase(0);
    mod.phase(0);
    updateFromParameters();
    updateWaveform();

    float modFreq =
        getInternalParameterValue("freq") * getInternalParameterValue("modMul");
    mod.freq(modFreq);
  }
  void onTriggerOff() override
  {
    mAmpEnv.triggerRelease();
    mModEnv.triggerRelease();
    mVibEnv.triggerRelease();
  }

  void updateFromParameters()
  {
    mModEnv.levels()[0] = getInternalParameterValue("idx1");
    mModEnv.levels()[1] = getInternalParameterValue("idx2");
    mModEnv.levels()[2] = getInternalParameterValue("idx2");
    mModEnv.levels()[3] = getInternalParameterValue("idx3");

    mAmpEnv.attack(getInternalParameterValue("attackTime"));
    mAmpEnv.release(getInternalParameterValue("releaseTime"));
    mAmpEnv.sustain(getInternalParameterValue("sustain"));

    mModEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mModEnv.lengths()[3] = getInternalParameterValue("releaseTime");

    mVibEnv.levels(getInternalParameterValue("vibRate1"),
                   getInternalParameterValue("vibRate2"),
                   getInternalParameterValue("vibRate2"),
                   getInternalParameterValue("vibRate1"));
    mVibEnv.lengths()[0] = getInternalParameterValue("vibRise");
    mVibEnv.lengths()[1] = getInternalParameterValue("vibRise");
    mVibEnv.lengths()[3] = getInternalParameterValue("vibRise");
    mVibDepth = getInternalParameterValue("vibDepth");
    
    mPan.pos(getInternalParameterValue("pan"));
  }
  void updateWaveform(){
        // Map table number to table in memory
    switch (int(getInternalParameterValue("table"))) {
      case 0:
        car.source(tbSaw);
        break;
      case 1:
        car.source(tbSqr);
        break;
      case 2:
        car.source(tbImp);
        break;
      case 3:
        car.source(tbSin);
        break;
      case 4:
        car.source(tbPls);
        break;
      case 5:
        car.source(tb__1);
        break;
      case 6:
        car.source(tb__2);
        break;
      case 7:
        car.source(tb__3);
        break;
      case 8:
        car.source(tb__4);
        break;
    }
  }
};


// The MyApp class is central to the functioning of the application. It manages the main operations of the program, 
// handling both the audio synthesis and the graphical interface. 
// The class inherits from both App (from AlloLib) and MIDIMessageHandler, making it capable of general application 
// management as well as handling MIDI messages.
class MyApp : public App, public MIDIMessageHandler
{
public:
  SynthGUIManager<FMWT> synthManager{"synth4VibWT"}; // GUI manager for FMWT voice
  RtMidiIn midiIn; // MIDI input carrier
  //    ParameterMIDI parameterMIDI;
  
  // An instance of the FMWT voice which will be controlled by this app.
  FMWT fmwt;   // <-- this is the synth we just made

  // Variables related to MIDI notes and the FM modulation.
  int midiNote;
  float mVibFrq;
  float mVibDepth;
  float tscale = 1;

  Mesh mSpectrogram;
  vector<float> spectrum;
  bool showGUI = true;
  bool showSpectro = true;
  bool navi = false;
  gam::STFT stft = gam::STFT(FFT_SIZE, FFT_SIZE / 4, 0, gam::HANN, gam::MAG_FREQ);

  void onInit() override
  {
    imguiInit();

    navControl().active(false); // Disable navigation via keyboard, since we
                                // will be using keyboard for note triggering
    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());
    // Check for connected MIDI devices
    if (midiIn.getPortCount() > 0)
    {
      // Bind ourself to the RtMidiIn object, to have the onMidiMessage()
      // callback called whenever a MIDI message is received
      MIDIMessageHandler::bindTo(midiIn);

      // Open the last device found
      unsigned int port = midiIn.getPortCount() - 1;
      midiIn.openPort(port);
      printf("Opened port to %s\n", midiIn.getPortName(port).c_str());
    }
    else
    {
      printf("Error: No MIDI devices found.\n");
    }
    // Declare the size of the spectrum
    spectrum.resize(FFT_SIZE / 2 + 1);
  }

  // This is called once (when the app first starts up)
  void onCreate() override
  {
    // Play example sequence. Comment this line to start from scratch
    //    synthManager.synthSequencer().playSequence("synth2.synthSequence");
    synthManager.synthRecorder().verbose(true);
    nav().pos(3, 0, 17);
  }

  // Different from the `onProcess` callback functions, this is called whenever
  // audio is detected (i.e., when your synth is playing)
  void onSound(AudioIOData &io) override
  {
    synthManager.render(io); // Render audio
    // STFT
    while (io()) // while there are audio samples...
    {
      io.out(0) = tanh(io.out(0));
      io.out(1) = tanh(io.out(1));
      if (stft(io.out(0)))
      { // Loop through all the frequency bins
        for (unsigned k = 0; k < stft.numBins(); ++k)
        {
          // Here we simply scale the complex sample
          spectrum[k] = tanh(pow(stft.bin(k).real(), 1.3));
          // spectrum[k] = stft.bin(k).real();
        }
      }
    }
  }

  // The `onAnimate()` function is called before rendering each frame and is used 
  // to update any animations or dynamic visual elements in the application.
  void onAnimate(double dt) override
  {
    navControl().active(navi); // Disable navigation via keyboard, since we
    imguiBeginFrame();
    synthManager.drawSynthControlPanel();
    imguiEndFrame();
    // Map table number to table in memory
    fmwt.mtable = int(synthManager.voice()->getInternalParameterValue("table"));
  }

  // The onDraw() function is called to render the graphical content of the application.
  // It's here that all the visual elements, such as the GUI and spectrogram, are drawn.
  void onDraw(Graphics &g) override
  {
    // Clear the graphics context. This ensures that we start with a blank slate 
    // before drawing the new frame.
    g.clear();

    // Render the synthesizer's visual elements using the synthManager.
    synthManager.render(g);

    // // Draw Spectrum
    mSpectrogram.reset(); // Reset the spectrogram mesh, preparing it for new data.
    mSpectrogram.primitive(Mesh::LINE_STRIP);

    if (showSpectro)  // If the 'showSpectro' flag is true, we draw the audio spectrum.
    {
      // Loop through the spectrum data and set the corresponding vertices 
      // and colors in the mSpectrogram mesh.
      for (int i = 0; i < FFT_SIZE / 2; i++)
      {
        mSpectrogram.color(HSV(0.5 - spectrum[i] * 100));
        mSpectrogram.vertex(i, spectrum[i], 0.0);
      }

      g.meshColor();                          // Apply the colors specified in the mSpectrogram mesh.

      g.pushMatrix();                         // Push the current matrix onto the stack.
      
      g.translate(-3, -3, 0);                 // move the mesh to the left and down by -3
                                              
                                              // scale the mesh..
      g.scale(10.0 / FFT_SIZE,                // ...along the x-axis by 10 / FFT_SIZE
              1000,                           // ...along the y-axis by 1000
              1.0                             // ...along the z-axis by 1
      );

      g.draw(mSpectrogram);                   // Draw the spectrogram mesh.
      
      g.popMatrix();                          // Pop the current matrix off the stack.
    }
    // GUI is drawn here
    if (showGUI)
    {
      imguiDraw();
    }
  }
  void onMIDIMessage(const MIDIMessage &m)
  {
    switch (m.type())
    {
    case MIDIByte::NOTE_ON:
    {
      int midiNote = m.noteNumber();
      if (midiNote > 0 && m.velocity() > 0.001)
      {
        synthManager.voice()->setInternalParameterValue(
            "freq", ::pow(2.f, (midiNote - 69.f) / 12.f) * 432.f);
        synthManager.voice()->setInternalParameterValue(
            "attackTime", 0.01 / m.velocity());
        synthManager.triggerOn(midiNote);
      }
      else
      {
        synthManager.triggerOff(midiNote);
      }
      break;
    }
    case MIDIByte::NOTE_OFF:
    {
      int midiNote = m.noteNumber();
      printf("Note OFF %u, Vel %f", m.noteNumber(), m.velocity());
      synthManager.triggerOff(midiNote);
      break;
    }
    default:;
    }
  }
  bool onKeyDown(Keyboard const &k) override
  {
    if (ParameterGUI::usingKeyboard())
    { // Ignore keys if GUI is using them
      return true;
    }
    if (!navi)
    {
      if (k.shift())  // If shift pressed then keyboard sets preset
      {
        int presetNumber = asciiToIndex(k.key());
        synthManager.recallPreset(presetNumber);
      }
      else
      {
        // Otherwise trigger note for polyphonic synth
        int midiNote = asciiToMIDI(k.key()) - 12;
        if (midiNote > 0)
        {
          synthManager.voice()->setInternalParameterValue(
              "freq", ::pow(2.f, (midiNote - 69.f) / 12.f) * 432.f);
          synthManager.voice()->setInternalParameterValue("table", fmwt.mtable);
          synthManager.triggerOn(midiNote);
        }
      }
    }
    switch (k.key())
    {
    case ']':
      showGUI = !showGUI;
      break;
    case '[':
      showSpectro = !showSpectro;
      break;
    case '-':
      tscale -= 0.1;
      break;
    case '+':
      tscale += 0.1;
      break;
    case '=':
      navi = !navi;
      break;
    }
    return true;
  }

  bool onKeyUp(Keyboard const &k) override
  {
    int midiNote = asciiToMIDI(k.key()) - 12;
    if (midiNote > 0)
    {
      synthManager.triggerOff(midiNote);
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }
};

int main()
{
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();
}
