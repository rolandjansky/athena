/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauEtMissObject.h"
ClassImp(TrigTauEtMissObject )


TrigTauEtMissObject::TrigTauEtMissObject()
  : m_Ex       (-1.e+9),	    
    m_Ey       (-1.e+9), 
    m_EtMiss   (-1.e+9),   
    m_EtSum    (-1.e+9)    
{}

TrigTauEtMissObject::TrigTauEtMissObject(const TrigTauEtMissObject & other)
  : TObject(other),
    m_Ex       ( other.m_Ex       ),
    m_Ey       ( other.m_Ey       ),
    m_EtMiss   ( other.m_EtMiss   ),
    m_EtSum    ( other.m_EtSum    ) 
{}

TrigTauEtMissObject::~TrigTauEtMissObject(){}

void TrigTauEtMissObject::reset()
{
  m_Ex        = -1.e+9;
  m_Ey        = -1.e+9;
  m_EtMiss    = -1.e+9;
  m_EtSum     = -1.e+9;
}
