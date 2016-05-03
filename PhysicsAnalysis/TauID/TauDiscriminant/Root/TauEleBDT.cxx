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
    if (this->eleBDTFile != "")
    {
        string eleBDTPath = find_calibFile(this->eleBDTFile);
        if(eleBDTPath == "")
        {
	  ATH_MSG_FATAL("File: " << this->eleBDTFile << " not found! ");
	  return StatusCode::FAILURE;
        }
         
        this->eleBDT = new MethodBDT("TauBDT:EleBDT");
        
        if (!this->eleBDT->build(eleBDTPath))
        {
	  ATH_MSG_FATAL("Loading electron BDT file " << eleBDTPath << " failed!");
	  return StatusCode::FAILURE;
        }
        
        if (this->eleBitsFile != "")
        {
            string eleBitsPath = find_calibFile(this->eleBitsFile);
            if(eleBitsPath == "")
            {
	      ATH_MSG_FATAL("File: " << this->eleBitsFile << " not found! ");
	      return StatusCode::FAILURE;
            }

            this->eleBits = new MethodCuts("TauBDT:EleBits");

            if (!this->eleBits->build(eleBitsPath))
            {
	      ATH_MSG_FATAL("Loading ele bits file " << eleBitsPath << " failed!");
	      return StatusCode::FAILURE;
            }
        }
	if(this->eleBitsRootFile != ""){
		string eleBitsRootPath = find_calibFile(this->eleBitsRootFile);
        	if(eleBitsRootPath == "")
	        {
		  ATH_MSG_FATAL("File: " << this->eleBitsRootFile << " not found! ");
		  return StatusCode::FAILURE;
        	}


		this->cutsFile = new TFile(eleBitsRootPath.c_str());	
		if(this->cutsFile){
		        this->hloose  = (TH2F*)this->cutsFile->Get("h2_BDTEleDecision_pteta_loose");
		       	this->hmedium = (TH2F*)this->cutsFile->Get("h2_BDTEleDecision_pteta_medium");
               		this->htight  = (TH2F*)this->cutsFile->Get("h2_BDTEleDecision_pteta_tight");
		}

	}
	else
	{
	  ATH_MSG_FATAL("No BDT bits file was specified!");
	  return StatusCode::FAILURE;
        }

	
    }
    else
    {
      ATH_MSG_FATAL("No BDTs were initialized!");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode TauEleBDT::execute(xAOD::TauJet& tauJet)
{
   
//     Analysis::TauPID* tauJetID = tauJet.tauID();

    // Initialize scores
  tauJet.setDiscriminant(xAOD::TauJetParameters::BDTEleScore, 0.);
    
    // Initialize bits
  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTLoose, 0);
  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTMedium, 0);
  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTTight, 0);

    // do not assign a meaningful score for tau1P3P-only candidates. return.
    

    // Set the response of the electron BDT
    if (this->eleBDT)
    {
        this->eleScore = this->eleBDT->response(tauJet);
	ATH_MSG_VERBOSE("BDTEleScore: " << this->eleScore);
        if (this->eleScore < 0. || this->eleScore > 1.)
        {
	  ATH_MSG_ERROR("Error in computing BDTElecScore!");
        }
        tauJet.setDiscriminant(xAOD::TauJetParameters::BDTEleScore, this->eleScore);
    }

    // if (this->eleBDT && this->eleBits) SL: comment out, set bits by hand
    if (this->eleBDT)
    {
        
        // SL: set bits by hand, do not use bits file
        //tauJetID->setIsTau(TauJetParameters::EleBDTLoose, this->eleBits->response(0));
        //tauJetID->setIsTau(TauJetParameters::EleBDTMedium, this->eleBits->response(1));
        //tauJetID->setIsTau(TauJetParameters::EleBDTTight, this->eleBits->response(2));

      if (tauJet.nTracks() == 1){
	double eta = fabs(tauJet.track(0)->eta());
	double pt = tauJet.pt();

	if(!this->cutsFile) {
	  ATH_MSG_ERROR("Cannot open EleBDT cut file");
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTLoose,  0 );
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTMedium, 0 );
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTTight,  0 );

	  return StatusCode::SUCCESS;
	}

	if(!hloose || !hmedium || !htight){
	  ATH_MSG_ERROR("Cannot get EleBDT cut histograms");
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTLoose,  0 );
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTMedium, 0 );
	  tauJet.setIsTau(xAOD::TauJetParameters::EleBDTTight,  0 );

		
	  return StatusCode::SUCCESS;
	}

	if(pt/1000. > 799) pt = 799*1000.0;
	if(eta > 2.99) eta = 2.99;

	float score_loose = hloose->GetBinContent(hloose->FindBin(pt/1000.,eta));
	bool failed_loose =  this->eleScore < score_loose;

	float score_medium = hmedium->GetBinContent(hmedium->FindBin(pt/1000.,eta));
	bool failed_medium =  this->eleScore < score_medium;

	float score_tight = htight->GetBinContent(htight->FindBin(pt/1000.,eta));
	bool failed_tight =  this->eleScore < score_tight;

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
    if(this->cutsFile){
	this->cutsFile->Close();
	delete this->cutsFile;
	//delete this->hloose;
	//delete this->hmedium;
	//delete this->htight;
    }
        
    delete this->eleBDT;
    delete this->eleBits;
    return StatusCode::SUCCESS;
}
