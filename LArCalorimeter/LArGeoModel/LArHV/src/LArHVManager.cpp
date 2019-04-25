/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/LArHVManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/LArHVManager.h"

LArHVManager::LArHVManager(const HECHVManager *hecHV
			   , const FCALHVManager *fcalHV
			   , const EMBPresamplerHVManager *embPreHV
			   , const EMECPresamplerHVManager *emecPreHV)
  : m_embHV()
  , m_emecHVInner(EMECHVModule::INNER)
  , m_emecHVOuter(EMECHVModule::OUTER)
  , m_hecHV(hecHV)
  , m_fcalHV(fcalHV)
  , m_embPreHV(embPreHV)
  , m_emecPreHV(emecPreHV)
{
  if (m_hecHV)  m_hecHV->ref();
  if (m_fcalHV) m_fcalHV->ref();
  if (m_embPreHV)  m_embPreHV->ref();
  if (m_emecPreHV) m_emecPreHV->ref();

}

void LArHVManager::reset() const
{
  m_embHV.reset();
  m_emecHVInner.reset();
  m_emecHVOuter.reset();
  if (m_hecHV)  m_hecHV->reset();
  if (m_fcalHV) m_fcalHV->reset();
  if (m_embPreHV)  m_embPreHV->reset();
  if (m_emecPreHV) m_emecPreHV->reset(); 
 return;
}


LArHVManager::~LArHVManager()
{
  if (m_hecHV)  m_hecHV->unref();
  if (m_fcalHV) m_fcalHV->unref();
  if (m_embPreHV)  m_embPreHV->unref();
  if (m_emecPreHV) m_emecPreHV->unref();

}

const EMBHVManager& LArHVManager::getEMBHVManager() const
{
  return m_embHV;
}

const EMECHVManager& LArHVManager::getEMECHVManager(IOType IO) const
{
  return IO==EMECHVModule::INNER ? m_emecHVInner : m_emecHVOuter;
}

const HECHVManager *LArHVManager::getHECHVManager() const
{
  return m_hecHV;
}

const FCALHVManager *LArHVManager::getFCALHVManager() const
{
  return m_fcalHV;
}

const EMBPresamplerHVManager *LArHVManager::getEMBPresamplerHVManager() const
{
  return m_embPreHV;
}


const EMECPresamplerHVManager *LArHVManager::getEMECPresamplerHVManager() const
{
  return m_emecPreHV;
}

