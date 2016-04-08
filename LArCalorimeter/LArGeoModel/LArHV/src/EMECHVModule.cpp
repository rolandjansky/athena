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
EMECHVModule::EMECHVModule(const EMECHVManager *manager, unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector):m_c(new Clockwork)
{
  m_c->manager=manager;
  m_c->iSide=iSide;
  m_c->iEta=iEta;
  m_c->iPhi=iPhi;
  m_c->iSector=iSector;
  
}

//##ModelId=478D18650319
unsigned int EMECHVModule::getEtaIndex() const
{
  return m_c->iEta;
}

//##ModelId=478D1865031B
unsigned int EMECHVModule::getPhiIndex() const
{
  return m_c->iPhi;
}

//##ModelId=478D1865031D
unsigned int EMECHVModule::getNumElectrodes() const
{
  IOType IO=m_c->manager->getWheelIndex();
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

  if (m_c->links.capacity()==0) m_c->links=std::vector<EMECHVElectrodeConstLink>(getNumElectrodes(),0);
  if (!m_c->links[iElectrode]) m_c->links[iElectrode]=EMECHVElectrodeConstLink( new EMECHVElectrode(EMECHVModuleConstLink(this), iElectrode));
  return m_c->links[iElectrode];
  
}

//##ModelId=478D18650322
EMECHVModule::~EMECHVModule()
{
  delete m_c;
}

//##ModelId=47A07B14017E
unsigned int EMECHVModule::getSideIndex() const
{
  return m_c->iSide;
}

//##ModelId=47A28F660267
EMECHVModule::IOType EMECHVModule::getWheelIndex() const
{
 return m_c->manager->getWheelIndex();
}

//##ModelId=47A07AFD0301
unsigned int EMECHVModule::getSectorIndex() const
{
  return m_c->iSector;
}

//##ModelId=47A7A9AD000E
double EMECHVModule::getEtaMin() const
{
  if (m_c->iSide==0) 
     return -m_c->manager->getDescriptor()->getEtaBinning().binUpper(m_c->iEta);
  else
    return m_c->manager->getDescriptor()->getEtaBinning().binLower(m_c->iEta);
}

//##ModelId=47A7A9BF0293
double EMECHVModule::getEtaMax() const
{
  if (m_c->iSide==0)
    return -m_c->manager->getDescriptor()->getEtaBinning().binLower(m_c->iEta);
  else
    return m_c->manager->getDescriptor()->getEtaBinning().binUpper(m_c->iEta);
}

//##ModelId=47A7A9C10344
double EMECHVModule::getPhiMin() const
{
  return 
    m_c->manager->getDescriptor()->getPhiBinning().binLower(m_c->iPhi)+
    m_c->manager->getDescriptor()->getSectorBinning().binLower(m_c->iSector);
}

//##ModelId=47A7A9C400D5
double EMECHVModule::getPhiMax() const
{
  return 
    m_c->manager->getDescriptor()->getPhiBinning().binLower(m_c->iPhi)+
    m_c->manager->getDescriptor()->getSectorBinning().binUpper(m_c->iSector);
}

const EMECHVManager *EMECHVModule::getManager() const {
  return m_c->manager;
}
