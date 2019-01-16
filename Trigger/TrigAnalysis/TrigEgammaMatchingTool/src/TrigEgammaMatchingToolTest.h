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
//#include "TriggerMatchingTool/IMatchingTool.h"
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

            /// Default constructor: 
            TrigEgammaMatchingToolTest();

        private:
            ToolHandle<Trig::TrigDecisionTool> m_trigdec;
            ToolHandle<ITrigEgammaMatchingTool> m_matchTool;
            //ToolHandle<IMatchingTool> m_matchTool2;


            std::vector<std::string> m_triggerList;
            std::vector<std::string> m_cmbTriggerList;
            std::map<std::string,int> m_counterBits;
            std::map<std::string,int> m_counterCmbMatchBits;
            std::map<std::string,int> m_counterMatch1Bits;
            std::map<std::string,int> m_counterMatch2Bits;
            std::map<std::string,int> m_counterMatch3Bits;
            std::map<std::string,int> m_counterMatch4Bits;
            StoreGateSvc * m_storeGate;

            void match(const std::string, const xAOD::Egamma *);
    }; 
}
#endif
