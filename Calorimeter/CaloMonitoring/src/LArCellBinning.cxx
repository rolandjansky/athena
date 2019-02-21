/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ***********************************************************
//                                                            
// LArCellBinning - class to hold granularity info   
// for layers of the LAr-Calorimeter. To be used with             
// LArCellMonTools class                                       
//                                                            
// ***********************************************************                                                                                                                                                   
#include "CaloMonitoring/LArCellBinning.h"
#include <iostream>
#include "TMath.h"


static const double Pi = TMath::Pi();

using namespace CaloMonitoring;

void LArCellBinning::doEtaBinning(const int nEtaRegions, const double* etaBreakPts, const int * nEtaBins) {
  
  unsigned nBins=0;
  for(int i=0; i<nEtaRegions; i++) {
    nBins += nEtaBins[i];
  }

  //m_etaBinArray = new double[nBins+1]; // to account for the high edge of the last bin
  m_etaBinArray.resize(nBins+1,0.0);

 
  // Loop over regions and figure out size of bins in each region
  int bin=0;
  for(int iReg=0; iReg<nEtaRegions; iReg++) {
    double startEta = etaBreakPts[iReg];
    double endEta   = etaBreakPts[iReg+1];
    double regSize  = endEta - startEta;        // will be negative on C side
    double dEta     = regSize/nEtaBins[iReg]; // this will also be <= 0 on C
    
    // Fill eta bin array
    for(int iBin=0; iBin<nEtaBins[iReg]; iBin++) {
      m_etaBinArray[bin] = startEta+iBin*dEta;
      bin++;
    }
  }

  // Add last bin for this layer
  m_etaBinArray[bin] = etaBreakPts[nEtaRegions];

  // check that in ascending order
  for(int j=0; j<getNTotEtaBins(); j++) {
    if(m_etaBinArray[j] >= m_etaBinArray[j+1]) {
      std::cerr << "ERROR: Bin array not in correct order. Make sure breakPoints array\n"
		<< "is in ascending order" << std::endl;
    }
  }
}

// Allows for the case for multiple granularity regions in phi (only really happens
// for the FCAL)
void LArCellBinning::doPhiBinning(const int nPhiRegions, const double* phiBreakPts, const int * nPhiBins) {
  
  
  unsigned nBins=0;
  for(int i=0; i<nPhiRegions; i++) {
    nBins += nPhiBins[i];
  }

  m_phiBinArray.resize(nBins+1,0.0);

  // Loop over regions and figure out size of bins in each region
  int bin=0;
  for(int iReg=0; iReg<nPhiRegions; iReg++) {
    double startPhi = phiBreakPts[iReg];
    double endPhi   = phiBreakPts[iReg+1];
    double regSize  = endPhi - startPhi;        // will be negative on C side
    double dPhi     = regSize/nPhiBins[iReg]; // this will also be <= 0 on C
    
    // Fill phi bin array
    for(int iBin=0; iBin<nPhiBins[iReg]; iBin++) {
      m_phiBinArray[bin] = startPhi+iBin*dPhi;
      bin++;
    }
  }
  
  // Add last bin for this layer
  m_phiBinArray[bin] = phiBreakPts[nPhiRegions];

  // check that in ascending order
  for(int j=0; j<getNTotPhiBins(); j++) {
    if(m_phiBinArray[j] >= m_phiBinArray[j+1]) {
      std::cerr << "ERROR: Bin array not in correct order. Make sure breakPoints array\n"
		<< "is in ascending order" << std::endl;
    }
  }
}

// Does phi binning using the common case with only one phi granularity region
void LArCellBinning::doPhiBinning(int nPhiBins) {
  
  int nPhiBinArray[1] = {nPhiBins};
  double phiBreakPtsArray[2] = {-Pi,Pi};

  doPhiBinning(1, phiBreakPtsArray, nPhiBinArray);
}

LArCellBinning LArCellBinning::etaMirror() const {
  LArCellBinning mirrored;

  //copy phi-binning:
  mirrored.m_phiBinArray=this->m_phiBinArray;

  //fill eta-binning
  const size_t etaSize=this->m_etaBinArray.size();
  mirrored.m_etaBinArray.resize(etaSize);
  for (size_t i=0;i<etaSize;++i) {
    mirrored.m_etaBinArray[i]=-1.0*m_etaBinArray[etaSize-i-1];
  }
  
  // check that in ascending order
  for(int j=0; j<mirrored.getNTotEtaBins(); j++) {
    if(mirrored.m_etaBinArray[j] >= mirrored.m_etaBinArray[j+1]) {
      std::cerr << "ERROR: Bin array not in correct order. Make sure breakPoints array\n"
		<< "is in ascending order" << std::endl;
    }
  }
  return mirrored;
}
