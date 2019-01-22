/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//---------------------------------------------------------------------------//
//                                                                           //
// class GeoXPEngine                                                         //
//                                                                           //
// This class does a Runge-Kutta integration of a particle in a magnetic     //
// field, without any slowing due to interactions with matter.               //
// --------------------------------------------------------------------------//

#ifndef LARG4VALIDATION_GEOXPENGINE_H
#define LARG4VALIDATION_GEOXPENGINE_H
#include <CLHEP/Geometry/Point3D.h>
#include <CLHEP/Vector/LorentzVector.h>

namespace Genfun {
  class AbsFunction;
}

class GeoXPEngine {

 public:

  // Constructor:
  GeoXPEngine( const Genfun::AbsFunction & Bx, 
	       const Genfun::AbsFunction & By,
	       const Genfun::AbsFunction & Bz,
	       const HepGeom::Point3D<double>          & x0,
	       const CLHEP::HepLorentzVector    & p0,
	       double                      q);

  // Destructor:
  ~GeoXPEngine();
  
  // Get the parameterized paths through the phase space.  The
  // parameter of the path is: time (in standard units).  When any
  // one of these is accessed, the path through the phase space is 
  // integrated up to the specified time; then, if another one is 
  // is accessed at or before that time, the calculation is not repeated
  // but either accessed (if times are equal) or interpolated (if time
  // is inferior to maximimum integration time).

  const Genfun::AbsFunction &  x() const;
  const Genfun::AbsFunction &  y() const;
  const Genfun::AbsFunction &  z() const;
  const Genfun::AbsFunction & px() const; 
  const Genfun::AbsFunction & py() const;
  const Genfun::AbsFunction & pz() const;

 private:

  // Outputs:
  const Genfun::AbsFunction       *m_x;
  const Genfun::AbsFunction       *m_y;
  const Genfun::AbsFunction       *m_z;
  const Genfun::AbsFunction       *m_px;
  const Genfun::AbsFunction       *m_py;
  const Genfun::AbsFunction       *m_pz;

};

#endif

