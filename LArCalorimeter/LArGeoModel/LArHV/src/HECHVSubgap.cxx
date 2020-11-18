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

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int HECHVSubgap::hvLineNo(const LArHVIdMapping* hvIdMapping) const {
  return m_c->module->getManager().hvLineNo(*this,hvIdMapping);
}
#endif
