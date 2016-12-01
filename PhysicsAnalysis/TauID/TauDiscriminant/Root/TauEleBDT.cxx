/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tool for BDT analysis.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#include "TauDiscriminant/TauEleBDT.h"
#include "TH2F.h"

StatusCode TauEleBDT::initialize()
{
  //StatusCode::enableFailure();
     
  if (this->m_eleBDTFile == "") {
    ATH_MSG_FATAL("No BDTs were initialized!");
    return StatusCode::FAILURE;
  }
 

  std::string eleBDTPath = find_file(this->m_eleBDTFile);
  if(eleBDTPath == "") {
    ATH_MSG_FATAL("File: " << this->m_eleBDTFile << " not found! ");
    return StatusCode::FAILURE;
  }
         
  this->m_eleBDT = new TauID::MethodBDT("TauBDT:EleBDT");
        
  if (!this->m_eleBDT->build(eleBDTPath)) {
    ATH_MSG_FATAL("Loading electron BDT file " << eleBDTPath << " failed!");
    return StatusCode::FAILURE;
  }
        
  if (!m_onlyDecorateScore and this->m_eleBitsFile != "") {
    std::string eleBitsPath = find_file(this->m_eleBitsFile);
    if(eleBitsPath == "") {
      ATH_MSG_FATAL("File: " << this->m_eleBitsFile << " not found! ");
      return StatusCode::FAILURE;
    }

    this->m_eleBits = new TauID::MethodCuts("TauBDT:EleBits");

    if (!this->m_eleBits->build(eleBitsPath)) {
      ATH_MSG_FATAL("Loading ele bits file " << eleBitsPath << " failed!");
      return StatusCode::FAILURE;
    }
  }
  if(!m_onlyDecorateScore and this->m_eleBitsRootFile != ""){
    std::string eleBitsRootPath = find_file(this->m_eleBitsRootFile);
    if(eleBitsRootPath == ""){
      ATH_MSG_FATAL("File: " << this->m_eleBitsRootFile << " not found! ");
      return StatusCode::FAILURE;
    }

    
    this->m_cutsFile = new TFile(eleBitsRootPath.c_str());	
    if(this->m_cutsFile){
      this->m_hloose  = (TH2F*)this->m_cutsFile->Get("h2_BDTEleDecision_pteta_loose");
      this->m_hmedium = (TH2F*)this->m_cutsFile->Get("h2_BDTEleDecision_pteta_medium");
      this->m_htight  = (TH2F*)this->m_cutsFile->Get("h2_BDTEleDecision_pteta_tight");
    }

  }
  else if(!m_onlyDecorateScore) {
    ATH_MSG_FATAL("No BDT bits file was specified!");
    return StatusCode::FAILURE;
  }

	
  return StatusCode::SUCCESS;
}

StatusCode TauEleBDT::decorate(xAOD::TauJet& tauJet)
{
  // decorate run 2 scores
  static SG::AuxElement::Accessor<float> acc_newScore("BDTEleScore_run2");
    
  auto nTracks = tauJet.nTracks();
  if(nTracks == 0) {
    acc_newScore(tauJet) = 0.0;
    return StatusCode::SUCCESS;
  }
    
  auto eleScore = this->m_eleBDT->response(tauJet);
  if (eleScore < 0. || eleScore > 1.) {
    ATH_MSG_ERROR("Error in computing BDTEleScore!");
  }

  acc_newScore(tauJet) = eleScore;
  return StatusCode::SUCCESS;
}


