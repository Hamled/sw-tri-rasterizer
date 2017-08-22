// rasterizer.cpp
// Very simple rasterizer for 2D triangles.
// The rasterizer logic is taken from Fabien Giesen:
// https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/

static const int screenWidth = 64;
static const int screenHeight = 36;

struct Point2D {
  int x, y;
};

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
  maxX = max(maxX, screenWidth - 1);
  maxY = max(maxY, screenHeight - 1);

  // Rasterize
  Point2D p;
  for (p.y = minY; p.y <= maxY; p.y++) {
    for (p.x = minX; p.x <= maxX; p.x++) {
      // Determine barycentric coordinates
      float w0 = (float)orient2d(v1, v2, p);
      float w1 = (float)orient2d(v2, v0, p);
      float w2 = (float)orient2d(v0, v1, p);

      // If p is on or inside all edges, render pixel
      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        /* renderPixel(p, w0, w1, w2); */
      }
    }
  }
}

int main() {
  return 0;
}
