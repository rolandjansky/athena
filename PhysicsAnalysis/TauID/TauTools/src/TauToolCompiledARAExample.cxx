/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TauToolCompiledARAaExample.cxx
/// Package : offline/PhysicsAnalysis/TauTools
/// Author  : Jyothsna Komaragiri and Dugan O'Neil
/// Created : September 2007
///
/// DESCRIPTION:
///           This example shows how to access TauExamples class in ARA C++
///
/// INSTRUCTIONS: Posted on TauTools Wiki page
///               https://twiki.cern.ch/twiki/bin/view/Atlas/TauAnalysis
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TauTools/TauToolCompiledARAExample.h"

//*****************************************
// Constructor
//*****************************************

TauToolCompiledARAExample::TauToolCompiledARAExample()
{
  mcpartTES = 0;
  taurecTES = 0;

  tt = 0;

  br_mcpart = 0;
  br_taurec = 0;

}

//***********************************************
// Destructor
//***********************************************

TauToolCompiledARAExample::~TauToolCompiledARAExample()
{}

//***********************************************
// Initialize method
//***********************************************

void TauToolCompiledARAExample::initialize(TTree* tt_in)
{
  tt = tt_in;

  ////////////////////////////////////////////////////////////////////////
  // 1: Set/Get the container keys to fetch from transient tree
  ////////////////////////////////////////////////////////////////////////
  tauexample.setTruthContainer("SpclMC");
  tauexample.setTauJetContainer("TauRecContainer");
  tauexample.setdeltaRMatchCut(0.2);
  
  br_mcpart = tt->GetBranch(tauexample.getTruthContainer().c_str());
  br_taurec = tt->GetBranch(tauexample.getTauJetContainer().c_str());

  br_mcpart->SetAddress(&mcpartTES);
  br_taurec->SetAddress(&taurecTES);

  ////////////////////////////////////////////////////////////////////////
  // 2: Create histograms to be filled in
  ////////////////////////////////////////////////////////////////////////
  tauexample.CreateHistos();
}


//***********************************************
// Execute method
//***********************************************

void TauToolCompiledARAExample::execute()
{

  ////////////////////////////////////////////////////////////////////////
  // 3: Loop over the events and call the efficiency/resolution 
  // methods for each event
  ////////////////////////////////////////////////////////////////////////
  int nevts = tt->GetEntries();

  for (Long64_t jentry=0; jentry < Long64_t(nevts); jentry++)
  {

    br_mcpart->GetEntry(jentry);
    br_taurec->GetEntry(jentry);

    if(jentry%500 == 0) {
      std::cout << "---------------------------------------" << std::endl;
      std::cout << "entry:\t" << jentry << std::endl;
      std::cout << "---------------------------------------" << std::endl;
    }

    tauexample.getTauRecProngEfficiency(taurecTES, mcpartTES);
    tauexample.getTauRecResolution(taurecTES, mcpartTES);


  }

}

//**********************************************
// Finalize method
//**********************************************

void TauToolCompiledARAExample::finalize()
{

  ////////////////////////////////////////////////////////////////////////
  // 4: Finalize and dump the histograms in ROOT file
  ////////////////////////////////////////////////////////////////////////
  tauexample.finalizeProng();
  tauexample.finalizeResolution();

  std::cout << "Finished!! " <<  std::endl;  
  //
  // uncomment the below lines if you want to store all the histos in a single ROOT file
  // otherwise the histos are stored seperately in two different ROOT files for each method.
  //TFile *exrootfile = new TFile("TauToolExamples.root","RECREATE");
  //tauexample.StoreHistos(exrootfile);
}
