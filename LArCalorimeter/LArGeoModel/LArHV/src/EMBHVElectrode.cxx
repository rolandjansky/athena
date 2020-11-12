/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int EMBHVElectrode::hvLineNo(int iGap, const LArHVIdMapping* hvIdMapping) const {
  return m_c->module->getManager().hvLineNo(*this, iGap, hvIdMapping);
}
#endif

unsigned int EMBHVElectrode::getElectrodeIndex() const{
  return m_c->iElectrode;
}
