/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerRateSlices_TrigMissingEtObject
#define TriggerRateSlices_TrigMissingEtObject

#include "TObject.h"

class TrigMissingEtObject : public TObject {

public:
    TrigMissingEtObject();
    TrigMissingEtObject(const TrigMissingEtObject &other);
    
    virtual ~TrigMissingEtObject();
    virtual void reset();

    //----------- public data members
    float m_Ex;
    float m_Ey;
    float m_EtMiss;
    float m_EtSum;   

private:    
    ClassDef(TrigMissingEtObject,1);
};

#endif
