/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECPresamplerHVModule.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "GeoModelKernel/CellBinning.h"
#include <stdexcept>
#include <iostream>

#include "EMECPresamplerHVPayload.h"

class EMECPresamplerHVModule::Clockwork {

public:

  const EMECPresamplerHVManager *manager;
  
  unsigned int iSide;
  unsigned int iPhi;
};

//##ModelId=478D18650316
EMECPresamplerHVModule::EMECPresamplerHVModule(const EMECPresamplerHVManager *manager, unsigned int iSide, unsigned int iPhi):c(new Clockwork)
{
  c->manager=manager;
  c->iSide=iSide;
  c->iPhi=iPhi;
}

//##ModelId=478D1865031B
unsigned int EMECPresamplerHVModule::getPhiIndex() const
{
  return c->iPhi;
}

//##ModelId=478D18650322
EMECPresamplerHVModule::~EMECPresamplerHVModule()
{
  delete c;
}

//##ModelId=47A07B14017E
unsigned int EMECPresamplerHVModule::getSideIndex() const
{
  return c->iSide;
}



//##ModelId=47A7A9AD000E
double EMECPresamplerHVModule::getEtaMin() const
{
  if (c->iSide==0) return -1.8;
  else return 1.5;
}

//##ModelId=47A7A9BF0293
double EMECPresamplerHVModule::getEtaMax() const
{
  if (c->iSide==0) return -1.5;
  else return 1.8;
}

//##ModelId=47A7A9C10344
double EMECPresamplerHVModule::getPhiMin() const
{
  return 
    c->manager->getPhiBinning()->binLower(c->iPhi);
}

//##ModelId=47A7A9C400D5
double EMECPresamplerHVModule::getPhiMax() const
{
  return 
    c->manager->getPhiBinning()->binUpper(c->iPhi);
}

const EMECPresamplerHVManager *EMECPresamplerHVModule::getManager() const {
  return c->manager;
}

//##ModelId=47AB8A070339
bool EMECPresamplerHVModule::hvOn(int iGap ) const
{
  EMECPresamplerHVPayload *payload = getManager()->getPayload(*this);
  if (payload->voltage[iGap]<-9999) return false;
  else return true;
}

double EMECPresamplerHVModule::voltage(int iGap) const {
  EMECPresamplerHVPayload *payload = getManager()->getPayload(*this);
  return payload->voltage[iGap];
}

double EMECPresamplerHVModule::current(int iGap) const {
  EMECPresamplerHVPayload *payload = getManager()->getPayload(*this);
  return payload->current[iGap];
}

void EMECPresamplerHVModule::voltage_current(int iGap,double& voltage, double&current) const {
 EMECPresamplerHVPayload *payload = getManager()->getPayload(*this);
 voltage = payload->voltage[iGap];
 current = payload->current[iGap];
}


int EMECPresamplerHVModule::hvLineNo(int iGap) const {
  EMECPresamplerHVPayload *payload = getManager()->getPayload(*this);
  return payload->hvLineNo[iGap];
}

