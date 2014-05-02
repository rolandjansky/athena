/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TINYHEPTRANSFORM_H_
#define _TINYHEPTRANSFORM_H_
//-------------------------------------------------------------------------//
//                                                                         //
// Tiny HepTransforms  Joe Boudreau September 2003                         //
//                                                                         //
// These are essentially alternate representations of HepGeom::Transform3D       //
// that are designed for memory optimization.                              //
//                                                                         //
// This header file includes the following classes:                        //
//                                                                         //
// tiny::HepTranslate3D                                                    //
// tiny::HepTranslateX3D                                                   //
// tiny::HepTranslateY3D                                                   //
// tiny::HepTranslateZ3D                                                   //
// tiny::HepRotate3D                                                       //
// tiny::HepRotateX3D                                                      //
// tiny::HepRotateY3D                                                      //
// tiny::HepRotateZ3D                                                      //
// tiny::HepReflect3D                                                      //
// tiny::HepReflectX3D                                                     //
// tiny::HepReflectY3D                                                     //
// tiny::HepReflectZ3D                                                     //
// tiny::HepScale3D                                                        //
// tiny::HepScale3XD                                                       //
// tiny::HepScale3YD                                                       //
// tiny::HepScale3ZD                                                       //
//                                                                         //
// tiny::HepGeom::Transform3D                                                    //
// tiny::HepProperTransform3D                                              //
//                                                                         //
// The "Input classes" all contain constructors with the same signature    //
// as their CLHEP counterparts.  However, they do not store more info      //
// than what they need; for example, HepTranslateZ3D stores just one       //
// floating point number. However it can cast itself to a HepGeom::Transform3D   //
// and so can be used exactly as a regular HepGeom::Transform3D.  (Sometimes     //
// you will need explicit casts in order to coax this behaviour).          //
//                                                                         //
// The "output classes" can be assigned the value of the HepGeom::Transform3D    //
// but store it in a memory-optimized way.  Six floats for a proper        //
// transform and nine floats for a transform with scaling.  In case        //
// an improper CLHEP Transform is assigned to a tiny transform, a run      //
// time error will occur.                                                  //
//                                                                         //
//-------------------------------------------------------------------------//

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <iostream> 
#include <stdexcept>

namespace tiny { 

  class Transform {

  public:
    
    virtual operator ::HepGeom::Transform3D() const = 0;

  };

  
  class HepReflectX3D: public Transform {

  public:

    virtual operator ::HepGeom::Transform3D() const {
      return ::HepReflectX3D();
    }

  };

  class HepReflectY3D: public Transform {

  public:

    virtual operator ::HepGeom::Transform3D() const {
      return ::HepReflectY3D();
    }

  };

  class HepReflectZ3D: public Transform {

  public:

    virtual operator ::HepGeom::Transform3D() const {
      return ::HepReflectZ3D();
    }

  };

  class HepTranslateX3D: public Transform {

  public:

    inline HepTranslateX3D(double a):
      _a(a) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepTranslateX3D(_a);
    }

  private:

