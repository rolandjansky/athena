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

// monitoring from HLT
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "GaudiKernel/ITHistSvc.h"

// data
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"

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

      StatusCode simulateItems();

      StatusCode createMultiplicityHist(const std::string & type, TrigConf::L1DataDef::TriggerType tt, unsigned int maxMult = 10 );

      std::string histBasePath() const { 
         return "/" + m_histStream + "/CTPEmulation";
      }


      // member variables

      // Needed services
      ServiceHandle<ITHistSvc> m_histSvc;
      ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;
      ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool; //!< tool to create L1 ROIs from ROIBResult

      // Inputs from the new L1Calo
      // gFEX
      const DataHandle< xAOD::EnergySumRoI >    m_gFEXMETPufit;   //!< MET from gFEX
      const DataHandle< xAOD::EnergySumRoI >    m_gFEXMETRho;     //!< MET from gFEX
      const DataHandle< xAOD::EnergySumRoI >    m_gFEXMETJwoJ;    //!< MET from gFEX
      const DataHandle< xAOD::JetRoIContainer > m_gJet;           //!< jets from gFEX 

      // eFEX
      const DataHandle< xAOD::TrigEMClusterContainer > m_eFEXCluster; //!< cluster from eFEX
      const DataHandle< xAOD::EmTauRoIContainer > m_eFEXTau; //!< taus from eFEX

      // jFEX
      const DataHandle< xAOD::JetRoIContainer > m_jJet;           //!< jets from jFEX 
      const DataHandle< xAOD::JetRoIContainer > m_jLJet;          //!< large jets from jFEX 


      // old ROI collections
      const DataHandle< xAOD::MuonRoIContainer > m_muonRoIs;
      const DataHandle< xAOD::JetRoIContainer > m_lgJetRoIs;

      // from BS
      const ROIB::RoIBResult * m_roibResult { nullptr };

      // Other inputs to the simulations
      const DataHandle< LVL1::MuCTPICTP > m_muctpiCTP;

      // properties
      BooleanProperty m_useCTPInput { false };

      // name of input collections

      // new FEX collections
      StringProperty m_gFEXMETPufitLoc {""};
      StringProperty m_gFEXMETRhoLoc {""};
      StringProperty m_gFEXMETJwoJLoc {""};
      StringProperty m_gJetLoc {""};
      StringProperty m_jJetLoc {""};
      StringProperty m_jLJetLoc {""};
      StringProperty m_eFEXClusterLoc {""};
      StringProperty m_eFEXTauLoc {""};

      // old ROI Collections
      StringProperty m_muonRoILoc {""};
      StringProperty m_lgJetRoILoc {""};

      // name of the CTP input words
      StringProperty m_muonCTPLoc {""};

      // output locations and control
      BooleanProperty m_isData { false };
      StringProperty m_roiOutputLoc {""};
      StringProperty m_roiOutputLoc_Rerun {""};
      StringProperty m_rdoOutputLoc {""};
      StringProperty m_rdoOutputLoc_Rerun {""};

      std::string m_histStream {""};

      // internal
      ThresholdMap*       m_thrMap { nullptr };
      ItemMap*            m_itemMap { nullptr };
      InternalTriggerMap* m_internalTrigger { nullptr };
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
