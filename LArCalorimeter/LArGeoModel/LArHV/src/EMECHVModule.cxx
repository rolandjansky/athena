/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVDescriptor.h"
#include "LArHV/EMECHVElectrode.h"
#include <iostream>

class EMECHVModule::Clockwork {
public:
  Clockwork(const EMECHVManager* managerPtr
	    , const EMECHVModule* modulePtr
	    , IOType iWheel
	    , unsigned int side
	    , unsigned int eta
	    , unsigned int phi
	    , unsigned int sector)
    : manager(managerPtr)
    , iSide(side)
    , iEta(eta)
    , iPhi(phi)
    , iSector(sector)
  {
    unsigned nElectrodes = iWheel==OUTER ? 24 : 4;
    electrodes.reserve(nElectrodes);
    for(unsigned iElectrode=0; iElectrode<nElectrodes; ++iElectrode) {
      electrodes.push_back(new EMECHVElectrode(modulePtr, iElectrode));
    }
  }
  ~Clockwork()
  {
    auto it=electrodes.begin();
    auto itend=electrodes.end();
    for(;it!=itend;++it) {
      delete *it;
    }
  }

  const EMECHVManager *manager;
  unsigned int iSide;
  unsigned int iEta;
  unsigned int iPhi;
  unsigned int iSector;
  std::vector<const EMECHVElectrode*> electrodes;
};


EMECHVModule::EMECHVModule(const EMECHVManager *manager
			   , IOType iWheel
			   , unsigned int iSide
			   , unsigned int iEta
			   , unsigned int iPhi
			   , unsigned int iSector)
  : m_c(new Clockwork(manager,this,iWheel,iSide,iEta,iPhi,iSector))
{
}

unsigned int EMECHVModule::getEtaIndex() const
{
  return m_c->iEta;
}

unsigned int EMECHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

unsigned int EMECHVModule::getNumElectrodes() const
{
  IOType IO=m_c->manager->getWheelIndex();
  if (IO==OUTER) {
    return 24;
  }
  else {
    return 4;
  }
}

const EMECHVElectrode& EMECHVModule::getElectrode(unsigned int iElectrode) const
{
  return *(m_c->electrodes[iElectrode]);
}

EMECHVModule::~EMECHVModule()
{
  delete m_c;
}

unsigned int EMECHVModule::getSideIndex() const
{
  return m_c->iSide;
}

EMECHVModule::IOType EMECHVModule::getWheelIndex() const
{
 return m_c->manager->getWheelIndex();
}

unsigned int EMECHVModule::getSectorIndex() const
{
  return m_c->iSector;
}

double EMECHVModule::getEtaMin() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor().getEtaBinning().binUpper(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor().getEtaBinning().binLower(m_c->iEta);
  }
}

double EMECHVModule::getEtaMax() const
{
  if (m_c->iSide==0) {
    return -m_c->manager->getDescriptor().getEtaBinning().binLower(m_c->iEta);
  }
  else {
    return m_c->manager->getDescriptor().getEtaBinning().binUpper(m_c->iEta);
  }
}

double EMECHVModule::getPhiMin() const
{
  return 
    m_c->manager->getDescriptor().getPhiBinning().binLower(m_c->iPhi)+
    m_c->manager->getDescriptor().getSectorBinning().binLower(m_c->iSector);
}

double EMECHVModule::getPhiMax() const
{
  return 
    m_c->manager->getDescriptor().getPhiBinning().binLower(m_c->iPhi)+
    m_c->manager->getDescriptor().getSectorBinning().binUpper(m_c->iSector);
}

const EMECHVManager& EMECHVModule::getManager() const 
{
  return *(m_c->manager);
}
