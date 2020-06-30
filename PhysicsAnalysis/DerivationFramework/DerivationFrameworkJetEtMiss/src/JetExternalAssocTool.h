/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetExternalAssocTool.h

#ifndef DerivationFramework_JetExternalAssocTool_H
#define DerivationFramework_JetExternalAssocTool_H

/// Qi Zeng
/// Nov 2016
///
/// Tool to build link to third party objects through the intermediate object
/// e.g. b already has link to c
///      this tool build a link from a to c by matching b to a, assuming thereis a one-to-one correspondence between a and b

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"

#include "xAODJet/JetContainer.h"

#include "TObjArray.h"
#include "TObjString.h"
#include <vector>
#include <string>

namespace DerivationFramework{

  class JetExternalAssocTool : public AthAlgTool, public IAugmentationTool {

  public:
    JetExternalAssocTool(const std::string& t, const std::string& n, const IInterface* p);

    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

    bool TransferLink(const xAOD::Jet& jet, const xAOD::Jet& jet_external) const;

  private:
    /// Properties.
    std::string              m_momentPrefix;
    std::string              m_containerName;

    std::string              m_ExternalJetCollectionName;
    std::vector<std::string> m_VectorOfOldLinkNames;
    std::vector<std::string> m_VectorOfNewLinkNames;

    bool                     m_dRMatch;
    double                   m_dRCut;

    /// decoration pointers
    typedef ElementLink<xAOD::IParticleContainer>            type_el;
    typedef std::vector<type_el>                             type_ghostlink;

    SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_dec_keys{this, "DecKeys", {}, "SG keys for external decorations"};

  };

}
#endif
