/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* **************************************************************************
                           ParticleToJetAssociator.h  -  Description
                             -------------------
    begin   : 10.09.04
    authors : Kyle Cranmer
    email   : kyle.cranmer@cern.ch

    comments: A. Wildauer: 27.01.2005 rewritten into an AlgTool

   ***************************************************************************/

#include "ParticleJetTools/ParticleToJetAssociator.h"

namespace Analysis
{

ParticleToJetAssociator::ParticleToJetAssociator(const std::string& name)
        : AsgTool(name) {
//          m_analysisTools("AnalysisTools") {
#ifndef ROOTCORE
    declareInterface<ParticleToJetAssociator>(this);
#endif

//    declareProperty("analysisTools", m_analysisTools);
    declareProperty("trackCone",  m_trackCone = 0.4);
    declareProperty("shareTracks", m_shareTracks = false);
    declareProperty("useVariableSizedTrackCone", m_useVariableSizedTrackCone = true);
    declareProperty("coneSizeFitPar1", m_coneSizeFitPar1 = +0.239);
    declareProperty("coneSizeFitPar2", m_coneSizeFitPar2 = -1.220);
    declareProperty("coneSizeFitPar3", m_coneSizeFitPar3 = -1.64e-5);
}

ParticleToJetAssociator::~ParticleToJetAssociator() {}

StatusCode ParticleToJetAssociator::initialize() {
   //if ( m_analysisTools.retrieve().isFailure() ) {
   //  ATH_MSG_FATAL("Failed to retrieve tool " << m_analysisTools);
   //  return StatusCode::FAILURE;
   //} else {
   //  ATH_MSG_DEBUG("Retrieved tool " << m_analysisTools);
   //}
   return StatusCode::SUCCESS;
}

StatusCode ParticleToJetAssociator::finalize() {
   return StatusCode::SUCCESS;
}

}

