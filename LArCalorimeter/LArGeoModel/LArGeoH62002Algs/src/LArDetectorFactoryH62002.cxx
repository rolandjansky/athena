/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorFactoryH62002.h"
#include "HECConstructionH62002.h"
#include "TableConstructionH62002.h"
#include "FrontBeamConstructionH62002.h"
#include "ExcluderConstruction.h"
#include "LArGeoH6Cryostats/H6CryostatConstruction.h"
#include "LArGeoH6Cryostats/WallsConstruction.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"

#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoXF.h"  
#include "GeoModelKernel/GeoSerialDenominator.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "StoreGate/StoreGateSvc.h"

// need if we want to use EMEC from LArGeoEndcap:
#include "LArGeoEndcap/EMECConstruction.h"
#include "LArGeoEndcap/EndcapPresamplerConstruction.h"

// For run options :
#include "LArG4RunControl/LArGeoTBH1GeoOptions.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/EMECDetDescr.h"

#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetDescr.h"
#include "GeoModelKernel/CellBinning.h"
#include "LArReadoutGeometry/HECLongBlock.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"

// V.N :  Patch LAr materials
#include "LArGeoCode/LArMaterialManager.h"
//

using namespace GeoGenfun;
using namespace GeoXF;





//LArDetectorFactoryH62002::LArDetectorFactoryH62002(StoreGateSvc *detStore)
LArGeo::LArDetectorFactoryH62002::LArDetectorFactoryH62002(StoreGateSvc *detStore)
  : log(NULL),
    m_cryoXpos(0),
    m_tableYpos(0),
    m_detectorStore(detStore),
    m_detectorManager(0)
{
}


LArGeo::LArDetectorFactoryH62002::~LArDetectorFactoryH62002()
{
}


void LArGeo::LArDetectorFactoryH62002::getSimulationParameters()
{

  StoreGateSvc* detStore;
  const LArGeoTBH1GeoOptions      *largeoTBH1geoOptions ;
  StatusCode status;

  ISvcLocator* svcLocator = Gaudi::svcLocator(); 
  IMessageSvc * msgSvc;


  status = svcLocator->service("MessageSvc", msgSvc);

  if(!status.isFailure()){
    log = new MsgStream(msgSvc, "LArDetectorFactory");
  } else {
    throw std::runtime_error("LArDetectorFactory: cannot initialze message service");
  }

 




  status = svcLocator->service("DetectorStore", detStore);


  if( status.isSuccess() ) {
 

  m_cryoXpos = 0.;
  m_tableYpos = 0.;

  status = detStore->retrieve(largeoTBH1geoOptions, "LArGeoTBH1GeoOptions");
  if ( !status.isFailure() ) {
    m_cryoXpos = largeoTBH1geoOptions->CryoXPosition();
    m_tableYpos = largeoTBH1geoOptions->TableYPosition();
  }
  else
   (*log)<< MSG::WARNING << "LArDectorFactoryH62002:\tCan't access LArGeoTBH1GeoOptions, using default values\n";
  }

 (*log)<< MSG::INFO<< endmsg;
 (*log)<< MSG::INFO << " Use cryo X : " <<  m_cryoXpos << " Gaudi::Units::mm" << endmsg;
 (*log)<< MSG::INFO << " Use table Y : " <<  m_tableYpos << " Gaudi::Units::mm" << endmsg;


}





