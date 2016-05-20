/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include "CLHEP/Vector/LorentzVector.h"
//#include "CLHEP/Units/SystemOfUnits.h"

//c++
#include <utility>

// root
#include "TFile.h"
#include "TObject.h"
#include "TCollection.h"
#include "TF1.h"
#include "TH1D.h"
//#include "TFormula.h"
#include "TKey.h"

//tau
#include "xAODTau/TauJet.h"
#include "tauRecTools/TauEventData.h"
#include "tauRecTools/TauIDPileupCorrection.h"

#include "xAODEventInfo/EventInfo.h"

//using CLHEP::GeV;

/********************************************************************/
TauIDPileupCorrection::TauIDPileupCorrection(const std::string& name) :
  TauRecToolBase(name),
  m_printMissingContainerINFO(true)
{
    declareProperty("ConfigPath", m_configPath);
    declareProperty("averageEstimator", m_averageEstimator=20.);
    declareProperty("minNTrack", m_minNTrackAtVertex=2);

    declareProperty("tauContainerKey", m_tauContainerKey = "TauJets");
    declareProperty("calibrationFile1Prong", m_file1P = "fitted.pileup_1prong_hlt.root");
    declareProperty("calibrationFile3Prong", m_file3P = "fitted.pileup_multiprongs_hlt.root");
    declareProperty("vertexContainerKey", m_vertexContainerKey = "PrimaryVertices");
    declareProperty("useMu", m_useMu = false);
}

/********************************************************************/
TauIDPileupCorrection::~TauIDPileupCorrection() {
}

/********************************************************************/
StatusCode TauIDPileupCorrection::fillCalibMap( const std::string& fullPath, std::map<std::string, TF1*> &calib_map) {
  TFile * file = TFile::Open(fullPath.c_str(), "READ");
  
  TString filename(fullPath.c_str());
  filename.ToLower();
  if(filename.Contains("_nvtx") && m_useMu==true) {
    ATH_MSG_WARNING("TauIDPileupCorrection initialize() : calib file indicates nvtx based calibration, but use <mu> is set.  you should set useMu to false ");
    //m_useMu=false;
  }
  else if((filename.Contains("_mu") || filename.Contains("_nmu")) && m_useMu==false) {
    ATH_MSG_WARNING("TauIDPileupCorrection initialize() : calib file indicates mu based calibration, but use <mu> is not set.  you should set useMu to true ");
    //m_useMu=true;
  }

  if (!file) {
    ATH_MSG_FATAL("Failed to open " << fullPath);
    return StatusCode::FAILURE;
  }
  
  // Loop over file contents
  TIter keyIterator(file->GetListOfKeys());
  
  TKey *myKey=0;
  while (  (myKey = dynamic_cast<TKey*> (keyIterator())) != 0 )
    {

      TObject* obj = myKey->ReadObj();

      ATH_MSG_DEBUG("Found a key of type: " << myKey->GetClassName());
      
      //      TFormula* form = dynamic_cast<TFormula*> (obj);//TF1, TF2 public TFormula Root<6.04.02, else
      TF1* fnc = dynamic_cast<TF1*> (obj);//TF1, TF2 public TFormula Root<6.04.02, else
//       if(fnc2==0){
// #if ROOT_VERSION_CODE >= ROOT_VERSION(6,4,2)	
// 	TF1* f1 = dynamic_cast<TF1*> (obj); // TF2 public TF1 in any case
// 	if(f1){
// 	  form = dynamic_cast<TFormula*> (f1->GetFormula()->Clone());
// 	  delete f1;
// 	}
// #endif
//       }
      if(fnc==0) delete obj;
      //TF1/TFormula not owned by file, used TKey::ReadObj() which means not owned by file
      calib_map[myKey->GetName()] = fnc;
    }
  
  file->Close();
  delete file;
 
  return StatusCode::SUCCESS;
 
}

