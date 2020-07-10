/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMBPresamplerHVDescriptor.h"
#include "EMBPresamplerHVPayload.h"

class EMBPresamplerHVModule::Clockwork {
public:
  Clockwork(const EMBPresamplerHVManager* managerPtr
	    , unsigned int side
	    , unsigned int eta
	    , unsigned int phi)
    : manager(managerPtr)
    , iSide(side)
    , iEta(eta)
    , iPhi(phi)
  {}
  const EMBPresamplerHVManager* manager;
  unsigned int iSide;
  unsigned int iEta;
  unsigned int iPhi;
};

EMBPresamplerHVModule::EMBPresamplerHVModule(const EMBPresamplerHVManager *manager
					     , unsigned int iSide
					     , unsigned int iEta
					     , unsigned int iPhi)
  : m_c(new Clockwork(manager,iSide,iEta,iPhi))
{
}

EMBPresamplerHVModule::~EMBPresamplerHVModule() 
{
  delete m_c;
}

unsigned int EMBPresamplerHVModule::getEtaIndex() const
{
  return m_c->iEta;
}

unsigned int EMBPresamplerHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

unsigned int EMBPresamplerHVModule::getSideIndex() const
{
  return m_c->iSide;
}

double EMBPresamplerHVModule::getEtaMin() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor()->getEtaPartitioning().binUpper(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor()->getEtaPartitioning().binLower(m_c->iEta);
  }
}

double EMBPresamplerHVModule::getEtaMax() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor()->getEtaPartitioning().binLower(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor()->getEtaPartitioning().binUpper(m_c->iEta);
  }
}

double EMBPresamplerHVModule::getPhiMin() const
{
  return m_c->manager->getDescriptor()->getPhiBinning().binLower(m_c->iPhi);
}

double EMBPresamplerHVModule::getPhiMax() const
{
  return m_c->manager->getDescriptor()->getPhiBinning().binUpper(m_c->iPhi);
}

const EMBPresamplerHVManager& EMBPresamplerHVModule::getManager() const 
{
  return *(m_c->manager);
}

bool EMBPresamplerHVModule::hvOn(int iGap) const
{
  EMBPresamplerHVPayload *payload = m_c->manager->getPayload(*this);
  return (payload->voltage[iGap]>=-9999);
}

double EMBPresamplerHVModule::voltage(int iGap) const {
  EMBPresamplerHVPayload *payload = m_c->manager->getPayload(*this);
  return payload->voltage[iGap];
}

double EMBPresamplerHVModule::current(int iGap) const {
  EMBPresamplerHVPayload *payload = m_c->manager->getPayload(*this);
  return payload->current[iGap];
}

void EMBPresamplerHVModule::voltage_current(int iGap,double& voltage, double&current) const {
 EMBPresamplerHVPayload *payload = m_c->manager->getPayload(*this);
 voltage = payload->voltage[iGap];
 current = payload->current[iGap];
}

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int EMBPresamplerHVModule::hvLineNo(int iGap, const LArHVIdMapping* hvIdMapping) const {
  return hvIdMapping
    ? m_c->manager->hvLineNo(*this,iGap,hvIdMapping)
    : m_c->manager->getPayload(*this)->hvLineNo[iGap];
}
#else
int EMBPresamplerHVModule::hvLineNo(int iGap) const {
  return m_c->manager->getPayload(*this)->hvLineNo[iGap];
}
#endif
