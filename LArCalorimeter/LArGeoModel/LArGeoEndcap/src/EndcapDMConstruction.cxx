/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
// ***********************************************************************
// Construction of passive materials around LAr EC cryostate.
// ***********************************************************************

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
#include "GeoModelKernel/GeoPcon.h"
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

LArGeo::EndcapDMConstruction::EndcapDMConstruction(bool ft) :
  m_activateFT(ft)
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
  if(it != ecCrateMap.end()) {
    recordIndex = ecCrateMap["Crate1"];
    double ztrEC = (*LArEndcapCrate)[recordIndex]->getDouble("ZTR");
    /* FEC position sould be corrected in the DB, but we have no time */
    if(m_activateFT && ztrEC == -249.05){
       ztrEC += 6.;
       log << MSG::DEBUG << "FEC position corrected to " << ztrEC
           << " to fit mother volume" << endmsg;
    }
    crate1ztr = ztrEC;
    pedestztr = ztrEC;
    BoardEztr = ztrEC;
    if(m_activateFT){
      // keep crate at same height as in Barrel - there is no difference
    } else {
      // keep the old behavoir
      double xtrEC = (*LArEndcapCrate)[recordIndex]->getDouble("XTR");
      crate1xtr = xtrEC;
    }
  }
  if(!m_activateFT){ // keep the old behavior
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
  } // otherwise do not move Pedest and BoardE in r - they are also like in the barrel

  const double epsilon = m_activateFT? 0.: 6.;

  // ----- build pedestal -----
  GeoBox     *Pedestal = new GeoBox(pedesthlen, pedesthwdt, pedesthhgt - epsilon);
  GeoBox     *Ped1     = new GeoBox(ped1hlen, ped1hwdt, ped1hhgt - epsilon);
  GeoTube    *Ped2     = new GeoTube(ped2minr, ped2maxr, ped2zhlen);
  GeoTube    *Ped3     = new GeoTube(ped3minr,ped3maxr , ped3zhlen);
  const GeoShape & CratePed=((*Pedestal).subtract(*Ped1).
			     subtract((*Ped2)  <<GeoTrf::TranslateY3D(-ped2ytr)*GeoTrf::RotateY3D(90*Gaudi::Units::deg)).
			     subtract((*Ped3)  <<GeoTrf::TranslateX3D(-ped3xtr)).
			     subtract((*Ped2)  <<GeoTrf::TranslateY3D(ped2ytr)*GeoTrf::RotateY3D(90*Gaudi::Units::deg)));

  GeoLogVol  *lvped   = new GeoLogVol("LAr::DM::Ped",&CratePed,alu);
  GeoPhysVol *pedestal   = new GeoPhysVol(lvped);

  // ----- build crates -----
  GeoBox     *Crate1   = new GeoBox(crate1hlen, crate1hwdt, crate1hhgt - epsilon);
  GeoBox     *Crate2   = new GeoBox(crate2hlen, crate2hwdt, crate2hhgt - epsilon);
  GeoBox     *Crate3   = new GeoBox(crate3hlen, crate3hwdt, crate3hhgt - epsilon);
  const GeoShape & FEBCrate=(*Crate1).subtract(*Crate2).add((*Crate3)  <<GeoTrf::TranslateX3D(-crate3xtr));

  GeoLogVol  *lvcrate = new GeoLogVol("LAr::DM::Crate",&FEBCrate,alu);
  GeoPhysVol *crate   = new GeoPhysVol(lvcrate);

  // ----- build boardenvelopes -----
  GeoBox     *BoardEnvelope   = new GeoBox(BoardEhlen, BoardEhwdt, BoardEhhgt - epsilon);
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

  if(m_activateFT){
    std::string name = "LAr::Endcap::SignalFT::";
    log << MSG::INFO << "creating " << name << " volumes" << endmsg;

    const GeoMaterial* iron = materialManager->getMaterial("std::Iron");

    const double wflange_height = 37.*Gaudi::Units::mm;
    const double wflange_R = 0.5*360.*Gaudi::Units::mm;
    const GeoMaterial* wflange_mat = materialManager->getMaterial("LAr::FT::WarmFlange");

    GeoShape* wflange = new GeoTube(0., wflange_R, wflange_height/2);
    GeoLogVol* wflangeLV = new GeoLogVol(name + "WarmFlange", wflange, wflange_mat);
    GeoPhysVol* wflangePV = new GeoPhysVol(wflangeLV);

    const double bellow_height = 225.*Gaudi::Units::mm;
    const double bellow_Router = 0.5*299.*Gaudi::Units::mm; // this also to be cut in warm wall
    const double bellow_wall = 15.*Gaudi::Units::mm;
    const GeoMaterial* bellow_mat = materialManager->getMaterial("LAr::FT::Bellow");

    const double bellow_Rinner = bellow_Router - bellow_wall;
    GeoShape* bellow = new GeoTube(bellow_Rinner, bellow_Router, bellow_height/2);
    GeoLogVol* bellowLV = new GeoLogVol(name + "Bellow", bellow, bellow_mat);
    GeoPhysVol* bellowPV = new GeoPhysVol(bellowLV);

    const GeoMaterial* vcables_mat = materialManager->getMaterial("LAr::FT::VacuumCables");

    GeoShape* vcables = new GeoTube(0., bellow_Rinner, bellow_height/2);
    GeoLogVol* vcablesLV = new GeoLogVol(name + "VacuumCables", vcables, vcables_mat);
    GeoPhysVol* vcablesPV = new GeoPhysVol(vcablesLV);

    const double cflange_height = 35.*Gaudi::Units::mm;
    const double cflange_Router = 0.5*283.*Gaudi::Units::mm;
    const GeoMaterial* cflange_mat = materialManager->getMaterial("LAr::FT::ColdFlange");
    GeoShape* cflange = new GeoTube(0., cflange_Router, cflange_height/2);
    GeoLogVol* cflangeLV = new GeoLogVol(name + "ColdFlange", cflange, cflange_mat);
    GeoPhysVol* cflangePV = new GeoPhysVol(cflangeLV);

    const double coldbox1_Router = cflange_Router;
    const double coldbox1_wall = 0.134*2.54*Gaudi::Units::cm;
    const double coldbox1_height = 90.*Gaudi::Units::mm;
    const double coldbox2_height = 16.*Gaudi::Units::mm;
    const double hole_r = 0.5*133.*Gaudi::Units::mm;
    const double hole_shift = -31.*Gaudi::Units::mm;
    const double coldbox3_Router = 0.5*140.*Gaudi::Units::mm; // this also to be cut in cold wall
    const double coldbox3_height = 220.*Gaudi::Units::mm;
    const GeoMaterial* coldbox_mat = iron;
    GeoShape* coldbox1 = new GeoTube(coldbox1_Router - coldbox1_wall, coldbox1_Router, coldbox1_height/2); // wide part
    GeoShape* coldbox11 = new GeoTube(0., coldbox1_Router, coldbox1_height/2); // wide part for FTenvelope
    GeoLogVol* coldbox1LV = new GeoLogVol(name + "ColdBox1", coldbox1, coldbox_mat);
    GeoPhysVol* coldbox1PV = new GeoPhysVol(coldbox1LV);
    GeoShape* coldbox21 = new GeoTube(0., coldbox1_Router, coldbox2_height/2); // plate
    GeoShape* coldbox22 = new GeoTube(0., hole_r, coldbox2_height); // hole in the plate
    const GeoShape& coldbox2 = coldbox21->subtract((*coldbox22) << GeoTrf::TranslateY3D(hole_shift));
    GeoLogVol* coldbox2LV = new GeoLogVol(name + "ColdBox2", &coldbox2, coldbox_mat);
    GeoPhysVol* coldbox2PV = new GeoPhysVol(coldbox2LV);
    GeoShape* coldbox3 = new GeoTube(hole_r, coldbox3_Router, coldbox3_height/2); // narrow part
    GeoShape* coldbox31 = new GeoTube(0., coldbox3_Router, coldbox3_height/2); // narrow part for FTenvelope
    GeoLogVol* coldbox3LV = new GeoLogVol(name + "ColdBox3", coldbox3, coldbox_mat);
    GeoPhysVol* coldbox3PV = new GeoPhysVol(coldbox3LV);

    GeoTrf::TranslateZ3D bellow_pos(-wflange_height/2 - bellow_height/2);
    GeoTrf::TranslateZ3D cflange_pos(-wflange_height/2 - bellow_height - cflange_height/2);
    GeoTrf::TranslateZ3D coldbox1_pos(-wflange_height/2 - bellow_height - cflange_height - coldbox1_height/2);
    GeoTrf::TranslateZ3D coldbox2_pos(-wflange_height/2 - bellow_height - cflange_height - coldbox1_height - coldbox2_height/2);
    GeoTrf::Translate3D coldbox2hole_pos(
      0., hole_shift,
      -wflange_height/2 - bellow_height - cflange_height - coldbox1_height - coldbox2_height/2
    );
    GeoTrf::Translate3D coldbox3_pos(
      0., hole_shift,
      -wflange_height/2 - bellow_height - cflange_height - coldbox1_height - coldbox2_height - coldbox3_height/2
    );

    const GeoShape& FTenvelope = wflange->add(
      (*bellow) << bellow_pos
    ).add(
      (*cflange) << cflange_pos
    ).add(
      (*coldbox11) << coldbox1_pos
    ).add(
      (*coldbox21) << coldbox2_pos
    ).add(
      (*coldbox31) << coldbox3_pos
    );

    GeoLogVol* FTLV = new GeoLogVol(name + "Envelope",
      &FTenvelope,
      materialManager->getMaterial("std::Air")
    );
    GeoPhysVol* FTPV = new GeoPhysVol(FTLV);

    FTPV->add(wflangePV);
    GeoTransform *bellow_trf = new GeoTransform(bellow_pos);
    FTPV->add(bellow_trf);
    FTPV->add(bellowPV);
    FTPV->add(bellow_trf);
    FTPV->add(vcablesPV);
    GeoTransform *cflange_trf = new GeoTransform(cflange_pos);
    FTPV->add(cflange_trf);
    FTPV->add(cflangePV);
    GeoTransform *coldbox1_trf = new GeoTransform(coldbox1_pos);
    FTPV->add(coldbox1_trf);
    FTPV->add(coldbox1PV);
    GeoTransform *coldbox2_trf = new GeoTransform(coldbox2_pos);
    FTPV->add(coldbox2_trf);
    FTPV->add(coldbox2PV);
    GeoTransform *coldbox3_trf = new GeoTransform(coldbox3_pos);
    FTPV->add(coldbox3_trf);
    FTPV->add(coldbox3PV);

    const GeoMaterial* lar_mat = materialManager->getMaterial("std::LiquidArgon");
    GeoShape* lar1 = new GeoTube(0., coldbox1_Router - coldbox1_wall, coldbox1_height/2);
    GeoShape* lar2 = new GeoTube(0., hole_r, coldbox1_height);
    const GeoShape& lar = lar1->subtract((*lar2) << GeoTrf::TranslateY3D(hole_shift));
    GeoLogVol* larLV = new GeoLogVol(name + "LAr", &lar, lar_mat);
    GeoPhysVol* larPV = new GeoPhysVol(larLV);
    FTPV->add(coldbox1_trf);
    FTPV->add(larPV);

    const GeoMaterial *pigtail_mat = materialManager->getMaterial("LAr::FT::Pigtail");

    const double pth = (coldbox1_height + coldbox2_height + coldbox3_height) / 2;
    GeoTransform *pigtail_trf = new GeoTransform(
      GeoTrf::Translate3D(0, hole_shift,
      -wflange_height/2 - bellow_height - cflange_height - pth
    ));
    GeoShape* pigtail = new GeoTube(0., hole_r, pth);
    GeoLogVol* pigtailLV = new GeoLogVol(name + "Pigtails", pigtail, pigtail_mat);
    GeoPhysVol* pigtailPV = new GeoPhysVol(pigtailLV);
    FTPV->add(pigtail_trf);
    FTPV->add(pigtailPV);

    // total lenght should be 28.5 cm
    const double ocable_len = 10.*Gaudi::Units::cm;
    const double ocable_R = (1.1/2)*sqrt(1920*2.85)*Gaudi::Units::mm;
    const GeoMaterial* ocable_mat = materialManager->getMaterial("LAr::FT::Cable");
    GeoShape* ocable = new GeoTube(0., ocable_R, ocable_len / 2);
    GeoLogVol* ocableLV = new GeoLogVol("LAr::Endcap::FTCables", ocable, ocable_mat);
    GeoPhysVol* ocablePV = new GeoPhysVol(ocableLV);

    const double chimney_height = 277.*Gaudi::Units::mm;
    const double chimney_wall = 5.*Gaudi::Units::mm;
    const double chimney_Router = 0.5*351.*Gaudi::Units::mm;
    const GeoMaterial* chimney_mat = iron;
    GeoShape* chimney1 = new GeoTube(chimney_Router - chimney_wall, chimney_Router, chimney_height/2);
    const double ch_lowring_h = 30.*Gaudi::Units::mm;
    const double ch_lowring_r = 20.*Gaudi::Units::mm;
    GeoShape* chimney2 = new GeoTube(chimney_Router, chimney_Router + ch_lowring_r, ch_lowring_h/2);
    const double ch_upring_r = 5.*Gaudi::Units::mm;
    const double ch_upring_h = 20.*Gaudi::Units::mm;
    GeoShape* chimney3 = new GeoTube(bellow_Router, chimney_Router + ch_upring_r, ch_upring_h/2);
    const GeoShape& chimney = chimney1->add(
      (*chimney2) << GeoTrf::TranslateZ3D(-chimney_height/2 + ch_lowring_h/2)
    ).add(
      (*chimney3) << GeoTrf::TranslateZ3D(chimney_height/2 - ch_upring_h/2)
    );
    GeoLogVol* chimneyLV = new GeoLogVol("LAr::Endcap::FTChimney", &chimney, chimney_mat);
    GeoPhysVol* chimneyPV = new GeoPhysVol(chimneyLV);

    // todo: take cryostat parameters from DB
    const double cryo_Router = 2475.*Gaudi::Units::mm; // cryo warm wall outer radius
    const double z_pos = -(472 - 412 + 247)*Gaudi::Units::mm;
    const double r0 = cryo_Router + chimney_height + wflange_height/2;
    const double r1 = cryo_Router + chimney_height/2;
    const double r2 = cryo_Router + chimney_height + wflange_height + ocable_len/2;
    auto put1 = [&envelope, z_pos](GeoPhysVol *object, double r, double phi)
    {
      envelope->add(new GeoTransform(
        GeoTrf::Translate3D(r*cos(phi), r*sin(phi), z_pos) *
        GeoTrf::RotateX3D(90.*Gaudi::Units::deg) *
        GeoTrf::RotateY3D(phi + 90.*Gaudi::Units::deg)
      ));
      envelope->add(object);
    };

    auto put = [&put1, &FTPV, &chimneyPV, &ocablePV, r0, r1, r2](double phi)
    {
      put1(FTPV, r0, phi);
      put1(chimneyPV, r1, phi);
      put1(ocablePV, r2, phi);
    };

    const double dphi = 5.*Gaudi::Units::deg;
    for(unsigned int i{0}; i < LArEndcapCratePhiPos->size(); ++ i){
      const int num = (*LArEndcapCratePhiPos)[i]->getInt("CRATENUM");
      const double phi = (*LArEndcapCratePhiPos)[i]->getDouble("PHIPOS")*Gaudi::Units::deg;
      if(num == 10){ // the topmost crate has one FT, positioned assymetrically
          put(phi + dphi);
      } else {
          put(phi - dphi);
          put(phi + dphi);
      }
    }
  } else { // No FTs, old LArServices variant
/*
   S.M. Feb 2020: these services are located between FEC and TileFingers.
                  In this area is mostly air and a few cables. It was
                  probably intended to go below the FEC
*/
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
}

