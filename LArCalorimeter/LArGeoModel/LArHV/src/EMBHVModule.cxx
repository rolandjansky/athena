/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVDescriptor.h"
#include "LArHV/EMBHVElectrode.h"

class EMBHVModule::Clockwork {
public:
  Clockwork(const EMBHVManager* managerPtr
	    , const EMBHVModule* modulePtr
	    , unsigned int side
	    , unsigned int eta
	    , unsigned int phi
	    , unsigned int sector) 
    : manager(managerPtr)
    , iSide(side)
    , iEta(eta)
    , iPhi(phi)
    , iSector(sector) {
    for(int i=0; i<64; ++i) {
      electrodes[i] = new EMBHVElectrode(modulePtr,i);
    }
  }
  ~Clockwork() {
    for(int i=0; i<64; ++i) {
      delete electrodes[i];
    }    
  }
  const EMBHVManager* manager;
  unsigned int iSide;
  unsigned int iEta;
  unsigned int iPhi;
  unsigned int iSector;
  const EMBHVElectrode* electrodes[64]; 
};

EMBHVModule::EMBHVModule(const EMBHVManager* manager
			 , unsigned int iSide
			 , unsigned int iEta
			 , unsigned int iPhi
			 , unsigned int iSector)
  : m_c(new Clockwork(manager,this,iSide,iEta,iPhi,iSector))
{
}

unsigned int EMBHVModule::getEtaIndex() const
{
  return m_c->iEta;
}

unsigned int EMBHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

unsigned int EMBHVModule::getSectorIndex() const
{
  return m_c->iSector;
}

EMBHVModule::~EMBHVModule() 
{
  delete m_c;
}

const EMBHVElectrode& EMBHVModule::getElectrode(unsigned int iElectrode) const
{
  return *(m_c->electrodes[iElectrode]);
}

unsigned int EMBHVModule::getSideIndex() const
{
  return m_c->iSide;
}

double EMBHVModule::getEtaMin() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor().getEtaBinning().binUpper(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor().getEtaBinning().binLower(m_c->iEta);
  }
}

double EMBHVModule::getEtaMax() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor().getEtaBinning().binLower(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor().getEtaBinning().binUpper(m_c->iEta);
  }
}

double EMBHVModule::getPhiMin() const
{
  return 
    m_c->manager->getDescriptor().getPhiBinning().binLower(m_c->iPhi)+
    m_c->iSector*m_c->manager->getDescriptor().getPhiBinning().getDelta()/2.0;
}

double EMBHVModule::getPhiMax() const
{
  return 
    m_c->manager->getDescriptor().getPhiBinning().binLower(m_c->iPhi)+
    (m_c->iSector+1)*m_c->manager->getDescriptor().getPhiBinning().getDelta()/2.0;

}

const EMBHVManager& EMBHVModule::getManager() const {
  return *(m_c->manager);
}

