/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVModuleConstLink.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVManager.h"
#include <cmath>

#include "EMECHVPayload.h"

class EMECHVElectrode::Clockwork {

public:

  //##ModelId=47A6A14D0354
  const EMECHVModule * module;
  
  //##ModelId=478D253700BA
  unsigned int iElectrode;

};


//##ModelId=478D253700BB
EMECHVElectrode::EMECHVElectrode(EMECHVModuleConstLink module, unsigned int iElectrode):m_c(new Clockwork())
{
  m_c->module=&*module;

  m_c->iElectrode=iElectrode;
}

//##ModelId=478D253700BD
unsigned int EMECHVElectrode::getElectrodeIndex() const
{
  return m_c->iElectrode;
}

//##ModelId=478D253700BF
EMECHVElectrode::~EMECHVElectrode()
{
  delete m_c;
}

//##ModelId=47AA6B8B03A4
EMECHVModuleConstLink EMECHVElectrode::getModule() const
{
  return m_c->module;
}

//##ModelId=47AA6C980288
double EMECHVElectrode::getPhi() const
{
  double N= m_c->module->getWheelIndex()==EMECHVModule::OUTER ? 768:256;
  return m_c->module->getPhiMin()+m_c->iElectrode*2.0*M_PI/N;
}

//##ModelId=47AB8A070339
bool EMECHVElectrode::hvOn(int iGap ) const
{
  EMECHVPayload *payload = m_c->module->getManager()->getPayload(*this);
  if (payload->voltage[iGap]<-9999) return false;
  else return true;
}

double EMECHVElectrode::voltage(int iGap) const {
  EMECHVPayload *payload = m_c->module->getManager()->getPayload(*this);
  return payload->voltage[iGap];
}

double EMECHVElectrode::current(int iGap) const {
  EMECHVPayload *payload = m_c->module->getManager()->getPayload(*this);
  return payload->current[iGap];
}

void EMECHVElectrode::voltage_current(int iGap,double& voltage, double&current) const {
 EMECHVPayload *payload = m_c->module->getManager()->getPayload(*this);
 voltage = payload->voltage[iGap];
 current = payload->current[iGap];
}

int EMECHVElectrode::hvLineNo(int iGap) const {
  EMECHVPayload *payload = m_c->module->getManager()->getPayload(*this);
  return payload->hvLineNo[iGap];
}

