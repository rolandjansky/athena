/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
// TopElectronMapCnv_p2.h 
// Header file for class TopElectronMapCnv_p2
// Converts transient TopElectronMap to persistent form
/////////////////////////////////////////////////////////////////// 
#ifndef TOP_ELECTRON_MAP_CNV_P2_H 
#define TOP_ELECTRON_MAP_CNV_P2_H 

#include "TopInputsTPCnv/TopInputMapTriggerCnv_p2.h"
#include "egammaEvent/ElectronContainer.h"

class TopElectronMapCnv_p2:public TopInputMapTriggerCnv_p2<ElectronContainer>{
  public:
    TopElectronMapCnv_p2();
    ~TopElectronMapCnv_p2(){;}
};

#endif // TOP_ELECTRON_MAP_CNV_P2_H 
