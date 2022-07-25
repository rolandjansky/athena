/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Jet_Manager_H
#define Jet_Manager_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "src/TrigBtagEmulationJet.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingUtilities.h"

#include "TrigBtagEmulationTool/EmulContext.h"

#include "TLorentzVector.h"
#include <string>
#include <vector>

namespace Trig {

  class JetManagerTool : 
    public AthAlgTool {
  public:
    JetManagerTool(const std::string& type, 
		   const std::string& name, 
		   const IInterface* parent);
    virtual ~JetManagerTool() = default;
    
    virtual StatusCode initialize() override;
        
    StatusCode retrieveByContainer(const EventContext& ctx,
				   EmulContext& emulCtx) const;
    
    const std::string& jetContainerName() const;    
    const std::string& bjetContainerName() const;
        
    const std::vector<TrigBtagEmulationJet>& getJets(const EmulContext& emulCtx) const;
    const std::vector<TrigBtagEmulationJet>& getSortedPreselJets(const EmulContext& emulCtx) const;
    
  private:
    // These are set in the initialize method given JetContainerName
    SG::ReadHandleKey< xAOD::JetContainer > m_jetInputKey {this, "InputJets", "", "Input Jet Collection Key, retrieved from reconstructed jets"};
    SG::ReadHandleKey< xAOD::JetContainer > m_bjetInputKey {this, "InputBJets", "", "Input b-Jet Collection Key, retrieved from reconstructed jets"};

    Gaudi::Property<std::string> m_btagging_link {this, "BTaggingLink", "btaggingLink"};    
    Gaudi::Property<std::string> m_jetcontainer {this, "JetContainerName", "", "Jet Container"};
  };

  inline const std::string& JetManagerTool::jetContainerName() const { return m_jetInputKey.key(); }
  inline const std::string& JetManagerTool::bjetContainerName() const { return m_bjetInputKey.key(); }
} // namespace

#endif
