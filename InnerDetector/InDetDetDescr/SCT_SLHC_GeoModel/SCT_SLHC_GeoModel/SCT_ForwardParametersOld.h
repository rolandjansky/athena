/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_ForwardParametersOld_H
#define SCT_SLHC_GeoModel_SCT_ForwardParametersOld_H

#include "SCT_SLHC_GeoModel/SCT_ForwardParameters.h"

#include <string>
#include <vector>
#include <map>

namespace InDetDDSLHC {

//class SCT_DataBase;

class SCT_ForwardParametersOld : public SCT_ForwardParameters{

public:

  // Constructor 
  SCT_ForwardParametersOld(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);
  ~SCT_ForwardParametersOld();
  //disable assign and copy
  SCT_ForwardParametersOld  & operator=(const SCT_ForwardParametersOld &) = delete;
  SCT_ForwardParametersOld(const SCT_ForwardParametersOld &) = delete;
  
  //////////////////////////////////////////////////////////////////////////////
  // Forward SCT
  //////////////////////////////////////////////////////////////////////////////
  int layoutType() const;

  // Wheel
  double materialIncreaseFactor(int iWheel) const;
  double fwdWheelZPosition(int iWheel) const;
  int    fwdWheelNumRings(int iWheel) const;
  double fwdDiscSupportInnerRadius(int iWheel) const;
  double fwdDiscSupportOuterRadius(int iWheel) const;
  double fwdDiscSupportThickness(int iWheel) const;
  std::string fwdDiscSupportMaterial(int iWheel) const;

  // Ring
  int    fwdRingNumModules(int iRing) const;
  double fwdRingInnerRadius(int iRing) const;
  double fwdRingMiddleRadius(int iRing) const;
  int    fwdWheelRingType(int iWheel, int iRingIndex) const;
  double fwdRingOuterRadius(int iRing) const;
  double fwdRingOffset(int iRing) const;
  double fwdRingModuleStagger(int iRing) const;
  double fwdRingPhiOfRefModule(int iRing) const;
  int    fwdRingStaggerOfRefModule(int iRing) const;
  //---int    fwdWheelStereoType(int iWheel, int iWheel) const;
  int    fwdWheelDoubleSided(int iRing) const;

  // Forward General
  int    fwdNumWheels() const;
  double fwdInnerRadius() const;
  double fwdIntermediateRadius() const;
  double fwdOuterRadius() const;
  double fwdZMin() const;
  double fwdZIntermediate() const;
  double fwdZMax() const;

 private:
  //SCT_DataBase* m_rdb;
  std::map<std::string, float>* m_SCT_Fwd_Parameters;
  std::map<std::string, std::string>* m_SCT_Fwd_Parameters_Strng;
  std::vector<std::vector<unsigned int>*>* m_wheels_rings;
};
}
#endif //SCT_SLHC_GeoModel_SCT_ForwardParametersOld_H
