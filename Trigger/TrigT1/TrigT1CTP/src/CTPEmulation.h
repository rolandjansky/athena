/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CTP_CTPEMULATION_H
#define TRIGT1CTP_CTPEMULATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/DataHandle.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// monitoring from HLT
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

// data
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/EmTauCTP.h"
#include "TrigT1Interfaces/JetCTP.h"
#include "TrigT1Interfaces/EnergyCTP.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"

// internal
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/ItemMap.h"
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/ResultBuilder.h"

#include "TH1.h"

#include <map>
#include <string>

namespace HLT {
   class ILvl1ResultAccessTool;
}

namespace ROIB {
   class RoIBResult;
}

namespace LVL1 {
   class CPRoIDecoder;
   class JEPRoIDecoder;
}

namespace LVL1CTP {

   class CTPEmulation : public AthAlgorithm, public IMonitoredAlgo {

   public:

      CTPEmulation( const std::string& name, ISvcLocator* pSvcLocator );
      ~CTPEmulation();

      virtual StatusCode initialize();
      virtual StatusCode start();
      virtual StatusCode beginRun();
      virtual StatusCode execute();
      virtual StatusCode finalize();

      virtual StatusCode bookHists();

   private:
      struct eFEXParWP {
         struct WPEntry {
            int etamin {-49};
            int etamax {49};
            float value {0.0};
            unsigned int maxEt {0};
            int priority {1};
         };
         std::vector<WPEntry> entries;
         const WPEntry & getWP(int ieta) const;
      };

   private:

      // private member functions
      StatusCode createInternalTriggerMap();

      StatusCode printConfiguration() const;

      StatusCode retrieveCollections();

      StatusCode fillInputHistograms();

      StatusCode extractMultiplicities() const;

      bool isNewThreshold(const TrigConf::TriggerThreshold * thr) const;

      unsigned int extractMultiplicitiesFromCTPInputData( const TrigConf::TriggerThreshold * thr ) const;

      unsigned int calculateMultiplicity( const TrigConf::TriggerThreshold * confThr ) const;

      unsigned int calculateJetMultiplicity( const TrigConf::TriggerThreshold * confThr ) const;
      unsigned int calculateEMMultiplicity( const TrigConf::TriggerThreshold * confThr ) const;
      unsigned int calculateTauMultiplicity( const TrigConf::TriggerThreshold * confThr ) const;
      unsigned int calculateMETMultiplicity( const TrigConf::TriggerThreshold * confThr ) const;
      unsigned int calculateMuonMultiplicity( const TrigConf::TriggerThreshold * confThr ) const;
      unsigned int calculateTopoMultiplicity( const TrigConf::TriggerThreshold * confThr ) const;

      StatusCode simulateItems();

      StatusCode createMultiplicityHist(const std::string & type, TrigConf::L1DataDef::TriggerType tt, unsigned int maxMult = 10 );

      std::string histBasePath() const { 
         return "/" + m_histStream + "/CTPEmulation";
      }

      // fill eFEX selection cuts object
      StatusCode setEFexConfig(const std::string & prop, std::map<unsigned int, eFEXParWP> & cfgMap);

      const xAOD::EnergySumRoI *
      getMET(const std::string & thresholdType) const;

      // member variables

      // Needed services
      ServiceHandle<ITHistSvc> m_histSvc;
      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
      ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
      ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool; //!< tool to create L1 ROIs from ROIBResult

      // Inputs from the new L1Calo
      // gFEX
      const DataHandle< xAOD::EnergySumRoI >    m_gFEXMETNC;      //!< MET from gFEX
      const DataHandle< xAOD::EnergySumRoI >    m_gFEXMETPufit;   //!< MET from gFEX
      const DataHandle< xAOD::EnergySumRoI >    m_gFEXMETRho;     //!< MET from gFEX
      const DataHandle< xAOD::EnergySumRoI >    m_gFEXMETJwoJ;    //!< MET from gFEX
      const DataHandle< xAOD::JetRoIContainer > m_gJet;           //!< jets from gFEX 
      // eFEX
      const DataHandle< xAOD::TrigEMClusterContainer > m_eFEXCluster; //!< cluster from eFEX
      const DataHandle< xAOD::EmTauRoIContainer > m_jFEXCluster; //!< cluster from eFEX
      const DataHandle< xAOD::EmTauRoIContainer > m_eFEXTau; //!< taus from eFEX
      const DataHandle< xAOD::EmTauRoIContainer > m_jFEXTau; //!< taus from eFEX
      // jFEX
      const DataHandle< xAOD::EnergySumRoI >    m_jFEXMETNC;      //!< MET from jFEX
      const DataHandle< xAOD::EnergySumRoI >    m_jFEXMETRho;     //!< MET from jFEX
      const DataHandle< xAOD::JetRoIContainer > m_jJet;           //!< jets from jFEX 
      const DataHandle< xAOD::JetRoIContainer > m_jLJet;          //!< large jets from jFEX 
      // Inputs from old L1Calo and L1Muon
      // from MC
      const DataHandle< xAOD::MuonRoIContainer > m_muonRoIs;
      const DataHandle< xAOD::JetRoIContainer >  m_lgJetRoIs;
      const DataHandle< LVL1::MuCTPICTP >        m_muctpiCTP;        //!< MUCTPI input
      const DataHandle< LVL1::EmTauCTP >         m_emtauCTP;         //!< EmTau input
      const DataHandle< LVL1::JetCTP >           m_jetCTP;           //!< Jet input
      const DataHandle< LVL1::EnergyCTP >        m_energyCTP;        //!< Energy input
      const DataHandle< LVL1::FrontPanelCTP >    m_topoCTP;          //!< Topo input
      // from data
      const ROIB::RoIBResult * m_roibResult { nullptr };


