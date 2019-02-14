/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileDetectorFactory.h"
#include "TileGeoSectionBuilder.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDddbManager.h"
#include "TileDetDescr/TileDetDescriptor.h"
#include "CaloIdentifier/TileID.h"

#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoModelKernel/GeoXF.h"
#include "GeoModelKernel/GeoSerialTransformer.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <stdexcept>
#include <iostream>

using namespace GeoGenfun;
using namespace GeoXF;
   

// Constructor:
TileDetectorFactory::TileDetectorFactory(StoreGateSvc *pDetStore,
                                         TileDetDescrManager *manager,
                                         bool addPlates, 
                                         int ushape,
                                         int glue,
                                         int cstube,
                                         MsgStream *log)
  : m_detectorStore(pDetStore)
  , m_detectorManager(manager)
  , m_log(log)
  , m_addPlatesToCellVolume(addPlates)
  , m_uShape(ushape)
  , m_glue(glue)
  , m_csTube(cstube)
  , m_testbeamGeometry(false)
  , m_verbose(log->level()<=MSG::VERBOSE)
{
}
  
// Destructor:
TileDetectorFactory::~TileDetectorFactory()
{
}
  
// Creation of geometry:
void TileDetectorFactory::create(GeoPhysVol *world) 
{
  (*m_log) << MSG::INFO <<" Entering TileDetectorFactory::create()" << endmsg;

  double dzGlue;     //Thickness of glue layer in the absorber
  double thicknessWedgeMother, heightWedgeMother, dy1WedgeMother, dy2WedgeMother;
  double zEndSection;

  int negSide = 0;
  int posSide = 1;

  const TileID* tileID = m_detectorManager->get_id();
  int nregion = 0;

  // -------- -------- MATERIAL MANAGER -------- ----------
  const StoredMaterialManager* theMaterialManager = nullptr;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(theMaterialManager, "MATERIALS")) 
  { 
    (*m_log) << MSG::ERROR << "Could not find Material Manager MATERIALS" << endmsg; 
    return; 
  }
  const GeoMaterial* matAir = theMaterialManager->getMaterial("std::Air");

  // -------- -------- SECTION BUILDER  -------- ----------
  TileDddbManager* dbManager = m_detectorManager->getDbManager();
  TileGeoSectionBuilder* sectionBuilder = new TileGeoSectionBuilder(theMaterialManager,dbManager,m_uShape,m_glue,m_csTube,m_log);

  // --------------- TILE  -------  TILE  --------- TILE ---------- TILE ------------
  // Envelope creation. 
  // Building three tree tops for standard setup and only one for commissioning
  GeoLogVol *lvTileEnvelopeBarrel = 0, *lvTileEnvelopePosEndcap = 0, *lvTileEnvelopeNegEndcap = 0;
  GeoPhysVol *pvTileEnvelopeBarrel = 0, *pvTileEnvelopePosEndcap = 0, *pvTileEnvelopeNegEndcap = 0;

  bool globalsExist = dbManager->GetNumberOfEnv()!=0;
  int ModuleNcp =0;

  if(dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL))
  {
    // Z planes
    dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
    double endCentralBarrel = dbManager->TILBdzmodul()/2.*Gaudi::Units::cm;
    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
    //sb double beginITC1 = (dbManager->TILBzoffset() + dbManager->TILEzshift() - dbManager->TILBdzmodul()/2.)*Gaudi::Units::cm;
    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
    double beginITC2 = (dbManager->TILBzoffset() + dbManager->TILEzshift() - dbManager->TILBdzmodul()/2.)*Gaudi::Units::cm;
    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
    double beginCrack = (dbManager->TILBzoffset() + dbManager->TILEzshift() - dbManager->TILBdzmodul()/2.)*Gaudi::Units::cm;
    double endCrack = (dbManager->TILBzoffset() + dbManager->TILEzshift() + dbManager->TILBdzmodul()/2.)*Gaudi::Units::cm;
    dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
    double endExtendedBarrel = (dbManager->TILBzoffset() + dbManager->TILEzshift() + dbManager->TILBdzmodul()/2.)*Gaudi::Units::cm;
    double endTile = dbManager->TILEzmam()*Gaudi::Units::cm;

    dbManager->SetCurrentTifg(1);
    double endBarrelFinger = endCentralBarrel + dbManager->TIFGdz()*Gaudi::Units::cm;

    // Offsets
    /* sb
    double shiftSec1 = (beginCrack + endCentralBarrel)/2.;
    double shiftSec2 = (endCrack + beginCrack)/2.;
    double shiftSec3 = (endExtendedBarrel + endCrack)/2.;
    double shiftSec4 = (endTile + endExtendedBarrel)/2.;
    */

    // R minimals
    dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
    double rminBarrel = dbManager->TILBrminimal()*Gaudi::Units::cm;
    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
    double rminITC1 = dbManager->TILBrminimal()*Gaudi::Units::cm;
    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
    double rminITC = dbManager->TILBrminimal()*Gaudi::Units::cm;
    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
    double rminCrack = dbManager->TILBrminimal()*Gaudi::Units::cm;
    dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
    double rminExtended = dbManager->TILBrminimal()*Gaudi::Units::cm;
    double rminFinger = dbManager->TILBrmax()*Gaudi::Units::cm;
    
    // R maximal
    double rmaxTotal = dbManager->TILErmam()*Gaudi::Units::cm;
    
    GeoPcon* tileEnvPconeBarrel    = new GeoPcon(0, 360*Gaudi::Units::deg);
    GeoPcon* tileEnvPconePosEndcap = new GeoPcon(0, 360*Gaudi::Units::deg);
    GeoPcon* tileEnvPconeNegEndcap = new GeoPcon(0, 360*Gaudi::Units::deg);

    // Negative Endcap
    tileEnvPconeNegEndcap->addPlane(-endTile,rminFinger,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-endExtendedBarrel,rminFinger,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-endExtendedBarrel,rminExtended,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-endCrack,rminExtended,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-endCrack,rminCrack,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-beginCrack,rminCrack,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-beginCrack,rminITC,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-beginITC2,rminITC,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-beginITC2,rminITC1,rmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-endBarrelFinger,rminITC1,rmaxTotal);
    // Barrel 

    tileEnvPconeBarrel->addPlane(-endBarrelFinger,rminFinger,rmaxTotal);
    tileEnvPconeBarrel->addPlane(-endCentralBarrel,rminFinger,rmaxTotal);
    tileEnvPconeBarrel->addPlane(-endCentralBarrel,rminBarrel,rmaxTotal);
    tileEnvPconeBarrel->addPlane(endCentralBarrel,rminBarrel,rmaxTotal);
    tileEnvPconeBarrel->addPlane(endCentralBarrel,rminFinger,rmaxTotal);
    tileEnvPconeBarrel->addPlane(endBarrelFinger,rminFinger,rmaxTotal);

    // Positive Endcap
    tileEnvPconePosEndcap->addPlane(endBarrelFinger,rminITC1,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(beginITC2,rminITC1,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(beginITC2,rminITC,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(beginCrack,rminITC,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(beginCrack,rminCrack,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(endCrack,rminCrack,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(endCrack,rminExtended,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(endExtendedBarrel,rminExtended,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(endExtendedBarrel,rminFinger,rmaxTotal);
    tileEnvPconePosEndcap->addPlane(endTile,rminFinger,rmaxTotal);

    lvTileEnvelopeBarrel = new GeoLogVol("TileCentralBarrel",tileEnvPconeBarrel,matAir);
    lvTileEnvelopePosEndcap = new GeoLogVol("TileEndcapPos",tileEnvPconePosEndcap,matAir);
    lvTileEnvelopeNegEndcap = new GeoLogVol("TileEndcapNeg",tileEnvPconeNegEndcap,matAir);

    pvTileEnvelopeBarrel = new GeoPhysVol(lvTileEnvelopeBarrel);
    pvTileEnvelopePosEndcap = new GeoPhysVol(lvTileEnvelopePosEndcap);
    pvTileEnvelopeNegEndcap = new GeoPhysVol(lvTileEnvelopeNegEndcap);

    /*
    const GeoShape& bbqEnvelope = ((*tileEnvPconeBarrel)<<GeoTrf::TranslateX3D(1000.)).
      add(((*tileEnvPconePosEndcap)<<GeoTrf::TranslateZ3D(0.))).
      add(((*tileEnvPconeNegEndcap)<<GeoTrf::TranslateZ3D(0.)));
    lvTileEnvelope = new GeoLogVol("Tile",&bbqEnvelope,matAir);
    pvTileEnvelope = new GeoPhysVol(lvTileEnvelope);

    pvTileEnvelope->add(new GeoTransform(GeoTrf::TranslateX3D(1000.)));
    pvTileEnvelope->add(pvTileEnvelopeBarrel);
    pvTileEnvelope->add(pvTileEnvelopePosEndcap);
    pvTileEnvelope->add(pvTileEnvelopeNegEndcap);
    */
  }
  else
  {
    dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
    dbManager->SetCurrentTifg(1);

    // Z planes
    double endCentralBarrel = dbManager->TILBdzmodul()/2.*Gaudi::Units::cm;
    double endEnvelope = endCentralBarrel + dbManager->TIFGdz()*Gaudi::Units::cm;

    // R minimals
    double rminBarrel = dbManager->TILBrminimal()*Gaudi::Units::cm;
    double rminFinger = dbManager->TILBrmax()*Gaudi::Units::cm;
    
    // R maximal
    double rmaxTotal = dbManager->TILErmam()*Gaudi::Units::cm;
    
    GeoPcon* tileEnvPconeBarrel = new GeoPcon(0, 360*Gaudi::Units::deg);
    tileEnvPconeBarrel->addPlane(-endEnvelope,rminFinger,rmaxTotal);
    tileEnvPconeBarrel->addPlane(-endCentralBarrel,rminFinger,rmaxTotal);
    tileEnvPconeBarrel->addPlane(-endCentralBarrel,rminBarrel,rmaxTotal);
    tileEnvPconeBarrel->addPlane(endCentralBarrel,rminBarrel,rmaxTotal);
    tileEnvPconeBarrel->addPlane(endCentralBarrel,rminFinger,rmaxTotal);
    tileEnvPconeBarrel->addPlane(endEnvelope,rminFinger,rmaxTotal);

    lvTileEnvelopeBarrel = new GeoLogVol("TileCentralBarrel",tileEnvPconeBarrel,matAir);
    pvTileEnvelopeBarrel = new GeoPhysVol(lvTileEnvelopeBarrel);
  }

  //-------------------------------- SECTIONS --------------------------------
  // Variables/functions for parameterizations

  double deltaPhi = 360./dbManager->TILEnmodul();

  Variable varInd;
  GENFUNCTION phiInd = deltaPhi*(varInd+0.5)*Gaudi::Units::deg;

  //------------------------------- B A R R E L --------------------------------------
  // Tube - barrel mother

  if(dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL))
  {

    GeoTube* barrelMother = new GeoTube(dbManager->TILBrminimal()*Gaudi::Units::cm,
					dbManager->TILErmam()*Gaudi::Units::cm,
					dbManager->TILBdzmodul()/2.*Gaudi::Units::cm);

    GeoLogVol* lvBarrelMother = new GeoLogVol("Barrel",barrelMother,matAir);
    GeoFullPhysVol* pvBarrelMother = new GeoFullPhysVol(lvBarrelMother);

    // Trd - module mother
    thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;

    dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - (dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()) - dbManager->TILBdzmast()))/(2.*(2.*dbManager->TILBnperiod() - 1));

    GeoTrd* barrelModuleMother = new GeoTrd(thicknessWedgeMother/2.,
					    thicknessWedgeMother/2.,
					    dy1WedgeMother,
					    dy2WedgeMother,
					    heightWedgeMother/2.);
    
    GeoLogVol* lvBarrelModuleMother = new GeoLogVol("BarrelModule",barrelModuleMother,matAir);
    GeoPhysVol* pvBarrelModuleMother = new GeoPhysVol(lvBarrelModuleMother);

    // Fill the section
    sectionBuilder->fillSection(pvBarrelModuleMother,
    				1,
    				dbManager->TILBrmaximal(),
    				dbManager->TILBrminimal(),
    				dzGlue,
    				deltaPhi);
/*
  // Position N barrel modules inside barrel mother
  for (j=0; j<dbManager->TILEnmodul(); j++)
  {
    phi = j*deltaPhi;

    GeoTransform* zrotateMod = new GeoTransform(GeoTrf::RotateZ3D(phi*Gaudi::Units::deg));
    GeoTransform* xtransMod = new GeoTransform(GeoTrf::TranslateX3D((dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2. * cm));
    GeoTransform* yrotateMod = new GeoTransform(GeoTrf::RotateY3D(90*Gaudi::Units::deg));

    pvBarrelMother->add(zrotateMod);
    pvBarrelMother->add(xtransMod);
    pvBarrelMother->add(yrotateMod);
    pvBarrelMother->add(pvBarrelModuleMother);
  }
*/

    // --- Using the parameterization -----
    TRANSFUNCTION xfBarrelModuleMother = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    GeoSerialTransformer* stBarrelModuleMother = new GeoSerialTransformer(pvBarrelModuleMother,
									  &xfBarrelModuleMother,
									  dbManager->TILEnmodul());

    pvBarrelMother->add(new GeoSerialIdentifier(1));
    pvBarrelMother->add(stBarrelModuleMother);
    
    GeoNameTag* ntBarrelMother = new GeoNameTag("TileBarrel");
    pvTileEnvelopeBarrel->add(ntBarrelMother);
    pvTileEnvelopeBarrel->add(pvBarrelMother);
  }

  int nModules;
  float zShift;
  
  if(globalsExist)
  {
    dbManager->SetCurrentEnvByType(1);
    nModules=dbManager->GetEnvNModules();
    zShift=dbManager->GetEnvZShift()*Gaudi::Units::cm;

    sectionBuilder->computeCellDim(m_detectorManager, 
                                   TILE_REGION_CENTRAL,
                                   m_addPlatesToCellVolume,
                                   zShift,  // shift for positive eta (normally zero)
                                   zShift); // shift for negative eta is the same
  } else 
  {
    nModules=dbManager->TILEnmodul();
    zShift=0.0;
    // do not compute cell volumes for old setups (before DC3), 
    // because cell-size table might be missing in DB
  }
  
  // -- Readout
  // Create two descriptor objects corresponding to positive and negative half of central barrel
  TileDetDescriptor* descriptor = new TileDetDescriptor();

  sectionBuilder->fillDescriptor(descriptor,
                                 TILE_REGION_CENTRAL,  
                                 negSide,
                                 m_testbeamGeometry,
                                 m_addPlatesToCellVolume,
                                 nModules,
                                 zShift);
    
  Identifier idRegion = tileID->region_id(nregion++);
  descriptor->set(idRegion);

  m_detectorManager->add(descriptor); 
  m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor));
  
  descriptor = new TileDetDescriptor();

  sectionBuilder->fillDescriptor(descriptor,
                                 TILE_REGION_CENTRAL,  
                                 posSide,
                                 m_testbeamGeometry,
                                 m_addPlatesToCellVolume,
                                 nModules,
                                 zShift);

  idRegion = tileID->region_id(nregion++);
  descriptor->set(idRegion);
  
  m_detectorManager->add(descriptor); 
  m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor));

  //-------------------------- E X T E N D E D   B A R R E L ---------------------------------
  // Tube - barrel mother

  if(dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL))
  {
  
    GeoTube* ebarrelMother = new GeoTube(dbManager->TILBrminimal()*Gaudi::Units::cm,
					 dbManager->TILErmam()*Gaudi::Units::cm,
					 dbManager->TILBdzmodul()/2.*Gaudi::Units::cm);

    GeoLogVol* lvEBarrelMother = new GeoLogVol("EBarrel",ebarrelMother,matAir);
    GeoFullPhysVol* pvEBarrelMotherPos = new GeoFullPhysVol(lvEBarrelMother);
    GeoFullPhysVol* pvEBarrelMotherNeg = new GeoFullPhysVol(lvEBarrelMother);

    // Trd - module mother
    thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;

    dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()))/(4.*dbManager->TILBnperiod());
    
    GeoTrd* ebarrelModuleMother = new GeoTrd(thicknessWedgeMother/2.,
					     thicknessWedgeMother/2.,
					     dy1WedgeMother,
					     dy2WedgeMother,
					     heightWedgeMother/2.);

    GeoLogVol* lvEBarrelModuleMother = new GeoLogVol("EBarrelModule",ebarrelModuleMother,matAir);
    GeoPhysVol* pvEBarrelModuleMother = new GeoPhysVol(lvEBarrelModuleMother);
    
    // Fill the section
    sectionBuilder->fillSection(pvEBarrelModuleMother,
    				2,
    				dbManager->TILBrmaximal(),
    				dbManager->TILBrminimal(),
    				dzGlue,
    				deltaPhi);

    // --- Position N modules inside mother (positive/negative) -----
    TRANSFUNCTION xfEBarrelModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    TRANSFUNCTION xfEBarrelModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);

    GeoSerialTransformer* stEBarrelModuleMotherPos = new GeoSerialTransformer(pvEBarrelModuleMother,
									      &xfEBarrelModuleMotherPos,
									      dbManager->TILEnmodul());

    pvEBarrelMotherPos->add(new GeoSerialIdentifier(1));						    
    pvEBarrelMotherPos->add(stEBarrelModuleMotherPos);


    GeoSerialTransformer* stEBarrelModuleMotherNeg = new GeoSerialTransformer(pvEBarrelModuleMother,
									      &xfEBarrelModuleMotherNeg,
									      dbManager->TILEnmodul());

    pvEBarrelMotherNeg->add(new GeoSerialIdentifier(1));						    
    pvEBarrelMotherNeg->add(stEBarrelModuleMotherNeg);


    GeoTransform* tfEBarrelMotherPos = new GeoTransform(GeoTrf::TranslateZ3D((dbManager->TILBzoffset()+dbManager->TILEzshift())*Gaudi::Units::cm));
    GeoNameTag* ntEBarrelMotherPos = new GeoNameTag("TileEBarrelPos");
    pvTileEnvelopePosEndcap->add(tfEBarrelMotherPos);
    pvTileEnvelopePosEndcap->add(ntEBarrelMotherPos);
    pvTileEnvelopePosEndcap->add(pvEBarrelMotherPos);

    
    GeoTransform* tfEBarrelMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D((-dbManager->TILBzoffset()-dbManager->TILEzshift())*Gaudi::Units::cm));
    GeoNameTag* ntEBarrelMotherNeg = new GeoNameTag("TileEBarrelNeg");
    pvTileEnvelopeNegEndcap->add(tfEBarrelMotherNeg);
    pvTileEnvelopeNegEndcap->add(ntEBarrelMotherNeg);
    pvTileEnvelopeNegEndcap->add(pvEBarrelMotherNeg);
  }

  int nModulesNeg,nModulesPos;
  float zShiftNeg,zShiftPos;
  
  if(globalsExist)
  {
    dbManager->SetCurrentEnvByType(2);
    nModulesNeg=dbManager->GetEnvNModules();
    zShiftNeg=dbManager->GetEnvZShift()*Gaudi::Units::cm;
    dbManager->SetCurrentEnvByType(3);
    nModulesPos=dbManager->GetEnvNModules();
    zShiftPos=dbManager->GetEnvZShift()*Gaudi::Units::cm;

    sectionBuilder->computeCellDim(m_detectorManager,
                                   TILE_REGION_EXTENDED,
                                   m_addPlatesToCellVolume,
                                   zShiftPos,  // shift for positive eta
                                   zShiftNeg); // shift for negative eta
  } else
  {
    nModulesPos=nModulesNeg=dbManager->TILEnmodul();
    zShiftPos=zShiftNeg=dbManager->TILEzshift()*Gaudi::Units::cm;
    // do not compute cell volumes for old setups (before DC3), 
    // because cell-size table might be missing in DB
  }
  
  // -- Readout
  // Create two descriptor objects corresponding to positive and negative half of extended barrel
  descriptor = new TileDetDescriptor();

  sectionBuilder->fillDescriptor(descriptor,
                                 TILE_REGION_EXTENDED,   
                                 negSide,
                                 m_testbeamGeometry,
                                 m_addPlatesToCellVolume,
                                 nModulesNeg,
                                 zShiftNeg);
  
  idRegion = tileID->region_id(nregion++);
  descriptor->set(idRegion);
  
  m_detectorManager->add(descriptor); 
  m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor));
  
  descriptor = new TileDetDescriptor();

  sectionBuilder->fillDescriptor(descriptor,
                                 TILE_REGION_EXTENDED,   
                                 posSide,
                                 m_testbeamGeometry,
                                 m_addPlatesToCellVolume,
                                 nModulesPos,
                                 zShiftPos);
  
  idRegion = tileID->region_id(nregion++);
  descriptor->set(idRegion);
  
  m_detectorManager->add(descriptor); 
  m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor));

  //-------------------------- I T C ---------------------------------
  // Tube - barrel mother

  if(dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2))
  {
    //sb double rMinITC = dbManager->TILBrminimal();
    double rMinITC2 = dbManager->TILBrminimal();
    double rMaxITC2 = dbManager->TILBrmaximal();
    double dzITC2 = dbManager->TILBdzmodul();

    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);

    GeoTube* itcWheel1 = new GeoTube(dbManager->TILBrminimal()*Gaudi::Units::cm,
				     dbManager->TILErmam()*Gaudi::Units::cm,
				     dbManager->TILBdzmodul()/2.*Gaudi::Units::cm);

    GeoTube* itcWheel2 = new GeoTube(rMinITC2*Gaudi::Units::cm,rMaxITC2*Gaudi::Units::cm,dzITC2/2.*Gaudi::Units::cm);
    GeoTrf::Translate3D itcWheel2OffsetPos(0.,0.,(dbManager->TILBdzmodul()-dzITC2)/2*Gaudi::Units::cm);
    GeoTrf::Translate3D itcWheel2OffsetNeg(0.,0.,(-dbManager->TILBdzmodul()+dzITC2)/2*Gaudi::Units::cm);

    const GeoShapeUnion& itcMotherPos = itcWheel1->add(*itcWheel2<<itcWheel2OffsetPos);
    const GeoShapeUnion& itcMotherNeg = itcWheel1->add(*itcWheel2<<itcWheel2OffsetNeg);

    GeoLogVol* lvITCMotherPos = new GeoLogVol("ITC",&itcMotherPos,matAir);
    GeoLogVol* lvITCMotherNeg = new GeoLogVol("ITC",&itcMotherNeg,matAir);
    GeoFullPhysVol* pvITCMotherPos = new GeoFullPhysVol(lvITCMotherPos);
    GeoFullPhysVol* pvITCMotherNeg = new GeoFullPhysVol(lvITCMotherNeg);


    // Common mother for ITC1/2 modules

    // -- first sub shape
    thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrminimal()* tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILBrmaximal()* tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;

    GeoTrd* itcModuleSub1 = new GeoTrd(thicknessWedgeMother/2.,
				       thicknessWedgeMother/2.,
				       dy1WedgeMother,
				       dy2WedgeMother,
				       heightWedgeMother/2.);

    // -- second sub shape
    thicknessWedgeMother = dzITC2 * Gaudi::Units::cm;
    heightWedgeMother = (rMaxITC2 - rMinITC2) * Gaudi::Units::cm;
    dy1WedgeMother = rMinITC2* tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = rMaxITC2* tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;

    GeoTrd* itcModuleSub2 = new GeoTrd(thicknessWedgeMother/2.,
				       thicknessWedgeMother/2.,
				       dy1WedgeMother,
				       dy2WedgeMother,
				       heightWedgeMother/2.);

    GeoTrf::Translate3D itcModuleSubShift ((dbManager->TILBdzmodul()-dzITC2)/2*Gaudi::Units::cm,
				      0.,
				      ((rMinITC2+rMaxITC2)-(dbManager->TILBrmaximal()+dbManager->TILBrminimal()))/2.*Gaudi::Units::cm);

    const GeoShapeUnion& itcModuleMother = itcModuleSub1->add(*itcModuleSub2<<itcModuleSubShift);

    GeoLogVol* lvITCModuleMother = new GeoLogVol("ITCModule",&itcModuleMother,matAir);
    GeoPhysVol* pvITCModuleMother = new GeoPhysVol(lvITCModuleMother);

    //Mother volume for ITC1
    //In plug1 it's necessary to produce GeoShapeUnion for mother volume that is composed by two parts:
    // 1. Mother for absorber and girder
    // 2. Mother for frontplate (since it's short)
    
    //First submother
    thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrmin()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    
    dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()))/(4.*dbManager->TILBnperiod());
    
    GeoTrd* plug1SubMother = new GeoTrd(thicknessWedgeMother/2.,
					thicknessWedgeMother/2.,
					dy1WedgeMother,
					dy2WedgeMother,
					heightWedgeMother/2.);

    //Second submother
    thicknessWedgeMother = (dbManager->TILBdzmodul() - dzITC2) * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILBrmin() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    
    GeoTrd* plug2SubMother = new GeoTrd(thicknessWedgeMother/2.,
					thicknessWedgeMother/2.,
					dy1WedgeMother,
					dy2WedgeMother,
					heightWedgeMother/2.);
    
    GeoTrf::Translate3D plug1SubOffset(-dzITC2*Gaudi::Units::cm/2.,
				  0.,
				  (dbManager->TILBrminimal()-dbManager->TILBrmaximal())*Gaudi::Units::cm/2.);
    
    const GeoShapeUnion& plug1ModuleMother = plug1SubMother->add(*plug2SubMother<<plug1SubOffset);
    GeoLogVol* lvPlug1ModuleMother = new GeoLogVol("Plug1Module",&plug1ModuleMother,matAir);
    GeoPhysVol* pvPlug1ModuleMother = new GeoPhysVol(lvPlug1ModuleMother);
    
    // Fill the section
    sectionBuilder->fillSection(pvPlug1ModuleMother, 3,
    				dbManager->TILBrmaximal(),
    				dbManager->TILBrminimal(),
    				dzGlue,
    				deltaPhi, ModuleNcp,
    				dzITC2);
    
    GeoTransform* tfPlug1ModuleMother = new GeoTransform(GeoTrf::Translate3D(0.,
									0.,
									(dbManager->TILBrmin()-dbManager->TILBrminimal())*Gaudi::Units::cm/2.));
    
    pvITCModuleMother->add(tfPlug1ModuleMother);
    pvITCModuleMother->add(pvPlug1ModuleMother);
    
    //Mother volume for ITC2
    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
    
    thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;

    dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - ((dbManager->TILBnperiod()-1)*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()) + dbManager->TILBdzspac()))/(4.*(dbManager->TILBnperiod() - 1));
    
    GeoTrd* plug2ModuleMother = new GeoTrd(thicknessWedgeMother/2.,
					   thicknessWedgeMother/2.,
					   dy1WedgeMother,
					   dy2WedgeMother,
					   heightWedgeMother/2.);
    
    GeoLogVol* lvPlug2ModuleMother = new GeoLogVol("Plug2Module",plug2ModuleMother,matAir);
    GeoPhysVol* pvPlug2ModuleMother = new GeoPhysVol(lvPlug2ModuleMother);
    
    // Fill the section
    sectionBuilder->fillSection(pvPlug2ModuleMother,
    				4,
    				dbManager->TILBrmaximal(),
    				dbManager->TILBrminimal(),
    				dzGlue,
    				deltaPhi);
    
    dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
    
    GeoTransform* tfPlug2ModuleMother = new GeoTransform(itcModuleSubShift);

    pvITCModuleMother->add(tfPlug2ModuleMother);
    pvITCModuleMother->add(pvPlug2ModuleMother);

    // --- Position N modules inside mother (positive/negative) -----
    TRANSFUNCTION xfITCModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    TRANSFUNCTION xfITCModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);

    GeoSerialTransformer* stITCModuleMotherPos = new GeoSerialTransformer(pvITCModuleMother,
									  &xfITCModuleMotherPos,
									  dbManager->TILEnmodul());
    pvITCMotherPos->add(new GeoSerialIdentifier(1));
    pvITCMotherPos->add(stITCModuleMotherPos);
    
    GeoSerialTransformer* stITCModuleMotherNeg = new GeoSerialTransformer(pvITCModuleMother,
									  &xfITCModuleMotherNeg,
									  dbManager->TILEnmodul());
    pvITCMotherNeg->add(new GeoSerialIdentifier(1));
    pvITCMotherNeg->add(stITCModuleMotherNeg);
    
    
    GeoTransform* tfITCMotherPos = new GeoTransform(GeoTrf::TranslateZ3D((dbManager->TILBzoffset()+dbManager->TILEzshift())*Gaudi::Units::cm));
    GeoNameTag* ntITCMotherPos = new GeoNameTag("TileITCPos");
    pvTileEnvelopePosEndcap->add(tfITCMotherPos);
    pvTileEnvelopePosEndcap->add(ntITCMotherPos);
    pvTileEnvelopePosEndcap->add(pvITCMotherPos);

    
    GeoTransform* tfITCMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D((-dbManager->TILBzoffset()-dbManager->TILEzshift())*Gaudi::Units::cm));
    GeoNameTag* ntITCMotherNeg = new GeoNameTag("TileITCNeg");
    pvTileEnvelopeNegEndcap->add(tfITCMotherNeg);
    pvTileEnvelopeNegEndcap->add(ntITCMotherNeg);
    pvTileEnvelopeNegEndcap->add(pvITCMotherNeg);
  }
  //-------------------------- G A P ---------------------------------
  // Tube - gap mother

  if(dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3))
  {
    GeoTube* gapMother = new GeoTube(dbManager->TILBrminimal()*Gaudi::Units::cm,
				     dbManager->TILBrmaximal()*Gaudi::Units::cm/cos(deltaPhi/2.*Gaudi::Units::deg),
				     dbManager->TILBdzmodul()/2.*Gaudi::Units::cm);

    GeoLogVol* lvGapMother = new GeoLogVol("Gap",gapMother,matAir);
    GeoFullPhysVol* pvGapMotherPos = new GeoFullPhysVol(lvGapMother);
    GeoFullPhysVol* pvGapMotherNeg = new GeoFullPhysVol(lvGapMother);

    // Trd - module mother
    thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;

    dzGlue = 0.;

    GeoTrd* gapModuleMother = new GeoTrd(thicknessWedgeMother/2.,
					 thicknessWedgeMother/2.,
					 dy1WedgeMother,
					 dy2WedgeMother,
					 heightWedgeMother/2.);

    GeoLogVol* lvGapModuleMother = new GeoLogVol("GapModule",gapModuleMother,matAir);
    GeoPhysVol* pvGapModuleMother = new GeoPhysVol(lvGapModuleMother);

    // Fill the section
    sectionBuilder->fillSection(pvGapModuleMother,
    				5,
    				dbManager->TILBrmaximal(),
    				dbManager->TILBrminimal(),
    				dzGlue,
    				deltaPhi);

    // --- Position N modules inside mother (positive/negative) -----
    TRANSFUNCTION xfGapModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    TRANSFUNCTION xfGapModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    
    GeoSerialTransformer* stGapModuleMotherPos = new GeoSerialTransformer(pvGapModuleMother,
									  &xfGapModuleMotherPos,
									  dbManager->TILEnmodul());
    pvGapMotherPos->add(new GeoSerialIdentifier(1));
    pvGapMotherPos->add(stGapModuleMotherPos);
    
    GeoSerialTransformer* stGapModuleMotherNeg = new GeoSerialTransformer(pvGapModuleMother,
									  &xfGapModuleMotherNeg,
									  dbManager->TILEnmodul());
    pvGapMotherNeg->add(new GeoSerialIdentifier(1));
    pvGapMotherNeg->add(stGapModuleMotherNeg);

    
    GeoTransform* tfGapMotherPos = new GeoTransform(GeoTrf::TranslateZ3D((dbManager->TILBzoffset()+dbManager->TILEzshift())*Gaudi::Units::cm));
    GeoNameTag* ntGapMotherPos = new GeoNameTag("TileGapPos");
    pvTileEnvelopePosEndcap->add(tfGapMotherPos);
    pvTileEnvelopePosEndcap->add(ntGapMotherPos);
    pvTileEnvelopePosEndcap->add(pvGapMotherPos);
    
    
    GeoTransform* tfGapMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D((-dbManager->TILBzoffset()-dbManager->TILEzshift())*Gaudi::Units::cm));
    GeoNameTag* ntGapMotherNeg = new GeoNameTag("TileGapNeg");
    pvTileEnvelopeNegEndcap->add(tfGapMotherNeg);
    pvTileEnvelopeNegEndcap->add(ntGapMotherNeg);
    pvTileEnvelopeNegEndcap->add(pvGapMotherNeg);
  }

  if(globalsExist)
  {
    dbManager->SetCurrentEnvByType(4);
    nModulesNeg=dbManager->GetEnvNModules();
    zShiftNeg=dbManager->GetEnvZShift()*Gaudi::Units::cm;
    dbManager->SetCurrentEnvByType(5);
    nModulesPos=dbManager->GetEnvNModules();
    zShiftPos=dbManager->GetEnvZShift()*Gaudi::Units::cm;

    sectionBuilder->computeCellDim(m_detectorManager,
                                   TILE_REGION_GAP,
                                   m_addPlatesToCellVolume,
                                   zShiftPos,  // shift for positive eta
                                   zShiftNeg); // shift for negative eta
  } else
  {
    nModulesPos=nModulesNeg=dbManager->TILEnmodul();
    zShiftPos=zShiftNeg=dbManager->TILEzshift()*Gaudi::Units::cm;
    // do not compute cell volumes for old setups (before DC3), 
    // because cell-size table might be missing in DB
  }
  
  // -- Readout
  // Create two descriptor objects corresponding to positive and negative half of gap/crack
  descriptor = new TileDetDescriptor();

  sectionBuilder->fillDescriptor(descriptor,
                                 TILE_REGION_GAP,
                                 negSide,
                                 m_testbeamGeometry,
                                 m_addPlatesToCellVolume,
                                 nModulesNeg,
                                 zShiftNeg);
  
  idRegion = tileID->region_id(nregion++);
  descriptor->set(idRegion);

  m_detectorManager->add(descriptor); 
  m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor));

  descriptor = new TileDetDescriptor();

  sectionBuilder->fillDescriptor(descriptor,
                                 TILE_REGION_GAP,
                                 posSide,
                                 m_testbeamGeometry,
                                 m_addPlatesToCellVolume,
                                 nModulesPos,
                                 zShiftPos);

  idRegion = tileID->region_id(nregion++);
  descriptor->set(idRegion);

  m_detectorManager->add(descriptor); 
  m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor));

  //-------------------------- C R A C K ---------------------------------
  // Tube - crack mother
  
  if(dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4))
  {
    GeoTube* crackMother = new GeoTube(dbManager->TILBrminimal()*Gaudi::Units::cm,
				       dbManager->TILBrmaximal()*Gaudi::Units::cm/cos(deltaPhi/2.*Gaudi::Units::deg),
				       dbManager->TILBdzmodul()/2.*Gaudi::Units::cm);
    
    GeoLogVol* lvCrackMother = new GeoLogVol("Crack",crackMother,matAir);
    GeoFullPhysVol* pvCrackMotherPos = new GeoFullPhysVol(lvCrackMother);
    GeoFullPhysVol* pvCrackMotherNeg = new GeoFullPhysVol(lvCrackMother);
    
    // Trd - module mother
    thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    
    dzGlue = 0.;
    
    GeoTrd* crackModuleMother = new GeoTrd(thicknessWedgeMother/2.,
					   thicknessWedgeMother/2.,
					   dy1WedgeMother,
					   dy2WedgeMother,
					   heightWedgeMother/2.);
    
    GeoLogVol* lvCrackModuleMother = new GeoLogVol("CrackModule",crackModuleMother,matAir);
    GeoPhysVol* pvCrackModuleMother = new GeoPhysVol(lvCrackModuleMother);
    
    // Fill the section
    sectionBuilder->fillSection(pvCrackModuleMother,
    				6,
    				dbManager->TILBrmaximal(),
    				dbManager->TILBrminimal(),
    				dzGlue,
    				deltaPhi);
    
    // --- Position N modules inside mother (positive/negative) -----
    TRANSFUNCTION xfCrackModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    TRANSFUNCTION xfCrackModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    
    GeoSerialTransformer* stCrackModuleMotherPos = new GeoSerialTransformer(pvCrackModuleMother,
									    &xfCrackModuleMotherPos,
									    dbManager->TILEnmodul());
    pvCrackMotherPos->add(new GeoSerialIdentifier(1));
    pvCrackMotherPos->add(stCrackModuleMotherPos);
    
    GeoSerialTransformer* stCrackModuleMotherNeg = new GeoSerialTransformer(pvCrackModuleMother,
									    &xfCrackModuleMotherNeg,
									    dbManager->TILEnmodul());
    pvCrackMotherNeg->add(new GeoSerialIdentifier(1));
    pvCrackMotherNeg->add(stCrackModuleMotherNeg);
    
    
    GeoTransform* tfCrackMotherPos = new GeoTransform(GeoTrf::TranslateZ3D((dbManager->TILBzoffset()+dbManager->TILEzshift())*Gaudi::Units::cm));
    GeoNameTag* ntCrackMotherPos = new GeoNameTag("TileCrackPos");
    pvTileEnvelopePosEndcap->add(tfCrackMotherPos);
    pvTileEnvelopePosEndcap->add(ntCrackMotherPos);
    pvTileEnvelopePosEndcap->add(pvCrackMotherPos);
    
    
    GeoTransform* tfCrackMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D((-dbManager->TILBzoffset()-dbManager->TILEzshift())*Gaudi::Units::cm));
    GeoNameTag* ntCrackMotherNeg = new GeoNameTag("TileCrackNeg");
    pvTileEnvelopeNegEndcap->add(tfCrackMotherNeg);
    pvTileEnvelopeNegEndcap->add(ntCrackMotherNeg);
    pvTileEnvelopeNegEndcap->add(pvCrackMotherNeg);
  }

  //-------------------------------- FINGERS --------------------------------

  //------------------------- B A R R E L  F I N G E R  ----------------------------
  // Tube - finger mother

  if(dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL) &&
     dbManager->SetCurrentTifg(1))
  {

    zEndSection = dbManager->TILBzoffset() + dbManager->TILBdzmodul()/2.;

    GeoTube* fingerMother = new GeoTube(dbManager->TILBrmax()*Gaudi::Units::cm,
					dbManager->TILErmam()*Gaudi::Units::cm,
					dbManager->TIFGdz()/2.*Gaudi::Units::cm);
    
    GeoLogVol* lvFingerMother = new GeoLogVol("Finger",fingerMother,matAir);
    GeoFullPhysVol* pvFingerMotherPos = new GeoFullPhysVol(lvFingerMother);
    GeoFullPhysVol* pvFingerMotherNeg = new GeoFullPhysVol(lvFingerMother);
    
    // Trd - one finger mother
    //    if(dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1))
    //      thicknessWedgeMother = (dbManager->TILBzoffset() - dbManager->TILBdzmodul()/2. + dbManager->TILEzshift() - zEndSection) * Gaudi::Units::cm;
    //    else
    thicknessWedgeMother = dbManager->TIFGdz()*Gaudi::Units::cm;

    dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
    heightWedgeMother = (dbManager->TILErmax() - dbManager->TILBrmax()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILErmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    
    GeoTrd* fingerModuleMother = new GeoTrd(thicknessWedgeMother/2.,
					    thicknessWedgeMother/2.,
					    dy1WedgeMother,
					    dy2WedgeMother,
					    heightWedgeMother/2.);
    
    GeoLogVol* lvFingerModuleMother = new GeoLogVol("FingerModule",fingerModuleMother,matAir);
    GeoPhysVol* pvFingerModuleMother = new GeoPhysVol(lvFingerModuleMother);

    // Fill the section
    sectionBuilder->fillFinger(pvFingerModuleMother, 1,
			       dbManager->TILErmax(),
			       dbManager->TILBrmax(),
			       deltaPhi,
			       m_testbeamGeometry,
			       ModuleNcp,
			       thicknessWedgeMother*(1./Gaudi::Units::cm));
    
    // --- Position N modules inside mother (positive/negative) -----
    TRANSFUNCTION xfFingerModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILErmax()+dbManager->TILBrmax())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    TRANSFUNCTION xfFingerModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILErmax()+dbManager->TILBrmax())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);

    GeoSerialTransformer* stFingerModuleMotherPos = new GeoSerialTransformer(pvFingerModuleMother,
									     &xfFingerModuleMotherPos,
									     dbManager->TILEnmodul());

    pvFingerMotherPos->add(new GeoSerialIdentifier(1));
    pvFingerMotherPos->add(stFingerModuleMotherPos);
  
    GeoSerialTransformer* stFingerModuleMotherNeg = new GeoSerialTransformer(pvFingerModuleMother,
									     &xfFingerModuleMotherNeg,
									     dbManager->TILEnmodul());
    pvFingerMotherNeg->add(new GeoSerialIdentifier(1));
    pvFingerMotherNeg->add(stFingerModuleMotherNeg);
    
    
    GeoTransform* tfFingerMotherPos = new GeoTransform(GeoTrf::TranslateZ3D((zEndSection+dbManager->TIFGdz()/2.)*Gaudi::Units::cm));
    GeoNameTag* ntFingerMotherPos = new GeoNameTag("TileFingerPos");
    pvTileEnvelopeBarrel->add(tfFingerMotherPos);
    pvTileEnvelopeBarrel->add(ntFingerMotherPos);
    pvTileEnvelopeBarrel->add(pvFingerMotherPos);

    
    GeoTransform* tfFingerMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D((-zEndSection-dbManager->TIFGdz()/2.)*Gaudi::Units::cm));
    GeoNameTag* ntFingerMotherNeg = new GeoNameTag("TileFingerNeg");
    pvTileEnvelopeBarrel->add(tfFingerMotherNeg);
    pvTileEnvelopeBarrel->add(ntFingerMotherNeg);
    pvTileEnvelopeBarrel->add(pvFingerMotherNeg);
  }

  //------------------------- E X T E N D E D  F I N G E R  ----------------------------
  // Tube - finger mother

  if(dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL) &&
     dbManager->SetCurrentTifg(2))
  {

    zEndSection = dbManager->TILBzoffset() + dbManager->TILBdzmodul()/2. + dbManager->TILEzshift();

    GeoTube* efingerMother = new GeoTube(dbManager->TILBrmax()*Gaudi::Units::cm,
					 dbManager->TILErmam()*Gaudi::Units::cm,
					 dbManager->TIFGdz()/2.*Gaudi::Units::cm);

    GeoLogVol* lvEFingerMother = new GeoLogVol("EFinger",efingerMother,matAir);
    GeoFullPhysVol* pvEFingerMotherPos = new GeoFullPhysVol(lvEFingerMother);
    GeoFullPhysVol* pvEFingerMotherNeg = new GeoFullPhysVol(lvEFingerMother);
    
    // Trd - one finger mother
    thicknessWedgeMother = dbManager->TIFGdz() * Gaudi::Units::cm;
    heightWedgeMother = (dbManager->TILErmax() - dbManager->TILBrmax()) * Gaudi::Units::cm;
    dy1WedgeMother = dbManager->TILBrmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    dy2WedgeMother = dbManager->TILErmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
    
    GeoTrd* efingerModuleMother = new GeoTrd(thicknessWedgeMother/2.,
					     thicknessWedgeMother/2.,
					     dy1WedgeMother,
					     dy2WedgeMother,
					     heightWedgeMother/2.);

    GeoLogVol* lvEFingerModuleMother = new GeoLogVol("EFingerModule",efingerModuleMother,matAir);
    GeoPhysVol* pvEFingerModuleMother = new GeoPhysVol(lvEFingerModuleMother);

    // Fill the section
    sectionBuilder->fillFinger(pvEFingerModuleMother,
			       2,
			       dbManager->TILErmax(),
			       dbManager->TILBrmax(),
			       deltaPhi,
			       m_testbeamGeometry);

    // --- Position N modules inside mother (positive/negative) -----
    TRANSFUNCTION xfEFingerModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILErmax()+dbManager->TILBrmax())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    TRANSFUNCTION xfEFingerModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILErmax()+dbManager->TILBrmax())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    
    GeoSerialTransformer* stEFingerModuleMotherPos = new GeoSerialTransformer(pvEFingerModuleMother,
									      &xfEFingerModuleMotherPos,
									      dbManager->TILEnmodul());
    pvEFingerMotherPos->add(new GeoSerialIdentifier(1));
    pvEFingerMotherPos->add(stEFingerModuleMotherPos);
    
    GeoSerialTransformer* stEFingerModuleMotherNeg = new GeoSerialTransformer(pvEFingerModuleMother,
									      &xfEFingerModuleMotherNeg,
									      dbManager->TILEnmodul());
    pvEFingerMotherNeg->add(new GeoSerialIdentifier(1));
    pvEFingerMotherNeg->add(stEFingerModuleMotherNeg);
    
    
    GeoTransform* tfEFingerMotherPos = new GeoTransform(GeoTrf::TranslateZ3D((zEndSection+dbManager->TIFGdz()/2.)*Gaudi::Units::cm));
    GeoNameTag* ntEFingerMotherPos = new GeoNameTag("TileEFingerPos");
    pvTileEnvelopePosEndcap->add(tfEFingerMotherPos);
    pvTileEnvelopePosEndcap->add(ntEFingerMotherPos);
    pvTileEnvelopePosEndcap->add(pvEFingerMotherPos);

    
    GeoTransform* tfEFingerMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D((-zEndSection-dbManager->TIFGdz()/2.)*Gaudi::Units::cm));
    GeoNameTag* ntEFingerMotherNeg = new GeoNameTag("TileEFingerNeg");
    pvTileEnvelopeNegEndcap->add(tfEFingerMotherNeg);
    pvTileEnvelopeNegEndcap->add(ntEFingerMotherNeg);
    pvTileEnvelopeNegEndcap->add(pvEFingerMotherNeg);
  }

  // --------- ----------- --------- -------- ------ --------- ------- ---------- 
  GeoNameTag *nTag = new GeoNameTag("Tile");

  if(pvTileEnvelopeBarrel)
  {
    world->add(nTag);

    if(globalsExist)
    {
      // Top transform for the central barrel
      dbManager->SetCurrentEnvByType(1);
      GeoTrf::Transform3D mz = GeoTrf::RotateZ3D(dbManager->GetEnvDPhi());
      GeoTrf::Transform3D my = GeoTrf::RotateY3D(dbManager->GetEnvDTheta());
      GeoTrf::Transform3D mx = GeoTrf::RotateZ3D(dbManager->GetEnvDPsi());
      GeoTrf::Transform3D vpos = GeoTrf::Translate3D(dbManager->GetEnvDX()*Gaudi::Units::cm,dbManager->GetEnvDY()*Gaudi::Units::cm,dbManager->GetEnvDZ()*Gaudi::Units::cm);
      GeoTransform* barrelTT = new GeoTransform(GeoTrf::Transform3D(vpos*(mx*(my*(mz)))));
      world->add(barrelTT);
    }

    world->add(pvTileEnvelopeBarrel);
    m_detectorManager->addTreeTop(pvTileEnvelopeBarrel);
  }

  if(pvTileEnvelopePosEndcap)
  {
    world->add(nTag);

    if(globalsExist)
    {
      // Top transform for the positive endcap
      dbManager->SetCurrentEnvByType(3);
      GeoTrf::Transform3D mz = GeoTrf::RotateZ3D(dbManager->GetEnvDPhi());
      GeoTrf::Transform3D my = GeoTrf::RotateY3D(dbManager->GetEnvDTheta());
      GeoTrf::Transform3D mx = GeoTrf::RotateZ3D(dbManager->GetEnvDPsi());
      GeoTrf::Transform3D vpos = GeoTrf::Translate3D(dbManager->GetEnvDX()*Gaudi::Units::cm,dbManager->GetEnvDY()*Gaudi::Units::cm,dbManager->GetEnvDZ()*Gaudi::Units::cm);
      GeoTransform* posEcTT = new GeoTransform(GeoTrf::Transform3D(vpos*(mx*(my*(mz)))));
      world->add(posEcTT);
    }

    world->add(pvTileEnvelopePosEndcap);
    m_detectorManager->addTreeTop(pvTileEnvelopePosEndcap);
  }

  if(pvTileEnvelopeNegEndcap)
  {
    world->add(nTag);

    if(globalsExist)
    {
      // Top transform for the positive endcap
      dbManager->SetCurrentEnvByType(2);
      GeoTrf::Transform3D mz = GeoTrf::RotateZ3D(dbManager->GetEnvDPhi());
      GeoTrf::Transform3D my = GeoTrf::RotateY3D(dbManager->GetEnvDTheta());
      GeoTrf::Transform3D mx = GeoTrf::RotateZ3D(dbManager->GetEnvDPsi());
      GeoTrf::Transform3D vpos = GeoTrf::Translate3D(dbManager->GetEnvDX()*Gaudi::Units::cm,dbManager->GetEnvDY()*Gaudi::Units::cm,dbManager->GetEnvDZ()*Gaudi::Units::cm);
      GeoTransform* negEcTT = new GeoTransform(GeoTrf::Transform3D(vpos*(mx*(my*(mz)))));
      world->add(negEcTT);
    }

    world->add(pvTileEnvelopeNegEndcap);
    m_detectorManager->addTreeTop(pvTileEnvelopeNegEndcap);
  }

  delete sectionBuilder;
}

