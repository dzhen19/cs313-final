#include <iostream>
#include <vector>
#include "tinygl-cpp.h"
#include <string>
#include <list>
using namespace tinygl;

struct Vertex
{
  int x;
  int y;
};

struct Stroke
{
  int parentStrokeIdx;
  int parentVertexIdx;
  std::vector<Vertex> vertices;
};

float distance(Vertex a, Vertex b)
{
  return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

class MyWindow : public Window
{
public:
  bool canDraw = false;
  bool mouseDownLogged = false;
  Vertex mouseDownPos = {0, 0};
  Vertex mousePos = {0, 0};
  Vertex closest = {0, 0};

  MyWindow(int w, int h) : Window(w, h) {}
  void setup() override
  {
    std::cout << "Window size: " << width() << ", " << height() << std::endl;
  }

  virtual void mouseMotion(int x, int y, int dx, int dy) override
  {
    mousePos = {x, y};
    if (!canDraw || y < 70)
    {
      return;
    }

    Vertex newPoint = {mousePos.x, mousePos.y};
    currentStroke.vertices.push_back(newPoint);
  }

  virtual void mouseDown(int button, int mods) override
  {
    canDraw = true;
    mouseDownPos = mousePos;
  }

  virtual void mouseUp(int button, int mods) override
  {
    Vertex root = currentStroke.vertices[0];
    if (strokes.size() > 0)
    {
      // find the nearest vertex
      float minDistance = INT_MAX;
      for (int i = 0; i < strokes.size(); i++)
      {
        for (int j = 0; j < strokes[i].vertices.size(); j++)
        {
          float _d = distance(root, strokes[i].vertices[j]);
          if (_d < minDistance)
          {
            closest = strokes[i].vertices[j];
            minDistance = _d;
            currentStroke.parentStrokeIdx = i;
            currentStroke.parentVertexIdx = j;
          }
        }
      }

      int dx = currentStroke.vertices[0].x - closest.x;
      int dy = currentStroke.vertices[0].y - closest.y;

      for (int i = 0; i < currentStroke.vertices.size(); i++)
      {
        currentStroke.vertices[i].x -= dx;
        currentStroke.vertices[i].y -= dy;
      }
    }
    else
    {
      currentStroke.parentStrokeIdx = 0;
      currentStroke.parentVertexIdx = 0;
    }

    strokes.push_back(currentStroke);

    // reset stroke
    currentStroke.vertices.clear();
    currentStroke.parentStrokeIdx = 0;
    currentStroke.parentVertexIdx = 0;
    canDraw = false;
  }

  void keyDown(int key, int mods) override
  {
    if (key == GLFW_KEY_C)
    {
      strokes.clear();
    }

    if (key == GLFW_KEY_S)
    {
      freopen("strokes.txt", "w", stdout);
      for (int i = 0; i < strokes.size(); i++)
      {
        std::cout << "BEGIN_STROKE" << std::endl;
        std::cout << "PARENT_STROKE_ID " << strokes[i].parentStrokeIdx << std::endl;
        std::cout << "PARENT_VERTEX_ID " << strokes[i].parentVertexIdx << std::endl;
        std::cout << "VERTICES ";
        for (int j = 0; j < strokes[i].vertices.size(); j++)
        {
          std::cout << strokes[i].vertices[j].x / width() << " " << strokes[i].vertices[j].y / height() << " ";
        }
        std::cout << std::endl;
        std::cout << "END_STROKE" << std::endl;
      }
    }
  }

  void draw() override
  {

    background(0.95f, 0.95f, 0.95f); // parameters: r, g, b

    // draw ground
    color(0.051, 0.455, 0.102, 1);
    square(width() / 2.0f, 35, width(), 70);

    for (int i = 0; i < strokes.size(); i++)
    {
      for (int j = 0; j < strokes[i].vertices.size(); j++)
      {
        color(0.58, 0.29, 0, 1);
        circle(strokes[i].vertices[j].x, strokes[i].vertices[j].y, 10);
      }
    }

    for (int i = 0; i < currentStroke.vertices.size(); i++)
    {
      color(0, 1, 0, 1);
      circle(currentStroke.vertices[i].x, currentStroke.vertices[i].y, 10);
    }

    color(1, 0, 0, 1);
    circle(closest.x, closest.y, 10);
  }

private:
  std::vector<Stroke> strokes;
  Stroke currentStroke;
};

int main()
{
  MyWindow window(500, 500);
  window.run();
}
