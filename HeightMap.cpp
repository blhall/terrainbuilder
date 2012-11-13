#include <iostream>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <queue>
#include <cstdlib>
using namespace std;

class HeightMap {
  private:
    int size;
    int center_cell;
    float high_value;
    float low_value;
    int mid_value;
    vector<vector<int> > map;
  public:
    HeightMap(int sze, float hv, float lv) {
      size = sze;
      high_value = hv;
      low_value = lv;
      mid_value = floor((low_value + high_value) / 2);
      center_cell = floor(size / 2);
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

    float getCell(int x, int y) {
      return map[x][y];
    }

    float getHeight(int x, int y) {
      return map[x][y];
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

    void setCell(int x, int y, float h) {
      map[x][y] = h;
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
    void cleanUp() {        
      //Initialize vector as empty
      for ( int x = 0; x < getSize(); x++ ) {
        map.push_back ( vector<int>() );
        for ( int y = 0; y < getSize(); y++ )
          map[x].push_back (0);
      }

      //Clear job queue
      while(!jobs.empty()) {
        this->jobs.pop();
      }

      setNW(mid_value);
      setNE(mid_value);
      setSW(mid_value);
      setSE(mid_value);

      Point mypoint;

      mypoint.left = 0;
      mypoint.top = 0;
      mypoint.right = getSize() - 1;
      mypoint.bottom = getSize() - 1;
      mypoint.height = mid_value;
      this->jobs.push(mypoint);
    }

    void run() {
      while(!this->jobs.empty()) {
        step();
      }
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
    }



    void diamond_square(int left, int top, int right, int bottom, float base_height) {
      int x_center = floor(left + right) / 2;
      int y_center = floor(top + bottom) / 2;

      float center_point_height = ((
            getCell(left, top) +
            getCell(right, top) +
            getCell(left, bottom) +
            getCell(right, bottom)
            ) / 4
          ) - (floor( rand() - 0.5) * base_height * 2);

      setCell(x_center, y_center, center_point_height);

      setCell(x_center, top,    floor(getCell(left, top)   + getCell(right, top   ) / 2 + ((rand() - 0.5) * base_height)));
      setCell(x_center, bottom, floor(getCell(left, bottom)+ getCell(right, bottom) / 2 + ((rand() - 0.5) * base_height)));
      setCell(left, y_center,   floor(getCell(left, top)   + getCell(left,  bottom) / 2 + ((rand() - 0.5) * base_height)));
      setCell(right, y_center,  floor(getCell(right, top)  + getCell(right, bottom) / 2 + ((rand() - 0.5) * base_height)));

      if(right - left > 2) {
        base_height = floor(pow(base_height, 2.0) - 0.75);

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
};
