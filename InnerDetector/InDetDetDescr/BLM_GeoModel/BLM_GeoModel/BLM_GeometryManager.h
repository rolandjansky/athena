/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BLM_GeometryManager.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BLMGEOMETRYMANAGER_H
#define BLMGEOMETRYMANAGER_H

#include "BLM_GeoModel/BLM_ModuleParameters.h"

  /** @class BLM_GeometryManager 

  Beam Loss Monitor Geometry Manager
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */  

class BLM_GeometryManager
{
 public:
  BLM_GeometryManager();

  BLM_ModuleParameters* Module(unsigned int module_number);
  void ModuleOn(unsigned int module_number);
  void ModuleOff(unsigned int module_number);
  unsigned int IsModuleOn(unsigned int module_number) const;

 private:
  BLM_ModuleParameters m_moduli[12];
  unsigned int m_on;
};
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(BLM_GeometryManager, 53024180, 1)
#endif
