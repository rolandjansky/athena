/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
// TopElectronMapCnv_p1.h 
// Header file for class TopElectronMapCnv_p1
// Converts transient TopElectronMap to persistent form
/////////////////////////////////////////////////////////////////// 
#ifndef TOPINPUTSTPCNV_TOPELECTRONMAPCNV_P1_H 
#define TOPINPUTSTPCNV_TOPELECTRONMAPCNV_P1_H 

#include "TopInputsTPCnv/TopInputMapTriggerCnv_p1.h"
#include "egammaEvent/ElectronContainer.h"
#include "TopInputs/TopElectronMap.h"

class TopElectronMapCnv_p1 : public TopInputMapTriggerCnv_p1<ElectronContainer>{
  public:
    TopElectronMapCnv_p1();
    ~TopElectronMapCnv_p1(){;}
};

#endif // not TOPINPUTSTPCNV_TOPELECTRONMAPCNV_P1_H 
