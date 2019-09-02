/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVDescriptor.h"
#include "LArHV/HECHVSubgap.h"

class HECHVModule::Clockwork {
public:
  Clockwork(const HECHVManager* managerPtr
	    , const HECHVModule* modulePtr
	    , unsigned int side
	    , unsigned int phi
	    , unsigned int sampling)
    : manager(managerPtr)
    , iSide(side)
    , iPhi(phi)
    , iSampling(sampling)
  {
    for(unsigned iSubgap=0; iSubgap<4; ++iSubgap) {
      subgaps[iSubgap] = new HECHVSubgap(modulePtr,iSubgap);
    }
  }
  ~Clockwork()
  {
    for(unsigned iSubgap=0; iSubgap<4; ++iSubgap) {
      delete subgaps[iSubgap];
    }
  }

  const HECHVManager *manager;
  unsigned int iSide;
  unsigned int iPhi;
  unsigned int iSampling;
  
  const HECHVSubgap* subgaps[4];
};

HECHVModule::HECHVModule(const HECHVManager *manager
			 , unsigned int iSide
			 , unsigned int iPhi
			 , unsigned int iSampling)
  :m_c(new Clockwork(manager,this,iSide,iPhi,iSampling))
{
}

unsigned int HECHVModule::getSideIndex() const
{
  return m_c->iSide;
}

unsigned int HECHVModule::getSamplingIndex() const
{
  return m_c->iSampling;
}

unsigned int HECHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

unsigned int HECHVModule::getNumSubgaps() const
{
  return 4;
}

const HECHVSubgap& HECHVModule::getSubgap(unsigned int iElectrode) const
{
  return *(m_c->subgaps[iElectrode]);
}

HECHVModule::~HECHVModule()
{
  delete m_c;
}

double HECHVModule::getPhiMin() const
{
  return m_c->manager->getDescriptor().getPhiBinning().binLower(m_c->iPhi);
}

double HECHVModule::getPhiMax() const
{
  return m_c->manager->getDescriptor().getPhiBinning().binUpper(m_c->iPhi);
}

const HECHVManager& HECHVModule::getManager() const
{
  return *(m_c->manager);
}