StatusCode TauEleBDT::execute(xAOD::TauJet& tauJet)
{
   

  if(m_onlyDecorateScore) {
    return decorate(tauJet);
  }

  //     Analysis::TauPID* tauJetID = tauJet.tauID();

    // Initialize scores
  tauJet.setDiscriminant(xAOD::TauJetParameters::BDTEleScore, 0.);
    
    // Initialize bits
  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTLoose, 0);
  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTMedium, 0);
  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTTight, 0);

    // do not assign a meaningful score for tau1P3P-only candidates. return.
    

    // Set the response of the electron BDT
    if (this->m_eleBDT)
    {
        this->m_eleScore = this->m_eleBDT->response(tauJet);
	ATH_MSG_VERBOSE("BDTEleScore: " << this->m_eleScore);
        if (this->m_eleScore < 0. || this->m_eleScore > 1.)
        {
	  ATH_MSG_ERROR("Error in computing BDTElecScore!");
        }
        tauJet.setDiscriminant(xAOD::TauJetParameters::BDTEleScore, this->m_eleScore);
    }

    // if (this->m_eleBDT && this->m_eleBits) SL: comment out, set bits by hand
    if (this->m_eleBDT)
    {
        
        // SL: set bits by hand, do not use bits file
        //tauJetID->setIsTau(TauJetParameters::EleBDTLoose, this->m_eleBits->response(0));
        //tauJetID->setIsTau(TauJetParameters::EleBDTMedium, this->m_eleBits->response(1));
        //tauJetID->setIsTau(TauJetParameters::EleBDTTight, this->m_eleBits->response(2));

      if (tauJet.nTracks() == 1){
	double eta = fabs(tauJet.track(0)->eta());
	double pt = tauJet.pt();

	if(!this->m_cutsFile) {
	  ATH_MSG_ERROR("Cannot open EleBDT cut file");
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTLoose,  0 );
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTMedium, 0 );
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTTight,  0 );

	  return StatusCode::SUCCESS;
	}

	if(!m_hloose || !m_hmedium || !m_htight){
	  ATH_MSG_ERROR("Cannot get EleBDT cut histograms");
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTLoose,  0 );
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTMedium, 0 );
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTTight,  0 );

		
	  return StatusCode::SUCCESS;
	}

	if(pt/1000. > 799) pt = 799*1000.0;
	if(eta > 2.99) eta = 2.99;

	float score_loose = m_hloose->GetBinContent(m_hloose->FindBin(pt/1000.,eta));
	bool failed_loose =  this->m_eleScore < score_loose;

	float score_medium = m_hmedium->GetBinContent(m_hmedium->FindBin(pt/1000.,eta));
	bool failed_medium =  this->m_eleScore < score_medium;

	float score_tight = m_htight->GetBinContent(m_htight->FindBin(pt/1000.,eta));
	bool failed_tight =  this->m_eleScore < score_tight;

	tauJet.setIsTau(xAOD::TauJetParameters::EleBDTLoose,  failed_loose );
        tauJet.setIsTau(xAOD::TauJetParameters::EleBDTMedium, failed_medium );
        tauJet.setIsTau(xAOD::TauJetParameters::EleBDTTight,  failed_tight );




	

      }
      else if(tauJet.nTracks() > 1){
	tauJet.setIsTau(xAOD::TauJetParameters::EleBDTLoose,  0 );
	tauJet.setIsTau(xAOD::TauJetParameters::EleBDTMedium, 0 );
	tauJet.setIsTau(xAOD::TauJetParameters::EleBDTTight,  0 );
	  
		

      }
      
      ATH_MSG_VERBOSE("Passes ele loose: " << tauJet.isTau(xAOD::TauJetParameters::EleBDTLoose));
      ATH_MSG_VERBOSE("Passes ele medium: " << tauJet.isTau(xAOD::TauJetParameters::EleBDTMedium));
      ATH_MSG_VERBOSE("Passes ele tight: " << tauJet.isTau(xAOD::TauJetParameters::EleBDTTight));
    }

    return StatusCode::SUCCESS;
}

StatusCode TauEleBDT::finalize()
{
    if(this->m_cutsFile){
	this->m_cutsFile->Close();
	delete this->m_cutsFile;
	//delete this->m_hloose;
	//delete this->m_hmedium;
	//delete this->m_htight;
    }
        
    if(this->m_eleBDT) { delete this->m_eleBDT; }
    if(this->m_eleBits) { delete this->m_eleBits; }
    return StatusCode::SUCCESS;
}
