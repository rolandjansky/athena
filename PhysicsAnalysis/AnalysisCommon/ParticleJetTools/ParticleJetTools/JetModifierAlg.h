/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETMODIFIERALG_H
#define PARTICLEJETTOOLS_JETMODIFIERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandleArray.h"
#include "JetInterface/IJetModifier.h"
#include "xAODJet/JetContainer.h"
#include <string>
#include <vector>

// this Algorithm simply calls the modify() method of a list of
// IJetModifiers during its own execute().
// initialize() and finalize() do nothing.
class JetModifierAlg : public AthAlgorithm {
    private:
        ToolHandleArray<IJetModifier> m_modifiers;
        std::vector<std::string> m_jetcollections;

    public:
        JetModifierAlg(
                std::string name
                , ISvcLocator* pSvcLocator)
            : AthAlgorithm(name, pSvcLocator) {

                declareProperty("Modifiers", m_modifiers);
                declareProperty("JetContainerNames", m_jetcollections);
            };

        StatusCode initialize() {
            if ( m_modifiers.retrieve().isFailure() ) {
                ATH_MSG_FATAL("jet modifier retrieval failed.");
                return StatusCode::FAILURE;
            }

            for (const ToolHandle<IJetModifier>& jmh : m_modifiers) {
                if ( jmh->initialize().isFailure() ) {
                    ATH_MSG_FATAL(jmh->name() + " tool initialization failed.");
                    return StatusCode::FAILURE;
                }
            }

            return StatusCode::SUCCESS;
        }

        StatusCode execute() {
            for (const std::string& jetContainerName : m_jetcollections) {
                xAOD::JetContainer* jets = NULL;
                if ( evtStore()->retrieve( jets, jetContainerName ).isFailure() ) {
                    ATH_MSG_FATAL(name() + 
                            "failed to retrieve jet collection \"" +
                            jetContainerName + "\"");
                    return StatusCode::FAILURE;
                } else {
                    ATH_MSG_INFO(name() + 
                            "retrieved jet collection \"" +
                            jetContainerName + "\"");
                }


                for (const ToolHandle<IJetModifier>& jmh : m_modifiers) {
                    if ( jmh->modify(*jets) ) {
                        ATH_MSG_FATAL(jmh->name() + " tool execution failed.");
                        return StatusCode::FAILURE;
                    } else {
                        ATH_MSG_INFO(jmh->name() + " tool execution success.");
                    }
                }
            }

            return StatusCode::SUCCESS;
        }

        StatusCode finalize() {
            for (const ToolHandle<IJetModifier>& jmh : m_modifiers) {
                if ( jmh->finalize().isFailure() ) {
                    ATH_MSG_FATAL(jmh->name() + " tool finalization failed.");
                    return StatusCode::FAILURE;
                }
            }

            return StatusCode::SUCCESS;
        }

};

#endif
