/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerRateSlices_TrigL2MissingEtObject
#define TriggerRateSlices_TrigL2MissingEtObject

#include "TriggerRateSlices/TrigMissingEtObject.h"
#include "TObject.h"
#include <vector>
#include <string>

class TrigL2MissingEtObject : public TrigMissingEtObject {

public:
    TrigL2MissingEtObject();
    TrigL2MissingEtObject(const TrigL2MissingEtObject &other);
    
    virtual ~TrigL2MissingEtObject();
    virtual void reset();

    //----------- public data members
    // basic information: global quantities
    float m_Ez;   // longitudinal component of missing energy
    float m_SumE; // scalar sum of total energy
    int   m_flag; // event flag
    // auxiliar information: all components
    std::string    m_c_name;
    short          m_c_status;
    float          m_c_ex;
    float          m_c_ey;
    float          m_c_ez;
    float          m_c_sumEt;
    float          m_c_sumE;
    float          m_c_calib0;
    float          m_c_calib1;
    short          m_c_sumOfSigns;
    unsigned short m_c_usedChannels;

private:    
    ClassDef(TrigL2MissingEtObject,1);
};

#endif
