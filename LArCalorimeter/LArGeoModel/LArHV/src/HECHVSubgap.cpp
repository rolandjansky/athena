/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/HECHVSubgap.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVManager.h"

#include "HECHVPayload.h"

class HECHVSubgap::Clockwork {

public:

    const HECHVModule *module;

    unsigned int iSubgap;
};

//##ModelId=47A0797F024A
HECHVSubgap::HECHVSubgap(HECHVModuleConstLink module, unsigned int iSubgap):c(new Clockwork())
{
  c->module=&*module;
  c->iSubgap=iSubgap;
}

//##ModelId=47A0797F024D
HECHVModuleConstLink HECHVSubgap::getModule() const
{
  return c->module;
}

//##ModelId=47A0797F024F
unsigned int HECHVSubgap::getSubgapIndex() const
{
  return c->iSubgap;
}

//##ModelId=47A0797F0251
HECHVSubgap::~HECHVSubgap()
{
  delete c;
}

//##ModelId=47AB8A2103AC
bool HECHVSubgap::hvOn() const
{
  HECHVPayload *payload = c->module->getManager()->getPayload(*this);
  if (payload->voltage<-9999) return false;
  else return true;
}

double HECHVSubgap::voltage() const {
  HECHVPayload *payload = c->module->getManager()->getPayload(*this);
  return payload->voltage;
}


double HECHVSubgap::current() const {
  HECHVPayload *payload = c->module->getManager()->getPayload(*this);
  return payload->current;
}

void HECHVSubgap::voltage_current(double& voltage, double&current) const {
 HECHVPayload *payload = c->module->getManager()->getPayload(*this);
 voltage = payload->voltage;
 current = payload->current;
}


int HECHVSubgap::hvLineNo() const {
  HECHVPayload *payload = c->module->getManager()->getPayload(*this);
  return payload->hvLineNo;
}