//## Other Operations (implementation)
void LArGeo::LArDetectorFactoryH62002::create(GeoPhysVol *world)
{

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    return; 
  }

  // V.N :  Patch LAr materials
  LArMaterialManager lArMaterialManager(m_detectorStore);
  lArMaterialManager.buildMaterials();
  //

  getSimulationParameters();

  //-----------------------------------------------------------------------------------//  
  // Get the materials that we shall use.                                              //  
  // ----------------------------------------------------------------------------------//  
  
  const GeoMaterial *air        = materialManager->getMaterial("std::Air");

  // 4databa :  // numbers taken from LArCalorimeter/LArG4TB/LArG4TBExpHall/src/LArG4TBEmecHecDetectorConstruction.cc
  // (That's a mighty big hall.....)
  double expHallX = 14000.*Gaudi::Units::mm;
  double expHallY = 14000.*Gaudi::Units::mm;
  double expHallZ = 50000.*Gaudi::Units::mm;
  //double cryoZpos = 12250.*Gaudi::Units::mm;
  //double cryoXrot = -90.*Gaudi::Units::deg; 
  //double cryoXpos = m_cryoXpos * Gaudi::Units::mm ;
  //double cryoXpos = 0.*Gaudi::Units::mm;  // <-- Should be made available in RunOptions! (Perhaps default in DB...)

  //-----------------------------------------------------------------------------------//  
  // Next make the box that describes the shape of the expHall volume:                 //  
  //                                                                                   //  
  const GeoBox      *expHallShape    = new GeoBox(expHallX, expHallY, expHallZ);       //  
  //                                                                                   //  
  // Bundle this with a material into a logical volume:                                //  
  //                                                                                   //  
  const GeoLogVol   *expHallLog    = new GeoLogVol("ExpHallLog", expHallShape, air);   //  
  //                                                                                   //  
  // ..And create a physical volume:                                                   //  
  //                                                                                   //  
  GeoPhysVol        *expHallPhys   = new GeoPhysVol(expHallLog);                       //  
  //                                                                                   //  
  // Add this to the list of top level physical volumes:                               //  
  //                                                                                   //  
  //-----------------------------------------------------------------------------------//  
  

  // For the moment it's still hard-coded, but eventually we'll
  // read a vector for translation and rotation from the database and apply it to the
  // the element we want to position in the following order:


  double Theta = -90. * Gaudi::Units::deg;
  double Phi   = 0.  * Gaudi::Units::deg;

  GeoTrf::Transform3D Mrot(GeoTrf::RotateZ3D(Phi)*GeoTrf::RotateX3D(Theta));
  GeoTrf::Translate3D pos3Vector(    m_cryoXpos*Gaudi::Units::mm,    0.*Gaudi::Units::mm,   12250.*Gaudi::Units::mm );

  H6CryostatConstruction  H6CryoCons;
  GeoVPhysVol* Envelope = 0;
  Envelope = H6CryoCons.GetEnvelope();
  expHallPhys->add(new GeoNameTag("LAr"));
  //expHallPhys->add( new GeoTransform( GeoTrf::Translate3D(pos3Vector)*GeoTrf::RotateX3D(Theta)*GeoTrf::RotateZ3D(Phi) ));
  expHallPhys->add( new GeoTransform( GeoTrf::Transform3D(pos3Vector*Mrot) ) );
  expHallPhys->add(Envelope);
 


  //Add the walls in front of the cryostat:
  {
    const double H62002WallsPos = 10182.*Gaudi::Units::mm;  // A wild guess at the moment.....
    WallsConstruction  WallsConstruction2002;
    GeoVPhysVol* frontwalls = WallsConstruction2002.GetEnvelope();
    if(frontwalls !=0 && expHallPhys !=0){
      expHallPhys->add( new GeoNameTag("LAr"));
      expHallPhys->add( new GeoTransform( GeoTrf::TranslateZ3D(H62002WallsPos) ) );  
      expHallPhys->add(frontwalls);    
    }
  }


  //Add the table instrumentation:
  {    
    const double H62002TablePos = 8320.*Gaudi::Units::mm;  
    TableConstructionH62002  TableConstruction;
    GeoVPhysVol* table = TableConstruction.GetEnvelope();
    if(table !=0 && expHallPhys !=0){
      expHallPhys->add( new GeoNameTag("LAr"));
      expHallPhys->add( new GeoTransform( GeoTrf::TranslateZ3D(H62002TablePos) ) );  
      expHallPhys->add(table);    
    }
  }
 (*log) << MSG::DEBUG << "Built Table Instrumentation " << endmsg;

  //Add the front beam instrumentation:
  {
    const double H62002FrontBeamPos = -20215.5*Gaudi::Units::mm;  // (Use this to get the Front dets. in Peter Schacht's position)   
    //const double H62002FrontBeamPos = -20439.*Gaudi::Units::mm; // (according to old code: [-21600+801+350]*Gaudi::Units::mm)   
    // (with 350=1/2 length of FrontBeam volume)
    FrontBeamConstructionH62002  FrontBeamConstruction;
    GeoVPhysVol* front = FrontBeamConstruction.GetEnvelope();
    if(front !=0 && expHallPhys !=0){
      expHallPhys->add( new GeoNameTag("LAr"));
      expHallPhys->add( new GeoTransform( GeoTrf::TranslateZ3D(H62002FrontBeamPos) ) );  
      expHallPhys->add(front);    
    }
  }
 (*log) << MSG::DEBUG << "Built Frontbeam Instrumentation " << endmsg;




  // Get LArPhysical, which is the actual cryostat

  GeoPhysVol* LArPhysical = H6CryoCons.GetLArPhysical();

 (*log) << MSG::DEBUG << "Got the Cryostat ready" << endmsg;


  // For the moment it is still commented out until I have
  // its true geometry confirmed; But really it is ready to go:
  // Add Rohacell Excluder 
  // double ThetaRoha = 0. * Gaudi::Units::deg;
  // double PhiRoha   = 0.  * Gaudi::Units::deg;
  // GeoTrf::Transform3D MrotRoha(GeoTrf::RotateZ3D(PhiRoha)*GeoTrf::RotateX3D(ThetaRoha));
  // GeoTrf::Translate3D pos3Roha(    0*Gaudi::Units::mm,   0.0*Gaudi::Units::mm ,   0.*Gaudi::Units::mm);

  {    
    ExcluderConstruction excluderConstruction;
    GeoPhysVol* excluder = excluderConstruction.GetEnvelope();
    if(excluder !=0 && LArPhysical !=0){
      LArPhysical->add( new GeoNameTag("LAr::H6::Cryostat::Excluder"));
      LArPhysical->add(excluder);    
    }
  }

 (*log) << MSG::DEBUG << "Built Rohacell Excluder " << endmsg;



  // Here include EMEC (from LArGeoTBEndcap):

  EMECDetectorManager *emecDetectorManager  = new EMECDetectorManager();


  double ThetaEmec = -90. * Gaudi::Units::deg;
  double PhiEmec   = 180.  * Gaudi::Units::deg;

  GeoTrf::Transform3D MrotEmec(GeoTrf::RotateZ3D(PhiEmec)*GeoTrf::RotateX3D(ThetaEmec));
  //  GeoTrf::Vector3D pos3Emec(    0*Gaudi::Units::mm,   869.0*Gaudi::Units::mm ,   1720.*Gaudi::Units::mm);
  GeoTrf::Translate3D pos3Emec(    0*Gaudi::Units::mm,   808.0*Gaudi::Units::mm ,   1720.*Gaudi::Units::mm);

  //use this line for physical construction of the EMEC outer wheel only:
  EMECConstruction emecConstruction(true, true, true);

  GeoVFullPhysVol* emecEnvelope = emecConstruction.GetEnvelope();
  LArPhysical->add(new GeoNameTag("LAr"));
  LArPhysical->add( new GeoTransform( GeoTrf::Transform3D(pos3Emec*MrotEmec) ) );
  LArPhysical->add(emecEnvelope);    
  { 
    StoredPhysVol *sEmecOuterWheel;   
    if (StatusCode::SUCCESS==m_detectorStore->retrieve(sEmecOuterWheel, "EMEC_OUTER_WHEEL_POS" )) {        
      GeoFullPhysVol *emecEnvelope= sEmecOuterWheel->getPhysVol();
      // Outer Wheel Sampling 1 Region 0:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,0,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      
      // Outer Wheel Sampling 1 Region 1:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,1,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      // Outer Wheel Sampling 1 Region 2:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,2,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      // Outer Wheel Sampling 1 Region 3:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,3,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      // Outer Wheel Sampling 1 Region 4:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,4,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      // Outer Wheel Sampling 1 Region 5:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,5,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      // Outer Wheel Sampling 2 Region 0:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,32,48);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,0,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      // Outer Wheel Sampling 2 Region 1:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,32,48);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,1,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      // Outer Wheel Sampling 3 Region 0:
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,32,48);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,3,0,0,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
    }

    StoredPhysVol *sEmecInnerWheel;   
    if (StatusCode::SUCCESS==m_detectorStore->retrieve(sEmecInnerWheel, "EMEC_INNER_WHEEL_POS" )) {        
      GeoFullPhysVol *emecEnvelope= sEmecInnerWheel->getPhysVol();
      // Inner Wheel Sampling 1 Region 0:
      {
      CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
      EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,0,1,phiBinning);
      EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
      emecDetectorManager->addDetectorRegion(region);
      }
      // Inner Wheel Sampling 2 Region 0:
      
      {
	CellBinning phiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,0,1,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
    } 
  }
  
  
  double ThetaPS = -90. * Gaudi::Units::deg;
  double PhiPS   = 180.  * Gaudi::Units::deg;
  GeoTrf::Transform3D MrotPS(GeoTrf::RotateZ3D(PhiPS)*GeoTrf::RotateX3D(ThetaPS));
  //GeoTrf::Vector3D pos3PS(    0*Gaudi::Units::mm,   945.5*Gaudi::Units::mm ,   1720.*Gaudi::Units::mm);
  GeoTrf::Translate3D pos3PS(    0*Gaudi::Units::mm,   888.5*Gaudi::Units::mm ,   1720.*Gaudi::Units::mm);
  
  //double zPSpos = -869. -(61. +2. +13.5);
  //std::string PresamplerName = baseName + "::Presampler::";
  EndcapPresamplerConstruction PresamplerConstruction(true);
  GeoFullPhysVol* PresamplerEnvelope = PresamplerConstruction.Envelope();
  if ( PresamplerEnvelope != 0 && LArPhysical != 0 ) {    
    //LArPhysical->add( new GeoTransform( GeoTrf::Translate3D(pos3PS)*GeoTrf::RotateX3D(ThetaPS)*GeoTrf::RotateZ3D(PhiPS) ));
     LArPhysical->add( new GeoTransform( GeoTrf::Transform3D(pos3PS*MrotPS) ) );
     LArPhysical->add( PresamplerEnvelope );
  }
  {
  //
    CellBinning presamplerPhiBinning(M_PI/2-M_PI/8,M_PI/2+M_PI/8,8,12);
    EMECDetDescr *presamplerDetDescr = new EMECDetDescr(emecDetectorManager,0,0,0,presamplerPhiBinning);
    EMECDetectorRegion *presamplerRegion = new EMECDetectorRegion(PresamplerEnvelope,presamplerDetDescr,EMECDetectorRegion::POS);
    emecDetectorManager->addDetectorRegion(presamplerRegion);
  }

 (*log) << MSG::DEBUG << "Built EMEC and Presampler " << endmsg;



  // Add HEC 
  double ThetaHec = 90. * Gaudi::Units::deg;
  double PhiHec   = 0.  * Gaudi::Units::deg;
  GeoTrf::Transform3D MrotHec(GeoTrf::RotateZ3D(PhiHec)*GeoTrf::RotateX3D(ThetaHec));
  GeoTrf::Translate3D pos3Hec(    0*Gaudi::Units::mm,   233.0*Gaudi::Units::mm ,   1720.*Gaudi::Units::mm);

  {    
    HECConstructionH62002 hecConstruction;
    GeoVFullPhysVol* hecEnvelope = hecConstruction.GetEnvelope();
    if(hecEnvelope !=0 && LArPhysical !=0){
      LArPhysical->add( new GeoNameTag("LAr"));
      //LArPhysical->add( new GeoTransform( GeoTrf::Translate3D(pos3Hec)*GeoTrf::RotateX3D(ThetaHec)*GeoTrf::RotateZ3D(PhiHec) ));
      LArPhysical->add( new GeoTransform( GeoTrf::Transform3D(pos3Hec*MrotHec) ) );
      LArPhysical->add(hecEnvelope);    
    }
  }

  HECDetectorManager *hecDetManager = new  HECDetectorManager();
  
  //
  // Add descriptors to the HEC detector manager:
  //

  unsigned int currentBlock=0;
  for (unsigned int s=0;s<3;s++) {
    unsigned int nBlocks = s==0 ? 1:2;
    unsigned int width = s==2 ? 2:  3;
    for (unsigned int r=0;r<2;r++) {
      unsigned int nPhi    = r==0? 2:1;
      double startPhi = s==2 ?  -36*M_PI/64 : -38*M_PI/64;
      CellBinning  phiBinning(startPhi, startPhi+width*2*2*M_PI/64, width*nPhi);
      HECDetDescr *d = new HECDetDescr(hecDetManager,s,r,phiBinning); 
      // Only POS endcap (1)
      //for (unsigned int endcap=1;endcap<2;endcap++)
      {
	StoredPhysVol *vol;                                                   
	if (StatusCode::SUCCESS==m_detectorStore->retrieve(vol, "HEC1_POS")) {                
	  HECDetectorRegion::DetectorSide side = HECDetectorRegion::POS;
	  HECDetectorRegion *region = new HECDetectorRegion(vol->getPhysVol(),d,side);
	  hecDetManager->addDetectorRegion(region);
	}                                                                     
      }
    }
    currentBlock+=nBlocks;
    }  
  

 (*log) << MSG::DEBUG << "Built HEC " << endmsg;






  m_detectorStore->record(hecDetManager,hecDetManager->getName());
  m_detectorStore->record(emecDetectorManager,emecDetectorManager->getName());
  m_detectorManager = new LArDetectorManager(0,emecDetectorManager,hecDetManager,0);

  m_detectorManager->addTreeTop(expHallPhys);

  




  //------------------------------------------------------------------------------------//
  // Now insert all of this into the world...                                           //
  GeoNameTag *tag = new GeoNameTag("LAr");                                              //
  world->add(tag);                                                                      //
  world->add(expHallPhys);                                                                  //
  //------------------------------------------------------------------------------------//


}

const LArDetectorManager *LArGeo::LArDetectorFactoryH62002::getDetectorManager() const
{
  return m_detectorManager;
}





