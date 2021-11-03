/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "CaloIdentifier/TileTBID.h"

#include "TileConditions/TileCablingService.h"

#include "IdDictParser/IdDictParser.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"


// //#include "GaudiKernel/DeclareFactoryEntries.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>




class TileCablingSvcMock {
 public:
  static void init_idhelpers ATLAS_NOT_THREAD_SAFE
    (IdDictParser& parser,
     std::string idDictFile = "IdDictTileCalorimeter.xml",
     TileCablingService::TileCablingType cablingType = TileCablingService::RUN2aCabling)
  {

     TileHWID* tileHWID = new TileHWID();
     TileID* tileID = new TileID();
     TileTBID* tileTBID = new TileTBID();

     tileID->set_do_neighbours (false);
     parser.register_external_entity ("TileCalorimeter", idDictFile);
     IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
     assert (tileHWID->initialize_from_dictionary (idd) == 0);
     assert (tileTBID->initialize_from_dictionary (idd) == 0);
     assert (tileID->initialize_from_dictionary (idd) == 0);

     ServiceHandle<StoreGateSvc> detStore("DetectorStore", "");
     assert(detStore.retrieve().isSuccess());

     assert( (detStore->record(tileHWID, "TileHWID")).isSuccess() );
     assert( (detStore->record(tileTBID, "TileTBID")).isSuccess() );
     assert( (detStore->record(tileID, "TileID")).isSuccess() );

     TileCablingService* svc = TileCablingService::getInstance_nc();
     svc->setTileHWID(tileHWID);
     svc->setTileTBID(tileTBID);
     svc->setTileID(tileID);
     svc->setCablingType(cablingType);
  }

  static void set_cabling_type ATLAS_NOT_THREAD_SAFE
    (TileCablingService::TileCablingType cablingType) {
   TileCablingService* svc = TileCablingService::getInstance_nc();
   svc->setCablingType(cablingType);
  }
};


std::string to_string(Identifier id, const TileID* tileID) {
  if (id.is_valid()) {
    if (tileID->is_tile(id)) {
      std::ostringstream oss;
      oss << tileID->system(id) << "/"
          << tileID->section(id) << "/"
          << tileID->side(id) << "/"
          << tileID->module(id) << "/"
          << tileID->tower(id) << "/"
          << tileID->sample(id) << "/"
          << tileID->pmt(id) << "/"
          << tileID->adc(id);
      return oss.str();
    } else {
      return "NOT TILE";
    }
  } else {
    return "INVALID";
  }
}

std::string to_string(HWIdentifier hwid, const TileHWID* tileHWID) {
  if (hwid.is_valid()) {
    if (tileHWID->is_tile(hwid)) {
      std::ostringstream oss;
      oss << tileHWID->system(hwid) << "/"
          << tileHWID->section(hwid) << "/"
          << tileHWID->ros(hwid) << "/"
          << tileHWID->drawer(hwid) << "/"
          << tileHWID->channel(hwid) << "/"
          << tileHWID->adc(hwid);
      return oss.str();
    } else {
      return "NOT TILE";
    }
  } else {
    return "INVALID";
  }
}
