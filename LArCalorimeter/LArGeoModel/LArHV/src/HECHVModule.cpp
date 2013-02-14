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
HECHVModule::HECHVModule(const HECHVManager *manager, unsigned int iSide, unsigned int iPhi, unsigned int iSampling):c(new Clockwork())
{
  c->manager=manager;
  c->iSide=iSide;
  c->iPhi=iPhi;
  c->iSampling=iSampling;
}

//##ModelId=47A078E80285
unsigned int HECHVModule::getSideIndex() const
{
  return c->iSide;
}

//##ModelId=47A078E80287
unsigned int HECHVModule::getSamplingIndex() const
{
  return c->iSampling;
}

//##ModelId=47A078E80289
unsigned int HECHVModule::getPhiIndex() const
{
  return c->iPhi;
}

//##ModelId=47A078E8028D
unsigned int HECHVModule::getNumSubgaps() const
{
  return 4;
}

//##ModelId=47A078E8028F
HECHVSubgapConstLink HECHVModule::getSubgap(unsigned int iElectrode) const
{
  if (!c->linkArray[iElectrode]) c->linkArray[iElectrode]=HECHVSubgapConstLink( new HECHVSubgap(this, iElectrode));
  return c->linkArray[iElectrode];
}

//##ModelId=47A078E80292
HECHVModule::~HECHVModule()
{
  delete c;
}

//##ModelId=47AB88CD00F2
double HECHVModule::getPhiMin() const
{
  return c->manager->getDescriptor()->getPhiBinning().binLower(c->iPhi);
}

//##ModelId=47AB88CD0105
double HECHVModule::getPhiMax() const
{
  return c->manager->getDescriptor()->getPhiBinning().binUpper(c->iPhi);
}

const HECHVManager *HECHVModule::getManager() const 
{
  return c->manager;
}

