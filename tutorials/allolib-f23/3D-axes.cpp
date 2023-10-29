#include "al/app/al_App.hpp"
#include "al/graphics/al_Font.hpp"
#include "al/graphics/al_Shapes.hpp" // for addSphere()

#include <iostream>
#include <iomanip>

using namespace al;

struct MyApp : public App {

  Mesh sphere;  

  Mesh xAxis;
  Mesh yAxis;
  Mesh zAxis;
  FontRenderer fontRender;

  void drawLabelXAxis(Graphics &g, float x, float labelOffset, float fontSize)
  {
    std::ostringstream oss;
    oss << std::fixed << std::showpoint;
    oss << std::setprecision(1);
    oss << "x=" << x;
    fontRender.alignCenter(); // makes the text centered around the position
    fontRender.write(oss.str().c_str(), fontSize);
    fontRender.renderAt(g, {x, labelOffset, 0.0});
  }

  void drawLabelYAxis(Graphics &g, float y, float labelOffset, float fontSize)
  {
    std::ostringstream oss;
    oss << std::fixed << std::showpoint;
    oss << std::setprecision(1);
    oss << "y=" << y;
    fontRender.alignLeft(); // makes the text centered around the position
    fontRender.write(oss.str().c_str(), fontSize);
    fontRender.renderAt(g, {labelOffset, y - (fontSize * 0.2), 0.0});
  }

   void drawLabelZAxis(Graphics &g, float z, float labelOffset, float fontSize)
  {
    std::ostringstream oss;
    oss << std::fixed << std::showpoint;
    oss << std::setprecision(1);
    oss << "z=" << z;
    fontRender.alignLeft(); // makes the text centered around the position
    fontRender.write(oss.str().c_str(), fontSize);
    fontRender.renderAt(g, {labelOffset, - (fontSize * 0.2), z});
  }

  void draw3DAxes(Graphics &g,
                float dim,
                float tickIncrement,
                float tickSize,
                float labelOffset,
                float fontSize)
  {
    xAxis.primitive(Mesh::LINE_STRIP);
    yAxis.primitive(Mesh::LINE_STRIP);
    zAxis.primitive(Mesh::LINE_STRIP);

    // Create x and y axes
    xAxis.vertex(-dim, 0, 0);
    xAxis.vertex(dim, 0, 0);

    yAxis.vertex(0, -dim, 0);
    yAxis.vertex(0, dim, 0);
    
    zAxis.vertex(0, 0, -dim);
    zAxis.vertex(0, 0, dim);
    
    // Create the X tick marks and labels

    for (float x = 0; x < dim; x += tickIncrement)
    {
      xAxis.vertex(x, 0);
      xAxis.vertex(x, tickSize);
      xAxis.vertex(x, -tickSize);
      xAxis.vertex(x, 0);
      drawLabelXAxis(g, x, labelOffset, fontSize);
    }

    for (float x = 0; x >= -dim; x -= tickIncrement)
    {
      xAxis.vertex(x, 0);
      xAxis.vertex(x, tickSize);
      xAxis.vertex(x, -tickSize);
      xAxis.vertex(x, 0);
      drawLabelXAxis(g, x, labelOffset, fontSize);
    }

    // Create the Y tick marks and labels

    for (float y = 0; y <= dim; y += tickIncrement)
    {
      yAxis.vertex(0, y);
      yAxis.vertex(tickSize, y);
      yAxis.vertex(-tickSize, y);
      yAxis.vertex(0, y);
      g.color(0, 1, 0);
      drawLabelYAxis(g, y, labelOffset, fontSize);
    }

    for (float y = 0; y >= -dim; y -= tickIncrement)
    {
      yAxis.vertex(0, y);
      yAxis.vertex(tickSize, y);
      yAxis.vertex(-tickSize, y);
      yAxis.vertex(0, y);
      g.color(0, 1, 0);
      drawLabelYAxis(g, y, labelOffset, fontSize);
    }

      // Create the Z tick marks and labels

    for (float z = 0; z <= dim; z += tickIncrement)
    {
      zAxis.vertex(0, 0, z);
      zAxis.vertex(tickSize, 0, z);
      zAxis.vertex(-tickSize, 0, z);
      zAxis.vertex(0, 0,  z);
      g.color(0, 0, 1);
      drawLabelZAxis(g, z, labelOffset, fontSize);
    }

    for (float z = 0; z >= -dim; z -= tickIncrement)
    {
      zAxis.vertex(0, 0, z);
      zAxis.vertex(tickSize, 0, z);
      zAxis.vertex(-tickSize, 0, z);
      zAxis.vertex(0, 0,  z);
      g.color(0, 0, 1);
      drawLabelZAxis(g, z, labelOffset, fontSize);
    }

    //  Draw the axes, using gnomon color convention 
    g.color(1, 0, 0);  // x is red
    g.draw(xAxis);

    g.color(0, 1, 0); // y is green
    g.draw(yAxis);

    g.color(0, 0, 1); // z is blue
    g.draw(zAxis);
  }

  void onCreate()
  {
    fontRender.load(Font::defaultFont().c_str(), 60, 1024);
    addSphere(sphere, 0.1);
    nav().pullBack(10);
    nav().faceToward({-1, 1, -1}, {0, 0, 0});
  }

  void onDraw(Graphics& g) {
    g.clear();
    
    draw3DAxes(g, 5, 1, 0.1, 0.1, 0.25);

    g.color(0.5,0.5,0);
    g.draw(sphere);
  }
};

int main() {
  std::cout << "Use the arrow keys, and q/z to rotate the camera" << std::endl;
  std::cout << "Use w and x to zoom in and out" << std::endl;
  std::cout << "Use a and d to move left and right" << std::endl;
  std::cout << "Use e and c to move up and down" << std::endl;
  std::cout << "Use ` to reset the camera" << std::endl;

  MyApp app;
  app.start();
  return 0;
}
