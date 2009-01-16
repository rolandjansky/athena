/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerRateSlices_TrigL1MissingEtObject
#define TriggerRateSlices_TrigL1MissingEtObject

#include "TriggerRateSlices/TrigMissingEtObject.h"
#include "TObject.h"

class TrigL1MissingEtObject : public TrigMissingEtObject {

public:
    TrigL1MissingEtObject();
    TrigL1MissingEtObject(const TrigL1MissingEtObject &other);
    
    virtual ~TrigL1MissingEtObject();
    virtual void reset();

    //----------- public data members
    float m_EtMissHit;
    float m_EtSumHit;

private:    
    ClassDef(TrigL1MissingEtObject,2);
};

#endif
