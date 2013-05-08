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
// Athena Algorithm to derive the calibration factors//
//                                                   //
///////////////////////////////////////////////////////


#include "TauTools/FindFactors.h"


//***********************************
// Constructor
//***********************************

FindFactors::FindFactors(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty( "TruthParticleContainer", m_TruthParticleContainerName = tools.get_TruthParticleContainerName());
  declareProperty( "TauJetContainer",        m_TauJetContainerName        = tools.get_TauJetContainerName());

  declareProperty( "DeltaRMatchCut",         m_DeltaRMatchCut             = tools.get_DeltaRMatchCut());

  declareProperty( "ROOTFileName",           m_ROOTFileName               = "TauCalibrationTools.root");
}





//************************************
// Initialize
//************************************

StatusCode FindFactors::initialize()
{
  msg(MSG::INFO) << "*****FindFactors: Initializing*****" << endreq;
  
  // Setting the properties of the TauCalibrationTools class
  //--------------------------------------------------------
  
  tools.set_TruthParticleContainerName(m_TruthParticleContainerName);
  tools.set_TauJetContainerName(m_TauJetContainerName);
  
  tools.set_DeltaRMatchCut(m_DeltaRMatchCut);
  
  // Creating the output ROOT file
  //------------------------------
  
  rootfile = new TFile(m_ROOTFileName.c_str(), "RECREATE");
  
  
  // Pass the TTreePointer to TauCalibrationTools
  //---------------------------------------------

  TTree* tt = new TTree("ETRatios", "Stores Et ratio (truth/reco), eta and numtracks");

  tools.set_TTreePointer(tt);
  tools.create_TreeStructure();

  return StatusCode::SUCCESS;
}





//************************************
// Execute
//************************************

StatusCode FindFactors::execute()
{
  msg(MSG::INFO) << "*****FindFactors: Executing*****" << endreq;

  // Declaring a storegate pointer
  //------------------------------

  StatusCode storegate_pointer;


  // Getting access to the truth particle container through the mcpartTES pointer
  //-----------------------------------------------------------------------------
  
  const TruthParticleContainer* mcpartTES = 0;
  storegate_pointer = evtStore()->retrieve(mcpartTES, m_TruthParticleContainerName.c_str());
  
  // Verifying if the pointer is successfully initialized
  //-----------------------------------------------------
  
  if( storegate_pointer.isFailure() || !mcpartTES )
  {
    msg(MSG::ERROR) << "No AOD Truth Particle Container with name: "
		    << m_TruthParticleContainerName
		    << " found in TDS"
		    << endreq;
    
    return StatusCode::FAILURE;
  }
  
  msg(MSG::DEBUG) << "Truth Particle Container found and retrieved" << endreq;





  // Getting access to the tau jet container through the tauRecTES pointer
  //----------------------------------------------------------------------
  
  const Analysis::TauJetContainer* tauRecTES = 0;
  storegate_pointer = evtStore()->retrieve(tauRecTES, m_TauJetContainerName.c_str());
  
  
  // verifying if the pointer is successfully initialized
  //-----------------------------------------------------
  
  if( storegate_pointer.isFailure() || !tauRecTES )
  {
    msg(MSG::ERROR) << "No AOD Tau Jet Container with name: "
		    << m_TauJetContainerName
		    << " found in TDS"
		    << endreq;
      
    return StatusCode::FAILURE;
  }
  
  msg(MSG::DEBUG) << "Tau Jet Container found and retrieved" << endreq;
  



  // Giving the pointer to our CalibrationFactorsTools object
  //---------------------------------------------------------

  tools.set_TruthParticleContainer(mcpartTES);
  tools.set_TauJetContainer(tauRecTES);

  
  // Filling the tree
  //-----------------

  tools.fill_Tree();

  return StatusCode::SUCCESS;
}





//************************************
// Finalize
//************************************

StatusCode FindFactors::finalize()
{
  msg(MSG::INFO) << "*****FindFactors: Finalizing*****" << endreq;

  rootfile->Write();
  rootfile->Close();
  delete rootfile;

  return StatusCode::SUCCESS;
}
