/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _GeoCaloCalibHit_h_
#define _GeoCaloCalibHit_h_
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
  GeoCaloCalibHit() {_failed=true;}

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
  const CaloCalibrationHit &data() const { return *_hit;}

  // Is this hit OK?
  operator bool () const;

  // Which type of hit is this??
  std::string hitType() const;

  // Get the detetector element:
  const CaloDetDescrElement* getDetDescrElement() const;

 private:

  void init(const std::string & collectionName);
  
  const CaloCalibrationHit         *_hit;
  static const CaloIdManager       *_man;
  static const CaloDM_ID           *_dmid;
  static const CaloDetDescrManager *_caloMan;
  int _subdet;
  int _type; // i.e. in the barrel or in the endcap.
  int _sampling;
  int _region;
  int _etaBin;
  int _phiBin;
  double _etaOffset;
  double _distance;
  bool _isBarrel;
  bool _failed;
  Type _hitType; // ie. active, inactive dead.
  const CaloDetDescrElement *_ddElement;
};

#include "GeoAdaptors/GeoCaloCalibHit.icc"

#endif
