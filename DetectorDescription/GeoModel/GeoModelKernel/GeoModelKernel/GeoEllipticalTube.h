/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoEllipticalTube_h
#define GeoEllipticalTube_h 1

#include "GeoModelKernel/GeoShape.h"

// Class: GeoEllipticalTube
//  
// This is a tube with elliptical cross section
// The equation of the surface in x/y is 1.0 = (x/dx)**2 + (y/dy)**2
//

class GeoEllipticalTube : public GeoShape
{
 public:
  GeoEllipticalTube(double XHalfLength, double YHalfLength, double ZHalfLength); 

  virtual double volume () const;

  virtual const std::string & type () const;
  virtual ShapeType typeID () const;

  virtual void exec (GeoShapeAction *action) const;

  static const std::string& getClassType ();
  static ShapeType getClassTypeID ();

  const double& getXHalfLength() const;
  const double& getYHalfLength() const;
  const double& getZHalfLength() const;

 protected:
  virtual ~GeoEllipticalTube();

 private:
  
  GeoEllipticalTube(const GeoEllipticalTube &right);
  GeoEllipticalTube & operator=(const GeoEllipticalTube &right);

  static const std::string classType;
  static const ShapeType classTypeID;

  double xHalfLength;
  double yHalfLength;
  double zHalfLength;
};


inline const std::string& GeoEllipticalTube::getClassType ()
{
  return classType;
}

inline ShapeType GeoEllipticalTube::getClassTypeID ()
{
  return classTypeID;
}

inline const double& GeoEllipticalTube::getXHalfLength() const
{
  return xHalfLength;
}

inline const double& GeoEllipticalTube::getYHalfLength() const
{
  return yHalfLength;
}

inline const double& GeoEllipticalTube::getZHalfLength() const
{
  return zHalfLength;
}


#endif
