/*
Allocore Example: 2D drawing

Description:
This demonstrates how to do 2D drawing by setting up an orthographic projection
matrix.

Author:
Lance Putnam, Feb. 2012
Keehong Youn, 2017

Modified by Phill Conrad, Eirini Schoinas, Riona Pampati, 2023

*/

#include "al/app/al_App.hpp"
#include "al/system/al_Time.hpp"
#include <iostream>
#include <iomanip>

#include "al/graphics/al_Font.hpp" // for text rendering


// Drawing on a 2D canvas using pixel coordinates

using namespace al;

struct MyApp : public App {
  Mesh xAxis;
  Mesh yAxis;

  float fontSize = 0.05;
  FontRenderer fontRender;


  Mesh verts2{Mesh::LINES};

  double timeAccum{0};

  void onCreate() {
    xAxis.primitive(Mesh::LINE_STRIP);
    yAxis.primitive(Mesh::LINE_STRIP);

    // Create x and y axes
    xAxis.vertex(-1, 0);
    xAxis.vertex(1, 0);

    yAxis.vertex(0, -1);
    yAxis.vertex(0, 1);


    fontRender.load(Font::defaultFont().c_str(), 60, 1024);

  }

  void onAnimate(double dt) {
   
  }

  void onDraw(Graphics &g) {
    g.clear(0);

    g.camera(Viewpoint::UNIT_ORTHO); // Ortho [0:width] x [0:height]
    g.color(1, 1, 0);
    g.draw(xAxis);

    g.color(0, 1, 1);
    g.draw(yAxis);

    g.color(1, 0, 0);

    float x_label_y_offset = -0.05;

    for (float x=-1.0; x<=1.0; x+=0.2) {
        std::ostringstream oss;
        oss << std::fixed << std::showpoint;
        oss << std::setprecision(1);
        oss << "(" << x << ",0)";
        fontRender.write(oss.str().c_str(), fontSize);
        fontRender.renderAt(g, {x, x_label_y_offset, 0.0});
    }

    // g.meshColor();
    // g.draw(verts2);
  }
};

int main() {
  MyApp app;
  app.fullScreen(true);
  app.start();
}
