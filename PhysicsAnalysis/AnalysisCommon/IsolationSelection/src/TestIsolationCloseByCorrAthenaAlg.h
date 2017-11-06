// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_TESTISOLATIONCLOSEBYCORRATHENAALG_H
#define ISOLATIONSELECTION_TESTISOLATIONCLOSEBYCORRATHENAALG_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/ToolHandle.h"

// Local include(s):
#include <xAODCore/ShallowCopy.h>
#include <xAODBase/ObjectType.h>
#include <xAODBase/IParticleHelpers.h>
#include "xAODEventInfo/EventInfo.h"

#include <GaudiKernel/ITHistSvc.h>

#include <IsolationSelection/TestMacroHelpers.h>


#include <memory>


class ITHistSvc;

namespace CP {
    class IIsolationCloseByCorrectionTool;
    class IIsolationSelectionTool;

    class TestIsolationCloseByCorrAthenaAlg: public AthAlgorithm {

        public:
            TestIsolationCloseByCorrAthenaAlg(const std::string& name, ISvcLocator* svcLoc);
            virtual ~TestIsolationCloseByCorrAthenaAlg() {
            }
            virtual StatusCode initialize();
            virtual StatusCode execute();

        private:
            template<class T> StatusCode CreateContainerLinks(const std::string& ContainerName, T*& Container) {
                const T* xAODContainer;
                ATH_CHECK(evtStore()->retrieve(xAODContainer, ContainerName));
                std::pair<T*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*xAODContainer);
                Container = shallowcopy.first;
                if (!xAOD::setOriginalObjectLink(*xAODContainer, *Container)) {
                    ATH_MSG_ERROR("Failed to set original object links on " << ContainerName);
                    return StatusCode::FAILURE;
                }
                ATH_CHECK(evtStore()->record(shallowcopy.first, "Shallow" + ContainerName));
                ATH_CHECK(evtStore()->record(shallowcopy.second, "Shallow" + ContainerName + "Aux."));
                return StatusCode::SUCCESS;

            }

            ServiceHandle<ITHistSvc> m_histSvc;

            ToolHandle<CP::IIsolationCloseByCorrectionTool> m_isoCloseByCorrTool;
            ToolHandle<CP::IIsolationSelectionTool> m_isoSelectorTool;

            TTree* m_tree;
            std::unique_ptr<IsoCorrectionTestHelper> m_ele_helper;
            std::unique_ptr<IsoCorrectionTestHelper> m_muo_helper;
            std::unique_ptr<IsoCorrectionTestHelper> m_pho_helper;

    };

}
#endif
