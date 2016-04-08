/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBHVModuleConstLink.h"
#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVManager.h"
#include <cmath>

#include "EMBHVPayload.h"

class EMBHVElectrode::Clockwork {

public:

    const EMBHVModule *module;
    unsigned int iElectrode;

};

//##ModelId=475697F20323
EMBHVElectrode::EMBHVElectrode(EMBHVModuleConstLink module, unsigned int iElectrode) :m_c(new Clockwork) 
{
  m_c->module=&*module;
  m_c->iElectrode=iElectrode;
}


//##ModelId=475697F20382
EMBHVElectrode::~EMBHVElectrode()
{
  delete m_c;
}

//##ModelId=479BCD0B039F
EMBHVModuleConstLink EMBHVElectrode::getModule() const
{
  return m_c->module;
}

//##ModelId=47AA6C740007
double EMBHVElectrode::getPhi() const
{
  return m_c->module->getPhiMin()+m_c->iElectrode*2.0*M_PI/1024.0;
}

//##ModelId=47AB8993017C
bool EMBHVElectrode::hvOn(int iGap) const
{
  EMBHVPayload *payload = m_c->module->getManager()->getPayload(*this);
  if (payload->voltage[iGap]<-9999) return false;
  else return true;
}

double EMBHVElectrode::voltage(int iGap) const {
  EMBHVPayload *payload = m_c->module->getManager()->getPayload(*this);
  return payload->voltage[iGap];
}

double EMBHVElectrode::current(int iGap) const {
  EMBHVPayload *payload = m_c->module->getManager()->getPayload(*this);
  return payload->current[iGap];
}


int EMBHVElectrode::hvLineNo(int iGap) const {
  EMBHVPayload *payload = m_c->module->getManager()->getPayload(*this);
  return payload->hvLineNo[iGap];
}



void EMBHVElectrode::voltage_current(int iGap,double& voltage, double&current) const {
 EMBHVPayload *payload = m_c->module->getManager()->getPayload(*this);
 voltage = payload->voltage[iGap];
 current = payload->current[iGap];
}

unsigned int EMBHVElectrode::getElectrodeIndex() const{
  return m_c->iElectrode;
}
