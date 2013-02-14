/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVDescriptor.h"
#include "LArHV/EMECHVElectrode.h"
#include <stdexcept>
#include <iostream>
class EMECHVModule::Clockwork {

public:

  const EMECHVManager *manager;
  
  unsigned int iSide;
  
  unsigned int iEta;
  
  unsigned int iPhi;
  
  unsigned int iSector;

  std::vector<EMECHVElectrodeConstLink> links;

};

//##ModelId=478D18650316
EMECHVModule::EMECHVModule(const EMECHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector):c(new Clockwork)
{
  c->manager=manager;
  c->iSide=iSide;
  c->iEta=iEta;
  c->iPhi=iPhi;
  c->iSector=iSector;
  
}

//##ModelId=478D18650319
unsigned int EMECHVModule::getEtaIndex() const
{
  return c->iEta;
}

//##ModelId=478D1865031B
unsigned int EMECHVModule::getPhiIndex() const
{
  return c->iPhi;
}

//##ModelId=478D1865031D
unsigned int EMECHVModule::getNumElectrodes() const
{
  IOType IO=c->manager->getWheelIndex();
  if (IO==OUTER) {
    return 24;
  }
  else if (IO==INNER) {
    return 4;
  }
  else {
    throw std::runtime_error("Unknown Wheel Index");
    return 0;
  }
}

//##ModelId=478D1865031F
EMECHVElectrodeConstLink EMECHVModule::getElectrode(unsigned int iElectrode) const
{

  if (c->links.capacity()==0) c->links=std::vector<EMECHVElectrodeConstLink>(getNumElectrodes(),0);
  if (!c->links[iElectrode]) c->links[iElectrode]=EMECHVElectrodeConstLink( new EMECHVElectrode(EMECHVModuleConstLink(this), iElectrode));
  return c->links[iElectrode];
  
}

//##ModelId=478D18650322
EMECHVModule::~EMECHVModule()
{
  delete c;
}

//##ModelId=47A07B14017E
unsigned int EMECHVModule::getSideIndex() const
{
  return c->iSide;
}

//##ModelId=47A28F660267
EMECHVModule::IOType EMECHVModule::getWheelIndex() const
{
 return c->manager->getWheelIndex();
}

//##ModelId=47A07AFD0301
unsigned int EMECHVModule::getSectorIndex() const
{
  return c->iSector;
}

//##ModelId=47A7A9AD000E
double EMECHVModule::getEtaMin() const
{
  if (c->iSide==0) 
     return -c->manager->getDescriptor()->getEtaBinning().binUpper(c->iEta);
  else
    return c->manager->getDescriptor()->getEtaBinning().binLower(c->iEta);
}

//##ModelId=47A7A9BF0293
double EMECHVModule::getEtaMax() const
{
  if (c->iSide==0)
    return -c->manager->getDescriptor()->getEtaBinning().binLower(c->iEta);
  else
    return c->manager->getDescriptor()->getEtaBinning().binUpper(c->iEta);
}

//##ModelId=47A7A9C10344
double EMECHVModule::getPhiMin() const
{
  return 
    c->manager->getDescriptor()->getPhiBinning().binLower(c->iPhi)+
    c->manager->getDescriptor()->getSectorBinning().binLower(c->iSector);
}

//##ModelId=47A7A9C400D5
double EMECHVModule::getPhiMax() const
{
  return 
    c->manager->getDescriptor()->getPhiBinning().binLower(c->iPhi)+
    c->manager->getDescriptor()->getSectorBinning().binUpper(c->iSector);
}

const EMECHVManager *EMECHVModule::getManager() const {
  return c->manager;
}
