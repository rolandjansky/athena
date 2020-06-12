/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_IHITOWERWEIGHTTOOL_H
#define HIEVENTUTILS_IHITOWERWEIGHTTOOL_H

#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgTool.h"
#include <map>

class TH3F;

class IHITowerWeightTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IHITowerWeightTool)

  public:

  virtual ~IHITowerWeightTool() {};
  virtual StatusCode initialize() = 0;
  virtual StatusCode configureEvent() = 0;
  virtual float getEtaPhiResponse(float eta, float phi) const = 0;
  virtual float getEtaPhiOffset(float eta, float phi) const = 0;
  virtual float getWeight(float eta, float phi, int sampling) const = 0;
  virtual float getWeightEta(float eta, float phi, int sampling) const = 0;
  virtual float getWeightPhi(float eta, float phi, int sampling) const = 0;
  virtual float getWeightMag(float eta, float phi, int sampling) const = 0;

};
#endif
