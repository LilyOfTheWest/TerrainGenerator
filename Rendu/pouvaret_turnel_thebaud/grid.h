#ifndef GRID_H
#define GRID_H

#include <vector>
#include <math.h>

class Grid {
 public:
  Grid(unsigned int size=1024,float minval=-1.0f,float maxval=1.0f);
  ~Grid();

  inline unsigned int nbVertices() const {return _nbVertices;}
  inline unsigned int nbFaces   () const {return _nbFaces;   }

  inline float *vertices() {return &_vertices[0];}
  inline int   *faces   () {return &_faces[0];   }
  inline float radius  () {return sqrt(2);      }

  unsigned int size();
//  void setSize(unsigned int size);
 
 private:
  unsigned int _nbVertices;
  unsigned int _nbFaces;
  unsigned int _size;

  std::vector<float> _vertices;
  std::vector<int>   _faces;
};

#endif //GRID_H
