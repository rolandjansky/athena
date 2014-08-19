/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirl/MuGirl.h"


MuGirl::MuGirl(const std::string& name, ISvcLocator* pSvcLocator) :
        AthAlgorithm(name, pSvcLocator),
        m_pMuGirlReconstruction("MuGirlNS::MuGirlRecoTool/MuGirlRecoTool",0)
{
    // configure property values
    // Tools
    declareProperty("MuGirlReconstruction", m_pMuGirlReconstruction);
}

MuGirl::~MuGirl()
{
}

StatusCode MuGirl::initialize()
{
    msg(MSG::INFO) << "Initializing MuGirl Algorithm - package version " << PACKAGE_VERSION << endreq;

    if (m_pMuGirlReconstruction.retrieve().isFailure())
    {
        msg(MSG::ERROR) <<"Could not retrieve MuGirlRecoTool"<<endreq;
        return StatusCode::FAILURE;
    }

    msg(MSG::INFO) << "Algorithm MuGirl initialized successfully" << endreq;
    
    return StatusCode::SUCCESS;
}

StatusCode MuGirl::finalize()
{
    msg(MSG::INFO) << "Finalized" << endreq;

    return StatusCode::SUCCESS;
}

StatusCode MuGirl::execute()
{
    if (msgLvl(MSG::DEBUG)) msg() << "Executing" << endreq;

    if (m_pMuGirlReconstruction->execute().isFailure()) {
        ATH_MSG_ERROR("MuGirlrecoTool could not perform reconstruction");
        return StatusCode::FAILURE;
    }

    if (msgLvl(MSG::DEBUG)) msg() << "MuGirl::execute done" << endreq;

    return StatusCode::SUCCESS;
}
