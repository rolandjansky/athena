/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloShower.h"


CaloShower::CaloShower(const CaloShower* pShower)
  : m_momentStore (pShower->m_momentStore),
    m_samplingStore (pShower->m_samplingStore)
{
}

CaloShower::CaloShower(const CaloShower& rShower)
  : m_momentStore (rShower.m_momentStore),
    m_samplingStore (rShower.m_samplingStore)
{
}

