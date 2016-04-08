/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.7%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.7%.codegen_version

//## begin module%4466058E0231.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4466058E0231.CLHEP::cm

//## begin module%4466058E0231.cp preserve=no
//## end module%4466058E0231.cp

//## Module: LArDetectorManager%4466058E0231; Pseudo Package body
//## Source file: /home/atlas/TEST-12.0.2-00/LArCalorimeter/LArGeoModel/LArReadoutGeometry/src/LArDetectorManager.cxx

//## begin module%4466058E0231.additionalIncludes preserve=no
//## end module%4466058E0231.additionalIncludes

//## begin module%4466058E0231.includes preserve=yes
//## end module%4466058E0231.includes

// LArDetectorManager
#include "LArReadoutGeometry/LArDetectorManager.h"
//## begin module%4466058E0231.additionalDeclarations preserve=yes
//## end module%4466058E0231.additionalDeclarations

#include <algorithm>


// Class LArDetectorManager 

LArDetectorManager::LArDetectorManager()
  : m_embManager (0),
    m_emecManager (0),
    m_hecManager (0),
    m_fcalManager (0),
    m_isTestBeam(false)
  //## begin LArDetectorManager::LArDetectorManager%4466058E0231_const.hasinit preserve=no
  //## end LArDetectorManager::LArDetectorManager%4466058E0231_const.hasinit
  //## begin LArDetectorManager::LArDetectorManager%4466058E0231_const.initialization preserve=yes
  //## end LArDetectorManager::LArDetectorManager%4466058E0231_const.initialization
{
  //## begin LArDetectorManager::LArDetectorManager%4466058E0231_const.body preserve=yes
  //## end LArDetectorManager::LArDetectorManager%4466058E0231_const.body
}

LArDetectorManager::LArDetectorManager (const EMBDetectorManager* emb, const EMECDetectorManager* emec, const HECDetectorManager* hec, const FCALDetectorManager* fcal)
  //## begin LArDetectorManager::LArDetectorManager%446607D90148.hasinit preserve=no
  //## end LArDetectorManager::LArDetectorManager%446607D90148.hasinit
  //## begin LArDetectorManager::LArDetectorManager%446607D90148.initialization preserve=yes
  :m_embManager(emb),m_emecManager(emec),m_hecManager(hec),m_fcalManager(fcal),m_isTestBeam(true)
  //## end LArDetectorManager::LArDetectorManager%446607D90148.initialization
{
  //## begin LArDetectorManager::LArDetectorManager%446607D90148.body preserve=yes
  setName("LArMgr");
  //## end LArDetectorManager::LArDetectorManager%446607D90148.body
}


LArDetectorManager::~LArDetectorManager()
{
  //## begin LArDetectorManager::~LArDetectorManager%4466058E0231_dest.body preserve=yes
  for (unsigned int i=0;i<getNumTreeTops();i++) getTreeTop(i)->unref();
  //## end LArDetectorManager::~LArDetectorManager%4466058E0231_dest.body
}



//## Other Operations (implementation)
PVConstLink LArDetectorManager::getTreeTop (unsigned int i) const
{
  //## begin LArDetectorManager::getTreeTop%446607D300E0.body preserve=yes
  return m_treeTop[i];
  //## end LArDetectorManager::getTreeTop%446607D300E0.body
}

unsigned int LArDetectorManager::getNumTreeTops () const
{
  //## begin LArDetectorManager::getNumTreeTops%446607D300E7.body preserve=yes
  return m_treeTop.size();
  //## end LArDetectorManager::getNumTreeTops%446607D300E7.body
}

void LArDetectorManager::addTreeTop (PVLink treeTop)
{
  //## begin LArDetectorManager::addTreeTop%446608BE02DD.body preserve=yes
  if (std::find(m_treeTop.begin(),m_treeTop.end(),treeTop)!=m_treeTop.end())  return;
  m_treeTop.push_back(treeTop);
  treeTop->ref();
  //## end LArDetectorManager::addTreeTop%446608BE02DD.body
}

// Additional Declarations
  //## begin LArDetectorManager%4466058E0231.declarations preserve=yes
  //## end LArDetectorManager%4466058E0231.declarations

//## begin module%4466058E0231.epilog preserve=yes
//## end module%4466058E0231.epilog