/********************************************************************/
StatusCode TauIDPileupCorrection::initialize() {


  // Open the 1P and 3P files
  std::string fullPath = find_file(m_file1P);
  if(!fillCalibMap( fullPath, m_calibFunctions1P ).isSuccess()) return StatusCode::FAILURE;
  
  fullPath = find_file(m_file3P);
  if(!fillCalibMap( fullPath, m_calibFunctions3P ).isSuccess()) return StatusCode::FAILURE;

  ATH_MSG_DEBUG("Found " << m_calibFunctions1P.size() << " variables to correct in 1P");
  ATH_MSG_DEBUG("Found " << m_calibFunctions3P.size() << " variables to correct in 3P");

  // Here comes the ugly part: define the equivalency of details and strings
  m_conversion.push_back(TauConversion("tau_centFrac_fit", xAOD::TauJetParameters::centFrac, xAOD::TauJetParameters::centFracCorrected));
  m_conversion.push_back(TauConversion("tau_massTrkSys_fit", xAOD::TauJetParameters::massTrkSys, xAOD::TauJetParameters::massTrkSysCorrected));
  m_conversion.push_back(TauConversion("tau_EMPOverTrkSysP_fit", xAOD::TauJetParameters::EMPOverTrkSysP, xAOD::TauJetParameters::EMPOverTrkSysPCorrected));
  m_conversion.push_back(TauConversion("tau_InnerTrkAvgDist_fit", xAOD::TauJetParameters::innerTrkAvgDist, xAOD::TauJetParameters::innerTrkAvgDistCorrected));
  m_conversion.push_back(TauConversion("tau_SumPtTrkFrac_fit", xAOD::TauJetParameters::SumPtTrkFrac, xAOD::TauJetParameters::SumPtTrkFracCorrected));
  m_conversion.push_back(TauConversion("tau_etOverPtLeadTrk_fit", xAOD::TauJetParameters::etOverPtLeadTrk, xAOD::TauJetParameters::etOverPtLeadTrkCorrected));
  m_conversion.push_back(TauConversion("tau_approx_ptRatio_fit", xAOD::TauJetParameters::ptRatioEflowApprox, xAOD::TauJetParameters::ptRatioEflowApproxCorrected));
  m_conversion.push_back(TauConversion("tau_approx_vistau_m_fit", xAOD::TauJetParameters::mEflowApprox, xAOD::TauJetParameters::mEflowApproxCorrected));
  m_conversion.push_back(TauConversion("tau_ChPiEMEOverCaloEME_fit", xAOD::TauJetParameters::ChPiEMEOverCaloEME, xAOD::TauJetParameters::ChPiEMEOverCaloEMECorrected));
  m_conversion.push_back(TauConversion("tau_trFlightPathSig_fit", xAOD::TauJetParameters::trFlightPathSig, xAOD::TauJetParameters::trFlightPathSigCorrected));
  m_conversion.push_back(TauConversion("tau_dRmax_fit", xAOD::TauJetParameters::dRmax, xAOD::TauJetParameters::dRmaxCorrected));
  m_conversion.push_back(TauConversion("tau_AbsipSigLeadTrk_BS_fit", xAOD::TauJetParameters::ipSigLeadTrk, xAOD::TauJetParameters::ipSigLeadTrkCorrected));
  m_conversion.push_back(TauConversion("tau_ipSigLeadTrk_fit", xAOD::TauJetParameters::ipSigLeadTrk, xAOD::TauJetParameters::ipSigLeadTrkCorrected));
  
  return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauIDPileupCorrection::execute(xAOD::TauJet& pTau) 
{ 
    // get detector axis values
    //double eta = pTau.etaDetectorAxis();
    //double absEta = std::abs(eta);

    // get primary vertex container
    StatusCode sc;
    const xAOD::VertexContainer * vxContainer = 0;
    
    // for tau trigger
    bool inTrigger = tauEventData()->inTrigger();
   
    // check if we were asked to use mu instead of nVertex from the container
    //TODO read the input root file to decide if we should use mu
    bool useMu = m_useMu;

    double nVertex = 0;
    
    // Only retrieve the container if we are not in trigger
    if ((sc.isFailure() || !inTrigger) && !useMu) {
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
      
      if (tauEventData()->hasObject("AvgInteractions")) scMu = tauEventData()->getObject("AvgInteractions", muTemp);
      else if (!inTrigger) {
        // Get mu from EventInfo
        const xAOD::EventInfo *eventInfo;
        StatusCode scEI = StatusCode::FAILURE;
        scEI = evtStore()->retrieve(eventInfo, "EventInfo");
        if (scEI.isFailure() || !eventInfo) {
          ATH_MSG_ERROR("Could not retrieve EventInfo");
          return StatusCode::FAILURE;
        }
        muTemp = eventInfo->averageInteractionsPerCrossing();
      }
      
      if(scMu.isSuccess()){
	ATH_MSG_DEBUG("AvgInteractions object in tau candidate = " << muTemp);
      } else {
	ATH_MSG_DEBUG("No AvgInteractions object in tau candidate or averageInteractionsPerCrossing in EventInfo");
      }
    
      nVertex = muTemp;
    }
    

    // Possibly get a new averageEstimator
    // TODO read averageEstimator from the root file
    double averageEstimator = m_averageEstimator;

    // Start corrections
    for(unsigned int i=0; i<m_conversion.size(); i++)
      {
	
	ATH_MSG_DEBUG("Attempting to correct variable " << m_conversion[i].detailName);
	float correction = 0;
	float tau_pt = pTau.ptDetectorAxis();

	if(pTau.nTracks() <= 1)
	  {
	    // No calibration function available
	    if(m_calibFunctions1P.count(m_conversion[i].detailName) < 1)
	      continue;
	    
	    ATH_MSG_DEBUG("Variable correction function found");
	    
	    // Calculate correction
	    correction = m_calibFunctions1P[m_conversion[i].detailName]->Eval(nVertex, tau_pt) - m_calibFunctions1P[m_conversion[i].detailName]->Eval(averageEstimator, tau_pt);
	  }

	if(pTau.nTracks() > 1)
	  {
	    // No calibration function available
	    if(m_calibFunctions3P.count(m_conversion[i].detailName) < 1)
	      continue;
	    
	    ATH_MSG_DEBUG("Variable correction function found");
	    
	    // Calculate correction
	    correction = m_calibFunctions3P[m_conversion[i].detailName]->Eval(nVertex, tau_pt) - m_calibFunctions3P[m_conversion[i].detailName]->Eval(averageEstimator, tau_pt);
	  }
	
	
	ATH_MSG_DEBUG("Correction is: " << correction);
	ATH_MSG_DEBUG("For Pile-up Estimator: " << nVertex);
	
	// Retrieve current value and set corrected value
	float uncorrectedValue;
	pTau.detail(m_conversion[i].detailUncorr, uncorrectedValue);

	float correctedValue;
	if (m_conversion[i].detailName == "tau_AbsipSigLeadTrk_BS_fit"
	    || m_conversion[i].detailName == "tau_ipSigLeadTrk_fit")
	  correctedValue = fabs(uncorrectedValue) - correction;
	else 
	  correctedValue = uncorrectedValue - correction;
	
	pTau.setDetail(m_conversion[i].detailCorr, correctedValue);
	
	ATH_MSG_DEBUG("Old value is: " << uncorrectedValue);
	ATH_MSG_DEBUG("New value is: " << correctedValue);
      }
    
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------

StatusCode TauIDPileupCorrection::finalize() {
  for( std::pair<std::string,TF1*> form : m_calibFunctions1P ) delete form.second;
  for( std::pair<std::string,TF1*> form : m_calibFunctions3P ) delete form.second;
  m_calibFunctions1P.clear();
  m_calibFunctions3P.clear();

    return StatusCode::SUCCESS;
}
