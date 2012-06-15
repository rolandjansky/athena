/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OLDHepVector3D_H
#define OLDHepVector3D_H

/*
  2006 CERN  Marcin Nowak

  Set of classes identical (hopefully) to CLHEP v.1.8.2 classes, but with different names.
  Only persistent data members are important, inheritance and the presence of virtual table.
  Methods are not needed.

*/

class OldHep3Vector {
public:
  OldHep3Vector() : dx(0), dy(0), dz(0) {}
  
  inline double	x() const { return dx; } 
  inline double	y() const { return dy; }
  inline double	z() const { return dz; }

  double dx, dy, dz;
};


class OldHepTransform3D {
 public:
  double m1, m2, m3, m4,
         m5, m6, m7, m8,
         m9, m10, m11, m12;

  OldHepTransform3D() :
        m1(0), m2(0), m3(0), m4(0),
        m5(0), m6(0), m7(0), m8(0),
        m9(0), m10(0), m11(0), m12(0) {} 
  virtual ~OldHepTransform3D() {}
};

  
class OldHepRotation {
public:
  OldHepRotation() :
        rxx(0), rxy(0), rxz(0),   ryx(0), ryy(0), ryz(0),   rzx(0), rzy(0), rzz(0) {}
  
  double rxx, rxy, rxz,   ryx, ryy, ryz,   rzx, rzy, rzz;
};


#include "CLHEP/Vector/ThreeVector.h"
class OldBasicVector3D {
public:
  OldBasicVector3D() {}
  virtual ~OldBasicVector3D() {}

  inline double	x() const { return vec.x(); } 
  inline double	y() const { return vec.y(); }
  inline double	z() const { return vec.z(); }

  CLHEP::Hep3Vector	vec;
};





class OldHepVector3D : public OldBasicVector3D {
public:
  OldHepVector3D() {}
  virtual ~OldHepVector3D() {}
};



#include "CLHEP/Geometry/BasicVector3D.h"
typedef HepGeom::BasicVector3D<double> MyBasicVector3D;
class OldHepPoint3D
   : public MyBasicVector3D
{
public:
  OldHepPoint3D() {}
  virtual ~OldHepPoint3D() {}

  inline double	x() const { return MyBasicVector3D::x(); } 
  inline double	y() const { return MyBasicVector3D::y(); }
  inline double	z() const { return MyBasicVector3D::z(); }
};





class OldHepLorentzVector
{
public:
  OldHepLorentzVector() : ee(0) {}

  inline double	x() const { return pp.x(); } 
  inline double	y() const { return pp.y(); }
  inline double	z() const { return pp.z(); }
  inline double	t() const { return ee; }

  double  	ee; 
  CLHEP::Hep3Vector pp;
};




#include "CLHEP/Matrix/GenMatrix.h"

class OldHepGenMatrix {
public :
  OldHepGenMatrix() {};
  virtual ~OldHepGenMatrix() {}
};


class OldHepMatrix : public CLHEP::HepGenMatrix
{
public:
  OldHepMatrix() : nrow(0), ncol(0), size(0) {}
  virtual ~OldHepMatrix() {}

  virtual int  num_row() const { return nrow; }
  virtual int  num_col() const { return ncol; }
  virtual int  num_size() const { return size; }

  virtual void invert(int&) {}
  virtual const double& operator()(int x, int y) const { return m[x*ncol+y]; }    // <- FIXME hack
  virtual double& operator()(int x, int y) {  return m[x*ncol+y]; }    // <- FIXME hack
  
//  std::vector<double,Alloc<double,25> > m;
  std::vector<double> m;
  int nrow, ncol;
  int size;
};






#endif







