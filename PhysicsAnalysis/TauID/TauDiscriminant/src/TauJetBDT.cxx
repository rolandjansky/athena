/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tool for BDT analysis.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#include "TauDiscriminant/TauJetBDT.h"

using namespace xAOD;

StatusCode TauJetBDT::prepare(const TauDetailsManager& manager)
{
    if (this->jetBDTFile != "")
    {
        string jetBDTPath = PathResolver::find_file(this->jetBDTFile, "DATAPATH");
    
        if(jetBDTPath == "")
        {
            msg(MSG::FATAL) << "File: " << this->jetBDTFile << " not found! " << endreq;
            return StatusCode::FAILURE;
        }
    
        this->jetBDT = new MethodBDT("TauBDT:JetBDT");
        this->jetBDT->setDetails(manager);
    
        if (!this->jetBDT->build(jetBDTPath))
        {
            msg(MSG::FATAL) << "Loading jet BDT file " << jetBDTPath << " failed!" << endreq;
            return StatusCode::FAILURE;
        }
        
        if (this->jetSigBitsFile != "")
        {
            string jetSigBitsPath = PathResolver::find_file(this->jetSigBitsFile, "DATAPATH");
            if(jetSigBitsPath == "")
            {
                msg(MSG::FATAL) << "File: " << this->jetSigBitsFile << " not found! " << endreq;
                return StatusCode::FAILURE;
            }
            
            this->jetSigBits = new MethodCuts("TauBDT:JetSigBits");
            this->jetSigBits->setDetails(manager);
            this->jetSigBits->addVariable("BDT",&(this->jetScore),'F');

            if (!this->jetSigBits->build(jetSigBitsPath))
            {
                msg(MSG::FATAL) << "Loading jet bits file " << jetSigBitsPath << " failed!" << endreq;
                return StatusCode::FAILURE;
            }
        }

        // Flat signal transformed jet score
        if (this->jetSigTransFile != "")
        {
            string jetSigTransPath = PathResolver::find_file(this->jetSigTransFile, "DATAPATH");
        
            if(jetSigTransPath == "")
            {
                msg(MSG::FATAL) << "File: " << this->jetSigTransFile << " not found! " << endreq;
                return StatusCode::FAILURE;
            }
        
            this->jetSigTrans = new MethodTransform("TauBDT:JetBDT:SignalTranform");
            this->jetSigTrans->setDetails(manager);
            this->jetSigTrans->addVariable("BDT",&(this->jetScore),'F');
        
            if (!this->jetSigTrans->build(jetSigTransPath))
            {
                msg(MSG::FATAL) << "Loading jet BDT signal transformation file " << jetSigTransPath << " failed!" << endreq;
                return StatusCode::FAILURE;
            }
        }

        // Flat background transformed jet score
        if (this->jetBkgTransFile != "")
        {
            string jetBkgTransPath = PathResolver::find_file(this->jetBkgTransFile, "DATAPATH");
        
            if(jetBkgTransPath == "")
            {
                msg(MSG::FATAL) << "File: " << this->jetBkgTransFile << " not found! " << endreq;
                return StatusCode::FAILURE;
            }
        
            this->jetBkgTrans = new MethodTransform("TauBDT:JetBDT:BackgroundTranform");
            this->jetBkgTrans->setDetails(manager);
            this->jetBkgTrans->addVariable("BDT",&(this->jetScore),'F');
                    
            if (!this->jetBkgTrans->build(jetBkgTransPath))
            {
                msg(MSG::FATAL) << "Loading jet BDT background transformation file " << jetBkgTransPath << " failed!" << endreq;
                return StatusCode::FAILURE;
            }
        }
        if (this->jetBkgBitsFile != "")
        {
            string jetBkgBitsPath = PathResolver::find_file(this->jetBkgBitsFile, "DATAPATH");
            if(jetBkgBitsPath == "")
            {
                msg(MSG::FATAL) << "File: " << this->jetBkgBitsFile << " not found! " << endreq;
                return StatusCode::FAILURE;
            }
            
            this->jetBkgBits = new MethodCuts("TauBDT:JetBkgBits");
            this->jetBkgBits->setDetails(manager);
            this->jetBkgBits->addVariable("BDT",&(this->jetScore),'F');
                         
            if (!this->jetBkgBits->build(jetBkgBitsPath))
            {
                msg(MSG::FATAL) << "Loading jet bits file " << jetBkgBitsPath << " failed!" << endreq;
                return StatusCode::FAILURE;
            }
        }
    }
    else
    {
        msg(MSG::FATAL) << "No BDTs were initialized!" << endreq;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode TauJetBDT::execute(xAOD::TauJet *tauJet, FakeTauBits* /*bits*/, FakeTauScores* /*scores*/)
{
   
//     Analysis::TauPID* tauJetID = tauJet->tauID();
    bool loose, medium, tight;

    // Initialize scores
    tauJet->setDiscriminant(TauJetParameters::BDTJetScore, static_cast<float>(0.));
    tauJet->setDiscriminant(TauJetParameters::BDTJetScoreSigTrans, static_cast<float>(0.));
    tauJet->setDiscriminant(TauJetParameters::BDTJetScoreBkgTrans, static_cast<float>(0.));
    
    // Initialize bits
    tauJet->setIsTau(TauJetParameters::JetBDTSigLoose, false);
    tauJet->setIsTau(TauJetParameters::JetBDTSigMedium, false);
    tauJet->setIsTau(TauJetParameters::JetBDTSigTight, false);
    
    tauJet->setIsTau(TauJetParameters::JetBDTBkgLoose, false);
    tauJet->setIsTau(TauJetParameters::JetBDTBkgMedium, false);
    tauJet->setIsTau(TauJetParameters::JetBDTBkgTight, false);

    // do not assign a meaningful score for tau1P3P-only candidates. return.
    

    // Set the response of the jet BDT
    if (this->jetBDT)
    {


        this->jetScore = this->jetBDT->response();
        if (msgLvl(MSG::VERBOSE))
        {
            msg(MSG::VERBOSE) << "BDTJetScore: " << this->jetScore << endreq;
        }
        if (this->jetScore < 0. || this->jetScore > 1.)
        {
            msg(MSG::ERROR) << "Error in computing BDTJetScore!" << endreq;
        }
        tauJet->setDiscriminant(TauJetParameters::BDTJetScore, this->jetScore);
    }
    else
    {
        tauJet->setDiscriminant(TauJetParameters::BDTJetScore, 0.);
    }

    if (this->jetBDT && this->jetSigBits)
    {
        loose = this->jetSigBits->response(0);
        medium = this->jetSigBits->response(1);
        tight = this->jetSigBits->response(2);
        tauJet->setIsTau(TauJetParameters::JetBDTSigLoose, loose);
        tauJet->setIsTau(TauJetParameters::JetBDTSigMedium, medium);
        tauJet->setIsTau(TauJetParameters::JetBDTSigTight, tight);
        if (msgLvl(MSG::DEBUG))
        {
            if (!((!loose && !medium && !tight) || (loose && !medium && !tight) || (loose && medium && !tight) || (loose && medium && tight)))
            {
                msg(MSG::VERBOSE) << "Bad bits!" << endreq;
            }
            msg(MSG::DEBUG) << "ET: " << tauJet->pt() << endreq;
            msg(MSG::DEBUG) << "jet sig loose: " << tauJet->isTau(TauJetParameters::JetBDTSigLoose) << endreq;
            msg(MSG::DEBUG) << "jet sig medium: " << tauJet->isTau(TauJetParameters::JetBDTSigMedium) << endreq;
            msg(MSG::DEBUG) << "jet sig tight: " << tauJet->isTau(TauJetParameters::JetBDTSigTight) << endreq;
        }
    }

    if (this->jetSigTrans)
    {
        float jetSigTransScore(this->jetSigTrans->response());
        if (msgLvl(MSG::VERBOSE))
        {
            msg(MSG::VERBOSE) << "Signal Transformed BDTJetScore: " << jetSigTransScore << endreq;
        }
        tauJet->setDiscriminant(TauJetParameters::BDTJetScoreSigTrans, jetSigTransScore);
    }
    
    if (this->jetBkgTrans)
    {
        float jetBkgTransScore(this->jetBkgTrans->response());
        if (msgLvl(MSG::VERBOSE))
        {
            msg(MSG::VERBOSE) << "Background Transformed BDTJetScore: " << jetBkgTransScore << endreq;
        }
        tauJet->setDiscriminant(TauJetParameters::BDTJetScoreBkgTrans, jetBkgTransScore);
    }
    
    if (this->jetBDT && this->jetBkgBits)
    {
        loose = this->jetBkgBits->response(0);
        medium = this->jetBkgBits->response(1);
        tight = this->jetBkgBits->response(2);
        tauJet->setIsTau(TauJetParameters::JetBDTBkgLoose, loose);
        tauJet->setIsTau(TauJetParameters::JetBDTBkgMedium, medium);
        tauJet->setIsTau(TauJetParameters::JetBDTBkgTight, tight);
        if (msgLvl(MSG::VERBOSE))
        {
            if (!((!loose && !medium && !tight) || (loose && !medium && !tight) || (loose && medium && !tight) || (loose && medium && tight)))
            {
                msg(MSG::VERBOSE) << "Bad bits!" << endreq;
            }
            msg(MSG::VERBOSE) << "ET: " << tauJet->pt() << endreq;
            msg(MSG::VERBOSE) << "jet bkg loose: " << tauJet->isTau(TauJetParameters::JetBDTBkgLoose) << endreq;
            msg(MSG::VERBOSE) << "jet bkg medium: " << tauJet->isTau(TauJetParameters::JetBDTBkgMedium) << endreq;
            msg(MSG::VERBOSE) << "jet bkg tight: " << tauJet->isTau(TauJetParameters::JetBDTBkgTight) << endreq;
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode TauJetBDT::finalize()
{
    delete this->jetBDT;
    delete this->jetSigBits;
    delete this->jetBkgBits;
    delete this->jetSigTrans;
    delete this->jetBkgTrans;
    return StatusCode::SUCCESS;
}
