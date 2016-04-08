/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _GeoLArHit_h_
#define _GeoLArHit_h_
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
  const LArHit &data() const { return *_hit;}

  // Is this hit OK?
  operator bool () const { return _man; }

 private:

  static void init();
  
  const LArHit* _hit;
  static const CaloDetDescrManager *_man;
};

#include "GeoAdaptors/GeoLArHit.icc"
#endif
