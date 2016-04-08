/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVDescriptor.h"
class EMBHVModule::Clockwork {

public:

  const EMBHVManager *       manager;
  
  unsigned int iSide;
  
  unsigned int iEta;
  
  unsigned int iPhi;

  unsigned int iSector;

  EMBHVElectrodeConstLink links[64]; 
  
};

//##ModelId=475656A60228
EMBHVModule::EMBHVModule(const EMBHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector):m_c(new Clockwork)
{
  // Might need to ref the manager here.?? 
  m_c->manager=manager;
  m_c->iSide=iSide;
  m_c->iEta=iEta;
  m_c->iPhi=iPhi;
  m_c->iSector=iSector;
  for (int i=0;i<64;i++) m_c->links[i]=0;
}


//##ModelId=475659560392
unsigned int EMBHVModule::getEtaIndex() const
{
  return m_c->iEta;
}

//##ModelId=4756597E0324
unsigned int EMBHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

//##ModelId=479B9A7301DC
unsigned int EMBHVModule::getSectorIndex() const
{
  return m_c->iSector;
}

//##ModelId=475656A60278
EMBHVModule::~EMBHVModule() 
{
  delete m_c;
}


//##ModelId=475696D40106
EMBHVElectrodeConstLink EMBHVModule::getElectrode(unsigned int iElectrode) const
{
  if (!m_c->links[iElectrode]) m_c->links[iElectrode] = EMBHVElectrodeConstLink (new EMBHVElectrode(this,iElectrode));
  return m_c->links[iElectrode];
}

//##ModelId=479BBABD034D
unsigned int EMBHVModule::getSideIndex() const
{
  return m_c->iSide;
}

//##ModelId=47AA68EE020D
double EMBHVModule::getEtaMin() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor()->getEtaBinning().binUpper(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor()->getEtaBinning().binLower(m_c->iEta);
  }
}

//##ModelId=47AA68EE0223
double EMBHVModule::getEtaMax() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor()->getEtaBinning().binLower(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor()->getEtaBinning().binUpper(m_c->iEta);
  }
}

//##ModelId=47AA68EE0236
double EMBHVModule::getPhiMin() const
{
  return 
    m_c->manager->getDescriptor()->getPhiBinning().binLower(m_c->iPhi)+
    m_c->iSector*m_c->manager->getDescriptor()->getPhiBinning().getDelta()/2.0;
}

//##ModelId=47AA68EE024B
double EMBHVModule::getPhiMax() const
{
  return 
    m_c->manager->getDescriptor()->getPhiBinning().binLower(m_c->iPhi)+
    (m_c->iSector+1)*m_c->manager->getDescriptor()->getPhiBinning().getDelta()/2.0;

}



const EMBHVManager *EMBHVModule::getManager() const {
  return m_c->manager;
}

