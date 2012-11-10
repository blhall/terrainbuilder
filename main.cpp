/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "vec3f.h"

using namespace std;

//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
  private:
    int w; //Width
    int l; //Length
    float** hs; //Heights
    bool** ms; //Mountains
    Vec3f** normals;
    bool computedNormals; //Whether normals is up-to-date
  public:
    Terrain(int w2, int l2) {
      w = w2;
      l = l2;

      //for(float il = 0.0f; il < l; il += 0.1f) {
      //  for(float iw = 0.0f; iw < w; iw += 0.1f) {
      //    ms[il][iw] = false;
      //  }
      //}
      ms = new bool*[l];
      for(int i = 0; i < l; i++) {
        ms[i] = new bool[w];
      }

      hs = new float*[l];
      for(int i = 0; i < l; i++) {
        hs[i] = new float[w];
      }

      normals = new Vec3f*[l];
      for(int i = 0; i < l; i++) {
        normals[i] = new Vec3f[w];
      }

      computedNormals = false;
    }

    ~Terrain() {
      for(int i = 0; i < l; i++) {
        delete[] hs[i];
      }
      delete[] hs;

      for(int i = 0; i < l; i++) {
        delete[] normals[i];
      }
      delete[] normals;
    }

    int width() {
      return w;
    }

    int length() {
      return l;
    }

    //Set mountain
    void setMountain(int x, int y) {
      ms[x][y] = true;
    }

    //Return Mountain
    bool isMountain(int x, int y) {
      return ms[x][y];
    }

    //Sets the height at (x, z) to y
    void setHeight(int x, int y, float z) {
      if(x < w && y < l) {
        hs[x][y] = z;
        computedNormals = false;
      }
    }

    //Returns the height at (x, z)
    float getHeight(int x, int y) {
      return hs[x][y];
    }

    float getMinHeight(int x, int y) {
      // This will return the Min and Max height of surrounding points
      float min = 1.0f;
      float h;
      for(int xa = -1; xa < 1; xa++) {
        for(int ya = -1; ya < 1; ya++) {
          float xh = x + xa;
          float yh = y + ya;
          if(xh < 0) {
            xh = 0;
          }
          if (yh < 0) {
            yh = 0;
          }
          h = getHeight(xh, yh);
          if(h < min) {
            min = h;
          }
        }
      }
      return min;
    }

    float getMaxHeight(int x, int y) {
      // This will return the Min and Max height of surrounding points
      float max = 1.0f;
      int h;
      for(int xa = -1; xa < 1; xa++) {
        for(int ya = -1; ya < 1; ya++) {
          float xh = x + xa;
          float yh = y + ya;
          if(xh < 0) {
            xh = 0;
          }
          if (yh < 0) {
            yh = 0;
          }
          h = getHeight(xh, yh);
          if(h > max) {
            max = h;
          }
        }
      }
      return max;
    }
    //Computes the normals, if they haven't been computed yet
    void computeNormals() {
      if (computedNormals) {
        return;
      }

      //Compute the rough version of the normals
      Vec3f** normals2 = new Vec3f*[l];
      for(int i = 0; i < l; i++) {
        normals2[i] = new Vec3f[w];
      }

      for(int z = 0; z < l; z++) {
        for(int x = 0; x < w; x++) {
          Vec3f sum(0.0f, 0.0f, 0.0f);

          Vec3f out;
          if (z > 0) {
            out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
          }
          Vec3f in;
          if (z < l - 1) {
            in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
          }
          Vec3f left;
          if (x > 0) {
            left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
          }
          Vec3f right;
          if (x < w - 1) {
            right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
          }

          if (x > 0 && z > 0) {
            sum += out.cross(left).normalize();
          }
          if (x > 0 && z < l - 1) {
            sum += left.cross(in).normalize();
          }
          if (x < w - 1 && z < l - 1) {
            sum += in.cross(right).normalize();
          }
          if (x < w - 1 && z > 0) {
            sum += right.cross(out).normalize();
          }

          normals2[z][x] = sum;
        }
      }

      //Smooth out the normals
      const float FALLOUT_RATIO = 0.5f;
      for(int z = 0; z < l; z++) {
        for(int x = 0; x < w; x++) {
          Vec3f sum = normals2[z][x];

          if (x > 0) {
            sum += normals2[z][x - 1] * FALLOUT_RATIO;
          }
          if (x < w - 1) {
            sum += normals2[z][x + 1] * FALLOUT_RATIO;
          }
          if (z > 0) {
            sum += normals2[z - 1][x] * FALLOUT_RATIO;
          }
          if (z < l - 1) {
            sum += normals2[z + 1][x] * FALLOUT_RATIO;
          }

          if (sum.magnitude() == 0) {
            sum = Vec3f(0.0f, 1.0f, 0.0f);
          }
          normals[z][x] = sum;
        }
      }

      for(int i = 0; i < l; i++) {
        delete[] normals2[i];
      }
      delete[] normals2;

      computedNormals = true;
    }

    //Returns the normal at (x, z)
    Vec3f getNormal(int x, int z) {
      if (!computedNormals) {
        computeNormals();
      }
      return normals[z][x];
    }
};

