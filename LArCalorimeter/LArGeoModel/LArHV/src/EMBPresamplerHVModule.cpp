/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMBPresamplerHVDescriptor.h"
#include "EMBPresamplerHVPayload.h"
class EMBPresamplerHVModule::Clockwork {

public:

  const EMBPresamplerHVManager *       manager;
  
  unsigned int iSide;
  
  unsigned int iEta;
  
  unsigned int iPhi;

};

//##ModelId=475656A60228
EMBPresamplerHVModule::EMBPresamplerHVModule(const EMBPresamplerHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi):c(new Clockwork)
{
  // Might need to ref the manager here.?? 
  c->manager=manager;
  c->iSide=iSide;
  c->iEta=iEta;
  c->iPhi=iPhi;
}


//##ModelId=475659560392
unsigned int EMBPresamplerHVModule::getEtaIndex() const
{
  return c->iEta;
}

//##ModelId=4756597E0324
unsigned int EMBPresamplerHVModule::getPhiIndex() const
{
  return c->iPhi;
}

//##ModelId=475656A60278
EMBPresamplerHVModule::~EMBPresamplerHVModule() 
{
  delete c;
}



//##ModelId=479BBABD034D
unsigned int EMBPresamplerHVModule::getSideIndex() const
{
  return c->iSide;
}

//##ModelId=47AA68EE020D
double EMBPresamplerHVModule::getEtaMin() const
{
  if (c->iSide==0) {
    return -c->manager->getDescriptor()->getEtaPartitioning().binUpper(c->iEta);
  }
  else {
    return c->manager->getDescriptor()->getEtaPartitioning().binLower(c->iEta);
  }
}

//##ModelId=47AA68EE0223
double EMBPresamplerHVModule::getEtaMax() const
{
  if (c->iSide==0) {
    return -c->manager->getDescriptor()->getEtaPartitioning().binLower(c->iEta);
  }
  else {
    return c->manager->getDescriptor()->getEtaPartitioning().binUpper(c->iEta);
  }
}

//##ModelId=47AA68EE0236
double EMBPresamplerHVModule::getPhiMin() const
{
  return 
    c->manager->getDescriptor()->getPhiBinning().binLower(c->iPhi);
}

//##ModelId=47AA68EE024B
double EMBPresamplerHVModule::getPhiMax() const
{
  return 
    c->manager->getDescriptor()->getPhiBinning().binUpper(c->iPhi);

}



const EMBPresamplerHVManager *EMBPresamplerHVModule::getManager() const {
  return c->manager;
}

bool EMBPresamplerHVModule::hvOn(int iGap) const
{
  EMBPresamplerHVPayload *payload = getManager()->getPayload(*this);
  if (payload->voltage[iGap]<-9999) return false;
  else return true;
}

double EMBPresamplerHVModule::voltage(int iGap) const {
  EMBPresamplerHVPayload *payload = getManager()->getPayload(*this);
  return payload->voltage[iGap];
}

double EMBPresamplerHVModule::current(int iGap) const {
  EMBPresamplerHVPayload *payload = getManager()->getPayload(*this);
  return payload->current[iGap];
}

void EMBPresamplerHVModule::voltage_current(int iGap,double& voltage, double&current) const {
 EMBPresamplerHVPayload *payload = getManager()->getPayload(*this);
 voltage = payload->voltage[iGap];
 current = payload->current[iGap];
}

int EMBPresamplerHVModule::hvLineNo(int iGap) const {
  EMBPresamplerHVPayload *payload = getManager()->getPayload(*this);
  return payload->hvLineNo[iGap];
}
