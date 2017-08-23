// rasterizer.cpp
// Very simple rasterizer for 2D triangles.
// Currently only renders a single triangle to a character-based framebuffer.
// The rasterizer logic is taken from Fabien Giesen:
// https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/

#include <iostream>
#include <cstring>
#include <cassert>
#include <chrono>
#include <thread>
#include <cmath>

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

static const float targetFrameRate = 60.f; // Hz
static const int targetFrameTime = (int)(1.f / targetFrameRate * 1000000.f); // in microseconds

static const int screenWidth = 64;
static const int screenHeight = 36;

static char frame[screenHeight][screenWidth + 1] = {0};

static const float rotationRate = 0.333; // Hz
static const float rotationAngle = M_PI / targetFrameRate * rotationRate;

struct Point2D {
  int x, y;
};

static const Point2D debugPoint = {-1, -1};

bool pointEq(const Point2D& a, const Point2D& b) {
  return a.x == b.x && a.y == b.y;
}

bool isPointDebugging(const Point2D& p) {
  return pointEq(debugPoint, p);
}

int min(int a, int b) {
  return a < b ? a : b;
}

int min3(int a, int b, int c) {
  if (a < b) {
    return a < c ? a : c;
  } else {
    return b < c ? b : c;
  }
}

int max(int a, int b) {
  return a > b ? a : b;
}

int max3(int a, int b, int c) {
  if (a > b) {
    return a > c ? a : c;
  } else {
    return b > c ? b : c;
  }
}

void clearFrame() {
  for(int y = 0; y < screenHeight; y++) {
    memset(frame[y], ' ', sizeof(' ') * screenWidth);
  }
}

void resetDisplayCursor() {
  for(int y = 0; y < screenHeight; y++) {
    std::cout << "\EM";
  }
}

void displayFrame() {
  for(int y = 0; y < screenHeight; y++) {
    std::cout << frame[y] << std::endl;
  }
}

void renderPixel(const Point2D& p, float w0, float w1, float w2) {
  assert(p.y >= 0 && p.y < screenHeight &&
         p.x >= 0 && p.x < screenWidth);

  if (isPointDebugging(p)) {
    frame[p.y][p.x] = '*';
  } else {
    frame[p.y][p.x] = '#';
  }
}

int orient2d(const Point2D& a, const Point2D& b, const Point2D& c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void drawTri(const Point2D& v0, const Point2D& v1, const Point2D& v2) {
  // Compute triangle bounding box
  int minX = min3(v0.x, v1.x, v2.x);
  int minY = min3(v0.y, v1.y, v2.y);
  int maxX = max3(v0.x, v1.x, v2.x);
  int maxY = max3(v0.y, v1.y, v2.y);

  // Clip against screen bounds
  minX = max(minX, 0);
  minY = max(minY, 0);
  maxX = min(maxX, screenWidth - 1);
  maxY = min(maxY, screenHeight - 1);

  // Rasterize
  const float triArea = (float)orient2d(v0, v1, v2);
  Point2D p;
  for (p.y = minY; p.y <= maxY; p.y++) {
    for (p.x = minX; p.x <= maxX; p.x++) {
      // Determine barycentric coordinates
      float w0 = (float)orient2d(v1, v2, p) / triArea;
      float w1 = (float)orient2d(v2, v0, p) / triArea;
      float w2 = (float)orient2d(v0, v1, p) / triArea;

      if (isPointDebugging(p)) {
        std::cout << "(" << p.x << "," << p.y << ") w0 = " << w0 << " w1 = " << w1 << " w2 = " << w2 << std::endl;
      }

      // If p is on or inside all edges, render pixel
      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        renderPixel(p, w0, w1, w2);
      }
    }
  }
}

Point2D triCentroid(const Point2D& a, const Point2D& b, const Point2D& c) {
  Point2D midpoint = { (int)std::round((a.x + b.x) / 2.f),
                       (int)std::round((a.y + b.y) / 2.f) };

  Point2D centroid = { c.x + (int)std::round((midpoint.x - c.x) * 2.f / 3.f),
                       c.y + (int)std::round((midpoint.y - c.y) * 2.f / 3.f) };

  return centroid;
}

Point2D difference(const Point2D& a, const Point2D& b) {
  return { a.x - b.x, a.y - b.y };
}

Point2D rotateAround(const Point2D& center, const Point2D& p, float angle) {
  const Point2D diff = difference(p, center);

  return { center.x + (int)std::round((float)diff.x * std::cos(angle) - (float)diff.y * std::sin(angle)),
           center.y + (int)std::round((float)diff.y * std::cos(angle) + (float)diff.x * std::sin(angle)) };
}

int main() {
  clearFrame();
  displayFrame();

  const Point2D a_start = { 10, 20 };
  const Point2D b_start = { 25,  1 };
  const Point2D c_start = { 40, 20 };
  const Point2D centroid = triCentroid(a_start, b_start, c_start);

  int numFrames = 0;
  while(numFrames++ >= 0) {
    auto frameStart = high_resolution_clock::now();

    const Point2D a = rotateAround(centroid, a_start, numFrames * rotationAngle);
    const Point2D b = rotateAround(centroid, b_start, numFrames * rotationAngle);
    const Point2D c = rotateAround(centroid, c_start, numFrames * rotationAngle);

    clearFrame();
    drawTri(a, b, c);

    resetDisplayCursor();
    displayFrame();

    std::this_thread::sleep_until(frameStart + microseconds(targetFrameTime));
  }

  return 0;
}
