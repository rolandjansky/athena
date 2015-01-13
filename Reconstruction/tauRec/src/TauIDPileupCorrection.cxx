/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PathResolver/PathResolver.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// root
#include "TFile.h"
#include "TObject.h"
#include "TCollection.h"
#include "TF1.h"
#include "TH1D.h"
#include "TKey.h"

//tau
#include "xAODTau/TauJet.h"
#include "tauRec/TauCandidateData.h"
#include "tauRec/TauIDPileupCorrection.h"

using CLHEP::GeV;

/********************************************************************/
TauIDPileupCorrection::TauIDPileupCorrection(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
TauToolBase(type, name, parent)
{
    declareInterface<TauToolBase > (this);
    declareProperty("averageEstimator", m_averageEstimator=20.);
    declareProperty("minNTrack", m_minNTrackAtVertex=2);

    declareProperty("tauContainerKey", m_tauContainerKey = "TauJets");
    declareProperty("calibrationFile1Prong", m_file1P = "fitted.pileup_1prong_hlt.root");
    declareProperty("calibrationFile3Prong", m_file3P = "fitted.pileup_multiprongs_hlt.root");
    declareProperty("vertexContainerKey", m_vertexContainerKey = "PrimaryVertices");
}

/********************************************************************/
TauIDPileupCorrection::~TauIDPileupCorrection() {
}

/********************************************************************/
StatusCode TauIDPileupCorrection::initialize() {


  // Open the 1P and 3P files
  std::string fullPath = PathResolver::find_file(m_file1P, "DATAPATH");
  TFile * file1P = TFile::Open(fullPath.c_str(), "READ");
  
  if (!file1P) {
    ATH_MSG_FATAL("Failed to open " << fullPath);
    return StatusCode::FAILURE;
  }
  
  // Loop over file contents
  TIter keyIterator1P(file1P->GetListOfKeys());
  
  while ( TObject *myObject = keyIterator1P() )
    {
      TKey* myKey = (TKey*)myObject;
      
      ATH_MSG_DEBUG("Found a key of type: " << myKey->GetClassName());
      
      if(std::string(myKey->GetClassName()) == "TF1")
	{
	  ATH_MSG_DEBUG("Key is a TF1: " << myKey->GetName());
	  // Copy the function to local map
	  m_calibFunctions1P[myKey->GetName()] = *((TF1*)file1P->Get(myKey->GetName()));
	}
    }
  
  file1P->Close();
  
  fullPath = PathResolver::find_file(m_file3P, "DATAPATH");
  TFile * file3P = TFile::Open(fullPath.c_str(), "READ");  

  TIter keyIterator3P(file3P->GetListOfKeys());
  
  while ( TObject *myObject = keyIterator3P() )
    {
      TKey* myKey = (TKey*)myObject;
      
      ATH_MSG_DEBUG("Found a key of type: " << myKey->GetClassName());
      
      if(std::string(myKey->GetClassName()) == "TF1")
	{
	  ATH_MSG_DEBUG("Key is a TF1: " << myKey->GetName());
	  // Copy the function to local map
	  m_calibFunctions3P[myKey->GetName()] = *((TF1*)file3P->Get(myKey->GetName()));
	}
    }

  file3P->Close();

  ATH_MSG_DEBUG("Found " << m_calibFunctions1P.size() << " variables to correct in 1P");
  ATH_MSG_DEBUG("Found " << m_calibFunctions3P.size() << " variables to correct in 3P");

  // Here comes the ugly part: define the equivalency of details and strings
  m_conversion.push_back(TauConversion("tau_centFrac_fit", xAOD::TauJetParameters::centFrac, xAOD::TauJetParameters::centFracCorrected));
  m_conversion.push_back(TauConversion("tau_massTrkSys_fit", xAOD::TauJetParameters::massTrkSys, xAOD::TauJetParameters::massTrkSysCorrected));
  m_conversion.push_back(TauConversion("tau_EMPOverTrkSysP_fit", xAOD::TauJetParameters::EMPOverTrkSysP, xAOD::TauJetParameters::EMPOverTrkSysPCorrected));
  m_conversion.push_back(TauConversion("tau_InnerTrkAvgDist_fit", xAOD::TauJetParameters::innerTrkAvgDist, xAOD::TauJetParameters::innerTrkAvgDistCorrected));
  m_conversion.push_back(TauConversion("tau_SumPtTrkFrac_fit", xAOD::TauJetParameters::SumPtTrkFrac, xAOD::TauJetParameters::SumPtTrkFracCorrected));
  m_conversion.push_back(TauConversion("tau_etOverPtLeadTrk_fit", xAOD::TauJetParameters::etOverPtLeadTrk, xAOD::TauJetParameters::etOverPtLeadTrkCorrected));
  m_conversion.push_back(TauConversion("tau_approx_ptRatio_fit", xAOD::TauJetParameters::ptRatioEflowApprox, xAOD::TauJetParameters::ptRatioEflowApprox));
  m_conversion.push_back(TauConversion("tau_approx_vistau_fit", xAOD::TauJetParameters::mEflowApprox, xAOD::TauJetParameters::mEflowApprox));
  m_conversion.push_back(TauConversion("tau_ChPiEMEOverCaloEME_fit", xAOD::TauJetParameters::ChPiEMEOverCaloEME, xAOD::TauJetParameters::ChPiEMEOverCaloEMECorrected));
  m_conversion.push_back(TauConversion("tau_trFlightPathSig_fit", xAOD::TauJetParameters::trFlightPathSig, xAOD::TauJetParameters::trFlightPathSigCorrected));
  m_conversion.push_back(TauConversion("tau_dRmax_fit", xAOD::TauJetParameters::dRmax, xAOD::TauJetParameters::dRmaxCorrected));
  m_conversion.push_back(TauConversion("tau_AbsipSigLeadTrk_BS_fit", xAOD::TauJetParameters::ipSigLeadTrk, xAOD::TauJetParameters::ipSigLeadTrkCorrected));
  
  return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauIDPileupCorrection::execute(TauCandidateData *data) 
{ 
    xAOD::TauJet *pTau = data->xAODTau;

    if (pTau == NULL) {
        ATH_MSG_ERROR("no candidate given");
        return StatusCode::FAILURE;
    }
       
    // get detector axis values
    //double eta = pTau->etaDetectorAxis();
    //double absEta = std::abs(eta);

    // get primary vertex container
    StatusCode sc;
    const xAOD::VertexContainer * vxContainer = 0;
    
    // for tau trigger
    bool inTrigger = false;
    if (data->hasObject("InTrigger?")) sc = data->getObject("InTrigger?", inTrigger);
   
    int nVertex = 0;
    
    // Only retrieve the container if we are not in trigger
    if (sc.isFailure() || !inTrigger) {
      // try standard 
      if (evtStore()->retrieve(vxContainer, m_vertexContainerKey).isFailure() || !vxContainer) {
	if (m_printMissingContainerINFO) {
	  ATH_MSG_WARNING(m_vertexContainerKey << " container not found --> skip TauEnergyCalibrationLC (no further info) ");
	  m_printMissingContainerINFO=false;
	}
	return StatusCode::SUCCESS;
      }
    
      // Calculate nVertex
      xAOD::VertexContainer::const_iterator vx_iter = vxContainer->begin();
      xAOD::VertexContainer::const_iterator vx_end = vxContainer->end();
      
      for (; vx_iter != vx_end; ++vx_iter) {
	if ((*vx_iter)->nTrackParticles() >= m_minNTrackAtVertex)
	  ++nVertex;
      }
    
      ATH_MSG_DEBUG("calculated nVertex " << nVertex );           
    
    } else {

      StatusCode scMu = StatusCode::FAILURE;
      double muTemp = 0.0;
      
      if (data->hasObject("AvgInteractions")) scMu = data->getObject("AvgInteractions", muTemp);
      
      if(scMu.isSuccess()){
	ATH_MSG_DEBUG("AvgInteractions object in tau candidate = " << muTemp);
      } else {
	ATH_MSG_DEBUG("No AvgInteractions object in tau candidate");
      }
    
      nVertex = muTemp;
  
    }
    



    // Start corrections
    for(unsigned int i=0; i<m_conversion.size(); i++)
      {
	
	ATH_MSG_DEBUG("Attempting to correct variable " << m_conversion[i].detailName);
	float correction = 0;

	if(pTau->nTracks() <= 1)
	  {
	    // No calibration function available
	    if(m_calibFunctions1P.count(m_conversion[i].detailName) < 1)
	      continue;
	    
	    ATH_MSG_DEBUG("Variable correction function found");
	    
	    // Calculate correction
	    correction = m_calibFunctions1P[m_conversion[i].detailName].Eval(nVertex) - m_calibFunctions1P[m_conversion[i].detailName].Eval(m_averageEstimator);
	  }

	if(pTau->nTracks() > 1)
	  {
	    // No calibration function available
	    if(m_calibFunctions3P.count(m_conversion[i].detailName) < 1)
	      continue;
	    
	    ATH_MSG_DEBUG("Variable correction function found");
	    
	    // Calculate correction
	    correction = m_calibFunctions3P[m_conversion[i].detailName].Eval(nVertex) - m_calibFunctions3P[m_conversion[i].detailName].Eval(m_averageEstimator);
	  }
	
	
	ATH_MSG_DEBUG("Correction is: " << correction);
	ATH_MSG_DEBUG("For Pile-up Estimator: " << nVertex);
	
	// Retrieve current value and set corrected value
	float uncorrectedValue;
	pTau->detail(m_conversion[i].detailUncorr, uncorrectedValue);
	pTau->setDetail(m_conversion[i].detailCorr, static_cast<float>( uncorrectedValue - correction ));
	
	ATH_MSG_DEBUG("Old value is: " << uncorrectedValue);
	ATH_MSG_DEBUG("New value is: " << uncorrectedValue - correction);
	
      }

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------

StatusCode TauIDPileupCorrection::finalize() {
    return StatusCode::SUCCESS;
}
