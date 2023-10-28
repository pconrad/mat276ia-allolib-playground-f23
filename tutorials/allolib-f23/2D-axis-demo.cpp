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

struct MyApp : public App
{
  Mesh xAxis;
  Mesh yAxis;

  // this font is for rendering text labels on the axes
  FontRenderer fontRender;

  void drawLabelAbove(Graphics &g, float x, float y, float labelOffset, float fontSize)
  {
    std::ostringstream oss;
    oss << std::fixed << std::showpoint;
    oss << std::setprecision(1);
    oss << "(" << x << "," << y << ")";
    fontRender.alignCenter(); // makes the text centered around the position
    fontRender.write(oss.str().c_str(), fontSize);
    fontRender.renderAt(g, {x, y + labelOffset, 0.0});
  }

  void drawLabelRight(Graphics &g, float x, float y, float labelOffset, float fontSize)
  {
    std::ostringstream oss;
    oss << std::fixed << std::showpoint;
    oss << std::setprecision(1);
    oss << "(" << x << "," << y << ")";
    fontRender.alignLeft(); // makes the text centered around the position
    fontRender.write(oss.str().c_str(), fontSize);
    fontRender.renderAt(g, {x + labelOffset, y - (fontSize * 0.2), 0.0});
  }

  void drawAxes(Graphics &g,
                float minX,
                float maxX,
                float minY,
                float maxY,
                float tickIncrement,
                float tickSize,
                float labelOffset,
                float fontSize)
  {
    xAxis.primitive(Mesh::LINE_STRIP);
    yAxis.primitive(Mesh::LINE_STRIP);

    // Create x and y axes
    xAxis.vertex(minX, 0);
    xAxis.vertex(maxX, 0);

    yAxis.vertex(0, minY);
    yAxis.vertex(0, maxY);

    // Create the X tick marks and labels

    for (float x = 0; x < maxX; x += tickIncrement)
    {
      xAxis.vertex(x, 0);
      xAxis.vertex(x, tickSize);
      xAxis.vertex(x, -tickSize);
      xAxis.vertex(x, 0);
      drawLabelAbove(g, x, 0, labelOffset, fontSize);
    }

    for (float x = 0; x >= minX; x -= tickIncrement)
    {
      xAxis.vertex(x, 0);
      xAxis.vertex(x, tickSize);
      xAxis.vertex(x, -tickSize);
      xAxis.vertex(x, 0);
      drawLabelAbove(g, x, 0, labelOffset, fontSize);
    }

    // Create the Y tick marks and labels

    for (float y = 0; y <= maxY; y += tickIncrement)
    {
      yAxis.vertex(0, y);
      yAxis.vertex(tickSize, y);
      yAxis.vertex(-tickSize, y);
      yAxis.vertex(0, y);
      g.color(1, 1, 1);
      drawLabelRight(g, 0, y, labelOffset, fontSize);
    }

    for (float y = 0; y >= minY; y -= tickIncrement)
    {
      yAxis.vertex(0, y);
      yAxis.vertex(tickSize, y);
      yAxis.vertex(-tickSize, y);
      yAxis.vertex(0, y);
      g.color(1, 1, 1);
      drawLabelRight(g, 0, y, labelOffset, fontSize);
    }

    //  Draw the axes
    g.color(1, 1, 0);
    g.draw(xAxis);

    g.color(0, 1, 1);
    g.draw(yAxis);
  }

  void onCreate()
  {
    fontRender.load(Font::defaultFont().c_str(), 60, 1024);
  }

  void onAnimate(double dt)
  {
  }


  void orthoFor2DAxes(Graphics &g)
  {
    // For 2D drawing based on a unit circle, use this:

    g.camera(Viewpoint::ORTHO_FOR_2D); // Ortho [0:width] x [0:height]
    drawAxes(g, -width(), width(), -height(), height(), 100.0, 20, 10, 20);
  }

  void unitOrthoAxes(Graphics &g)
  {
    // For 2D drawing based on a unit circle, use this:

    g.camera(Viewpoint::UNIT_ORTHO); //  fits [-1:1] X [-1:1] inside
    drawAxes(g, -1, 1, -1, 1, 0.2, 0.02, 0.05, 0.05);
  }

  void unitOrthoInclusiveAxes(Graphics &g)
  {
    // For 2D drawing based on a unit circle, use this:

    g.camera(Viewpoint::UNIT_ORTHO_INCLUSIVE); // fits inside [-1:1] X [-1:1]
    drawAxes(g, -1, 1, -1, 1, 0.2, 0.02, 0.05, 0.05);
  }

   void identityAxes(Graphics &g)
  {
    // To treat window as lower left (-1,-1), center (0,0), upper right (1,1),
    // no matter what the size or shape of the window is, use this:

    g.camera(Viewpoint::IDENTITY); // 
    drawAxes(g, -1, 1, -1, 1, 0.2, 0.02, 0.05, 0.05);

    // Note that shapes will be distorted if the window is not square, which 
    // is the normal default case for new windows.
  }


  void onDraw(Graphics &g)
  {
    g.clear(0);

    // uncomment just one of the following

    // orthoFor2DAxes(g);
    // unitOrthoAxes(g);
    // unitOrthoInclusiveAxes(g);
    identityAxes(g);
  }
};

int main()
{
  MyApp app;

  // uncomment next line to automatically go full screen
  // app.fullScreen(true);
  app.start();
}
