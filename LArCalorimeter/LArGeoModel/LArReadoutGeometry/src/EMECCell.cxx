/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/EMECCell.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVDescriptor.h"
#include "LArHV/EMECPresamplerHVModule.h"

// Class EMECCell 

EMECCell::~EMECCell()
{
}

unsigned int EMECCell::getNumElectrodes() const {
  if (m_electrode.size()==0 && !m_presamplerModule) initHV();
  return m_electrode.size();
}

const EMECHVElectrodeConstLink & EMECCell::getElectrode (unsigned int i) const {
  if (m_electrode.size()==0 && !m_presamplerModule) initHV();
  return m_electrode[i];
}

const EMECPresamplerHVModuleConstLink & EMECCell::getPresamplerHVModule () const {
  if (m_electrode.size()==0 && !m_presamplerModule) initHV();
  return m_presamplerModule;
}

void EMECCell::initHV() const {

  if (getSamplingIndex()==0) {
    const EMECPresamplerHVManager *presamplerHVManager=getDescriptor()->getManager()->getPresamplerHVManager();
    double phiUpper = getPhiMaxNominal();
    double phiLower = getPhiMinNominal();
    double phi=fabs(phiUpper+phiLower)/2.0;

    const CellBinning * phiBinning=presamplerHVManager->getPhiBinning();
    unsigned int iPhi = int((phi - phiBinning->getStart())/phiBinning->getDelta()) + phiBinning->getFirstDivisionNumber();
    unsigned int iSide=getEndcapIndex();
    m_presamplerModule = presamplerHVManager->getHVModule(iSide,iPhi);

  }
  else {
  
    double eta=fabs(getEtaMax()+getEtaMin())/2.0;
    EMECHVModule::IOType iotype = eta < 2.5 ? EMECHVModule::OUTER : EMECHVModule::INNER;
    //std::cout << "in  EMECCell::initHV()   eta,type " << eta << " " << iotype << std::endl;
    
    const EMECHVManager *hvManager=getDescriptor()->getManager()->getHVManager(iotype);
    double phiLower = getPhiMinNominal();
    double phiUpper = getPhiMaxNominal();
    double phiMid=(phiLower+phiUpper)/2.0;
    
    const CellPartitioning & etaBinning=hvManager->getDescriptor()->getEtaBinning();
    const CellBinning & phiBinning=hvManager->getDescriptor()->getPhiBinning();
    const CellBinning & sectorBinning=hvManager->getDescriptor()->getSectorBinning();
    unsigned int iEta=0;
    for (unsigned int i=etaBinning.getFirstDivisionNumber();i<=etaBinning.getFirstDivisionNumber()+etaBinning.getNumDivisions();i++) {
      if (eta>=etaBinning.binLower(i) && eta<=etaBinning.binUpper(i)) {
	iEta=i;
      }
    }
    unsigned int iPhi = int((phiMid - phiBinning.getStart())/phiBinning.getDelta()) + phiBinning.getFirstDivisionNumber();
    
    double N         = iotype==EMECHVModule::OUTER ? 768:256;
    
    unsigned int iSector = int((phiMid   - phiBinning.binLower(iPhi) - sectorBinning.getStart())/sectorBinning.getDelta()) + sectorBinning.getFirstDivisionNumber();
    double phiSectorStart = phiBinning.binLower(iPhi) +iSector*sectorBinning.getDelta();
    
    unsigned int iSide=getEndcapIndex();
    
    double phi0 = (iSide==1) ? M_PI/N : -(M_PI)/N;      // delta phi between first absorber and electrode at phi=0

    const double inv_2pi = M_1_PI/2;
    unsigned int iOffset  = int(N*(phiLower-phiSectorStart + phi0)*inv_2pi +0.5);
    double dPhi = fabs((phiUpper-phiLower));
    unsigned int NE      =int(dPhi*inv_2pi*N+0.5);
    
    //std::cout << " iSide,iEta,iPhi,iSector " << iSide << " " << iEta << " " << iPhi << " " << iSector << std::endl;
    //std::cout << " phi1,phi2 cell, phi1 sector " << phiLower << " " << phiUpper << " " << phiSectorStart << std::endl;
    //std::cout << " first electrode " << iOffset << " " << " Nelectrodes " << NE << std::endl;
    
    
    
    EMECHVModuleConstLink hvMod = hvManager->getHVModule(iSide,iEta,iPhi,iSector);  
    //std::cout <<   " hvMod << " << hvMod << std::endl;
    for (unsigned int iElectrode=iOffset;iElectrode<iOffset+NE;iElectrode++) {
      EMECHVElectrodeConstLink hvElec = hvMod->getElectrode(iElectrode);
      m_electrode.push_back(hvElec);
    }
    
  } 
}


