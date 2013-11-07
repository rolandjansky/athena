/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_ForwardParameters_H
#define SCT_SLHC_GeoModel_SCT_ForwardParameters_H

#include "SCT_SLHC_GeoModel/SCT_ParametersBase.h"
#include "InDetGeoModelUtils/PairIndexMap.h"

#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"

namespace InDetDDSLHC {

class SCT_DataBase;

class SCT_ForwardParameters : public SCT_ParametersBase{

public:

  // Constructor 
  SCT_ForwardParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);
  virtual ~SCT_ForwardParameters();
  //////////////////////////////////////////////////////////////////////////////
  // Forward SCT
  //////////////////////////////////////////////////////////////////////////////
  virtual int layoutType() const;

  // Wheel
  virtual double  materialIncreaseFactor(int iWheel) const;
  virtual double fwdWheelZPosition(int iWheel) const;
  virtual int    fwdWheelNumRings(int iWheel) const;
  virtual double fwdDiscSupportInnerRadius(int iWheel) const;
  virtual double fwdDiscSupportOuterRadius(int iWheel) const;
  virtual double fwdDiscSupportThickness(int iWheel) const;
  virtual std::string fwdDiscSupportMaterial(int iWheel) const;

  // Ring
  virtual int    fwdRingNumModules(int iRing) const;
  virtual double fwdRingInnerRadius(int iRing) const;
  virtual double fwdRingMiddleRadius(int iRing) const;
  virtual int    fwdWheelRingType(int iWheel, int iRingIndex) const;
  virtual double fwdRingOuterRadius(int iRing) const;
  virtual double fwdRingOffset(int iRing) const;
  virtual double fwdRingModuleStagger(int iRing) const;
  virtual double fwdRingPhiOfRefModule(int iRing) const;
  virtual int    fwdRingStaggerOfRefModule(int iRing) const;
  //---virtual int    fwdWheelStereoType(int iWheel, int iWheel) const;
  virtual int    fwdWheelDoubleSided(int iRing) const;

  // Forward General
  virtual int    fwdNumWheels() const;
  virtual double fwdInnerRadius() const;
  virtual double fwdIntermediateRadius() const;
  virtual double fwdOuterRadius() const;
  virtual double fwdZMin() const;
  virtual double fwdZIntermediate() const;
  virtual double fwdZMax() const;

 private:
  int getRingMapIndex(int iWheel, int iRingIndex) const;
  InDetDD::PairIndexMap m_wheelRingIndexMap;

  IRDBRecordset_ptr SctFwdGeneral;
  IRDBRecordset_ptr SctFwdWheel;
  IRDBRecordset_ptr SctFwdRing;
  IRDBRecordset_ptr SctFwdWheelRingMap;
  IRDBRecordset_ptr SctFwdDiscSupport;
};
}
#endif //SCT_SLHC_GeoModel_SCT_ForwardParameters_H
