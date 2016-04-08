/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/LArHVManager.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/LArHVManager.h"

LArHVManager::LArHVManager(const EMBHVManager  *embHV, const EMECHVManager *emecHVInner, const EMECHVManager *emecHVOuter, const HECHVManager *hecHV, const FCALHVManager *fcalHV, const EMBPresamplerHVManager *embPreHV, const EMECPresamplerHVManager *emecPreHV): 
  m_embHV(embHV),m_emecHVInner(emecHVInner),m_emecHVOuter(emecHVOuter),m_hecHV(hecHV),m_fcalHV(fcalHV),m_embPreHV(embPreHV), m_emecPreHV(emecPreHV)
{
  if (m_embHV)  m_embHV->ref();
  if (m_emecHVInner) m_emecHVInner->ref();
  if (m_emecHVOuter) m_emecHVOuter->ref();
  if (m_hecHV)  m_hecHV->ref();
  if (m_fcalHV) m_fcalHV->ref();
  if (m_embPreHV)  m_embPreHV->ref();
  if (m_emecPreHV) m_emecPreHV->ref();

}

void LArHVManager::reset() const
{
  if (m_embHV) m_embHV->reset();
  if (m_emecHVInner) m_emecHVInner->reset();
  if (m_emecHVOuter) m_emecHVOuter->reset();
  if (m_hecHV)  m_hecHV->reset();
  if (m_fcalHV) m_fcalHV->reset();
  if (m_embPreHV)  m_embPreHV->reset();
  if (m_emecPreHV) m_emecPreHV->reset(); 
 return;
}


LArHVManager::~LArHVManager()
{
  if (m_embHV)  m_embHV->unref();
  if (m_emecHVInner) m_emecHVInner->unref();
  if (m_emecHVOuter) m_emecHVOuter->unref();
  if (m_hecHV)  m_hecHV->unref();
  if (m_fcalHV) m_fcalHV->unref();
  if (m_embPreHV)  m_embPreHV->unref();
  if (m_emecPreHV) m_emecPreHV->unref();

}

const EMBHVManager *LArHVManager::getEMBHVManager() const
{
  return m_embHV;
}

const EMECHVManager *LArHVManager::getEMECHVManager(IOType IO) const
{
  if (IO==EMECHVModule::INNER) return m_emecHVInner;
  if (IO==EMECHVModule::OUTER) return m_emecHVOuter;
  return NULL;
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

