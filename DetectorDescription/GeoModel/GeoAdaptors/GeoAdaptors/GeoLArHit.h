/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOADAPTORS_GEOLARHIT_H
#define GEOADAPTORS_GEOLARHIT_H
//----------------------------------------------------------//
//                                                          //
// Adaptor for LArHits.                                    //
//                                                          //
// Joe Boudreau Apr 14.                                     //
//                                                          //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"

class CaloDetDescrManager;
class LArHit;
class CaloDetDescrElement;

class GeoLArHit 
{
 public:

  // Constructor:
  GeoLArHit(const LArHit & h);

  // Get the absolute global position:
  const CaloDetDescrElement* getDetDescrElement() const;
  double Energy() const;
  double Time() const;
  int SamplingLayer() const;

  // Underlying hit.
  const LArHit &data() const { return m_hit;}

  // Is this hit OK?
  operator bool () const { return true; }

 private:
  static const CaloDetDescrManager* init();
  const CaloDetDescrManager* mgr() const;
  
  const LArHit& m_hit;
};

#include "GeoAdaptors/GeoLArHit.icc"
#endif
