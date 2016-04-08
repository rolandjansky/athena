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
EMBPresamplerHVModule::EMBPresamplerHVModule(const EMBPresamplerHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi):m_c(new Clockwork)
{
  // Might need to ref the manager here.?? 
  m_c->manager=manager;
  m_c->iSide=iSide;
  m_c->iEta=iEta;
  m_c->iPhi=iPhi;
}


//##ModelId=475659560392
unsigned int EMBPresamplerHVModule::getEtaIndex() const
{
  return m_c->iEta;
}

//##ModelId=4756597E0324
unsigned int EMBPresamplerHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

//##ModelId=475656A60278
EMBPresamplerHVModule::~EMBPresamplerHVModule() 
{
  delete m_c;
}



//##ModelId=479BBABD034D
unsigned int EMBPresamplerHVModule::getSideIndex() const
{
  return m_c->iSide;
}

//##ModelId=47AA68EE020D
double EMBPresamplerHVModule::getEtaMin() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor()->getEtaPartitioning().binUpper(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor()->getEtaPartitioning().binLower(m_c->iEta);
  }
}

//##ModelId=47AA68EE0223
double EMBPresamplerHVModule::getEtaMax() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor()->getEtaPartitioning().binLower(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor()->getEtaPartitioning().binUpper(m_c->iEta);
  }
}

//##ModelId=47AA68EE0236
double EMBPresamplerHVModule::getPhiMin() const
{
  return 
    m_c->manager->getDescriptor()->getPhiBinning().binLower(m_c->iPhi);
}

//##ModelId=47AA68EE024B
double EMBPresamplerHVModule::getPhiMax() const
{
  return 
    m_c->manager->getDescriptor()->getPhiBinning().binUpper(m_c->iPhi);

}



const EMBPresamplerHVManager *EMBPresamplerHVModule::getManager() const {
  return m_c->manager;
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
