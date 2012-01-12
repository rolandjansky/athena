/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauEtMissL1Object.h"
ClassImp(TrigTauEtMissL1Object )


TrigTauEtMissL1Object::TrigTauEtMissL1Object()
  :  TrigTauEtMissObject(),
     m_EtMissHit(0),
     m_EtSumHit (0)
{}

TrigTauEtMissL1Object::TrigTauEtMissL1Object(const TrigTauEtMissL1Object & other)
  : TrigTauEtMissObject(other),
    m_EtMissHit( other.m_EtMissHit),
    m_EtSumHit ( other.m_EtSumHit )
{}

TrigTauEtMissL1Object::~TrigTauEtMissL1Object(){}

void TrigTauEtMissL1Object::reset()
{
  TrigTauEtMissObject::reset();
  m_EtMissHit = 0;
  m_EtSumHit  = 0;
}