void buildTerrain(Terrain* t) {
  float min,max;
  for(int y = 0; y < t->length(); y++) {
    for(int x = 0; x < t->width(); x++) {
      min = t->getMinHeight(x,y) * 0.5f;
      max = t->getMaxHeight(x,y) * 1.5f;
      float h = min + (float)rand()/((float)RAND_MAX/(max-min));
      //printf ("Max= %f, Min= %f, H= %f\n", max,min,h);


      t->setHeight(x,y,h); //Center
    }
  }
}

void squareStep(Terrain* t, int xStart, int yStart, int d, float height) {
  d--;
  int xMax = d;
  int yMax = d;
  int xCenter = xMax/2;
  int yCenter = yMax/2;
  int min = -height;
  int max = height;
  float h = min + (float)rand()/((float)RAND_MAX/(max-min));
  t->setHeight(xStart,yStart,h); //Lower Left
  t->setHeight(xStart,yMax,h); //Upper Left
  t->setHeight(xMax,yStart,h); //Lower Right
  t->setHeight(xMax,yMax,h); //Upper Right
  h = h + 1;
  t->setHeight(xCenter, yCenter, h); //Center
};

void diamondStep(Terrain* t, int xStart, int yStart, int d, float height) { 
  d--;
  int xMax = d/2;
  int yMax = d/2;
  int xCenter = xMax/2;
  int yCenter = yMax/2;
  int min = -height;
  int max = height;
  float h = min + (float)rand()/((float)RAND_MAX/(max-min));
  t->setHeight(xMax/2,yStart,h); //Bottom
  t->setHeight(xStart,yMax/2,h); //Left
  t->setHeight(xMax/2,yMax,h); //Top
  t->setHeight(xMax,yMax/2,h); //Right
  h = h + 1;
  t->setHeight(xCenter, yCenter, h); //Center}
};

void buildRiver(Terrain* t) {
  //Build River
  int riverAxis = rand() % 2 + 1;
  printf ("River Axis %d\n",riverAxis);
  if (riverAxis == 1) {
    float riverHeight = -5.0;
    int   riverStart  = rand() % 161 + 20;
    int   riverWidth  = rand() % 16 + 6;
    int   riverAmp    = rand() % 9 + 2;
    double riverCycle  = (rand()% 10 + 1) * .01;
    int   riverEnd    = riverStart + riverWidth;

    printf ("riverAmp = %d riverCycle = %f\n", riverAmp, riverCycle);
    for(int x = riverStart; x < riverEnd; x++) {
      for(double y = 0.0; y < t->length(); y += 0.1) {
        double xr = riverAmp * sin(riverCycle * y) + riverStart;
        t->setHeight(xr, y, riverHeight);
      }
    } 
  }
  else if (riverAxis == 2) {
    float riverHeight = -5.0;
    int   riverStart  = rand() % 161 + 20;
    int   riverWidth  = rand() % 16 + 6;
    int   riverAmp    = rand() % 9 + 2;
    double riverCycle  = (rand()% 10 + 1) * .01;
    int   riverEnd    = riverStart + riverWidth;

    printf ("riverAmp = %d riverCycle = %f\n", riverAmp, riverCycle);
    for(double x = 0.0; x < t->width(); x += 0.1) {
      for(int y = riverStart; y < riverEnd; y++) {
        double yr = riverAmp * sin(riverCycle * x) + riverStart;
        t->setHeight(x, yr, riverHeight);
      }
    }
  }
};

