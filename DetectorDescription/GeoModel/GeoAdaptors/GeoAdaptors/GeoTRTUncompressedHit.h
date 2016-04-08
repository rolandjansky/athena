/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _GeoTRTUncompressedHit_h_
#define _GeoTRTUncompressedHit_h_
//----------------------------------------------------------//
//                                                          //
// And adaptor for TRT_UncompressedHits.                    //
//                                                          //
// Joe Boudreau Feb 04.                                     //
//                                                          //
// This adaptor class allows TRT_UncompressedHits to behave //
// as if they knew which detector they were in..            //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"
class TRTUncompressedHit;

namespace InDetDD {
  class TRT_DetectorManager;
}

class GeoTRTUncompressedHit {

 public:

  // Constructor:
  GeoTRTUncompressedHit(const TRTUncompressedHit & h);

  // Get the absolute global position:
  HepGeom::Point3D<double> getGlobalPosition() const;

  // Is this hit OK? 
  operator bool () const { return _mgr; }

 private:
  
  static void init();

  const TRTUncompressedHit                        *_hit;
  static const InDetDD::TRT_DetectorManager       *_mgr;

};

#include "GeoAdaptors/GeoTRTUncompressedHit.icc"

#endif
