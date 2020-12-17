/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECPresamplerHVModule.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "GeoModelKernel/CellBinning.h"
#include "EMECPresamplerHVPayload.h"

class EMECPresamplerHVModule::Clockwork {
public:
  Clockwork(const EMECPresamplerHVManager* managerPtr
	    , unsigned int side
	    , unsigned int phi)
    : manager(managerPtr)
    , iSide(side)
    , iPhi(phi)
  {}
  const EMECPresamplerHVManager *manager;
  unsigned int iSide;
  unsigned int iPhi;
};

EMECPresamplerHVModule::EMECPresamplerHVModule(const EMECPresamplerHVManager* manager
					       , unsigned int iSide
					       , unsigned int iPhi)
  :m_c(new Clockwork(manager,iSide,iPhi))
{
}

EMECPresamplerHVModule::~EMECPresamplerHVModule()
{
  delete m_c;
}

unsigned int EMECPresamplerHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

unsigned int EMECPresamplerHVModule::getSideIndex() const
{
  return m_c->iSide;
}

double EMECPresamplerHVModule::getEtaMin() const
{
  if (m_c->iSide==0) return -1.8;
  else return 1.5;
}

double EMECPresamplerHVModule::getEtaMax() const
{
  if (m_c->iSide==0) return -1.5;
  else return 1.8;
}

double EMECPresamplerHVModule::getPhiMin() const
{
  return m_c->manager->getPhiBinning()->binLower(m_c->iPhi);
}

double EMECPresamplerHVModule::getPhiMax() const
{
  return m_c->manager->getPhiBinning()->binUpper(m_c->iPhi);
}

const EMECPresamplerHVManager& EMECPresamplerHVModule::getManager() const 
{
  return *(m_c->manager);
}

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int EMECPresamplerHVModule::hvLineNo(int /*iGap*/, const LArHVIdMapping* hvIdMapping) const
{
  return getManager().hvLineNo(*this,hvIdMapping);
}
#endif
