/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETASSOCCONSTALG_H
#define PARTICLEJETTOOLS_JETASSOCCONSTALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODCore/ShallowCopy.h"
#include "AsgTools/ToolHandleArray.h"
#include "ParticleJetTools/JetParticleAssociation.h"
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticle.h"
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
    JetAssocConstAlg(
        std::string name
        , ISvcLocator* pSvcLocator)
      : AthAlgorithm(name, pSvcLocator) {

        declareProperty("Associators", m_assocs);
        declareProperty("JetCollections", m_jetcollections);
      };

    StatusCode initialize() {
      if ( m_assocs.retrieve().isFailure() ) {
        ATH_MSG_FATAL(name() + " jet-particle association tool retrieval failed.");
        return StatusCode::FAILURE;
      }

      for (const ToolHandle<JetParticleAssociation>& jpa : m_assocs) {
        if ( jpa->initialize().isFailure() ) {
          ATH_MSG_FATAL(jpa->name() + " tool initialization failed.");
          return StatusCode::FAILURE;
        }
      }

      return StatusCode::SUCCESS;
    }

    StatusCode execute() {
      for (const std::string& jetContainerName : m_jetcollections) {
        SG::ReadHandle<xAOD::JetContainer> jetsHandle(jetContainerName);

        if ( !jetsHandle.isValid()) {
          ATH_MSG_FATAL(name() + 
              " failed to retrieve jet collection \"" +
              jetContainerName + "\"");
          return StatusCode::FAILURE;
        } else {
          ATH_MSG_INFO(name() + 
              " retrieved jet collection \"" +
              jetContainerName + "\"");
        }

        const xAOD::JetContainer& jets = *jetsHandle;

        std::pair<xAOD::JetContainer*,xAOD::ShallowAuxContainer*> shallowcopy
          = xAOD::shallowCopyContainer(jets);

        // Make unique_ptrs so as to avoid needing to delete
        std::unique_ptr<xAOD::JetContainer> jets_copy(shallowcopy.first);
        std::unique_ptr<xAOD::ShallowAuxContainer> jets_copyaux(shallowcopy.second);


        for (const ToolHandle<JetParticleAssociation>& jpa : m_assocs) {
          SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > > dec(jpa->outputCollectionName());

          // modify the shallow copy of the jets
          if ( jpa->modify(*jets_copy) ) {
            ATH_MSG_FATAL(jpa->name() + " tool execution failed.");
            return StatusCode::FAILURE;
          } else {
            ATH_MSG_INFO(jpa->name() + " tool execution success.");
          }

          // directly copy over the decoration to the original jet
          // collections
          for (size_t iJet = 0; iJet < jets.size(); iJet++)
            dec(*jets.at(iJet)) = dec(*jets_copy->at(iJet));

        }
      }

      return StatusCode::SUCCESS;
    }


    StatusCode finalize() {
      for (const ToolHandle<JetParticleAssociation>& jpa : m_assocs) {
        if ( jpa->finalize().isFailure() ) {
          ATH_MSG_FATAL(jpa->name() + " tool finalization failed.");
          return StatusCode::FAILURE;
        }
      }

      return StatusCode::SUCCESS;
    }

};

#endif
