/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <random>

#include "RandomErrorAlg.h"

RandomErrorAlg::RandomErrorAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode RandomErrorAlg::execute(const EventContext& /*eventContext*/) const {

    static thread_local std::random_device rd;
    static thread_local std::default_random_engine generator (rd());
    std::uniform_real_distribution<double> distribution (0., 1.);

    if ( distribution(generator) < m_errorProbability ) {
        ATH_MSG_ERROR("Returning random StatusCode::FAILURE");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}
