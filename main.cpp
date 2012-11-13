#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <queue>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "vec3f.h"

using namespace std;

struct Tile {
  float nw;
  float ne;
  float sw;
  float se;
};

class HeightMap {
  private:
    int size;
    int center_cell;
    float high_value;
    float low_value;
    int mid_value;
    float** map;
    int l;
    int w;
    bool computedNormals;
    Vec3f** normals;
  public:
    HeightMap(int sze, float hv, float lv) {
      //printf("Initializing Height Map\n");
      size = sze;
      l = size;
      w = size;
      high_value = hv;
      low_value = lv;
      mid_value = floor((low_value + high_value) / 2);
      center_cell = floor(size / 2);

      //printf("Map %d \n", l);
      map = new float*[l];
      //printf("Map2\n");
      for(int x = 0; x < l; x++) {
        //printf("Map %d \n", i);
        map[x] = new float[w];
      }

      //printf("Vec3f\n");
      normals = new Vec3f*[l];
      for(int i = 0; i < l; i++) {
        normals[i] = new Vec3f[w];
      }

      computedNormals = false;
    }

    struct Point {
      int left;
      int top;
      int right;
      int bottom;
      float height;
    };

    queue<Point> jobs;

    int getSize() {
      return size;
    }

    int width() {
      return size;
    }

    int length() {
      return size;
    }

    float getCell(int x, int z) {
      return map[x][z];
    }

    float getHeight(int x, int z) {
      return map[x][z];
    }

    int CenterCell() {
      return center_cell;
    }

    float highValue() {
      return high_value;
    }

    float lowValue() {
      return low_value;
    }

    int midValue() {
      return mid_value;
    }

    void setCell(int x, int z, float y) {
      if(y < 0) {
        y = 0.0f;
      }
      //printf("Setting (%d,%d) = %f\n",x,z,y);
      map[x][z] = y;
    }

    void setNW(float h) {
      setCell(0, 0, h);
    }

    void setNE(float h) {
      setCell(0, getSize() - 1, h);
    }

    void setSW(float h) {
      setCell(getSize() - 1, 0, h);
    }

    void setSE(float h) {
      setCell(getSize() - 1, getSize() - 1, h);
    }

    void setCenter(float h) {
      setCell(center_cell, center_cell, h);
    }

    bool isMountain(int x, int y) {
      return true;
    }
    void setMountain(int x, int y) {
    }

    Tile getTile(int x, int y) {
      Tile mytile;
      mytile.nw = getCell(x,   y  );
      mytile.ne = getCell(x+1, y  );
      mytile.sw = getCell(x,   y+1);
      mytile.se = getCell(x+1, y+1);

      return mytile;
    }

    void cleanUp() {
      //printf("Running Cleanup\n");
      //Initialize array

      //Clear job queue
      while(!this->jobs.empty()) {
        this->jobs.pop();
      }

      //printf("Jobs Clear.\n");

      setNW(midValue());
      setNE(midValue());
      setSW(midValue());
      setSE(midValue());

      Point mypoint;

      mypoint.left = 0;
      mypoint.top = 0;
      mypoint.right = getSize() - 1;
      mypoint.bottom = getSize() - 1;
      mypoint.height = midValue();
      this->jobs.push(mypoint);
      //printf("Finished cleaning\n");
    }

    void run() {
      while(!this->jobs.empty()) {
        // printf("Stepping\n");
        step();
      }
      //printf("Done stepping\n");
      return;
    }

    void step() {
      //Take top of queue off and process it
      Point mypoint = this->jobs.front();
      diamond_square(
          mypoint.left,
          mypoint.top, 
          mypoint.right, 
          mypoint.bottom, 
          mypoint.height
          );
      this->jobs.pop();
      //printf("Job queue at:%d\n", this->jobs.size());
    }

