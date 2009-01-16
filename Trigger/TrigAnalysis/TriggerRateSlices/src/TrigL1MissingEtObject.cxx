/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerRateSlices/TrigL1MissingEtObject.h"

ClassImp(TrigL1MissingEtObject )


TrigL1MissingEtObject::TrigL1MissingEtObject()
  :  TrigMissingEtObject(),
     m_EtMissHit(0),
     m_EtSumHit (0) {
}

TrigL1MissingEtObject::TrigL1MissingEtObject(const TrigL1MissingEtObject & other)
  : TrigMissingEtObject(other),
    m_EtMissHit( other.m_EtMissHit),
    m_EtSumHit ( other.m_EtSumHit ) {
}

TrigL1MissingEtObject::~TrigL1MissingEtObject(){}

void TrigL1MissingEtObject::reset() {
  TrigMissingEtObject::reset();
  m_EtMissHit = 0;
  m_EtSumHit  = 0;
}
