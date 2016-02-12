/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***********************************************************
//                                                            
// CaloLayer - class to hold granularity info   
// for layers of the Calorimeter. To be used with             
// CaloCellVecMon class                                       
//                                                            
// ***********************************************************                                                                                                                                                   

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "CaloMonitoring/CaloLayer.h"

#include "TMath.h"


static const double Pi = TMath::Pi();

using namespace CaloMonitoring;
using namespace std;

//
CaloLayer::CaloLayer(const char* name,Partition partition, int layerNumber, Side side) {

  m_name = name;
  
  m_nTotEtaBins = 0;
  m_etaBinArray = 0;
  m_nTotPhiBins = 0;
  m_phiBinArray = 0;

  setProperties(partition, layerNumber, side);
}

// need to delete dynamically allocated arrays
CaloLayer::~CaloLayer() {
  delete [] m_etaBinArray;
  delete [] m_phiBinArray;
}

// 
void CaloLayer::setProperties(Partition partition, int layerNumber, Side side) {
  m_partition = partition;
  m_layerNumber = layerNumber;
  m_side = side;
}

//
void CaloLayer::doEtaBinning(const int nEtaRegions, double* etaBreakPts, int * nEtaBins) {
  
  //
  m_nTotEtaBins=0;
  for(int i=0; i<nEtaRegions; i++) {
    m_nTotEtaBins += nEtaBins[i];
  }

  //
  const int nBins = m_nTotEtaBins;
  m_etaBinArray = new double[nBins+1]; // to account for the high edge of the last bin

  // initialize the eta bin array
  for(int iBin=0; iBin<m_nTotEtaBins; iBin++)
    m_etaBinArray[iBin] = 0.;

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
  for(int j=0; j<m_nTotEtaBins; j++) {
    if(m_etaBinArray[j] >= m_etaBinArray[j+1]) {
      cerr << "ERROR: Bin array not in correct order. Make sure breakPoints array\n"
	   << "is in ascending order" << endl;
    }
  }
}

// Allows for the case for multiple granularity regions in phi (only really happens
// for the FCAL)
void CaloLayer::doPhiBinning(const int nPhiRegions, double* phiBreakPts, int * nPhiBins) {
  
  //
  m_nTotPhiBins=0;
  for(int i=0; i<nPhiRegions; i++) {
    m_nTotPhiBins += nPhiBins[i];
  }

  //
  const int nBins = m_nTotPhiBins+1;
  m_phiBinArray = new double[nBins];

  // initialize the phi bin array
  for(int iBin=0; iBin<m_nTotPhiBins+1; iBin++)
    m_phiBinArray[iBin] = 0.;

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
  for(int j=0; j<m_nTotPhiBins; j++) {
    if(m_phiBinArray[j] >= m_phiBinArray[j+1]) {
      cerr << "ERROR: Bin array not in correct order. Make sure breakPoints array\n"
	   << "is in ascending order" << endl;
    }
  }
}

// Does phi binning using the common case with only one phi granularity region
void CaloLayer::doPhiBinning(int nPhiBins) {
  
  //
  int nPhiBinArray[1] = {nPhiBins};
  double phiBreakPtsArray[2] = {-Pi,Pi};

  //
  doPhiBinning(1, phiBreakPtsArray, nPhiBinArray);
}


//
void CaloLayer::doSideCBinning(CaloLayer * lyr) {
  
  if(lyr == this) {
    std::cerr << "CaloLayer - ERROR: can't do side C binning using self for " << getName()
	      << ". Program may crash" << std::endl;
    return;
  }

  // phi bins for side A and C are the same:
  m_nTotPhiBins = lyr->getNTotPhiBins();
  const int nPhiBins = m_nTotPhiBins;
  m_phiBinArray = new double[nPhiBins+1];
  double * sideAPhiBinArray;
  sideAPhiBinArray = lyr->getPhiBinArray();
  
  for(int bin=0; bin<nPhiBins+1; bin++) {
    m_phiBinArray[bin] = sideAPhiBinArray[bin];
  }
  // eta bin array for side C is basically the negative of the side A bin array
  // in opposite order
  m_nTotEtaBins = lyr->getNTotEtaBins();
  const int nEtaBins = m_nTotEtaBins;
  m_etaBinArray = new double[nEtaBins+1];
  double * sideAEtaBinArray;
  sideAEtaBinArray = lyr->getEtaBinArray();

  for(int bin=0; bin<nEtaBins+1; bin++) {
    m_etaBinArray[bin] = -sideAEtaBinArray[nEtaBins-bin];
  }
}

