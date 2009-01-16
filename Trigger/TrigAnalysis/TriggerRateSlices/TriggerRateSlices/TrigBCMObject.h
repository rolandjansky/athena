/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerRateSlices_TrigBCMObject
#define TriggerRateSlices_TrigBCMObject

#include "TROOT.h"
#include "TSystem.h"
#include "TClass.h"

#include "TObject.h"
#include <vector>

class TrigBCMObject : public TObject {

public:
    TrigBCMObject();
    virtual ~TrigBCMObject();

    // ntuple branches - add new variables here
    std::vector<int> m_bcm_pulse_x;
    std::vector<int> m_bcm_pulse_w;
    std::vector<int> m_bcm_pulse_mod;
    std::vector<int> m_bcm_pulse_hithr;
    
    int m_bcm_A_toC_trigger;
    int m_bcm_C_toA_trigger;
    int m_bcm_Wide_trigger;
    int m_bcm_Ideal_trigger;

private:
    ClassDef(TrigBCMObject,1);
};

#endif
