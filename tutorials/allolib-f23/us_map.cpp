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
#include <fstream>
#include <vector>

#include "al/graphics/al_Font.hpp" // for text rendering

// Drawing on a 2D canvas using pixel coordinates

using namespace al;

std::vector<std::vector<Vec3f>> points;

struct MyApp : public App
{
    Mesh xAxis;
    Mesh yAxis;

    // this font is for rendering text labels on the axes
    FontRenderer fontRender;

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


    void drawUnitedStates(Graphics &g) {
        for (auto shape : points) {
            Mesh m;
            m.primitive(Mesh::LINE_STRIP);
            for (auto p : shape) {
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

    void onCreate()
    {
        fontRender.load(Font::defaultFont().c_str(), 60, 1024);
    }

    void onAnimate(double dt)
    {
    }

    void onDraw(Graphics &g)
    {
        g.clear(0);

        // uncomment just one of the following

        // orthoFor2DAxes(g);
        // unitOrthoAxes(g);
        // unitOrthoInclusiveAxes(g);

        g.camera(Viewpoint::IDENTITY);

        float xScale = 180.0;
        float yScale = 90.0;

        g.scale(1.0 / xScale, 1.0 / yScale, 1.0);

        float fontSize = 10.0;
        float tickSize = 5.0;
        float labelOffset = 5.0;

        drawYAxis(g, -90, 90, 30.0, tickSize, labelOffset, fontSize);
        drawXAxis(g, -180, 180, 30.0, tickSize, labelOffset, fontSize);
        drawUnitedStates(g);
    }
};


std::vector<std::string> split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

int readPoints()
{
    // file is taken from KML file with only the <coordinates> sections pulled out
    // and then the <coordinates> open/close tags put on their own lines
    // and each trio of x,y,z put on a separate line

    std::ifstream infile("../united_states.dat");
    std::string line;
    std::vector<Vec3f> thisShape;

    while (std::getline(infile, line))
    {
        if (line == "<coordinates>") 
            continue;
        if (line == "</coordinates>")  {
            points.push_back(thisShape);
            thisShape.clear();
            continue;
        }
            
        auto parts = split(line, ',');
        float x = std::stof(parts[0]);
        float y = std::stof(parts[1]);
        float z = std::stof(parts[2]);
        thisShape.push_back(Vec3f(x, y, z));
    }
    infile.close();
}

int main()
{

    readPoints();

    std::cout << "Read " << points.size() << " shapes from united_states.dat" << std::endl;

    MyApp app;

    // uncomment next line to automatically go full screen
    // app.fullScreen(true);
    app.start();
}
