/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVManager.h"
#include <cmath>

#include "EMECHVPayload.h"

class EMECHVElectrode::Clockwork {
public:
  Clockwork(const EMECHVModule* modulePtr, unsigned int electrode)
    : module(modulePtr)
    , iElectrode(electrode)
  {}
  const EMECHVModule * module;
  unsigned int iElectrode;
};

EMECHVElectrode::EMECHVElectrode(const EMECHVModule* module, unsigned int iElectrode)
  : m_c(new Clockwork(module,iElectrode))
{
}

unsigned int EMECHVElectrode::getElectrodeIndex() const
{
  return m_c->iElectrode;
}

EMECHVElectrode::~EMECHVElectrode()
{
  delete m_c;
}

const EMECHVModule& EMECHVElectrode::getModule() const
{
  return *(m_c->module);
}

double EMECHVElectrode::getPhi() const
{
  double N= m_c->module->getWheelIndex()==EMECHVModule::OUTER ? 768:256;
  return m_c->module->getPhiMin()+m_c->iElectrode*2.0*M_PI/N;
}

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int EMECHVElectrode::hvLineNo(int iGap, const LArHVIdMapping* hvIdMapping) const {
  return m_c->module->getManager().hvLineNo(*this, iGap, hvIdMapping);
}
#endif
