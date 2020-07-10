/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVModule.h"
#include <cmath>

#include "EMBHVPayload.h"

class EMBHVElectrode::Clockwork {
public:
  Clockwork(const EMBHVModule* modulePtr, unsigned int electrode)
    : module(modulePtr)
    , iElectrode(electrode)
  {}
  const EMBHVModule* module;
  unsigned int iElectrode;
};

EMBHVElectrode::EMBHVElectrode(const EMBHVModule* module, unsigned int iElectrode) 
  : m_c(new Clockwork(module,iElectrode))
{
}

EMBHVElectrode::~EMBHVElectrode()
{
  delete m_c;
}

const EMBHVModule& EMBHVElectrode::getModule() const
{
  return *(m_c->module);
}

double EMBHVElectrode::getPhi() const
{
  return m_c->module->getPhiMin()+m_c->iElectrode*2.0*M_PI/1024.0;
}

bool EMBHVElectrode::hvOn(int iGap) const
{
  EMBHVPayload *payload = m_c->module->getManager().getPayload(*this);
  return (payload->voltage[iGap]>=-9999);
}

double EMBHVElectrode::voltage(int iGap) const {
  EMBHVPayload *payload = m_c->module->getManager().getPayload(*this);
  return payload->voltage[iGap];
}

double EMBHVElectrode::current(int iGap) const {
  EMBHVPayload *payload = m_c->module->getManager().getPayload(*this);
  return payload->current[iGap];
}

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int EMBHVElectrode::hvLineNo(int iGap, const LArHVIdMapping* hvIdMapping) const {
  return hvIdMapping
    ? m_c->module->getManager().hvLineNo(*this, iGap, hvIdMapping)
    : m_c->module->getManager().getPayload(*this)->hvLineNo[iGap];
}
#else
int EMBHVElectrode::hvLineNo(int iGap) const {
  return m_c->module->getManager().getPayload(*this)->hvLineNo[iGap];
}
#endif

void EMBHVElectrode::voltage_current(int iGap,double& voltage, double&current) const {
 EMBHVPayload *payload = m_c->module->getManager().getPayload(*this);
 voltage = payload->voltage[iGap];
 current = payload->current[iGap];
}

unsigned int EMBHVElectrode::getElectrodeIndex() const{
  return m_c->iElectrode;
}
