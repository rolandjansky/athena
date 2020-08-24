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

LArHVManager::LArHVManager()
  : m_embHV()
  , m_emecHVInner(EMECHVModule::INNER)
  , m_emecHVOuter(EMECHVModule::OUTER)
  , m_hecHV()
  , m_fcalHV()
  , m_embPreHV()
  , m_emecPreHV()
{
}

void LArHVManager::reset() const
{
  m_embHV.reset();
  m_emecHVInner.reset();
  m_emecHVOuter.reset();
  m_hecHV.reset();
  m_fcalHV.reset();
  m_embPreHV.reset();
  m_emecPreHV.reset(); 
}


LArHVManager::~LArHVManager()
{
}

const EMBHVManager& LArHVManager::getEMBHVManager() const
{
  return m_embHV;
}

const EMECHVManager& LArHVManager::getEMECHVManager(IOType IO) const
{
  return IO==EMECHVModule::INNER ? m_emecHVInner : m_emecHVOuter;
}

const HECHVManager& LArHVManager::getHECHVManager() const
{
  return m_hecHV;
}

const FCALHVManager& LArHVManager::getFCALHVManager() const
{
  return m_fcalHV;
}

const EMBPresamplerHVManager& LArHVManager::getEMBPresamplerHVManager() const
{
  return m_embPreHV;
}

const EMECPresamplerHVManager& LArHVManager::getEMECPresamplerHVManager() const
{
  return m_emecPreHV;
}

