/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tool for pi0 BDT analysis.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#include "TauDiscriminant/TauPi0BDT.h"

StatusCode TauPi0BDT::prepare(const TauDetailsManager& manager)
{
    // Primary pi0 BDT preparation
    if (this->pi0BDTPrimaryFile != "")
    {
        string pi0BDTPrimaryPath = PathResolver::find_file(this->pi0BDTPrimaryFile, "DATAPATH");
        if(pi0BDTPrimaryPath == "")
        {
            msg(MSG::FATAL) << "File: " << this->pi0BDTPrimaryFile << " not found! " << endreq;
            return StatusCode::FAILURE;
        }
         
        this->pi0BDTPrimary = new MethodBDT("TauBDT:Pi0BDTPrimary");
        this->pi0BDTPrimary->addVariable("EMPOverTrkSysP", manager.getFloatDetailAddress(Details::EMPOVERTRKSYSP),'F');
        this->pi0BDTPrimary->addVariable("ChPiEMEOverCaloEME", manager.getFloatDetailAddress(Details::CHPIEMEOVERCALOEME),'F');
        this->pi0BDTPrimary->addVariable("PSSFraction", manager.getFloatDetailAddress(Details::PSSFRACTION),'F');
        this->pi0BDTPrimary->addVariable("EtOverPtLeadTrk", manager.getFloatDetailAddress(Details::ETOVERPTLEADTRK),'F');
        //this->pi0BDTPrimary->addVariable("mEflow", manager.getFloatDetailAddress(Details::MEFLOW),'F');
        this->pi0BDTPrimary->addVariable("nStrip", manager.getIntDetailAddress(Details::NSTRIP),'I');

        
        if (!this->pi0BDTPrimary->build(pi0BDTPrimaryPath))
        {
            msg(MSG::FATAL) << "Loading primary pi0 BDT file " << pi0BDTPrimaryPath << " failed!" << endreq;
            return StatusCode::FAILURE;
        }
    }

    // Secondary pi0 BDT preparation
    if (this->pi0BDTSecondaryFile != "")
    {
        string pi0BDTSecondaryPath = PathResolver::find_file(this->pi0BDTSecondaryFile, "DATAPATH");
        if(pi0BDTSecondaryPath == "")
        {
            msg(MSG::FATAL) << "File: " << this->pi0BDTSecondaryFile << " not found! " << endreq;
            return StatusCode::FAILURE;
        }
         
        this->pi0BDTSecondary = new MethodBDT("TauBDT:Pi0BDTSecondary");
        this->pi0BDTSecondary->addVariable("EMPOverTrkSysP", manager.getFloatDetailAddress(Details::EMPOVERTRKSYSP),'F');
        this->pi0BDTSecondary->addVariable("ChPiEMEOverCaloEME", manager.getFloatDetailAddress(Details::CHPIEMEOVERCALOEME),'F');
        this->pi0BDTSecondary->addVariable("PSSFraction", manager.getFloatDetailAddress(Details::PSSFRACTION),'F');
        this->pi0BDTSecondary->addVariable("EtOverPtLeadTrk", manager.getFloatDetailAddress(Details::ETOVERPTLEADTRK),'F');
        //this->pi0BDTSecondary->addVariable("mEflow", manager.getFloatDetailAddress(Details::MEFLOW),'F');
        this->pi0BDTSecondary->addVariable("nStrip", manager.getIntDetailAddress(Details::NSTRIP),'I');
        
        if (!this->pi0BDTSecondary->build(pi0BDTSecondaryPath))
        {
            msg(MSG::FATAL) << "Loading secondary pi0 BDT file " << pi0BDTSecondaryPath << " failed!" << endreq;
            return StatusCode::FAILURE;
        }
    }
    
    if (!this->pi0BDTPrimary && !this->pi0BDTSecondary)
    {
        msg(MSG::FATAL) << "No BDTs were initialized!" << endreq;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode TauPi0BDT::execute(xAOD::TauJet* /*tauJet*/, FakeTauBits* /*bits*/, FakeTauScores* scores)
{
    // Get primary pi0 BDT score
    if (this->pi0BDTPrimary && scores)
    {
        float pi0BDTPrimaryScore(this->pi0BDTPrimary->response());
        if (msgLvl(MSG::VERBOSE))
        {
            msg(MSG::VERBOSE) << "Primary Pi0 BDT score: " << pi0BDTPrimaryScore << endreq;
        }
        scores->setScore(TauScore::BDT_PI0_PRIMARY, pi0BDTPrimaryScore);
    }

    // Get secondary pi0 BDT score
    if (this->pi0BDTSecondary && scores)
    {
        float pi0BDTSecondaryScore(this->pi0BDTSecondary->response());
        if (msgLvl(MSG::VERBOSE))
        {
            msg(MSG::VERBOSE) << "Secondary Pi0 BDT score: " << pi0BDTSecondaryScore << endreq;
        }
        scores->setScore(TauScore::BDT_PI0_SECONDARY, pi0BDTSecondaryScore);
    }

    return StatusCode::SUCCESS;
}

StatusCode TauPi0BDT::finalize()
{
    delete this->pi0BDTPrimary;
    delete this->pi0BDTSecondary;
    return StatusCode::SUCCESS;
}
