/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVDescriptor.h"
#include "LArHV/HECHVSubgap.h"

class HECHVModule::Clockwork {

public:

  const HECHVManager *manager;
  unsigned int iSide;
  unsigned int iPhi;
  unsigned int iSampling;
  
  HECHVSubgapConstLink linkArray[4];

};



//##ModelId=47A078E80280
HECHVModule::HECHVModule(const HECHVManager *manager, unsigned int iSide, unsigned int iPhi, unsigned int iSampling):m_c(new Clockwork())
{
  m_c->manager=manager;
  m_c->iSide=iSide;
  m_c->iPhi=iPhi;
  m_c->iSampling=iSampling;
}

//##ModelId=47A078E80285
unsigned int HECHVModule::getSideIndex() const
{
  return m_c->iSide;
}

//##ModelId=47A078E80287
unsigned int HECHVModule::getSamplingIndex() const
{
  return m_c->iSampling;
}

//##ModelId=47A078E80289
unsigned int HECHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

//##ModelId=47A078E8028D
unsigned int HECHVModule::getNumSubgaps() const
{
  return 4;
}

//##ModelId=47A078E8028F
HECHVSubgapConstLink HECHVModule::getSubgap(unsigned int iElectrode) const
{
  if (!m_c->linkArray[iElectrode]) m_c->linkArray[iElectrode]=HECHVSubgapConstLink( new HECHVSubgap(this, iElectrode));
  return m_c->linkArray[iElectrode];
}

//##ModelId=47A078E80292
HECHVModule::~HECHVModule()
{
  delete m_c;
}

//##ModelId=47AB88CD00F2
double HECHVModule::getPhiMin() const
{
  return m_c->manager->getDescriptor()->getPhiBinning().binLower(m_c->iPhi);
}

//##ModelId=47AB88CD0105
double HECHVModule::getPhiMax() const
{
  return m_c->manager->getDescriptor()->getPhiBinning().binUpper(m_c->iPhi);
}

const HECHVManager *HECHVModule::getManager() const 
{
  return m_c->manager;
}

