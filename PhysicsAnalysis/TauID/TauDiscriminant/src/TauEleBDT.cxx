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
#include <PathResolver/PathResolver.h>

using namespace xAOD;

StatusCode TauEleBDT::prepare(const TauDetailsManager& manager)
{
    if (this->eleBDTFile != "")
    {
        string eleBDTPath = PathResolver::find_file(this->eleBDTFile, "DATAPATH");
        if(eleBDTPath == "")
        {
            msg(MSG::FATAL) << "File: " << this->eleBDTFile << " not found! " << endreq;
            return StatusCode::FAILURE;
        }
         
        this->eleBDT = new MethodBDT("TauBDT:EleBDT");
        this->eleBDT->setDetails(manager);
        
        if (!this->eleBDT->build(eleBDTPath))
        {
            msg(MSG::FATAL) << "Loading electron BDT file " << eleBDTPath << " failed!" << endreq;
            return StatusCode::FAILURE;
        }
        
        if (this->eleBitsFile != "")
        {
            string eleBitsPath = PathResolver::find_file(this->eleBitsFile, "DATAPATH");
            if(eleBitsPath == "")
            {
                msg(MSG::FATAL) << "File: " << this->eleBitsFile << " not found! " << endreq;
                return StatusCode::FAILURE;
            }

            this->eleBits = new MethodCuts("TauBDT:EleBits");
            this->eleBits->setDetails(manager);
            this->eleBits->addVariable("BDT",&(this->eleScore),'F');

            if (!this->eleBits->build(eleBitsPath))
            {
                msg(MSG::FATAL) << "Loading ele bits file " << eleBitsPath << " failed!" << endreq;
                return StatusCode::FAILURE;
            }
        }
	if(this->eleBitsRootFile != ""){
		string eleBitsRootPath = PathResolver::find_file(this->eleBitsRootFile, "DATAPATH");
        	if(eleBitsRootPath == "")
	        {
        	    msg(MSG::FATAL) << "File: " << this->eleBitsRootFile << " not found! " << endreq;
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
        	msg(MSG::FATAL) << "No BDT bits file was specified!" << endreq;
	     	return StatusCode::FAILURE;
        }

	
    }
    else
    {
        msg(MSG::FATAL) << "No BDTs were initialized!" << endreq;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode TauEleBDT::execute(xAOD::TauJet *tauJet, FakeTauBits* /*bits*/, FakeTauScores* /*scores*/)
{
   
//     Analysis::TauPID* tauJetID = tauJet->tauID();

    // Initialize scores
    tauJet->setDiscriminant(TauJetParameters::BDTEleScore, 0.);
    
    // Initialize bits
    tauJet->setIsTau(TauJetParameters::EleBDTLoose, 0);
    tauJet->setIsTau(TauJetParameters::EleBDTMedium, 0);
    tauJet->setIsTau(TauJetParameters::EleBDTTight, 0);

    // do not assign a meaningful score for tau1P3P-only candidates. return.
    

    // Set the response of the electron BDT
    if (this->eleBDT)
    {
        this->eleScore = this->eleBDT->response();
        if (msgLvl(MSG::VERBOSE))
        {
            msg(MSG::VERBOSE) << "BDTEleScore: " << this->eleScore << endreq;
        }
        if (this->eleScore < 0. || this->eleScore > 1.)
        {
            msg(MSG::ERROR) << "Error in computing BDTElecScore!" << endreq;
        }
        tauJet->setDiscriminant(TauJetParameters::BDTEleScore, this->eleScore);
    }

    // if (this->eleBDT && this->eleBits) SL: comment out, set bits by hand
    if (this->eleBDT)
    {
        
        // SL: set bits by hand, do not use bits file
        //tauJetID->setIsTau(TauJetParameters::EleBDTLoose, this->eleBits->response(0));
        //tauJetID->setIsTau(TauJetParameters::EleBDTMedium, this->eleBits->response(1));
        //tauJetID->setIsTau(TauJetParameters::EleBDTTight, this->eleBits->response(2));

      if (tauJet->nTracks() == 1){
	double eta = fabs(tauJet->track(0)->eta());
	double pt = tauJet->pt();

	if(!this->cutsFile) {
		msg(MSG::ERROR)<<"Cannot open EleBDT cut file"<<endreq;
		tauJet->setIsTau(TauJetParameters::EleBDTLoose,  0 );
	        tauJet->setIsTau(TauJetParameters::EleBDTMedium, 0 );
        	tauJet->setIsTau(TauJetParameters::EleBDTTight,  0 );

		return StatusCode::SUCCESS;
	}

	if(!hloose || !hmedium || !htight){
		msg(MSG::ERROR)<<"Cannot get EleBDT cut histograms"<<endreq;
                tauJet->setIsTau(TauJetParameters::EleBDTLoose,  0 );
                tauJet->setIsTau(TauJetParameters::EleBDTMedium, 0 );
                tauJet->setIsTau(TauJetParameters::EleBDTTight,  0 );

		
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

	tauJet->setIsTau(TauJetParameters::EleBDTLoose,  failed_loose );
        tauJet->setIsTau(TauJetParameters::EleBDTMedium, failed_medium );
        tauJet->setIsTau(TauJetParameters::EleBDTTight,  failed_tight );




	

      }
      else if(tauJet->nTracks() > 1){
	  tauJet->setIsTau(TauJetParameters::EleBDTLoose,  0 );
          tauJet->setIsTau(TauJetParameters::EleBDTMedium, 0 );
          tauJet->setIsTau(TauJetParameters::EleBDTTight,  0 );
	  
		

      }
      
      if (msgLvl(MSG::VERBOSE))
        {
	  msg(MSG::VERBOSE) << "Passes ele loose: " << tauJet->isTau(TauJetParameters::EleBDTLoose) << endreq;
	  msg(MSG::VERBOSE) << "Passes ele medium: " << tauJet->isTau(TauJetParameters::EleBDTMedium) << endreq;
	  msg(MSG::VERBOSE) << "Passes ele tight: " << tauJet->isTau(TauJetParameters::EleBDTTight) << endreq;
        }
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
