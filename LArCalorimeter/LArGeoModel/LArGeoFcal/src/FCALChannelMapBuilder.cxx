/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoFcal/FCALChannelMapBuilder.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "StoreGate/StoreGateSvc.h"

StatusCode LArGeo::buildFcalChannelMap(StoreGateSvc* detStore
				       , IRDBAccessSvc* paramSvc
				       , IMessageSvc* msgSvc)
{
  MsgStream log(msgSvc, "buildFcalChannelMap");
  FCAL_ChannelMap* cmap = new FCAL_ChannelMap(0);

  std::vector<IRDBRecordset_ptr> recordsets { paramSvc->getRecordsetPtr("FCalElecMod1","")
      , paramSvc->getRecordsetPtr("FCalElecMod2","")
      , paramSvc->getRecordsetPtr("FCalElecMod3","") };

  for(IRDBRecordset_ptr recordset : recordsets) {
    auto itStart = recordset->begin();
    auto itEnd = recordset->end();

    for(;itStart!=itEnd;++itStart) {
      const IRDBRecord* rec = *itStart;
      cmap->add_tube(rec->getString("TILENAME")
		     , rec->getInt("MODNUMBER")
		     , rec->getInt("IDENTIFIER")
		     , rec->getInt("I")
		     , rec->getInt("J")
		     , rec->getDouble("X")
		     , rec->getDouble("Y")
		     , rec->getString("HVFEEDTHROUGHID"));
    }
  }

  cmap->finish();

  StatusCode sc = detStore->record(cmap,"FCAL_ChannelMap");
  if(sc.isFailure()) {
    log << MSG::FATAL << "Failed to build FCAL Channel Map" << endmsg;
  }

  return sc;
}
