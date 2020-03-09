/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaMatchingToolTest.h 

#ifndef TrigEgammaMatchingToolTest_H
#define TrigEgammaMatchingToolTest_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
class IExecuteTool;

namespace Trig{
    class TrigEgammaMatchingToolTest : public AthAlgorithm { 

        public: 

            /// Constructor with parameters: 
            TrigEgammaMatchingToolTest(const std::string& name, ISvcLocator* pSvcLocator);

            /// Destructor: 
            ~TrigEgammaMatchingToolTest(); 

            /// Athena algorithm's Hooks
            StatusCode  initialize();
            StatusCode  execute();
            StatusCode  finalize();



        private:
            
            std::vector<std::string> m_triggerList;
            ToolHandle<Trig::TrigDecisionTool> m_trigdec;
            ToolHandle<TrigEgammaMatchingTool> m_matchTool;
            StoreGateSvc * m_storeGate;



            TrigEgammaMatchingToolTest();
            
            Trig::TrigEgammaMatchingTool *match(){return &*m_matchTool;}
            
            void inspect(const std::string, const xAOD::Egamma *);
    }; 
}
#endif
