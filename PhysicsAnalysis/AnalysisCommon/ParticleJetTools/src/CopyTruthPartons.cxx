/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthPartons.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODTruth/TruthParticleContainer.h"
//#include "GeneratorUtils/PIDUtils.h"
#include "TruthUtils/PIDUtils.h"

using namespace std;


CopyTruthPartons::CopyTruthPartons(const std::string& name, ISvcLocator* pSvcLocator)
  : CopyTruthParticles(name, pSvcLocator) { }

bool CopyTruthPartons::classify(const xAOD::TruthParticle* tp) {
    return MC::PID::isParton(tp->pdgId());
}
