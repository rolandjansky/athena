/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1_CTMONITORING_BSMONITORING_H
#define TRIGT1_CTMONITORING_BSMONITORING_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaKernel/CondCont.h" 
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CoolKernel/IObject.h"

#include "TrigConfData/L1Menu.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "TrigT1Result/MuCTPI_RDO.h"
//#include "TrigT1Result/MuCTPI_RIO.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_RIO.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "TrigT1MuctpiBits/MuCTPI_Bits.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"

// RPC and TGC includes for access to SL data
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "MuonDigitContainer/TgcDigit.h"

class CTP_RDO;
class CTP_RIO;
class MuCTPI_RDO;
//class MuCTPI_RIO;
class CTP_BC;

namespace ROIB {
  class RoIBResult;
}

namespace LVL1 {
  class MuCTPICTP;
  class EmTauCTP;
  class JetCTP;
  class EnergyCTP;
  class MbtsCTP;
  class BcmCTP;
  class BptxCTP;
  class NimCTP;
  class RecMuonRoiSvc;
}

namespace TrigT1CTMonitoring {
  class BSMonitoringAlgorithm : public AthMonitorAlgorithm {

  public:BSMonitoringAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~BSMonitoringAlgorithm()=default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:
    StringProperty m_packageName{this,"PackageName","CTPMonitor","group name for histograming"};

    void doMuonRoI( const MuCTPI_RDO* theMuCTPI_RDO,
                    //const MuCTPI_RIO* theMuCTPI_RIO,
                    const ROIB::RoIBResult* roib,
		    const EventContext& ctx ) const;

    void doCtp( const CTP_RDO* theCTP_RDO,
                const CTP_RIO* theCTP_RIO,
		const EventContext& ctx ) const;

    void doMuctpi(const MuCTPI_RDO* theMuCTPI_RDO,
                  //const MuCTPI_RIO* theMuCTPI_RIO,
                  const RpcSectorLogicContainer* theRPCContainer,
                  const Muon::TgcCoinDataContainer* theTGCContainer,
		  const EventContext& ctx ) const;

    void doCtpMuctpi( const CTP_RDO* theCTP_RDO,
                      //const CTP_RIO* theCTP_RIO,
                      const MuCTPI_RDO* theMuCTPI_RDO,
                      //const MuCTPI_RIO* theMuCTPI_RIO,
		      const EventContext& ctx ) const;

    void dumpData(const CTP_RDO* theCTP_RDO,
                  //const CTP_RIO* theCTP_RIO,
                  const MuCTPI_RDO* theMuCTPI_RDO,
                  //const MuCTPI_RIO* theMuCTPI_RIO,
                  const ROIB::RoIBResult* roib,
		  const EventContext& ctx ) const;

    StatusCode compareRerun(const CTP_BC &bunchCrossing,
			    const EventContext& ctx ) const;

    //ToolHandle< LVL1::ITrigT1MuonRecRoiTool > m_rpcRoiTool{ this, "RPCRecRoiTool", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool", "RPC Rec Roi Tool"};
    //ToolHandle< LVL1::ITrigT1MuonRecRoiTool > m_tgcRoiTool{ this, "TGCRecRoiTool", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool", "TGC Rec Roi Tool"};

    SG::ReadCondHandleKey<AthenaAttributeList> m_LBLBFolderInputKey{ this, "LBLBFolderInputKey", "/TRIGGER/LUMI/LBLB" };
    SG::ReadCondHandleKey<CondAttrListCollection> m_FILLSTATEFolderInputKey{ this, "FILLSTATEFolderInputKey", "/LHC/DCS/FILLSTATE" };
    SG::ReadCondHandleKey<AthenaAttributeList> m_DataTakingModeFolderInputKey{ this, "DataTakingModeFolderInputKey", "/TDAQ/RunCtrl/DataTakingMode" };

