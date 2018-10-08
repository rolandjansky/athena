/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETASSOCCONSTALG_H
#define PARTICLEJETTOOLS_JETASSOCCONSTALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandleArray.h"
#include "ParticleJetTools/JetParticleAssociation.h"
#include <string>
#include <vector>

// this Algorithm simply calls the modify() method of a list of
// JetParticleAssociations during its own execute().
// initialize() and finalize() do nothing.
class JetAssocConstAlg : public AthAlgorithm {
  private:
    ToolHandleArray<JetParticleAssociation> m_assocs;
    std::vector<std::string> m_jetcollections;

  public:
    JetAssocConstAlg(std::string name , ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
};

#endif
