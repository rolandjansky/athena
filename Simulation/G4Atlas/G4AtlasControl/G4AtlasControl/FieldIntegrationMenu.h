/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldIntegrationMenu_H
#define FieldIntegrationMenu_H

#include "FadsField/FieldIntParameters.h"
#include "G4AtlasControl/SimpleMenu.h"

class FieldIntegrationMenu : public SimpleMenu {
public:
  FieldIntegrationMenu();
  void list();
  void Set_kineticEnergyThreshold(const std::string regionName,double val);
  // in the following parameters
  //   idx=0 for e+/e- track below E-threshold
  //   idx=1 for e+/e- track above E-threshold
  //      and all other charged track except mu+/mu-
  //      of any kinetic energy
  //   idx=2 for mu+/mu- track of any kinetic energy
  void Set_maxLengthForConstField(const std::string regionName,int idx,double val);
  void Set_missDistance(const std::string regionName,int idx,double val);
  void Set_typicalLongStep(const std::string regionName,int idx,double val);
  void Set_tolerableBiasError(const std::string regionName,int idx,double val);
  void Set_tolerableIntegrationError(const std::string regionName,int idx,double val);
  void Set_expectedNumBoundaries(const std::string regionName,int idx,double val);
  void Set_expectedTrackLength(const std::string regionName,int idx,double val);
private:
  FADS::FieldIntParameters* Get_fieldIntParameters(const std::string regionName);
};

#endif


