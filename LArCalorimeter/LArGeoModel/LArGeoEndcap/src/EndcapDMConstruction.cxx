/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "EndcapDMConstruction.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoGenericFunctions/Variable.h"

// ***********************************************************************
//
// Construction of passive materials in the LAr EC cryostate.
// Initial implementation consists of the lectronics crates 
// copied over from the Barrel DM description.
//
// ***********************************************************************

LArGeo::EndcapDMConstruction::EndcapDMConstruction()
{

}

LArGeo::EndcapDMConstruction::~EndcapDMConstruction()
{
}

void LArGeo::EndcapDMConstruction::create(GeoFullPhysVol* envelope)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IMessageSvc* msgSvc(0);
  if(svcLocator->service("MessageSvc", msgSvc, true)==StatusCode::FAILURE) 
    throw std::runtime_error("Error in EndcapDMConstruction, cannot access MessageSvc");
  MsgStream log(msgSvc, "EndcapDMConstruction");
  log << MSG::INFO << "Start building EC electronics geometry" << endmsg;

  StoreGateSvc* detStore(0);
  if(svcLocator->service("DetectorStore", detStore, false)==StatusCode::FAILURE) 
    throw std::runtime_error("Error in EndcapDMConstruction, cannot access DetectorStore");

  IGeoModelSvc* geoModel(0);
  if(svcLocator->service("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in EndcapDMConstruction, cannot access GeoModelSvc");

  IRDBAccessSvc* rdbAccess(0);
  if(svcLocator->service("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in EndcapDMConstruction, cannot access RDBAccessSvc");

  DecodeVersionKey keyLAr(geoModel, "LAr");
  const StoredMaterialManager* materialManager = nullptr;
  if(StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) 
    throw std::runtime_error("Error in EndcapDMConstruction, stored MaterialManager is not found");

  IRDBRecordset_ptr LArEndcapCrate = rdbAccess->getRecordsetPtr("LArEndcapCrate",keyLAr.tag(),keyLAr.node());
  if(LArEndcapCrate->size()==0) {
    log << MSG::INFO << "Skip building EC electronics. Database flag not set" << endmsg;
    return;
  }
  IRDBRecordset_ptr LArEndcapCratePhiPos = rdbAccess->getRecordsetPtr("LArEndcapCratePhiPos",keyLAr.tag(),keyLAr.node());
  IRDBRecordset_ptr BarrelDMTraps = rdbAccess->getRecordsetPtr("BarrelDMTraps",keyLAr.tag(),keyLAr.node());
  IRDBRecordset_ptr BarrelDMTrds = rdbAccess->getRecordsetPtr("BarrelDMTrds",keyLAr.tag(),keyLAr.node());
  IRDBRecordset_ptr BarrelDMTubes = rdbAccess->getRecordsetPtr("BarrelDMTubes",keyLAr.tag(),keyLAr.node());
  IRDBRecordset_ptr BarrelDMBoxes = rdbAccess->getRecordsetPtr("BarrelDMBoxes",keyLAr.tag(),keyLAr.node());

  std::map<std::string, unsigned int> boxMap;
  for(unsigned int j=0; j<BarrelDMBoxes->size(); j++) {
    std::string key = (*BarrelDMBoxes)[j]->getString("BOXNAME");
    boxMap[key] = j;
  }
  std::map<std::string, unsigned int> tubeMap;
  for(unsigned int i=0; i<BarrelDMTubes->size(); i++) {
    std::string key = (*BarrelDMTubes)[i]->getString("TUBENAME");
    tubeMap[key] = i;
  }
  std::map<std::string, unsigned int> ecCrateMap;
  for(unsigned int i=0; i<LArEndcapCrate->size(); i++) {
    std::string key = (*LArEndcapCrate)[i]->getString("BOXNAME");
    ecCrateMap[key] = i;
  }

  unsigned int recordIndex;

  // Get materials
  const GeoMaterial *alu               = materialManager->getMaterial("std::Aluminium");
  const GeoMaterial* matBoardsEnvelope = materialManager->getMaterial("LAr::BoardsEnvelope");

  ////----------- Building Front-end crates --------------------
  recordIndex = tubeMap["Ped2"];
  double ped2zhlen = (*BarrelDMTubes)[recordIndex]->getDouble("ZHLEN");
  double ped2minr = (*BarrelDMTubes)[recordIndex]->getDouble("MINR");
  double ped2maxr = (*BarrelDMTubes)[recordIndex]->getDouble("MAXR");
  double ped2ytr = (*BarrelDMTubes)[recordIndex]->getDouble("YTR");
  recordIndex = tubeMap["Ped3"];
  double ped3zhlen = (*BarrelDMTubes)[recordIndex]->getDouble("ZHLEN");
  double ped3minr = (*BarrelDMTubes)[recordIndex]->getDouble("MINR");
  double ped3maxr = (*BarrelDMTubes)[recordIndex]->getDouble("MAXR");
  double ped3xtr = (*BarrelDMTubes)[recordIndex]->getDouble("XTR");

  recordIndex = boxMap["Pedest"];
  double pedesthlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
  double pedesthwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
  double pedesthhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
  double pedestxtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
  double pedestztr = (*BarrelDMBoxes)[recordIndex]->getDouble("ZTR");
  recordIndex = boxMap["Ped1"];
  double ped1hlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
  double ped1hwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
  double ped1hhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
  recordIndex = boxMap["Crate1"];
  double crate1hlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
  double crate1hwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
  double crate1hhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
  double crate1xtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
  double crate1ztr = (*BarrelDMBoxes)[recordIndex]->getDouble("ZTR");
  recordIndex = boxMap["Crate2"];
  double crate2hlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
  double crate2hwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
  double crate2hhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
  recordIndex = boxMap["Crate3"];
  double crate3hlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
  double crate3hwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
  double crate3hhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
  double crate3xtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
  recordIndex = boxMap["BoardE"];
  double BoardEhlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
  double BoardEhwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
  double BoardEhhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
  double BoardExtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
  double BoardEytr = (*BarrelDMBoxes)[recordIndex]->getDouble("YTR");
  double BoardEztr = (*BarrelDMBoxes)[recordIndex]->getDouble("ZTR");

  // For endcap cryostat overwrite some values 
  std::map<std::string, unsigned int>::const_iterator it = ecCrateMap.find("Crate1");
  if(it!=ecCrateMap.end()) {
    recordIndex = ecCrateMap["Crate1"];
    double ztrEC = (*LArEndcapCrate)[recordIndex]->getDouble("ZTR");
    double xtrEC = (*LArEndcapCrate)[recordIndex]->getDouble("XTR");
    crate1ztr = ztrEC;
    pedestztr = ztrEC;
    BoardEztr = ztrEC;    
    crate1xtr = xtrEC;
  }
  it = ecCrateMap.find("Pedest");
  if(it!=ecCrateMap.end()) {
    recordIndex = ecCrateMap["Pedest"];
    pedestxtr = (*LArEndcapCrate)[recordIndex]->getDouble("XTR");
  }
  it = ecCrateMap.find("BoardE");
  if(it!=ecCrateMap.end()) {
    recordIndex = ecCrateMap["BoardE"];
    BoardExtr = (*LArEndcapCrate)[recordIndex]->getDouble("XTR");
  }

  double epsilon(6.); // Squeeze boxes in EC, as there is not enough room

  // ----- build pedestal -----
  GeoBox     *Pedestal = new GeoBox(pedesthlen, pedesthwdt, pedesthhgt-epsilon);
  GeoBox     *Ped1     = new GeoBox(ped1hlen, ped1hwdt, ped1hhgt-epsilon);
  GeoTube    *Ped2     = new GeoTube(ped2minr, ped2maxr, ped2zhlen);
  GeoTube    *Ped3     = new GeoTube(ped3minr,ped3maxr , ped3zhlen);
  const GeoShape & CratePed=((*Pedestal).subtract(*Ped1).
			     subtract((*Ped2)  <<GeoTrf::TranslateY3D(-ped2ytr)*GeoTrf::RotateY3D(90*Gaudi::Units::deg)).
			     subtract((*Ped3)  <<GeoTrf::TranslateX3D(-ped3xtr)).
			     subtract((*Ped2)  <<GeoTrf::TranslateY3D(ped2ytr)*GeoTrf::RotateY3D(90*Gaudi::Units::deg)));

  GeoLogVol  *lvped   = new GeoLogVol("LAr::DM::Ped",&CratePed,alu);
  GeoPhysVol *pedestal   = new GeoPhysVol(lvped);

  // ----- build crates -----
  GeoBox     *Crate1   = new GeoBox(crate1hlen, crate1hwdt, crate1hhgt-epsilon);
  GeoBox     *Crate2   = new GeoBox(crate2hlen, crate2hwdt, crate2hhgt-epsilon);
  GeoBox     *Crate3   = new GeoBox(crate3hlen, crate3hwdt, crate3hhgt-epsilon);
  const GeoShape & FEBCrate=(*Crate1).subtract(*Crate2).add((*Crate3)  <<GeoTrf::TranslateX3D(-crate3xtr));

  GeoLogVol  *lvcrate = new GeoLogVol("LAr::DM::Crate",&FEBCrate,alu);
  GeoPhysVol *crate   = new GeoPhysVol(lvcrate);

  // ----- build boardenvelopes -----
  GeoBox     *BoardEnvelope   = new GeoBox(BoardEhlen, BoardEhwdt, BoardEhhgt-epsilon);
  GeoLogVol  *lvbenv = new GeoLogVol("LAr::DM::FEBoard",BoardEnvelope,matBoardsEnvelope);
  GeoPhysVol *boardenvelope   = new GeoPhysVol(lvbenv);

  //-------------- Place volumes in envelope ----------------------------
  GeoTransform* xfCrateBase(new GeoTransform(GeoTrf::TranslateX3D(crate1xtr)*GeoTrf::TranslateZ3D(crate1ztr)));
  GeoTransform* xfPedestBase(new GeoTransform(GeoTrf::TranslateX3D(pedestxtr)*GeoTrf::TranslateZ3D(pedestztr)));
  GeoTransform* xfBoardEBase1(new GeoTransform(GeoTrf::TranslateY3D(BoardEytr)*GeoTrf::TranslateX3D(BoardExtr)*GeoTrf::TranslateZ3D(BoardEztr)));
  GeoTransform* xfBoardEBase2(new GeoTransform(GeoTrf::TranslateY3D(-BoardEytr)*GeoTrf::TranslateX3D(BoardExtr)*GeoTrf::TranslateZ3D(BoardEztr)));

  for(unsigned i(0); i<LArEndcapCratePhiPos->size(); ++i) {
    double phiPos = (*LArEndcapCratePhiPos)[i]->getDouble("PHIPOS")*Gaudi::Units::deg;
    GeoTransform* xfPhiPos(new GeoTransform(GeoTrf::RotateZ3D(phiPos)));

    envelope->add(xfPhiPos);
    envelope->add(xfCrateBase);
    envelope->add(crate);

    envelope->add(xfPhiPos);
    envelope->add(xfPedestBase);
    envelope->add(pedestal);

    envelope->add(xfPhiPos);
    envelope->add(xfBoardEBase1);
    envelope->add(boardenvelope);

    envelope->add(xfPhiPos);
    envelope->add(xfBoardEBase2);
    envelope->add(boardenvelope);
  }

  // Build services
  IRDBRecordset_ptr EndcapDMTubes = rdbAccess->getRecordsetPtr("LArEndcapDMTubes",keyLAr.tag(),keyLAr.node());
  if(EndcapDMTubes->size()) {
    const GeoMaterial* matECServices = materialManager->getMaterial("LAr::LArECServices");    
    for(unsigned i(0); i<EndcapDMTubes->size(); ++i) {
      GeoTube* endcapTube = new GeoTube((*EndcapDMTubes)[i]->getDouble("RMIN"),(*EndcapDMTubes)[i]->getDouble("RMAX"),(*EndcapDMTubes)[i]->getDouble("DZ"));
      GeoLogVol* endcapTubeLv = new GeoLogVol((*EndcapDMTubes)[i]->getString("TUBENAME"),endcapTube,matECServices);
      GeoPhysVol* endcapTubePv = new GeoPhysVol(endcapTubeLv);
      envelope->add(new GeoTransform(GeoTrf::TranslateZ3D((*EndcapDMTubes)[i]->getDouble("ZPOS"))));
      envelope->add(endcapTubePv);
    }
  }

}

