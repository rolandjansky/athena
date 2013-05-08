/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////
//                                                   //
// Name:   src/TauCalibrationTools.cxx               //
// Author: Michel Trottier-McDonald                  //
//                                                   //
// Created: June 2009                                //
//                                                   //
// DESCRIPTION:                                      //
//                                                   //
// Provides methods to derive the calibration        //
// factors for taus while running tau reconstruction,//
// as well as Plots to evaluate the calibration.     //
//                                                   //
///////////////////////////////////////////////////////



//********************************
// Include statements
//********************************

#include "TauTools/TauCalibrationTools.h"

#include <iostream>
#include <fstream>
#include "math.h"


//********************************
// Constructor
//********************************

TauCalibrationTools::TauCalibrationTools()
{
  m_TruthParticleContainerName = "SpclMC";
  m_TauJetContainerName        = "TauRecContainer";
  m_DeltaRMatchCut             = 0.2;
}



//********************************
// Destructor
//********************************

TauCalibrationTools::~TauCalibrationTools()
{}




//********************************
// create_TreeStructure
//********************************

void TauCalibrationTools::create_TreeStructure()
{
  // Creating the different trees, and their branches
  //-------------------------------------------------

  tree->Branch("Variables", &tauData.ETratio, "ETratio:ETpull:ET:truthVisET:eta:truthViseta:ntracks/I:nPi0/I");
}




//********************************
// fill_Tree
//********************************

void TauCalibrationTools::fill_Tree()
{
  // Getting the hadronic truth taus
  //--------------------------------

  TruthParticleContainer* hadTau = 0;
  hadTau = truthHandler.getHadronicTruth(truthTauContainer);
  
  
  TruthParticleContainer::const_iterator truthTauItr  = hadTau->begin();
  TruthParticleContainer::const_iterator truthTauItrE = hadTau->end();

  
  // Loop over all truth taus in the event
  //------------------------------------------

  for(; truthTauItr != truthTauItrE; ++truthTauItr)
  {
    
    // Find a matched reconstructed tau to the current truth tau
    
    bool isMatched = false;
    const Analysis::TauJet* matchedRecoTau = matchtool.matchTruthGeomOnly(*truthTauItr, recoTauContainer, isMatched);
    
    
    // In the case the truth tau is matched
    
    if(isMatched)
    {
      
      // Retrieve visible Et from truth tau
      
      CLHEP::HepLorentzVector truthTauVisSum = truthHandler.getTauVisibleSumTruth(*truthTauItr);
      
      float truthVisET  = truthTauVisSum.et()/1000;
      float truthViseta = fabs(truthTauVisSum.eta());
      
      
      // Retrieve ET from the reconstructed tau
      
      const Analysis::TauCommonDetails* tauDetails(0);
      tauDetails = matchedRecoTau->details<const Analysis::TauCommonDetails>();
      
      const Analysis::TauPID* taupid = matchedRecoTau->tauID();
      
      bool cutSafeLoose    = taupid->isTau(TauJetParameters::TauCutLoose);
      float recoET         = matchedRecoTau->et()/1000;
      float recoeta        = fabs(matchedRecoTau->eta());
      int recoNtracks      = matchedRecoTau->numTrack();
      double recoETnoCalib = (tauDetails->seedCalo_etEMCalib()+tauDetails->seedCalo_etHadCalib())/1000;
      int nPi0             = int(tauDetails->nPi0());
      
      
      // Retrieve the seed type
      
      int tautype = recoHandler.TauType(matchedRecoTau);
      
      
      // Fill the tree
      
      //if((cutSafeLoose)&&((tautype == 1)||(tautype == 3)))
      if((cutSafeLoose)&&((tautype == TAU_REC)||(tautype == TAU_BOTH)))
      {
	tauData.ETratio     = truthVisET/recoET;
	tauData.ETpull      = (recoETnoCalib-truthVisET)/truthVisET;
	tauData.ET          = recoET;
	tauData.truthVisET  = truthVisET;
	tauData.eta         = recoeta;
	tauData.truthViseta = truthViseta;
	tauData.ntracks     = recoNtracks;
	tauData.nPi0        = nPi0;
	
	tree->Fill();
      }
    } // if isMatched
  } // loop on truth taus
  
  // cleaning up
  if (hadTau != 0)
    delete hadTau;

} // fill_Tree()
