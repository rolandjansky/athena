/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/FCALHVLine.h"
#include "LArHV/FCALHVModule.h"
#include "LArHV/FCALHVManager.h"
#include "FCALHVPayload.h"

class FCALHVLine::Clockwork {
public:
  Clockwork(const FCALHVModule* modulePtr
	    , unsigned int line)
    : module(modulePtr)
    , iLine(line)
  {}

  const FCALHVModule *module;
  unsigned int iLine;
};

FCALHVLine::FCALHVLine(const FCALHVModule* module
		       , unsigned int iLine)
  : m_c(new Clockwork(module,iLine))
{
}

FCALHVLine::~FCALHVLine()
{
  delete m_c;
}

const FCALHVModule& FCALHVLine::getModule() const
{
  return *(m_c->module);
}

unsigned int FCALHVLine::getLineIndex() const
{
  return m_c->iLine;
}

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int FCALHVLine::hvLineNo(const LArHVIdMapping* hvIdMapping) const {
  return m_c->module->getManager().hvLineNo(*this,hvIdMapping);
}
#endif