    void diamond_square(int left, int top, int right, int bottom, float base_height) {
      int x_center = floor((left + right) / 2);
      int y_center = floor((top + bottom) / 2);

      float center_point_height = ((
            getCell(left, top) +
            getCell(right, top) +
            getCell(left, bottom) +
            getCell(right, bottom)
            ) / 4
          ) - (floor( (rand()/RAND_MAX) - 0.5) * base_height * 2);
      //printf("CenterPoint height %f\n",center_point_height);

      if(center_point_height < 0) {
        center_point_height = -center_point_height;
      }

      setCell(x_center, y_center, center_point_height);

      setCell(x_center, top,    floor(getCell(left, top)   + getCell(right, top   ) / 2 + ((rand()/RAND_MAX - 0.5) * base_height)));
      setCell(x_center, bottom, floor(getCell(left, bottom)+ getCell(right, bottom) / 2 + ((rand()/RAND_MAX - 0.5) * base_height)));
      setCell(left, y_center,   floor(getCell(left, top)   + getCell(left,  bottom) / 2 + ((rand()/RAND_MAX - 0.5) * base_height)));
      setCell(right, y_center,  floor(getCell(right, top)  + getCell(right, bottom) / 2 + ((rand()/RAND_MAX - 0.5) * base_height)));

      if(right - left > 2) {
        base_height = floor(pow(base_height, 2.0) - 0.75);
        //printf ("New base height %f\n", base_height);
        Point mypoint;          

        //diamond_square(left, top, x_center, y_center, base_height ));
        mypoint.left = left;
        mypoint.top = top;
        mypoint.right = x_center;
        mypoint.bottom = y_center;
        mypoint.height = base_height;
        this->jobs.push(mypoint);

        //diamond_square(x_center, top, right, y_center, base_height));
        mypoint.left = x_center;
        mypoint.top = top;
        mypoint.right = right;
        mypoint.bottom = y_center;
        mypoint.height = base_height;
        this->jobs.push(mypoint);

        //diamond_square(left, y_center, x_center, bottom, base_height));
        mypoint.left = left;
        mypoint.top = y_center;
        mypoint.right = x_center;
        mypoint.bottom = bottom;
        mypoint.height = base_height;
        this->jobs.push(mypoint);

        //diamond_square(x_center, y_center, right, bottom, base_height));
        mypoint.left = x_center;
        mypoint.top = y_center;
        mypoint.right = right;
        mypoint.bottom = bottom;
        mypoint.height = base_height;
        this->jobs.push(mypoint);
      }
    }
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
            out = Vec3f(0.0f, map[z - 1][x] - map[z][x], -1.0f);
          }
          Vec3f in;
          if (z < l - 1) {
            in = Vec3f(0.0f, map[z + 1][x] - map[z][x], 1.0f);
          }
          Vec3f left;
          if (x > 0) {
            left = Vec3f(-1.0f, map[z][x - 1] - map[z][x], 0.0f);
          }
          Vec3f right;
          if (x < w - 1) {
            right = Vec3f(1.0f, map[z][x + 1] - map[z][x], 0.0f);
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
//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
HeightMap* loadMap(int n) {
  int size = (pow(2,n)+1);
  int h_value = 2;
  int l_value = 1;
  HeightMap* t = new HeightMap(size, h_value, l_value);
  t->cleanUp();
  t->run();
  //printf("Returning\n");
  //t->computeNormals();
  return t;
};

void buildRiver(HeightMap* t) {
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
        t->setCell(xr, y, riverHeight);
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
        t->setCell(x, yr, riverHeight);
      }
    }
  }
};

void buildMountain(HeightMap* t) {
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
  t->setCell(centerX, centerY, mHeight); //Set cap
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
            t->setCell(x, y, h);
            t->setMountain(x,y);
          }
          else {
            float eh = t->getHeight(x,y);
            eh = ((h + eh) * .5f);
            t->setCell(x, y, eh);
          }
        }
      }
    }
  }
};

float _angle = 60.0f;
float viewAngle = 30.0f;
HeightMap* _terrain;
float scalecnt = 40.f;

void cleanup() {
  printf("Deleting terrain\n");
  delete _terrain;
}

