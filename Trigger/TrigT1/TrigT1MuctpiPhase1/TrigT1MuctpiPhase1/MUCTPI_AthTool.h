// This file is really -*- C++ -*-.

/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

#ifndef TRIGT1MUCTPIPHASE1_MUCTPI_ATHTOOL_H
#define TRIGT1MUCTPIPHASE1_MUCTPI_ATHTOOL_H

/*
class description
*/

#include "TrigT1Interfaces/IMuctpiSimTool.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"

#include "TrigT1Result/MuCTPI_RDO.h"
#include "xAODTrigger/MuonRoIContainer.h"

#include "TrigT1MuctpiPhase1/Configuration.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

//#include "TrigConfL1Data/Run3MuonTriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/L1DataBaseclass.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

namespace TrigConf {
   class ILVL1ConfigSvc;
   class TriggerThreshold;
}

namespace LVL1MUCTPIPHASE1 {

  class SimController;

  class MUCTPI_AthTool : public extends2<AthAlgTool, LVL1MUCTPI::IMuctpiSimTool, IIncidentListener>
  {
    
  public:
    
    MUCTPI_AthTool(const std::string& type, const std::string& name, 
		   const IInterface* parent);

    virtual ~MUCTPI_AthTool();

    virtual void handle(const Incident&) override;
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

    virtual StatusCode fillMuCTPIL1Topo(LVL1::MuCTPIL1Topo& l1topoCandidates, int bcidOffset) const override;
    
  private:

    /// Event loop method for running as part of digitization
    StatusCode executeFromDigi();
    /// Event loop method for running on an AOD file
    StatusCode executeFromAOD();
    /// Event loop method for running on an RDO file
    StatusCode executeFromRDO();
    /// Save the outputs of the simulation into StoreGate
    StatusCode saveOutput(int bcidOffset = 0);

    
    std::vector<int> m_bcidOffsetList = {-2,-1,1,2};

    std::string m_inputSource;
    std::string m_aodLocId;
    std::string m_rdoLocId;
    std::string m_tgcLocId;
    std::string m_rpcLocId;
    std::string m_roiOutputLocId;
    std::string m_rdoOutputLocId;
    std::string m_ctpOutputLocId;
    std::string m_l1topoOutputLocId;
    std::string m_geometryXMLFile;

    SG::ReadHandleKey<LVL1MUONIF::Lvl1MuCTPIInputPhase1> m_muctpiPhase1KeyRPC{this, "MuctpiPhase1LocationRPC", "L1MuctpiStoreRPC", "Location of muctpiPhase1 for Rpc"};
    SG::ReadHandleKey<LVL1MUONIF::Lvl1MuCTPIInputPhase1> m_muctpiPhase1KeyTGC{this, "MuctpiPhase1LocationTGC", "L1MuctpiStoreTGC", "Location of muctpiPhase1 for Tgc"};
    SG::ReadHandleKey<MuCTPI_RDO> m_MuCTPI_RDOReadKey{this, "MUCTPI_RDOReadKey", "MUCTPI_RDO", "Location of MuCTPI_RDO"};
    SG::WriteHandleKey<MuCTPI_RDO> m_MuCTPI_RDOWriteKey{this, "MUCTPI_RDOWriteKey", "MUCTPI_RDO", "Location of MuCTPI_RDO"};
    SG::WriteHandleKey<xAOD::MuonRoIContainer> m_MuCTPI_xAODWriteKey{this, "MUCTPI_xAODLocation", "LVL1MuonRoIs", "Location of xAOD::MuonRoIContainer"};
    SG::WriteHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPIL1TopoKey;
    SG::WriteHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPIL1TopoKey_m2;
    SG::WriteHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPIL1TopoKey_m1;
    SG::WriteHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPIL1TopoKey_p1;
    SG::WriteHandleKey<LVL1::MuCTPIL1Topo> m_MuCTPIL1TopoKey_p2;

    // These properties control how the multiplicity summation happens:
    std::string m_multiplicityStrategyName;
    std::string m_multiplicityXMLFile;

    std::string m_overlapStrategyName;
    std::string m_lutXMLFile;
    std::string m_runPeriod;

    bool m_flagMode;


    // Locations of the inputs and outputs of the simulation in StoreGate:
    static const std::string m_DEFAULT_locationMuCTPItoCTP;
    static const std::string m_DEFAULT_locationMuCTPItoL1Topo;
    static const std::string m_DEFAULT_locationMuCTPItoRoIB;
    static const std::string m_DEFAULT_L1MuctpiStoreLocationRPC;
    static const std::string m_DEFAULT_L1MuctpiStoreLocationTGC;
    static const std::string m_DEFAULT_AODLocID;
    static const std::string m_DEFAULT_RDOLocID;
    static const std::string m_DEFAULT_roibLocation;
    static const std::string m_DEFAULT_geometryXMLFile;
    
    ServiceHandle< TrigConf::ILVL1ConfigSvc > m_configSvc;
    SimController* m_theMuctpi;

    /// Function pointer to the execute function we want to use:
    StatusCode ( LVL1MUCTPIPHASE1::MUCTPI_AthTool::*m_executeFunction )( void );
    
  };
}

#endif // TRIGT1MUCTPIPHASE1_MUCTPI_ATHTOOL_H
