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
  embHV(embHV),emecHVInner(emecHVInner),emecHVOuter(emecHVOuter),hecHV(hecHV),fcalHV(fcalHV),embPreHV(embPreHV), emecPreHV(emecPreHV)
{
  if (embHV)  embHV->ref();
  if (emecHVInner) emecHVInner->ref();
  if (emecHVOuter) emecHVOuter->ref();
  if (hecHV)  hecHV->ref();
  if (fcalHV) fcalHV->ref();
  if (embPreHV)  embPreHV->ref();
  if (emecPreHV) emecPreHV->ref();

}

void LArHVManager::reset() const
{
  if (embHV) embHV->reset();
  if (emecHVInner) emecHVInner->reset();
  if (emecHVOuter) emecHVOuter->reset();
  if (hecHV)  hecHV->reset();
  if (fcalHV) fcalHV->reset();
  if (embPreHV)  embPreHV->reset();
  if (emecPreHV) emecPreHV->reset(); 
 return;
}


LArHVManager::~LArHVManager()
{
  if (embHV)  embHV->unref();
  if (emecHVInner) emecHVInner->unref();
  if (emecHVOuter) emecHVOuter->unref();
  if (hecHV)  hecHV->unref();
  if (fcalHV) fcalHV->unref();
  if (embPreHV)  embPreHV->unref();
  if (emecPreHV) emecPreHV->unref();

}

const EMBHVManager *LArHVManager::getEMBHVManager() const
{
  return embHV;
}

const EMECHVManager *LArHVManager::getEMECHVManager(IOType IO) const
{
  if (IO==EMECHVModule::INNER) return emecHVInner;
  if (IO==EMECHVModule::OUTER) return emecHVOuter;
  return NULL;
}

const HECHVManager *LArHVManager::getHECHVManager() const
{
  return hecHV;
}

const FCALHVManager *LArHVManager::getFCALHVManager() const
{
  return fcalHV;
}

const EMBPresamplerHVManager *LArHVManager::getEMBPresamplerHVManager() const
{
  return embPreHV;
}


const EMECPresamplerHVManager *LArHVManager::getEMECPresamplerHVManager() const
{
  return emecPreHV;
}

