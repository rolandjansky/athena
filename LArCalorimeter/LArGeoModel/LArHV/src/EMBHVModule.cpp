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
EMBHVModule::EMBHVModule(const EMBHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector):c(new Clockwork)
{
  // Might need to ref the manager here.?? 
  c->manager=manager;
  c->iSide=iSide;
  c->iEta=iEta;
  c->iPhi=iPhi;
  c->iSector=iSector;
  for (int i=0;i<64;i++) c->links[i]=0;
}


//##ModelId=475659560392
unsigned int EMBHVModule::getEtaIndex() const
{
  return c->iEta;
}

//##ModelId=4756597E0324
unsigned int EMBHVModule::getPhiIndex() const
{
  return c->iPhi;
}

//##ModelId=479B9A7301DC
unsigned int EMBHVModule::getSectorIndex() const
{
  return c->iSector;
}

//##ModelId=475656A60278
EMBHVModule::~EMBHVModule() 
{
  delete c;
}


//##ModelId=475696D40106
EMBHVElectrodeConstLink EMBHVModule::getElectrode(unsigned int iElectrode) const
{
  if (!c->links[iElectrode]) c->links[iElectrode] = EMBHVElectrodeConstLink (new EMBHVElectrode(this,iElectrode));
  return c->links[iElectrode];
}

//##ModelId=479BBABD034D
unsigned int EMBHVModule::getSideIndex() const
{
  return c->iSide;
}

//##ModelId=47AA68EE020D
double EMBHVModule::getEtaMin() const
{
  if (c->iSide==0) {
    return -c->manager->getDescriptor()->getEtaBinning().binUpper(c->iEta);
  }
  else {
    return c->manager->getDescriptor()->getEtaBinning().binLower(c->iEta);
  }
}

//##ModelId=47AA68EE0223
double EMBHVModule::getEtaMax() const
{
  if (c->iSide==0) {
    return -c->manager->getDescriptor()->getEtaBinning().binLower(c->iEta);
  }
  else {
    return c->manager->getDescriptor()->getEtaBinning().binUpper(c->iEta);
  }
}

//##ModelId=47AA68EE0236
double EMBHVModule::getPhiMin() const
{
  return 
    c->manager->getDescriptor()->getPhiBinning().binLower(c->iPhi)+
    c->iSector*c->manager->getDescriptor()->getPhiBinning().getDelta()/2.0;
}

//##ModelId=47AA68EE024B
double EMBHVModule::getPhiMax() const
{
  return 
    c->manager->getDescriptor()->getPhiBinning().binLower(c->iPhi)+
    (c->iSector+1)*c->manager->getDescriptor()->getPhiBinning().getDelta()/2.0;

}



const EMBHVManager *EMBHVModule::getManager() const {
  return c->manager;
}

