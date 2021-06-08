/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoEndcap/MbtsReadoutBuilder.h"

#include "CaloDetDescr/MbtsDetDescrManager.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloIdentifier/TileTBID.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelUtilities/GeoDBUtils.h"

#include "StoreGate/StoreGateSvc.h"

#define SYSTEM_OF_UNITS Gaudi::Units

StatusCode LArGeo::buildMbtsReadout(StoreGateSvc* detStore
				    , IRDBAccessSvc* paramSvc
				    , IMessageSvc* msgSvc
				    , double zposMM
				    , const std::map<std::string,unsigned>& trdMap
				    , const std::string& detKey
				    , const std::string& detNode)
{
  MsgStream log(msgSvc, "buildMbtsReadout"); 
  
  IRDBRecordset_ptr larPositionRec     = paramSvc->getRecordsetPtr("LArPosition",detKey,detNode);
  IRDBRecordset_ptr mbtsGen            = paramSvc->getRecordsetPtr("MBTSGen",detKey,detNode);
  IRDBRecordset_ptr mbtsScin           = paramSvc->getRecordsetPtr("MBTSScin",detKey,detNode);
  IRDBRecordset_ptr mbtsTrds           = paramSvc->getRecordsetPtr("MBTSTrds",detKey,detNode);

  // Get ID helper
  const TileTBID* tileTBID = nullptr;
  StatusCode sc = detStore->retrieve(tileTBID);
  if(sc.isFailure() || !tileTBID) {
    log << MSG::ERROR << "Failed to initialize TileTBID" << endmsg;
    return sc;
  }
  
  const IRDBRecord *posRec = GeoDBUtils::getTransformRecord(larPositionRec, "LARCRYO_EC_POS");
  if(!posRec) {
    log << MSG::ERROR << "No lar position record in the database" << endmsg;
    return StatusCode::FAILURE;
  }

  GeoTrf::Transform3D xfPos = GeoDBUtils::getTransform(posRec);
  double globalZMM = xfPos.translation().z() + zposMM;

  // Create MBTS manager
  MbtsDetDescrManager* mbtsManager = new MbtsDetDescrManager();

  // Create Calo DDEs for both sides
  for(int side=0; side<2; side++) {
    int sidesign = (side ? -1 : 1);

    for(unsigned int scinId=0; scinId<2; scinId++) {
      int nScin(0), eta(0);
      double dx1Scin(0.), dzScin(0.), zposScin(0.), rposScin(0.), scineta(0.), scindeta(0.), deltaPhi(0.), startPhi(0.);

      if(mbtsGen->size()==0) {
	// The "old" description:
	const IRDBRecord* curScin = (*mbtsScin)[scinId];

	nScin = curScin->getInt("SCINNUM");
	eta = curScin->getInt("SCIN_ID")-1;
	dx1Scin = curScin->getDouble("DX1")*SYSTEM_OF_UNITS::mm;
	dzScin  = curScin->getDouble("DZ")*SYSTEM_OF_UNITS::mm;
	zposScin = curScin->getDouble("ZPOS")*SYSTEM_OF_UNITS::mm;
	rposScin = curScin->getDouble("RPOS")*SYSTEM_OF_UNITS::mm;
	if(!curScin->isFieldNull("ETA")) {
	  scineta = curScin->getDouble("ETA");
	}
	if(!curScin->isFieldNull("DETA")) {
	  scindeta = curScin->getDouble("DETA");
	}
	deltaPhi = 360.*SYSTEM_OF_UNITS::deg/nScin;
	if(!curScin->isFieldNull("STARTPHI")) {
	  startPhi = curScin->getDouble("STARTPHI");
	}
      }
      else {
	// The "new" description
	std::string scinName = (scinId==0?"MBTS1":"MBTS2");
	const IRDBRecord* curScin = (*mbtsTrds)[trdMap.at(scinName)];
	nScin = (*mbtsGen)[0]->getInt("NSCIN");
	eta = curScin->getInt("SCIN_ID")-1;
	dx1Scin = curScin->getDouble("DX1")*SYSTEM_OF_UNITS::mm;
	dzScin  = curScin->getDouble("DZ")*SYSTEM_OF_UNITS::mm;
	zposScin = (*mbtsGen)[0]->getDouble("ZPOSENV")*SYSTEM_OF_UNITS::mm;
	rposScin = ((*mbtsGen)[0]->getDouble("RPOSENV")+curScin->getDouble("ZPOS"))*SYSTEM_OF_UNITS::mm;
	scineta = curScin->getDouble("ETA");
	scindeta = curScin->getDouble("DETA");
	startPhi = (*mbtsGen)[0]->getDouble("STARTPHI");
	deltaPhi = 360.*SYSTEM_OF_UNITS::deg/nScin;
      }

      for(int phi=0; phi<nScin; phi++) {
	// Construct CaloDDE
	MbtsDetectorElement* mbtsDDE = new MbtsDetectorElement();
	// Construct Identifier
	mbtsDDE->set_id(tileTBID->channel_id(sidesign,phi,eta));
	mbtsDDE->set_z((globalZMM + zposScin)*sidesign);
	mbtsDDE->set_dz(dx1Scin);
	mbtsDDE->set_r(rposScin);
	mbtsDDE->set_dr(dzScin);
	mbtsDDE->set_phi((phi+startPhi)*deltaPhi);
	mbtsDDE->set_dphi(deltaPhi*0.5);
	mbtsDDE->set_eta(scineta);
	mbtsDDE->set_deta(scindeta);
	mbtsDDE->compute_derived();
	// Store CaloDDE into the manager
	mbtsManager->add(mbtsDDE);
      } // Loop over indivudual scintillators
    } // Loop over mbtsScin contents
  } // Loop over sides

  // Record the manager into the DS
  sc = detStore->record(mbtsManager,"MBTS");
  if(sc.isFailure()) {
    log << MSG::FATAL << "Failed to record MBTS Detector Manager into the DS" << endmsg;
  }
  return sc;
}
