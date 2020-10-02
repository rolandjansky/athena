/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOADAPTORS_GEOCALOCALIBHIT_H
#define GEOADAPTORS_GEOCALOCALIBHIT_H
//----------------------------------------------------------//
//                                                          //
// Adaptor for CaloCalibHits.                               //
//                                                          //
// Joe Boudreau      Apr 14, 2005                           //
// Mikhail Leltchouk Apr 30, 2005                           //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"

class CaloIdManager;
class CaloCalibrationHit;
class CaloDM_ID;
class CaloDetDescrElement;
class CaloDetDescrManager;
#include <string>

class GeoCaloCalibHit 
{
 public:

  enum Type {ACTIVE, INACTIVE, DEAD, TILEAI, TILEDM, TILEACTIVE, TILEINACTIVE, TILEDEAD, UNRECOGNIZED};

  // Default constructor:
  GeoCaloCalibHit() {m_failed=true;}

  // Constructor:
  GeoCaloCalibHit(const CaloCalibrationHit & h, const std::string & collectionName);

  double energyEM() const;
  double energyNonEM() const;
  double energyInvisible() const;
  double energyEscaped() const;
  double energyTotal() const;

  int subdet() const;
  int type() const;
  int sampling() const;
  int region() const;

  double phiMin() const;
  double phiMax() const;
  double etaMin() const;
  double etaMax() const;
  double eta() const;
  double phi() const;
  double distance() const;
  bool isBarrel() const;
  bool isEndcap() const;

  // Underlying hit.
  const CaloCalibrationHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const;

  // Which type of hit is this??
  std::string hitType() const;

  // Get the detetector element:
  const CaloDetDescrElement* getDetDescrElement() const;

 private:
  static const CaloDetDescrManager* initDDMgr();
  const CaloDetDescrManager* ddmgr() const;

  void init(const std::string & collectionName);
  
  const CaloCalibrationHit         *m_hit;

  int m_subdet;
  int m_type; // i.e. in the barrel or in the endcap.
  int m_sampling;
  int m_region;
  int m_etaBin;
  int m_phiBin;
  double m_etaOffset;
  double m_distance;
  bool m_isBarrel;
  bool m_failed;
  Type m_hitType; // ie. active, inactive dead.
  const CaloDetDescrElement *m_ddElement;
};

#include "GeoAdaptors/GeoCaloCalibHit.icc"

#endif
