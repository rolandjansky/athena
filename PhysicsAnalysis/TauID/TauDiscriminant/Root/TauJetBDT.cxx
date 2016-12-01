/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tool for BDT analysis.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#include "TauDiscriminant/TauJetBDT.h"

using namespace TauID;
using std::string;

TauJetBDT::TauJetBDT(const string& name):
  TauRecToolBase(name),
  m_jetScore(-1.),
  m_jetBDTFile(""),
  m_jetSigBitsFile(""),
  m_jetSigTransFile(""),
  m_jetBDT(nullptr),
  m_jetSigBits(nullptr),
  m_jetSigTrans(nullptr)
{
  declareProperty("jetBDT", this->m_jetBDTFile);
  declareProperty("jetSigBits",this->m_jetSigBitsFile);
  declareProperty("jetSigTrans",this->m_jetSigTransFile);
}


StatusCode TauJetBDT::initialize()
{
    if (this->m_jetBDTFile != "")
    {
        string jetBDTPath = find_file(this->m_jetBDTFile);
    
        if(jetBDTPath == "")
        {
	  ATH_MSG_FATAL("File: " << this->m_jetBDTFile << " not found! ");
	  return StatusCode::FAILURE;
        }
    
        this->m_jetBDT = new TauID::MethodBDT("TauBDT:JetBDT");
    
        if (!this->m_jetBDT->build(jetBDTPath))
        {
	  ATH_MSG_FATAL("Loading jet BDT file " << jetBDTPath << " failed!");
	  return StatusCode::FAILURE;
        }
        
        if (this->m_jetSigBitsFile != "")
        {
            string jetSigBitsPath = find_file(this->m_jetSigBitsFile);
            if(jetSigBitsPath == "")
            {
	      ATH_MSG_FATAL("File: " << this->m_jetSigBitsFile << " not found! ");
	      return StatusCode::FAILURE;
            }
            
            this->m_jetSigBits = new TauID::MethodCuts("TauBDT:JetSigBits");

            if (!this->m_jetSigBits->build(jetSigBitsPath))
            {
	      ATH_MSG_FATAL("Loading jet bits file " << jetSigBitsPath << " failed!");
	      return StatusCode::FAILURE;
            }
        }

        // Flat signal transformed jet score
        if (this->m_jetSigTransFile != "")
        {
            string jetSigTransPath = find_file(this->m_jetSigTransFile);
        
            if(jetSigTransPath == "")
            {
	      ATH_MSG_FATAL("File: " << this->m_jetSigTransFile << " not found! ");
	      return StatusCode::FAILURE;
            }
        
            this->m_jetSigTrans = new TauID::MethodTransform("TauBDT:JetBDT:SignalTranform");
        
            if (!this->m_jetSigTrans->build(jetSigTransPath))
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
    if (this->m_jetBDT)
    {


        this->m_jetScore = this->m_jetBDT->response(tauJet);
	ATH_MSG_VERBOSE("BDTJetScore: " << this->m_jetScore);
        if (this->m_jetScore < 0. || this->m_jetScore > 1.)
        {
	  ATH_MSG_ERROR("Error in computing BDTJetScore!");
        }
        tauJet.setDiscriminant(xAOD::TauJetParameters::BDTJetScore, this->m_jetScore);
    }
    else
    {
        tauJet.setDiscriminant(xAOD::TauJetParameters::BDTJetScore, 0.);
    }

    if (this->m_jetBDT && this->m_jetSigBits)
    {
        loose = this->m_jetSigBits->response(tauJet, 0);
        medium = this->m_jetSigBits->response(tauJet, 1);
        tight = this->m_jetSigBits->response(tauJet, 2);
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

    if (this->m_jetSigTrans)
    {
        float jetSigTransScore(this->m_jetSigTrans->response(tauJet));
	ATH_MSG_VERBOSE("Signal Transformed BDTJetScore: " << jetSigTransScore);
        tauJet.setDiscriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans, jetSigTransScore);
    }
    
    return StatusCode::SUCCESS;
}

StatusCode TauJetBDT::finalize()
{
    delete this->m_jetBDT;
    delete this->m_jetSigBits;
    delete this->m_jetSigTrans;
    return StatusCode::SUCCESS;
}