    float _a;

  };

  class HepTranslateY3D: public Transform {

  public:

    inline HepTranslateY3D(double a):
      _a(a) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepTranslateY3D(_a);
    }

  private:

    float _a;

  };

  class HepTranslateZ3D: public Transform {

  public:

    inline HepTranslateZ3D(double a):
      _a(a) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepTranslateZ3D(_a);
    }

  private:

    float _a;

  };

  class HepScaleX3D: public Transform {

  public:

    inline HepScaleX3D(double a):
      _a(a) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepScaleX3D(_a);
    }

  private:

    float _a;

  };

  class HepScaleY3D: public Transform {

  public:

    inline HepScaleY3D(double a):
      _a(a) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepScaleY3D(_a);
    }

  private:

    float _a;

  };

  class HepScaleZ3D: public Transform {

  public:

    inline HepScaleZ3D(double a):
      _a(a) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepScaleZ3D(_a);
    }

  private:

    float _a;

  };

  class HepRotateX3D: public Transform {

  public:

    inline HepRotateX3D(double a):
      _a(a) {}
    
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepRotateX3D(_a);
    }

  private:

    float _a;

  };

  class HepRotateY3D: public Transform {

  public:

    inline HepRotateY3D(double a):
      _a(a) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepRotateY3D(_a);
    }

  private:

    float _a;

  };

  class HepRotateZ3D: public Transform {

  public:

    inline HepRotateZ3D(double a):
      _a(a) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepRotateZ3D(_a);
    }

  private:

    float _a;

  };



  class HepRotate3D: public Transform {

  public:

    inline HepRotate3D(const CLHEP::HepRotation &m) {
      double angle;
      CLHEP::Hep3Vector axis;
      m.getAngleAxis(angle, axis);
      axis.setMag(angle);
      ax=axis.x(); ay = axis.y(); az=axis.z();
      
    }

    inline HepRotate3D(double a, const HepGeom::Vector3D<double> & v) {
      CLHEP::Hep3Vector axis=v;
      axis.setMag(a);
      ax=axis.x(); ay=axis.y(); az=axis.z();
    }

    inline HepRotate3D(double a, 
		       const HepGeom::Point3D<double> & p1,
		       const HepGeom::Point3D<double> & p2) {
      ::HepRotate3D tmp(a, p1, p2);
      const CLHEP::HepRotation & m = tmp.getRotation();
      double angle;
      CLHEP::Hep3Vector axis;
      m.getAngleAxis(angle, axis);
      axis.setMag(angle);
      ax=axis.x(); ay = axis.y(); az=axis.z();
    }

    inline HepRotate3D(const HepGeom::Point3D<double> & fr1, 
		       const HepGeom::Point3D<double> & fr2,
		       const HepGeom::Point3D<double> & to1,
		       const HepGeom::Point3D<double> & to2) {
      ::HepRotate3D tmp(fr1, fr2, to1, to2);
      const CLHEP::HepRotation & m = tmp.getRotation();
      double angle;
      CLHEP::Hep3Vector axis;
      m.getAngleAxis(angle, axis);
      axis.setMag(angle);
      ax=axis.x(); ay = axis.y(); az=axis.z();
    }
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepRotate3D(sqrt(ax*ax+ay*ay+az*az), HepGeom::Vector3D<double> (ax,ay,az));
    }

  private:

    float ax, ay, az;

  };

  class HepTranslate3D: public Transform {

  public:
    inline HepTranslate3D (const CLHEP::Hep3Vector & v):
    ax(v.x()),ay(v.y()), az(v.z()){}

    inline HepTranslate3D (double dx, double dy, double dz):
      ax(dx),ay(dy),az(dz) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepTranslate3D(ax,ay,az);
    }

  private:
    float ax, ay, az;
  };

  class HepScale3D: public Transform {

  public:

    inline HepScale3D (double s):
    ax(s),ay(s), az(s){}

    inline HepScale3D (double sx, double sy, double sz):
      ax(sx),ay(sy),az(sz) {}
    
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepScale3D(ax,ay,az);
    }

  private:
    float ax, ay, az;
  };

  class HepReflect3D: public Transform {

  public:
    
    inline HepReflect3D(double a, double b, double c, double d) {
      double mag = sqrt(a*a+b*b+c*c);
      d  = d/mag;
      ax = d*a/mag; 
      ay = d*b/mag;
      az = d*c/mag;
 
    }

    inline HepReflect3D(const HepGeom::Normal3D<double> & normal,
			const HepGeom::Point3D<double> & point) {
      HepGeom::Normal3D<double> nNorm=normal;
      nNorm.setMag(1.0);
      double d = nNorm*point;
      nNorm.setMag(d);
    }

    virtual operator ::HepGeom::Transform3D() const {
      double mag = sqrt(ax*ax+ay*ay+az*az);
      return ::HepReflect3D(ax/mag,ay/mag,az/mag ,mag);
    }

  private:
    float ax, ay, az;
  };


  class HepProperTransform3D: public Transform {

  public:
    
    inline HepProperTransform3D():
      tx(0),ty(0), tz(0), rx(0), ry(0), rz(0) {}


    inline HepProperTransform3D(const ::HepGeom::Transform3D & source) {
      HepGeom::Scale3D     scale;
      HepGeom::Rotate3D    rotate;
      HepGeom::Translate3D translate;
      
      source.getDecomposition(scale, rotate, translate);
      const double EPS=5E-5;
      if (fabs(scale.xx()-1.0)>EPS || fabs(scale.yy()-1.0)>EPS || fabs(scale.zz()-1.0)>EPS) {

	throw std::range_error("tiny::HepProperTransform3D detects an improper transformation (scaling or reflection)");
      }
      
      double angle;
      CLHEP::HepRotation rotation( HepRep3x3(rotate.xx(), rotate.xy(), rotate.xz(),
						    rotate.yx(), rotate.yy(), rotate.yz(),
						    rotate.zx(), rotate.zy(), rotate.zz()));
      CLHEP::Hep3Vector axis;
      rotation.getAngleAxis(angle, axis);
      axis.setMag(angle);
      rx=axis.x(); ry = axis.y(); rz=axis.z();      
      
      tx = translate.dx();
      ty = translate.dy();
      tz = translate.dz();

    }

    inline const HepProperTransform3D & operator = (const ::HepGeom::Transform3D & source) {
      HepGeom::Scale3D     scale;
      HepGeom::Rotate3D    rotate;
      HepGeom::Translate3D translate;
      
      source.getDecomposition(scale, rotate, translate);
      const double EPS=5E-5;
      if (fabs(scale.xx()-1.0)>EPS || fabs(scale.yy()-1.0)>EPS || fabs(scale.zz()-1.0)>EPS) {
	throw std::range_error("tiny::HepProperTransform3D detects an improper transformation (scaling or reflection)");
      }
      
      double angle;
      CLHEP::HepRotation rotation( HepRep3x3(rotate.xx(), rotate.xy(), rotate.xz(),
						    rotate.yx(), rotate.yy(), rotate.yz(),
						    rotate.zx(), rotate.zy(), rotate.zz()));
      CLHEP::Hep3Vector axis;
      rotation.getAngleAxis(angle, axis);
      axis.setMag(angle);
      rx=axis.x(); ry = axis.y(); rz=axis.z();      
      
      tx = translate.dx();
      ty = translate.dy();
      tz = translate.dz();
      return *this;
    }
  
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepGeom::Transform3D(CLHEP::CLHEP::HepRotation(CLHEP::Hep3Vector(rx,ry,rz),sqrt(rx*rx+ry*ry+rz*rz)),
			      CLHEP::Hep3Vector(tx,ty,tz));
    }

  private:
    
    float tx, ty, tz;
    float rx, ry, rz;

  };


  class HepGeom::Transform3D: public Transform {

  public:
    
    inline HepGeom::Transform3D():
      tx(0),ty(0), tz(0), rx(0), ry(0), rz(0), sx(0), sy(0), sz(0) {}


    inline HepGeom::Transform3D(const ::HepGeom::Transform3D & source) {
      HepGeom::Scale3D     scale;
      HepGeom::Rotate3D    rotate;
      HepGeom::Translate3D translate;
      
      source.getDecomposition(scale, rotate, translate);
      
      double angle;
      CLHEP::HepRotation rotation( HepRep3x3(rotate.xx(), rotate.xy(), rotate.xz(),
						    rotate.yx(), rotate.yy(), rotate.yz(),
						    rotate.zx(), rotate.zy(), rotate.zz()));
      CLHEP::Hep3Vector axis;
      rotation.getAngleAxis(angle, axis);
      axis.setMag(angle);
      rx=axis.x(); ry = axis.y(); rz=axis.z();      
      
      tx = translate.dx();
      ty = translate.dy();
      tz = translate.dz();
      
      sx = scale.xx();
      sy = scale.yy();
      sz = scale.zz();

    }

    inline const HepGeom::Transform3D & operator = (const ::HepGeom::Transform3D & source) {
      HepGeom::Scale3D     scale;
      HepGeom::Rotate3D    rotate;
      HepGeom::Translate3D translate;
      
      source.getDecomposition(scale, rotate, translate);
      
      double angle;
      CLHEP::HepRotation rotation( HepRep3x3(rotate.xx(), rotate.xy(), rotate.xz(),
						    rotate.yx(), rotate.yy(), rotate.yz(),
						    rotate.zx(), rotate.zy(), rotate.zz()));
      CLHEP::Hep3Vector axis;
      rotation.getAngleAxis(angle, axis);
      axis.setMag(angle);
      rx=axis.x(); ry = axis.y(); rz=axis.z();      
      
      tx = translate.dx();
      ty = translate.dy();
      tz = translate.dz();

      sx = scale.xx();
      sy = scale.yy();
      sz = scale.zz();

      return *this;
    }
  
    virtual operator ::HepGeom::Transform3D() const {
      return ::HepGeom::Transform3D(CLHEP::CLHEP::HepRotation(CLHEP::Hep3Vector(rx,ry,rz),sqrt(rx*rx+ry*ry+rz*rz)),
			      CLHEP::Hep3Vector(tx,ty,tz))*HepGeom::Scale3D(sx,sy,sz);
    }

  private:
    
    float tx, ty, tz;
    float rx, ry, rz;
    float sx, sy, sz;
  };

}
#endif
