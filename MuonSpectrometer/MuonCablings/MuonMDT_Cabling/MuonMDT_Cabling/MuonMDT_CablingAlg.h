/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
   MuonMDT_CablingAlg reads raw condition data and writes derived condition data to the condition store
*/

#ifndef MUONMDT_CABLING_MUONMDT_CABLINGALG_H
#define MUONMDT_CABLING_MUONMDT_CABLINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "GaudiKernel/AlgTool.h"
#include "MuonCondInterface/IMDTCablingDbTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class Identifier; 
class IIOVSvc;
class IIOVDbSvc;
class StatusCode;
class MuonMDT_CablingMap;

class MuonMDT_CablingAlg: public AthAlgorithm {

 public:

  MuonMDT_CablingAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MuonMDT_CablingAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 private:

  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyMez;
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyMap;
  SG::WriteCondHandleKey<MuonMDT_CablingMap> m_writeKey{this, "WriteKey", "MuonMDT_CablingMap", "Key of output MDT cabling map"};
  ServiceHandle<ICondSvc> m_condSvc;
  
};

#endif