void specialFunc( int key, int x, int y) {
  int modifiers;
  switch(key) {
    case GLUT_KEY_UP :
      modifiers = glutGetModifiers();
      if (modifiers == (GLUT_ACTIVE_SHIFT)) {
        viewAngle += 10.0f;
      }
      else {
        scalecnt += 10.0f;
      }
      break;

    case GLUT_KEY_DOWN :
      modifiers = glutGetModifiers();
      if (modifiers == (GLUT_ACTIVE_SHIFT)) {
        viewAngle -= 10.0f;
      }
      else {
        scalecnt -= 10.0f;
      }
      break;

    case GLUT_KEY_LEFT :
      _angle = 10.0f + _angle;
      break;

    case GLUT_KEY_RIGHT :
      _angle -= 10.0f;
      break;
  }
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
  gluPerspective(45.0f, (double)w / (double)h, 1.0, 200.0);
}
void draw_ground(int x, int z) {
  int nwx = x;
  int nwz = z;
  int nex = x + 1;
  int nez = z;
  int sex = x + 1;
  int sez = z + 1;
  int swx = x;
  int swz = z + 1;
  //this should draw the ground on top of the triangles instead of the triangles
  Tile mytile = _terrain->getTile(x,z);

  glVertex3f(nwx, mytile.nw, nwz);
  glVertex3f(nex, mytile.ne, nez);
  glVertex3f(sex, mytile.se, sez);
  glVertex3f(swx, mytile.sw, swz);
}
void draw_triangle(int x, int z) {
  int nwx = x;
  int nwz = z;
  int nex = x + 1;
  int nez = z;
  int sex = x + 1;
  int sez = z + 1;
  int swx = x;
  int swz = z + 1;
  int centerx = floor((nwx+sex)/2);
  int centerz = floor((nwz+sez)/2);

  //Previous was (X, Y, Z);
  //Begin first Triangle
  //Draw the tile as two polygons, a polygon for the back triangle, NW -> NE -> SW -> NW
  //NW->NE->CENTER
  //glVertex3f(x(width),y(height),z(length/depth));
  glVertex3f(nwx, _terrain->getCell(nwx,nwz), nwz);
  glVertex3f(nex, _terrain->getCell(nex,nez), nez);
  glVertex3f(centerx, _terrain->getCell(centerx,centerz), centerz);
  //NE->SE->CENTER
  glVertex3f(nex, _terrain->getCell(nex,nez), nez);
  glVertex3f(sex, _terrain->getCell(sex,sez), sez);
  glVertex3f(centerx, _terrain->getCell(centerx,centerz), centerz);
  //SE->SW->CENTER
  glVertex3f(sex, _terrain->getCell(sex,sez), sez);
  glVertex3f(swx, _terrain->getCell(swx,swz), swz);
  glVertex3f(centerx, _terrain->getCell(centerx,centerz),centerz);
  //SW->NW->CENTER
  glVertex3f(swx, _terrain->getCell(swx,swz), swz);
  glVertex3f(nwx, _terrain->getCell(nwx,nwz), nwz);
  glVertex3f(centerx, _terrain->getCell(centerx,centerz), centerz);
}
void drawScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -100.0f);
  //Angle of Map
  glRotatef(viewAngle, 1.0f, 0.0f, 0.0f);
  //Rotation
  glRotatef(-_angle, 0.0f, 1.0f, 0.0f);

  GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

  GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
  GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

  float scale = scalecnt / max(_terrain->width() - 1, _terrain->length() - 1);
  glScalef(scale, scale, scale);
  glTranslatef(-(float)(_terrain->width() - 1) / 2,
      0.0f,
      -(float)(_terrain->length() - 1) / 2);

  for(int z = 0; z < _terrain->length() - 1; z++) {
    for(int x = 0; x < _terrain->width() - 1; x++) {
      //Forms hills
      glBegin(GL_TRIANGLE_STRIP);
      glColor3f(0.3f, 0.9f, 0.0f);
      draw_triangle(x,z);
      glEnd();

      //Should connect points, currently not working
      glBegin(GL_QUADS);
      glColor3f(1.0f, 1.0f, 1.0f);
      //draw_ground(x,z);
      glEnd();

      //Forms outline
      glBegin(GL_LINES);
      //glColor3f(0.3f, 0.9f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f );
      draw_triangle(x,z);
      //draw_ground(x,z);
      glEnd();
    }
  }
  //printf("Done drawing\n");

  glutSwapBuffers();
}
void update(int value) {
  //printf("Updating\n");
  if (_angle > 360) {
    _angle -= 360;
  }
  
  glutPostRedisplay();
  glutTimerFunc(25, update, 0);
}
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);

  glutCreateWindow("TerrainBuilder");
  initRendering();

  _terrain = loadMap(5);

  //printf("Draw scene\n");
  glutDisplayFunc(drawScene);
  //printf("KeyPress\n");
  glutKeyboardFunc(handleKeypress);
  glutSpecialFunc(specialFunc);
  //printf("handleResize\n");
  glutReshapeFunc(handleResize);
  //printf("Update Call\n");
  glutTimerFunc(25, update, 0);

  glutMainLoop();
  return 0;
}
