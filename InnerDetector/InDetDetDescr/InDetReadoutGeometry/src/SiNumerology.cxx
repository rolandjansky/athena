/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/SiNumerology.h"


namespace InDetDD {

SiNumerology::SiNumerology()
  : m_numLayers(0),
    m_numDisks(0),
    m_maxPhiCells(0),
    m_maxEtaCells(0),
    m_maxNumBarrelEta(0),
    m_maxNumEndcapRings(0),
    m_maxNumBarrelPhiModules(0),
    m_maxNumEndcapPhiModules(0),
    m_numDisksDBM(0), 
    m_numBarrelDBM(0), 
    m_maxNumEndcapPhiModulesDBM(0) 
{}
  

void SiNumerology::setNumLayers(int nLayers)
{
  m_phiModulesForLayer.resize(nLayers);
  m_etaModulesForLayer.resize(nLayers);
  m_numLayers = nLayers;
}

    
void SiNumerology::setNumDisks(int nDisks)
{
  m_numDisks = nDisks;
  m_ringsForDisk.resize(nDisks);
  m_phiModulesForDiskRing.resize(nDisks);
}

void SiNumerology::setNumRingsForDisk(int disk, int nRings)
{ 
  m_ringsForDisk[disk] = nRings;
  m_phiModulesForDiskRing[disk].resize(nRings);
  m_maxNumEndcapRings = std::max(m_maxNumEndcapRings, nRings);
}

void SiNumerology::setNumPhiModulesForDiskRing(int disk, int ring, int nPhiModules)
{
  m_phiModulesForDiskRing[disk][ring] = nPhiModules;
  m_maxNumEndcapPhiModules = std::max(m_maxNumEndcapPhiModules, nPhiModules);
}

void SiNumerology::setNumPhiModulesForLayer(int layer, int nPhiModules)
{
  m_phiModulesForLayer[layer] = nPhiModules;      
  m_maxNumBarrelPhiModules = std::max(m_maxNumBarrelPhiModules, nPhiModules);
}

void SiNumerology::setNumEtaModulesForLayer(int layer, int nEtaModules)
{
  m_etaModulesForLayer[layer] = nEtaModules;
  m_maxNumBarrelEta = std::max(m_maxNumBarrelEta, nEtaModules);
}

void SiNumerology::setMaxNumPhiCells(int cells)
{
  m_maxPhiCells = std::max(m_maxPhiCells,cells);
}

void SiNumerology::setMaxNumEtaCells(int cells)
{
  m_maxEtaCells = std::max(m_maxEtaCells,cells);
}
 
 
void SiNumerology::addBarrel(int id)
{
  m_barrelIds.push_back(id);
}

void SiNumerology::addEndcap(int id)
{
  m_endcapIds.push_back(id);
}

// DBM  
void SiNumerology::setNumDisksDBM(int nDisks) 
{ 
  m_numDisksDBM = nDisks; 
  if(nDisks>0){ 
    m_phiModulesForDiskRingDBM.resize(nDisks); 
    for(int disk=0; disk<nDisks; disk++) 
      m_phiModulesForDiskRingDBM[disk].resize(1); 
    return; 
  } 
   
  m_endcapIdsDBM.clear(); 
  m_phiModulesForDiskRingDBM.clear(); 
  m_maxNumEndcapPhiModulesDBM = 0; 
  m_phiModulesForDiskRingDBM.clear();   
 
} 
 
void SiNumerology::setNumBarrelDBM(int nBarrel) 
{ 
  m_numBarrelDBM = nBarrel; 
} 
 
void SiNumerology::setNumPhiModulesForDiskRingDBM(int disk, int ring, int nPhiModules) 
{ 
  m_phiModulesForDiskRingDBM[disk][ring] = nPhiModules; 
  m_maxNumEndcapPhiModulesDBM = std::max(m_maxNumEndcapPhiModules, nPhiModules); 
} 
 
void SiNumerology::addEndcapDBM(int id) 
{ 
  m_endcapIdsDBM.push_back(id); 
} 
	
} // End namespace
