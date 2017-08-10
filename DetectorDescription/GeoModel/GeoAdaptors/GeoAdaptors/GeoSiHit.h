/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOADAPTORS_GEOSIHIT_h
#define GEOADAPTORS_GEOSIHIT_h
//----------------------------------------------------------//
//                                                          //
// An adaptor for SiHits.                                   //
//                                                          //
// Joe Boudreau Feb 04.                                     //
//                                                          //
// This adaptor class allows SiHits to behave               //
// as if they knew which detector they were in.             //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"
class SiHit;
class PixelID;
class SCT_ID;
namespace InDetDD {
  class SCT_DetectorManager;
  class PixelDetectorManager;
}

class GeoSiHit {

 public:

  // Constructor:
  GeoSiHit(const SiHit & h);

  // Get the absolute global position:
  HepGeom::Point3D<double> getGlobalPosition() const;

  // Underlying hit.
  const SiHit &data() const { return *m_hit;}

  // Is this hit ok?

  operator bool () const { return s_sct || s_pix; }

 private:
  
  static void init();

  const SiHit                                     *m_hit;
  static const InDetDD::SCT_DetectorManager       *s_sct;
  static const InDetDD::PixelDetectorManager      *s_pix;
  static const PixelID                            *s_pID;
  static const SCT_ID                             *s_sID;
};

#include "GeoAdaptors/GeoSiHit.icc"

#endif