    SG::ReadHandleKey<MuCTPI_RDO> m_MuCTPI_RDOKey{ this, "MuCTPI_RDOKey", "MUCTPI_RDO" };
    //SG::ReadHandleKey<MuCTPI_RIO> m_MuCTPI_RIOKey{ this, "MuCTPI_RIOKey", "MUCTPI_RIO" };
    SG::ReadHandleKey<CTP_RDO> m_CTP_RDOKey{ this, "CTP_RDOKey", "CTP_RDO" };
    SG::ReadHandleKey<CTP_RIO> m_CTP_RIOKey{ this, "CTP_RIOKey", "CTP_RIO" };
    SG::ReadHandleKey<CTP_RDO> m_CTP_RDO_RerunKey{ this, "CTP_RDO_RerunKey", "CTP_RDO_Rerun" };
    SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey{ this, "RoIBResultKey", "RoIBResult" };
    SG::ReadHandleKey<RpcSectorLogicContainer> m_RPCContainerKey{ this, "RPCContainerKey", "RPC_SECTORLOGIC" };
    SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TGCContainerKey{ this, "TGCContainerKey", "TrigT1CoinDataCollection" };

    Gaudi::Property<bool> m_isSim{ this, "isSimulation",  false, "isSimulation" };
    //Gaudi::Property<std::string> m_baseDirName{ this, "DirectoryName", "CT/", "Directory in output root file where the histograms will be stored." };
    Gaudi::Property<bool> m_inclusiveTriggerThresholds{ this, "InclusiveTriggerThresholds", true, "Flag to activate the inclusive counting of PT thresholds in trigger patterns" };
    Gaudi::Property<bool> m_processMuctpi{ this, "ProcessMuctpiData", true, "Flag to activate the processing of Muctpi data" };
    Gaudi::Property<bool> m_processMuctpiRIO{ this, "ProcessMuctpiDataRIO",  true, "Flag to activate the processing of the Muctpi RIO" };
    Gaudi::Property<bool> m_compareRerun{ this, "CompareRerun", false, "Flag to activate the processing of RoIBResult data" };
    Gaudi::Property<bool> m_runOnESD{ this, "RunOnESD", false, "Flag to run only on the ESD" };

    Gaudi::Property<bool> m_processCTP{ this, "ProcessCTPData", true, "Flag to activate the processing of CTP data" };
    Gaudi::Property<bool> m_processRoIB{ this, "ProcessRoIBResult", true, "Flag to activate the processing of RoIBResult data" };

    Gaudi::Property<double> m_defaultBcIntervalInNs{ this, "DefaultBcIntervalInNs", 24.9507401, "Default bunch-crossing duration to use if not accessible in COOL" };
    Gaudi::Property<int64_t> m_bcsPerTurn{ this, "BCsPerTurn", 3564, "Number of bunch crossings per turn" };
    Gaudi::Property<std::string > m_lbTimeCoolFolderName{ this, "LumiBlockTimeCoolFolderName", "/TRIGGER/LUMI/LBLB", "COOL folder in COOLONL_TRIGGER holding info about start and stop times for luminosity blocks" };
    Gaudi::Property<std::string > m_fillStateCoolFolderName{ this, "FillStateCoolFolderName", "/LHC/DCS/FILLSTATE", "COOL folder in COOLOFL_DCS holding the LHC fill state info" };
    Gaudi::Property<std::string> m_dataTakingModeCoolFolderName{ this, "DataTakingModeCoolFolderName", "/TDAQ/RunCtrl/DataTakingMode", "COOL folder in COOLONL_TDAQ holding the ATLAS data taking mode info" };
    Gaudi::Property<std::vector<std::string>> m_ignorePatterns{ this, "IgnorePatterns", {"L1_TRT", "L1_ZB", "_AFP", "L1_BPTX", "L1_BCM", "L1_LUCID"}, "patters that are excluded from check (no regex)"};

  };
}
#endif
