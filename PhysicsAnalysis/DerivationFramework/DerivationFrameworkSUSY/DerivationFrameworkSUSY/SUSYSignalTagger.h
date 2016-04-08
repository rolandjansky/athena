/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DerivationFrameworkSUSY/SUSYSignalTagger.h
 * @author Martin Tripiana
 * @date May. 2015
 * @brief tool to decorate EventInfo with the SUSY signal process information
*/


#ifndef DerivationFramework_SUSYSignalTagger_H
#define DerivationFramework_SUSYSignalTagger_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODTruth/TruthParticleContainer.h"

namespace DerivationFramework {

  class SUSYSignalTagger : public AthAlgTool, public IAugmentationTool {

  public:
    SUSYSignalTagger(const std::string& t, const std::string& n, const IInterface* p);
    ~SUSYSignalTagger();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;


  private:

    std::string m_eventInfoName;
    std::string m_mcName;    

    bool FindSusyHardProc(const xAOD::TruthParticleContainer* truthP, int& pdgid1, int& pdgid2) const;
    unsigned int finalStateID(const int SUSY_Spart1_pdgId, const int SUSY_Spart2_pdgId) const;

  }; /// class

} /// namespace


#endif
