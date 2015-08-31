/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoTorus_h
#define GeoTorus_h 1

#include "GeoModelKernel/GeoShape.h"


class GeoTorus : public GeoShape  
{
 public:
  GeoTorus (double Rmin, double Rmax, double Rtor, double SPhi, double DPhi);

  virtual double volume () const;

  virtual const std::string & type () const;
  virtual ShapeType typeID () const;

  static const std::string& getClassType ();
  static ShapeType getClassTypeID ();

  virtual void exec (GeoShapeAction *action) const;

  const double& getRMin () const;
  const double& getRMax () const;
  const double& getRTor () const;
  const double& getSPhi () const;
  const double& getDPhi () const;

 protected:
  virtual ~GeoTorus();

 private:
  GeoTorus(const GeoTorus &right);
  GeoTorus & operator=(const GeoTorus &right);

  static const std::string classType;
  static const ShapeType classTypeID;

  double rMin;
  double rMax;
  double rTor;
  double sPhi;
  double dPhi;
};

inline const std::string& GeoTorus::getClassType ()
{
  return classType;
}

inline ShapeType GeoTorus::getClassTypeID ()
{
  return classTypeID;
}

inline const double& GeoTorus::getRMin () const
{
  return rMin;
}

inline const double& GeoTorus::getRMax () const
{
  return rMax;
}

inline const double& GeoTorus::getRTor () const
{
  return rTor;
}

inline const double& GeoTorus::getSPhi () const
{
  return sPhi;
}

inline const double& GeoTorus::getDPhi () const
{
  return dPhi;
}


#endif
