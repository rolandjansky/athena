/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  const SiHit &data() const { return *m_hit;}

  // Is this hit ok?

  operator bool () const { return true; }

 private:
  static const InDetDD::PixelDetectorManager* initPixMgr();
  static const InDetDD::PixelDetectorManager* initPlrMgr();
  static const InDetDD::SCT_DetectorManager*  initSctMgr();
  static const HGTD_DetectorManager*          initHgtdMgr();
  static const PixelID*                       initPixID();
  static const SCT_ID*                        initSctID();
  static const HGTD_ID*                       initHgtdID();

  const InDetDD::PixelDetectorManager* pixMgr() const;
  const InDetDD::PixelDetectorManager* plrMgr() const;
  const InDetDD::SCT_DetectorManager*  sctMgr() const;
  const HGTD_DetectorManager*          hgtdMgr() const;
  const PixelID*                       pixID()  const;
  const SCT_ID*                        sctID()  const;
  const HGTD_ID*                       hgtdID() const;

  const SiHit                                     *m_hit;
};

#include "GeoAdaptors/GeoSiHit.icc"

#endif
