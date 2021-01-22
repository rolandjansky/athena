/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _GeoSiHit_h_
#define _GeoSiHit_h_
//----------------------------------------------------------//
//                                                          //
// And adaptor for SiHits.                                  //
//                                                          //
// Joe Boudreau Feb 04.                                     //
//                                                          //
// This adaptor class allows SiHits to behave               //
// as if they knew which detector they were in..            //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"
class SiHit;
class PixelID;
class SCT_ID;
class HGTD_ID;
namespace InDetDD {
  class SCT_DetectorManager;
  class PixelDetectorManager;
}
class HGTD_DetectorManager;

class GeoSiHit {

 public:

  // Constructor:
  GeoSiHit(const SiHit & h);

  // Get the absolute global position:
  HepGeom::Point3D<double> getGlobalPosition() const;

  // Underlying hit.
  const SiHit &data() const { return *_hit;}

  // Is this hit ok?

  operator bool () const { return _sct || _pix || _hgtd; }

 private:
  
  static void init();

  const SiHit                                     *_hit;
  static const InDetDD::SCT_DetectorManager       *_sct;
  static const InDetDD::PixelDetectorManager      *_pix;
  static const HGTD_DetectorManager               *_hgtd;
  static const PixelID                            *_pID;
  static const SCT_ID                             *_sID;
  static const HGTD_ID                            *_hID;
};

#include "GeoAdaptors/GeoSiHit.icc"

#endif