      // properties
      BooleanProperty m_useCTPInput { false };
      BooleanProperty m_useROIBOutput { false };

      // name of input collections
      // new FEX collections

      // see https://twiki.cern.ch/twiki/bin/view/Atlas/L1CaloUpgradeSimulation#Mapping_of_container_names_to_me

      // MET
      StringProperty m_jFEX_MET_NCLoc  {"jNOISECUT_MET"};
      StringProperty m_jFEX_MET_RhoLoc {"jXERHO_MET"};

      StringProperty m_gFEX_MET_PufitLoc {"gXEPUFIT_MET"};
      StringProperty m_gFEX_MET_RhoLoc   {"gXERHO_MET"};
      StringProperty m_gFEX_MET_JwoJLoc  {"gXEJWOJ_MET"};
      StringProperty m_gFEX_MET_NCLoc    {"gXENOISECUT_MET"};

      // jets
      StringProperty m_gFEX_Jet_Loc    {"gL1Jets"};
      StringProperty m_jFEX_Jet_Loc    {"jRoundJetsPUsub"};
      StringProperty m_jFEX_Jet_LR_Loc {"jRoundLargeRJetsPUsub"};

      // electrons
      StringProperty m_eFEX_Cluster_Loc {"SClusterCl"};
      StringProperty m_jFEX_Cluster_Loc {"jEles"};

      // taus
      StringProperty m_eFEX_Tau_Loc {"SClusterTau"};
      StringProperty m_jFEX_Tau_Loc {"jTaus"};

      // name of the CTP input words
      StringProperty m_muonCTPLoc   { LVL1MUCTPI::DEFAULT_MuonCTPLocation};
      StringProperty m_emtauCTPLoc  { LVL1::TrigT1CaloDefs::EmTauCTPLocation };
      StringProperty m_jetCTPLoc    { LVL1::TrigT1CaloDefs::JetCTPLocation };
      StringProperty m_energyCTPLoc { LVL1::TrigT1CaloDefs::EnergyCTPLocation };
      StringProperty m_topoCTPLoc   { LVL1::DEFAULT_L1TopoCTPLocation };
      // output locations and control
      BooleanProperty m_isData { false };
      StringProperty m_roiOutputLoc { LVL1CTP::DEFAULT_CTPSLinkLocation };
      StringProperty m_roiOutputLoc_Rerun { LVL1CTP::DEFAULT_CTPSLinkLocation_Rerun };
      StringProperty m_rdoOutputLoc { LVL1CTP::DEFAULT_RDOOutputLocation };
      StringProperty m_rdoOutputLoc_Rerun { LVL1CTP::DEFAULT_RDOOutputLocation_Rerun };

      // property defining the eFEX parameters
      StringProperty m_eFEXREta { "" };
      StringProperty m_eFEXRHad { "" };
      StringProperty m_eFEXWStot { "" };
      std::map<unsigned int, eFEXParWP> m_reta;
      std::map<unsigned int, eFEXParWP> m_rhad;
      std::map<unsigned int, eFEXParWP> m_wstot;


      std::string m_histStream {"EXPERT"};

      // internal
      ThresholdMap*       m_thrMap { nullptr };
      ItemMap*            m_itemMap { nullptr };
      InternalTriggerMap  m_internalTrigger;
      ResultBuilder*      m_resultBuilder { nullptr };  

      // results
      std::vector<uint32_t> m_itemCountsSumTBP;
      std::vector<uint32_t> m_itemCountsSumTAP;
      std::vector<uint32_t> m_itemCountsSumTAV;

      // to decode the L1 Run-2 hardware ROIs from data
      LVL1::CPRoIDecoder * m_decoder { nullptr };
      LVL1::JEPRoIDecoder * m_jetDecoder { nullptr };
   };

}

#endif
