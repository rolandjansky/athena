/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerRateSlices_TrigEFMissingEtObject
#define TriggerRateSlices_TrigEFMissingEtObject

#include "TriggerRateSlices/TrigMissingEtObject.h"
#include "TObject.h"
#include <vector>
#include <string>

class TrigEFMissingEtObject : public TrigMissingEtObject {

public:
    TrigEFMissingEtObject();
    TrigEFMissingEtObject(const TrigEFMissingEtObject &other);
    
    virtual ~TrigEFMissingEtObject();
    virtual void reset();

    //----------- public data members
    // basic information: global quantities
    float m_Ez;   // longitudinal component of missing energy
    float m_SumE; // scalar sum of total energy
    int   m_flag; // event flag
    // auxiliar information: all components
    unsigned int m_comp_number; // size of all following vectors
    std::vector<std::string>    m_c_name;
    std::vector<short>          m_c_status;
    std::vector<float>          m_c_ex;
    std::vector<float>          m_c_ey;
    std::vector<float>          m_c_ez;
    std::vector<float>          m_c_sumEt;
    std::vector<float>          m_c_sumE;
    std::vector<float>          m_c_calib0;
    std::vector<float>          m_c_calib1;
    std::vector<short>          m_c_sumOfSigns;
    std::vector<unsigned short> m_c_usedChannels;

private:    
    ClassDef(TrigEFMissingEtObject,1);
};

#endif
