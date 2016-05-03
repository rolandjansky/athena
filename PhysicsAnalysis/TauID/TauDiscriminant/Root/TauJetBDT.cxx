/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tool for BDT analysis.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#include "TauDiscriminant/TauJetBDT.h"


TauJetBDT::TauJetBDT(const string& name):
  TauDiscriToolBase(name),
  jetScore(-1.),
  jetBDTFile(""),
  jetSigBitsFile(""),
  jetSigTransFile(""),
  jetBDT(nullptr),
  jetSigBits(nullptr),
  jetSigTrans(nullptr)
{
  declareProperty("jetBDT", this->jetBDTFile);
  declareProperty("jetSigBits",this->jetSigBitsFile);
  declareProperty("jetSigTrans",this->jetSigTransFile);
}


StatusCode TauJetBDT::initialize()
{
    if (this->jetBDTFile != "")
    {
        string jetBDTPath = find_calibFile(this->jetBDTFile);
    
        if(jetBDTPath == "")
        {
	  ATH_MSG_FATAL("File: " << this->jetBDTFile << " not found! ");
	  return StatusCode::FAILURE;
        }
    
        this->jetBDT = new TauID::MethodBDT("TauBDT:JetBDT");
    
        if (!this->jetBDT->build(jetBDTPath))
        {
	  ATH_MSG_FATAL("Loading jet BDT file " << jetBDTPath << " failed!");
	  return StatusCode::FAILURE;
        }
        
        if (this->jetSigBitsFile != "")
        {
            string jetSigBitsPath = find_calibFile(this->jetSigBitsFile);
            if(jetSigBitsPath == "")
            {
	      ATH_MSG_FATAL("File: " << this->jetSigBitsFile << " not found! ");
	      return StatusCode::FAILURE;
            }
            
            this->jetSigBits = new TauID::MethodCuts("TauBDT:JetSigBits");

            if (!this->jetSigBits->build(jetSigBitsPath))
            {
	      ATH_MSG_FATAL("Loading jet bits file " << jetSigBitsPath << " failed!");
	      return StatusCode::FAILURE;
            }
        }

        // Flat signal transformed jet score
        if (this->jetSigTransFile != "")
        {
            string jetSigTransPath = find_calibFile(this->jetSigTransFile);
        
            if(jetSigTransPath == "")
            {
	      ATH_MSG_FATAL("File: " << this->jetSigTransFile << " not found! ");
	      return StatusCode::FAILURE;
            }
        
            this->jetSigTrans = new TauID::MethodTransform("TauBDT:JetBDT:SignalTranform");
        
            if (!this->jetSigTrans->build(jetSigTransPath))
            {
	      ATH_MSG_FATAL("Loading jet BDT signal transformation file " << jetSigTransPath << " failed!");
	      return StatusCode::FAILURE;
            }
        }
    }
    else
    {
      ATH_MSG_FATAL("No BDTs were initialized!");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode TauJetBDT::execute(xAOD::TauJet& tauJet)
{
   
//     Analysis::TauPID* tauJetID = tauJet.tauID();
    bool loose, medium, tight;

    // Initialize scores
    tauJet.setDiscriminant(xAOD::TauJetParameters::BDTJetScore, static_cast<float>(0.));
    tauJet.setDiscriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans, static_cast<float>(0.));
    
    // Initialize bits
    tauJet.setIsTau(xAOD::TauJetParameters::JetBDTSigLoose, false);
    tauJet.setIsTau(xAOD::TauJetParameters::JetBDTSigMedium, false);
    tauJet.setIsTau(xAOD::TauJetParameters::JetBDTSigTight, false);
    
    // Set the response of the jet BDT
    if (this->jetBDT)
    {


        this->jetScore = this->jetBDT->response(tauJet);
	ATH_MSG_VERBOSE("BDTJetScore: " << this->jetScore);
        if (this->jetScore < 0. || this->jetScore > 1.)
        {
	  ATH_MSG_ERROR("Error in computing BDTJetScore!");
        }
        tauJet.setDiscriminant(xAOD::TauJetParameters::BDTJetScore, this->jetScore);
    }
    else
    {
        tauJet.setDiscriminant(xAOD::TauJetParameters::BDTJetScore, 0.);
    }

    if (this->jetBDT && this->jetSigBits)
    {
        loose = this->jetSigBits->response(tauJet, 0);
        medium = this->jetSigBits->response(tauJet, 1);
        tight = this->jetSigBits->response(tauJet, 2);
        tauJet.setIsTau(xAOD::TauJetParameters::JetBDTSigLoose, loose);
        tauJet.setIsTau(xAOD::TauJetParameters::JetBDTSigMedium, medium);
        tauJet.setIsTau(xAOD::TauJetParameters::JetBDTSigTight, tight);
        if (msgLvl(MSG::DEBUG))
        {
            if (!((!loose && !medium && !tight) || (loose && !medium && !tight) || (loose && medium && !tight) || (loose && medium && tight)))
            {
	      ATH_MSG_VERBOSE("Bad bits!");
            }
            ATH_MSG_DEBUG("ET: " << tauJet.pt());
            ATH_MSG_DEBUG("jet sig loose: " << tauJet.isTau(xAOD::TauJetParameters::JetBDTSigLoose));
            ATH_MSG_DEBUG("jet sig medium: " << tauJet.isTau(xAOD::TauJetParameters::JetBDTSigMedium));
            ATH_MSG_DEBUG("jet sig tight: " << tauJet.isTau(xAOD::TauJetParameters::JetBDTSigTight));
        }
    }

    if (this->jetSigTrans)
    {
        float jetSigTransScore(this->jetSigTrans->response(tauJet));
	ATH_MSG_VERBOSE("Signal Transformed BDTJetScore: " << jetSigTransScore);
        tauJet.setDiscriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans, jetSigTransScore);
    }
    
    return StatusCode::SUCCESS;
}

StatusCode TauJetBDT::finalize()
{
    delete this->jetBDT;
    delete this->jetSigBits;
    delete this->jetSigTrans;
    return StatusCode::SUCCESS;
}
