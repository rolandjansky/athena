/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        TJVFReweighting

Author:      Dan Short
Created:     July 2011

Description: Tool to reweight JVF in WTR control region.
******************************************************************************/

// Preprocessor magic for debugging
#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

// This class' header
#include "SUSYTools/TJVFReweighting.h"

// STL includes
#include <iostream>
#include <vector>

// include math
#include <math.h>

// ROOT includes
#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TString.h>

ClassImp(SUSY::TJVFReweighting)

//=============================================================================
// Constructor
//=============================================================================
SUSY::TJVFReweighting::TJVFReweighting(const char* name) :
  TNamed(name,"notitle"),
  m_jvfWeightsFile(0)
{
}



//=============================================================================
// Destructor
//=============================================================================
SUSY::TJVFReweighting::~TJVFReweighting()
{
}



//=============================================================================
// Initialization
//=============================================================================
int SUSY::TJVFReweighting::initialize(const TString& jvfWeightsFileName, const TString& channel, int nJets){
  // use an int as a StatusCode
  int sc(0);
  
  // Cache the current directory in the root file
  TDirectory* origDir = gDirectory;
  
  //----------------------------------
  // Get all ROOT files and histograms
  //----------------------------------
  
  // Load the weights ROOT file
  m_jvfWeightsFile = new TFile(jvfWeightsFileName,"READ");
  if(!m_jvfWeightsFile){
    std::cerr << "ERROR: No weights ROOT file!" << std::endl;
    sc = -1;
    return sc;
  }
  else{ // Get the weights histograms
    if(channel == "electron"){
      if(nJets == 3) m_jvfHist = (TH1D*)m_jvfWeightsFile->Get( "jvfWeights_1Ele_3Jet_25" );
      if(nJets == 4) m_jvfHist = (TH1D*)m_jvfWeightsFile->Get( "jvfWeights_1Ele_4Jet_25" );
    }
    else if(channel == "muon"){
      if(nJets == 3) m_jvfHist = (TH1D*)m_jvfWeightsFile->Get( "jvfWeights_1Muo_3Jet_25" );
      if(nJets == 4) m_jvfHist = (TH1D*)m_jvfWeightsFile->Get( "jvfWeights_1Muo_4Jet_25" );
    }
    if (!m_jvfHist){
      std::cerr << "ERROR: No jvfWeights ROOT histogram!" << std::endl;
      sc = -2;
      return sc;
    }
    else{
      m_jvfHist->SetDirectory(0);
    }
    m_jvfWeightsFile->Close();
  }
  
  // Return to the original ROOT directory
  gDirectory = origDir;
  
  return sc;
}

//=============================================================================
// Method to get the JVF scale factor for each jet
//=============================================================================
float SUSY::TJVFReweighting::getJVFWeight(bool passJvf){
  float scaleFactor = 1.;
  if(passJvf){
    if(!m_jvfHist){ // Check that we have valid pointers
      std::cerr << "m_jvfHist not in memory! returning 1." << std::endl;
      return 1.;
    }
    scaleFactor = m_jvfHist->GetBinContent(2);
    return scaleFactor;
  }
  else{
    if(!m_jvfHist){ // Check that we have valid pointers
      std::cerr << "m_jvfHist not in memory! returning 1." << std::endl;
      return 1.;
    }
    scaleFactor = m_jvfHist->GetBinContent(1);
    return scaleFactor;
  }

  std::cerr << "Nothing done! returning 1." << std::endl;

  return 1.;
}
