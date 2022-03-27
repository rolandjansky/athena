/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
   MuonMDT_CablingAlg reads raw condition data and writes derived condition data to the condition store
*/

#ifndef MUONMDT_CABLING_MUONMDT_CABLINGALG_H
#define MUONMDT_CABLING_MUONMDT_CABLINGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class IIOVSvc;
class IIOVDbSvc;


class MuonMDT_CablingAlg: public AthAlgorithm {

 public:

  MuonMDT_CablingAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MuonMDT_CablingAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  
  using CablingData = MuonMDT_CablingMap::CablingData;
 private:
   ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
   SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyMez{this, "MezzanineFolders", "/MDT/CABLING/MEZZANINE_SCHEMA"};
   SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyMap{this, "MapFolders", "/MDT/CABLING/MAP_SCHEMA"};
   SG::WriteCondHandleKey<MuonMDT_CablingMap> m_writeKey{this, "WriteKey", "MuonMDT_CablingMap", "Key of output MDT cabling map"};

  /// Retrieves the general MDT station info from the coral attribute
  bool extractStationInfo(const coral::AttributeList& atr, CablingData& map_data ) const;
  /// Retrieves the channel info from the coral attribute
  bool extractLayerInfo(std::vector<std::string>& , CablingData& map_data ) const;

  /// Load the mezzanine schem into the cabling
  StatusCode loadMezzanineSchema(const EventContext& ctx, EventIDRange& iov_range, MuonMDT_CablingMap& cabling_map) const;
  /// Load the cabling schema of the tubes

  StatusCode loadCablingSchema(const EventContext& ctx, EventIDRange& iov_range, MuonMDT_CablingMap& cabling_map) const;

};

#endif
