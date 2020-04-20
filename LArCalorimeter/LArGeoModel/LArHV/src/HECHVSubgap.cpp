/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/HECHVSubgap.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVManager.h"

#include "HECHVPayload.h"

class HECHVSubgap::Clockwork {
public:
  Clockwork(const HECHVModule* modulePtr
	    , unsigned int subgap)
    : module(modulePtr)
    , iSubgap(subgap)
  {}

  const HECHVModule *module;
  unsigned int iSubgap;
};

HECHVSubgap::HECHVSubgap(const HECHVModule* module
			 , unsigned int iSubgap)
  :m_c(new Clockwork(module,iSubgap))
{
}

HECHVSubgap::~HECHVSubgap()
{
  delete m_c;
}

const HECHVModule& HECHVSubgap::getModule() const
{
  return *(m_c->module);
}

unsigned int HECHVSubgap::getSubgapIndex() const
{
  return m_c->iSubgap;
}

bool HECHVSubgap::hvOn() const
{
  HECHVPayload *payload = m_c->module->getManager().getPayload(*this);
  return (payload->voltage>=-9999);
}

double HECHVSubgap::voltage() const {
  HECHVPayload *payload = m_c->module->getManager().getPayload(*this);
  return payload->voltage;
}


double HECHVSubgap::current() const {
  HECHVPayload *payload = m_c->module->getManager().getPayload(*this);
  return payload->current;
}

void HECHVSubgap::voltage_current(double& voltage, double&current) const {
 HECHVPayload *payload = m_c->module->getManager().getPayload(*this);
 voltage = payload->voltage;
 current = payload->current;
}

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int HECHVSubgap::hvLineNo(const LArHVIdMapping* hvIdMapping) const {
  return hvIdMapping
    ? m_c->module->getManager().hvLineNo(*this,hvIdMapping)
    : m_c->module->getManager().getPayload(*this)->hvLineNo;
}
#else
int HECHVSubgap::hvLineNo() const {
  return m_c->module->getManager().getPayload(*this)->hvLineNo;
}
#endif
