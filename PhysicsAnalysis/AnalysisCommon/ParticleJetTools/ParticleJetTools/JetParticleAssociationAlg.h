/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETPARTICLEASSOCIATIONALG_H
#define PARTICLEJETTOOLS_JETPARTICLEASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandleArray.h"
#include "ParticleJetTools/JetParticleAssociation.h"

// this Algorithm simply calls the execute() method of the appropriate
// JetParticleAssociation during its own execute().
// initialize() and finalize() do nothing.
class JetParticleAssociationAlg : public AthAlgorithm {
    private:
        ToolHandleArray<JetParticleAssociation> m_tools;

    public:
        JetParticleAssociationAlg(
                  std::string name
                , ISvcLocator* pSvcLocator)
        : AthAlgorithm(name, pSvcLocator) {

            declareProperty("Associations", m_tools);
        };

        StatusCode initialize() {
            if ( m_tools.retrieve().isFailure() ) {
                ATH_MSG_FATAL("JetParticleAssociation tool retrieval failed.");
                return StatusCode::FAILURE;
            }

            for (const ToolHandle<JetParticleAssociation>& jpa : m_tools) {
                if ( jpa->initialize().isFailure() ) {
                    ATH_MSG_FATAL(jpa->name() + " tool initialization failed.");
                    return StatusCode::FAILURE;
                }
            }

            return StatusCode::SUCCESS;
        }

        StatusCode execute() {
            for (const ToolHandle<JetParticleAssociation>& jpa : m_tools) {
                if ( jpa->execute().isFailure() ) {
                    ATH_MSG_FATAL(jpa->name() + " tool execution failed.");
                    return StatusCode::FAILURE;
                }
            }

            return StatusCode::SUCCESS;
        }

        StatusCode finalize() {
            for (const ToolHandle<JetParticleAssociation>& jpa : m_tools) {
                if ( jpa->finalize().isFailure() ) {
                    ATH_MSG_FATAL(jpa->name() + " tool finalization failed.");
                    return StatusCode::FAILURE;
                }
            }

            return StatusCode::SUCCESS;
        }

};

#endif
