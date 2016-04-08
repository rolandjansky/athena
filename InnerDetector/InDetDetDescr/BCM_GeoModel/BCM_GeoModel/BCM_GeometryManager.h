/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_GeometryManager.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef BCMGEOMETRYMANAGER_H
#define BCMGEOMETRYMANAGER_H

#include "BCM_GeoModel/BCM_ModuleParameters.h"

  /** @class BCM_GeometryManager 

  Beam Condition Monitor Geometry Manager
      @author  Bostjan Macek <bostjan.macek@cern.ch>
  */  

class BCM_GeometryManager
{
 public:
  BCM_GeometryManager();

  BCM_ModuleParameters* Module(int module_number);
  void ModuleOn(int module_number);
  void ModuleOff(int module_number);
  int IsModuleOn(int module_number) const;

 private:
  BCM_ModuleParameters m_moduli[8];
  unsigned int m_on;
};
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(BCM_GeometryManager, 15478805, 1)

#endif