void buildMountain(Terrain* t) {
  //Build Mountains
  //Point of Origin
  //start at angle 1.0 loop through find x,y and setting height
  //then add 1.0 to angle
  float mHeight     = 20.0f;
  float maxCurc     = 20.0f;
  int   centerX     = (int)(rand()/(RAND_MAX/(t->width()-maxCurc)));
  int   centerY     = (int)(rand()/(RAND_MAX/(t->length()-maxCurc)));
  float h           = mHeight;

  //printf ("%f,%f,%f\n", centerX,centerY,mHeight);
  t->setHeight(centerX, centerY, mHeight); //Set cap
  for(double mDistance = 1.0f; mDistance < maxCurc; mDistance += 1.0f) {
    for(double mAngle = 1.0f; mAngle < 360; mAngle += 1.0f) {
      int x = (int)(centerX + mDistance * cos(mAngle));
      int y = (int)(centerY + mDistance * sin(mAngle));
      //printf ("%f,%f,%f\n", x,y,h);
      h = mHeight - mDistance;
      if (h < 0) {
        h = 0;
      }
      if(y > 0 && y < t->length()) {
        if(x > 0 && x < t->width()) {
          if(!t->isMountain(x,y)) {
            t->setHeight(x, y, h);
            t->setMountain(x,y);
          }
          else {
            float eh = t->getHeight(x,y);
            eh = ((h + eh) * .5f);
            t->setHeight(x, y, eh);
          }
        }
      }
    }
  }
};

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(float width, float length) {
  //Build basic terrain
  Terrain* t = new Terrain(width, length);
  for(int y = 0; y < length; y++) {
    for(int x = 0; x < width; x++) {
      float min = -2.0f;
      float max =  3.0f;
      float h = 1.0f;//min + (float)rand()/((float)RAND_MAX/(max-min));
      t->setHeight(x, y, h);
    }
  }

  //squareStep(Terrain* t, int xStart, int yStart, int d, float height)
  int d = 20;
  int h = 10;
  //for(int x = 0; x < 20; x++) {
    //for(int y = 0; y < 20; y++) {
      squareStep( t, 5, 5, d, h);
      diamondStep(t, 5, 5, d, h);
      d--;
      h--;
    //}
  //}

  int numMount = rand() % 20 + 1;
  printf ("Making %d mountains.\n", numMount);
  for(int i = 0; i < numMount; i++) {
    //buildMountain(t);
  }

  int numRivers = rand() % 3 + 1;
  printf ("Making %d rivers.\n", numRivers);
  for(int i = 0; i < numRivers; i++) {
    //buildRiver(t);
  }

  int terPasses = 20;
  printf ("Generating terrain.\n");
  for (int i = 0; i < terPasses; i++) {
    //buildTerrain(t);
  }

  t->computeNormals();
  return t;
}



float _angle = 60.0f;
Terrain* _terrain;

void cleanup() {
  delete _terrain;
}

void handleKeypress(unsigned char key, int x, int y) {
  switch (key) {
    case 27: //Escape key
      cleanup();
      exit(0);
  }
}

void initRendering() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void drawScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -10.0f);
  glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(-_angle, 0.0f, 1.0f, 0.0f);

  GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

  GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
  GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

  float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
  glScalef(scale, scale, scale);
  glTranslatef(-(float)(_terrain->width() - 1) / 2,
      0.0f,
      -(float)(_terrain->length() - 1) / 2);

  glColor3f(0.3f, 0.9f, 0.0f);
  for(int z = 0; z < _terrain->length() - 1; z++) {
    //Makes OpenGL draw a triangle at every three consecutive vertices
    glBegin(GL_TRIANGLE_STRIP);
    for(int x = 0; x < _terrain->width(); x++) {
      Vec3f normal = _terrain->getNormal(x, z);
      glNormal3f(normal[0], normal[1], normal[2]);
      glVertex3f(x, _terrain->getHeight(x, z), z);
      normal = _terrain->getNormal(x, z + 1);
      glNormal3f(normal[0], normal[1], normal[2]);
      glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
    }
    glEnd();
  }

  glutSwapBuffers();
}

void update(int value) {
  _angle += 1.0f;
  if (_angle > 360) {
    _angle -= 360;
  }

  glutPostRedisplay();
  glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(200, 200);

  glutCreateWindow("TerrainBuilder");
  initRendering();

  _terrain = loadTerrain(100,100);

  glutDisplayFunc(drawScene);
  glutKeyboardFunc(handleKeypress);
  glutReshapeFunc(handleResize);
  glutTimerFunc(25, update, 0);

  glutMainLoop();
  return 0;
}
