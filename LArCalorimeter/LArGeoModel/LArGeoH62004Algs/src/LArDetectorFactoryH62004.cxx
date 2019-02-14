/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetectorFactoryH62004.h"
#include "ModulesConstructionH62004.h"
#include "WarmTCConstructionH62004.h"
#include "FrontBeamConstructionH62004.h"
#include "MiddleBeamConstructionH62004.h"
#include "MovableTableConstructionH62004.h"
#include "LArGeoH6Cryostats/H6CryostatConstruction.h"

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
#include "AthContainers/DataVector.h"

// need if we want tu use EMEC from LArGeoTBEC:
#include "LArGeoTBEC/EMECModuleConstruction.h"
#include "LArGeoEndcap/EndcapPresamplerConstruction.h"

// For run options :
#include "LArG4RunControl/LArGeoTB2004Options.h"
#include "LArGeoCode/LArMaterialManager.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetDescr.h"
#include "LArReadoutGeometry/HECLongBlock.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "GeoModelKernel/CellBinning.h"
#include "CaloDetDescr/CaloDetDescrElementContainer.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/CaloDetDescriptor.h"

#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"

#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/EMECDetDescr.h"

//#include "LArGeoRAL/RAL.h"
//#include "LArGeoCode/VDetectorParameters.h"

using namespace GeoGenfun;
using namespace GeoXF;


LArGeo::LArDetectorFactoryH62004::LArDetectorFactoryH62004(StoreGateSvc *detStore)
  : m_cryoXpos(0),
    m_tableYpos(0),
    m_detectorStore(detStore),
    m_detectorManager(0),
    m_cell_id(0)
{
}


LArGeo::LArDetectorFactoryH62004::~LArDetectorFactoryH62004()
{
}


void LArGeo::LArDetectorFactoryH62004::getSimulationParameters()
{

//  StoreGateSvc* detStore;
  const LArGeoTB2004Options      *largeoTB2004Options = nullptr;
  
  StatusCode status;
//  ISvcLocator* svcLocator = Gaudi::svcLocator(); 
//  status = svcLocator->service("DetectorStore", detStore);


//  if( status.isSuccess() ) {
 

  m_cryoXpos = 0.;
  m_tableYpos = 0.;

//  status = detStore->retrieve(largeoTBH1geoOptions, "LArGeoTBH1GeoOptions");
  status = m_detectorStore->retrieve(largeoTB2004Options, "LArGeoTB2004Options");
  if ( !status.isFailure() ) {
    m_cryoXpos = largeoTB2004Options->CryoXPosition();
    m_tableYpos = largeoTB2004Options->TableYPosition();
  }
  else {
    std::cout << "LArDectorFactoryH62004:\tCan't access LArGeoTB2004Options, using default values\n";
    m_cryoXpos = -65.;
    m_tableYpos = 70.;
  }

  std::cout << " Use cryo X : " <<  m_cryoXpos << " Gaudi::Units::mm" << std::endl;
  std::cout << " Use table Y : " <<  m_tableYpos << " Gaudi::Units::mm" << std::endl;
  const_cast<LArGeoTB2004Options*>(largeoTB2004Options)->printMe();


}

//## Other Operations (implementation)
void LArGeo::LArDetectorFactoryH62004::create(GeoPhysVol *world)
{

//   VDetectorParameters* parameters = (VDetectorParameters *) new  LArGeo::RAL();
//   VDetectorParameters::SetInstance(parameters);

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    return; 
  }
   // V.N :  Patch LAr materials
   LArMaterialManager lArMaterialManager (m_detectorStore);
   lArMaterialManager.buildMaterials();


   StatusCode status = m_detectorStore->retrieve(m_cell_id, "CaloCell_ID");
   if (status.isFailure())   {
      std::cout << MSG::FATAL << "Could not get CaloCell_ID helper !" << std::endl;
     return;
   }  else { 
      std::cout << MSG::DEBUG << " Found the CaloCell_ID helper. " << std::endl; 
   }

  getSimulationParameters();

  //-----------------------------------------------------------------------------------//  
  // Get the materials that we shall use.                                              //  
  // ----------------------------------------------------------------------------------//  
  
  const GeoMaterial *air        = materialManager->getMaterial("std::Air");

  double expHallX = 14000.*Gaudi::Units::mm;
  double expHallY = 14000.*Gaudi::Units::mm;
  double expHallZ = 50000.*Gaudi::Units::mm;

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

  GeoTrf::Transform3D Mrot = GeoTrf::RotateZ3D(Phi) * GeoTrf::RotateX3D(Theta);
  GeoTrf::Translation3D pos3Vector(- m_cryoXpos*Gaudi::Units::mm
				   , 0.*Gaudi::Units::mm
				   , 12250.*Gaudi::Units::mm );

  H6CryostatConstruction  H6CryoCons;
  GeoVPhysVol* CryoEnvelope = 0;
  CryoEnvelope = H6CryoCons.GetEnvelope();
  expHallPhys->add(new GeoNameTag("LAr"));
  //expHallPhys->add( new GeoTransform( GeoTrf::Translate3D(pos3Vector)*GeoTrf::RotateX3D(Theta)*GeoTrf::RotateZ3D(Phi) ));
  expHallPhys->add(new GeoTransform(pos3Vector*Mrot));// GeoTrf::Transform3D(Mrot, pos3Vector) ) );
  expHallPhys->add(CryoEnvelope);
 

  // Get LArPhysical so that we can add the HEC
  GeoPhysVol* LArPhysical = 0;
  LArPhysical = H6CryoCons.GetLArPhysical();

  // Add the front beam instrumentation:
  FrontBeamConstructionH62004 FrontBeamConstruction;
  // DB ?
  const double bard_z = 100.0*Gaudi::Units::cm;
  const double z_bard=-2160.0*Gaudi::Units::cm+80.1*Gaudi::Units::cm+16.*Gaudi::Units::cm+bard_z;
  {                                             // (with 350=1/2 length of FrontBeam volume)
    GeoVPhysVol* front = 0;
    front = FrontBeamConstruction.GetEnvelope();
    if(front !=0 && expHallPhys !=0){
      expHallPhys->add( new GeoNameTag("H62004::Front"));
      expHallPhys->add( new GeoTransform( GeoTrf::TranslateZ3D(z_bard) ) );  
      expHallPhys->add(front);    
    }
  }
  // Add middle chambers
  MiddleBeamConstructionH62004 MiddleBeamConstruction;
  const double z_bardm=-2160.0*Gaudi::Units::cm+1362.3*Gaudi::Units::cm;
  const double bttb_pos = 833.5*Gaudi::Units::cm;
  {
     GeoVPhysVol* middle = 0;
     middle = MiddleBeamConstruction.GetEnvelope();
     if(middle != 0 && expHallPhys !=0){
        double ym_pos = m_tableYpos  * (z_bardm + 2160*Gaudi::Units::cm) * (1./(bttb_pos + 2160*Gaudi::Units::cm));
	expHallPhys->add( new GeoNameTag("H62004::Middle"));
	expHallPhys->add( new GeoTransform( GeoTrf::TranslateY3D(ym_pos) * GeoTrf::TranslateZ3D(z_bardm) ) );
	expHallPhys->add(middle);
     }
  }
  // Add MovableTable
  MovableTableConstructionH62004 MovableTable;
  {
     GeoVPhysVol* mov = 0;
     mov = MovableTable.GetEnvelope();
     if(mov != 0 && expHallPhys !=0){
	expHallPhys->add( new GeoNameTag("H62004::Movable"));
	expHallPhys->add( new GeoTransform( GeoTrf::TranslateY3D(m_tableYpos) *  GeoTrf::TranslateZ3D(bttb_pos) ) );
	expHallPhys->add(mov);
     }
  }


   // WarmTC after the cryostat
   double WTC_tild = -1.1*Gaudi::Units::deg;   // 24 Gaudi::Units::mm tild on 1250 Gaudi::Units::mm length !! should go to DB ?
   double WTC_len = 591.5*Gaudi::Units::mm;
   double WTC_sci_z = 12.7*Gaudi::Units::mm;
   double Muon_dist = 120.0*Gaudi::Units::mm;
   double Muon_z = 1.0*Gaudi::Units::cm;
   double bcry_zpos = 1225.0*Gaudi::Units::cm;
   double bcry_rwarm = 129.55*Gaudi::Units::cm;
   double WTC_x = 0.0*Gaudi::Units::mm;
   double WTC_y = 0.0*Gaudi::Units::mm;
   double WTC_z = 460.0*Gaudi::Units::mm - 120.*Gaudi::Units::mm - 10.*Gaudi::Units::mm;
   double z_m = (86.0*Gaudi::Units::mm + WTC_len + WTC_sci_z + Muon_dist + Muon_z) / 2;

   WarmTCConstructionH62004 wtcConstruction;
   {
     std::cout<<"WTC constructed: "<<std::endl;
     GeoVPhysVol* wtc = 0;
     wtc = wtcConstruction.GetEnvelope();
     std::cout<<"WTC envelope: "<<wtc<<"/"<<expHallPhys<<std::endl;
     if(wtc !=0 && expHallPhys !=0){
       expHallPhys->add( new GeoNameTag("LAr"));
       GeoTrf::RotateX3D rotTC(WTC_tild);
       expHallPhys->add( new GeoTransform( GeoTrf::Translation3D(WTC_x, WTC_y, bcry_zpos + bcry_rwarm + WTC_z + z_m) * rotTC));
       expHallPhys->add(wtc);    
     }
   }

   //  modules in cryostat
   ModulesConstructionH62004 moduleConstruction;
   {    
     std::cout<<"Module constructed: "<<std::endl;
     GeoVPhysVol* module = 0;
     module = moduleConstruction.GetEnvelope();
     std::cout<<"Module envelope: "<<module<<"/"<<LArPhysical<<std::endl;
     if(module !=0 && LArPhysical !=0){
       LArPhysical->add( new GeoNameTag("LAr::H6::Cryostat::Modules"));
       LArPhysical->add(module);    
     }
   }


  EMECDetectorManager *emecDetectorManager  = NULL;
  FCALDetectorManager  *fcalDetectorManager = NULL;
  HECDetectorManager *hecDetManager = NULL;
  StoredPhysVol *sEmecInnerWheel;   
  if (StatusCode::SUCCESS==m_detectorStore->retrieve(sEmecInnerWheel, "EMEC_INNER_WHEEL_POS" )) {        
      emecDetectorManager = new EMECDetectorManager();
      GeoFullPhysVol *emecEnvelope= sEmecInnerWheel->getPhysVol();
      // Inner Wheel Sampling 1 Region 0:
      {
//      CellBinning phiBinning(M_PI/2-M_PI/8-2*M_PI/768/2,M_PI/2+M_PI/8-2*M_PI/768/2,8,20);
      CellBinning phiBinning(M_PI/2+M_PI/8-2*M_PI/768/2,M_PI/2+3*M_PI/8-2*M_PI/768/2,8,20);
//      CellBinning phiBinning(M_PI/2,M_PI/2+M_PI/4,8,20);
      EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,1,0,1,phiBinning);
      EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
      emecDetectorManager->addDetectorRegion(region);
      }
      // Inner Wheel Sampling 2 Region 0:
      
      {
//	CellBinning phiBinning(M_PI/2-M_PI/8-2*M_PI/768/2,M_PI/2+M_PI/8-2*M_PI/768/2,8,20);
	CellBinning phiBinning(M_PI/2+M_PI/8-2*M_PI/768/2,M_PI/2+3*M_PI/8-2*M_PI/768/2,8,20);
//	CellBinning phiBinning(M_PI/2,M_PI/2+M_PI/4,8,20);
	EMECDetDescr *detDescr = new EMECDetDescr(emecDetectorManager,2,0,1,phiBinning);
	EMECDetectorRegion *region = new EMECDetectorRegion(emecEnvelope,detDescr,EMECDetectorRegion::POS);
	emecDetectorManager->addDetectorRegion(region);
      }
      std::cout<<"Added "<<emecDetectorManager->getNumDetectorRegions()<<" regions in EMEC"<<std::endl;
  }

  if(m_detectorStore->contains<StoredPhysVol>("HEC_POS")) {
     StoredPhysVol *vol;
     if (StatusCode::SUCCESS==m_detectorStore->retrieve(vol,"HEC_POS")) {
        try {
           hecDetManager = new  HECDetectorManager(true);
           for (unsigned int s=0;s<3;s++) {
            for (unsigned int r=0;r<2;r++) {
	     unsigned int nPhi    = r==0? 2:1;
	
	      
	     unsigned int width = 8;
//	     double startPhi =  M_PI/2. - M_PI/128.;
//	     double endPhi   =  M_PI -M_PI/128.;
	     double startPhi =  M_PI/2. - 1.e-9;
	     double endPhi   =  M_PI - 1.e-9;
	      
	     CellBinning  phiBinning(startPhi, endPhi, width*nPhi, 8*nPhi);
	     HECDetDescr *d = new HECDetDescr(hecDetManager,s,r,phiBinning, true); 

	     HECDetectorRegion::DetectorSide side = (HECDetectorRegion::DetectorSide) 1;
	     HECDetectorRegion *region = new HECDetectorRegion(vol->getPhysVol(),d,side);
	     hecDetManager->addDetectorRegion(region);
           }
          }
           std::cout<<"Added "<<hecDetManager->getNumDetectorRegions()<<" regions in HEC"<<std::endl;
        } 
        catch (std::exception & ex) {
            //log << MSG::WARNING << "Unable to build HEC detector manager. " << ex.what() << endmsg;
           std::cout << "Unable to build HEC detector manager. " << ex.what() << std::endl;
        }
     } else {
        //log << MSG::DEBUG << " No Stored PV for MODULES_POS in Detector Store" << endmsg;
        std::cout << " No Stored PV for HEC_POS in Detector Store" << std::endl;
     }
  }

  if(m_detectorStore->contains<StoredPhysVol>("FCAL1")) {
     StoredPhysVol *fcal1;
     if (StatusCode::SUCCESS==m_detectorStore->retrieve(fcal1,"FCAL1")) {
	try {
	   fcalDetectorManager = new FCALDetectorManager();
	   FCALModule *detDescr = new FCALModule(fcal1->getPhysVol(),FCALModule::FCAL1,FCALModule::POS);
	   fcalDetectorManager->addModule(detDescr);
	}
	catch (std::exception & ex) {
           std::cout << "Unable to build FCAL1 detector manager. " << ex.what() << std::endl;
        }
     } else {
        std::cout << " No Stored PV for FCAL1 in Detector Store" << std::endl;
     }
  }

  if(m_detectorStore->contains<StoredPhysVol>("FCAL2")) {
     StoredPhysVol *fcal2;
     if (StatusCode::SUCCESS==m_detectorStore->retrieve(fcal2,"FCAL2")) {
	try {
	   if(!fcalDetectorManager) fcalDetectorManager = new FCALDetectorManager();
	   FCALModule *detDescr = new FCALModule(fcal2->getPhysVol(),FCALModule::FCAL2,FCALModule::POS);
	   fcalDetectorManager->addModule(detDescr);
	}
	catch (std::exception & ex) {
           std::cout << "Unable to build FCAL2 detector manager. " << ex.what() << std::endl;
        }
     } else {
        std::cout << " No Stored PV for FCAL2 in Detector Store" << std::endl;
     }
  }

  if(m_detectorStore->contains<StoredPhysVol>("ColdTC")) {
     StoredPhysVol *fcal3;
     if (StatusCode::SUCCESS==m_detectorStore->retrieve(fcal3,"ColdTC")) {
        storeDDE();
     } else {
        std::cout << " No Stored PV for ColdTC in Detector Store" << std::endl;
     }
  }
  
  if (fcalDetectorManager) {
     std::cout<<"Added "<<fcalDetectorManager->getNumTreeTops()<<" modules in FCAL"<<std::endl;
     StatusCode sc = m_detectorStore->record(fcalDetectorManager,  fcalDetectorManager->getName());
     if(sc.isFailure())
      //log << MSG::ERROR << "Unable to record hecDetectorManager" << endmsg;
      std::cout << "Unable to record fcalDetectorManager" << std::endl;
  }

  if (emecDetectorManager)    
  {
    StatusCode sc = m_detectorStore->record(emecDetectorManager,  emecDetectorManager->getName());
    if(sc.isFailure())
      //log << MSG::ERROR << "Unable to record hecDetectorManager" << endmsg;
      std::cout << "Unable to record emecDetectorManager" << std::endl;
  }

  if (hecDetManager)    
  {
    StatusCode sc = m_detectorStore->record(hecDetManager,  hecDetManager->getName());
    if(sc.isFailure())
      //log << MSG::ERROR << "Unable to record hecDetectorManager" << endmsg;
      std::cout << "Unable to record hecDetectorManager" << std::endl;
  }


  m_detectorManager = new LArDetectorManager(0,emecDetectorManager,hecDetManager,fcalDetectorManager);
  m_detectorManager->isTestBeam(true); 

  m_detectorManager->addTreeTop(expHallPhys);


  //------------------------------------------------------------------------------------//
  // Now insert all of this into the world...                                           //
  GeoNameTag *tag = new GeoNameTag("LAr");                                              //
  world->add(tag);                                                                      //
  world->add(expHallPhys);                                                                  //
  //------------------------------------------------------------------------------------//


}

const LArDetectorManager *LArGeo::LArDetectorFactoryH62004::getDetectorManager() const
{
  return m_detectorManager;
}


StatusCode LArGeo::LArDetectorFactoryH62004::storeDDE()
{
   CaloDetDescrElementContainer *cDDEvec = new CaloDetDescrElementContainer() ;

   const int nctc = 32;
   //const float ctcz =  5597.5;
   const float ctcpar[][6] = {  
//  id           eta     phi       x              y       r
{990530560,	3.37,	1.67,	-37.93,	        385.12,	386.98},
{990530816,	3.37,	1.87,	-112.33,	370.32,	386.98},
{990531072,	3.37,	2.06,	-182.42,	341.29,	386.98},
{990531328,	3.37,	2.26,	-245.5,	        299.14,	386.98},
{990543616,	3.37,	2.45,	-299.14,	245.5,	386.98},
{990543360,	3.37,	2.65,	-341.29,	182.42,	386.98},
{990543104,	3.37,	2.85,	-370.32,	112.33,	386.98},
{990542848,	3.37,	3.04,	-385.12,	37.93,	386.98},
{990528256,	4,	1.67,	-20.09,	        203.99,	204.98},
{990528000,	4,	1.87,	-59.5,	        196.15,	204.98},
{990527744,	4,	2.06,	-96.63,	        180.78,	204.99},
{990527488,	4,	2.26,	-130.04,	158.45,	204.98},
{990540032,	4,	2.45,	-158.45,	130.04,	204.98},
{990539776,	4,	2.65,	-180.78,	96.63,	204.99},
{990540288,	4,	2.85,	-196.15,	59.5,	204.98},
{990540544,	4,	3.04,	-203.99,	20.09,	204.98},
{990530304,	3.25,	1.96,	-166.86,	402.83,	436.02},
{990530048,	3.32,	1.96,	-155.77,	376.07,	407.05},
{990529792,	3.4,	1.96,	-143.83,	347.24,	375.85},
{990529536,	3.49,	1.96,	-130.8, 	315.78,	341.8},
{990529280,	3.61,	1.96,	-116.3, 	280.76,	303.9},
{990529024,	3.76,	1.96,	-99.66,	        240.6,	260.42},
{990528768,	3.99,	1.96,	-79.46,	        191.84,	207.65},
{990528512,	4.44,	1.96,	-50.57,	        122.08,	132.14},
{990542592,	3.25,	2.75,	-402.83,	166.86,	436.02},
{990542336,	3.32,	2.75,	-376.07,	155.77,	407.05},
{990542080,	3.4,	2.75,	-347.24,	143.83,	375.85},
{990541824,	3.49,	2.75,	-315.78,	130.8,	341.8},
{990541568,	3.61,	2.75,	-280.76,	116.3,	303.9},
{990541312,	3.76,	2.75,	-240.6, 	99.66,	260.42},
{990541056,	3.99,	2.75,	-191.84,	79.46,	207.65},
{990540800,	4.44,	2.75,	-122.08,	50.57,	132.14},
                  };

   
/*
   std::vector<Identifier>::const_iterator reg_b = cell_id->reg_begin();
   std::vector<Identifier>::const_iterator reg_e = cell_id->reg_end();
   for(; reg_b != reg_e; ++reg_b) {
      std::cout<<"Region: "<<std::hex<<*reg_b<<std::dec<<" Subcalo: "<<cell_id->sub_calo(*reg_b)<<" Sample: "<<cell_id->calo_sample(*reg_b)<<std::endl;
   }
   */

   CaloCell_ID::CaloSample sample = CaloCell_ID::FCAL2;
   Identifier reg_id = m_cell_id->region_id(m_cell_id->cell_id((int)CaloCell_ID::LARFCAL, 2, 3, 0, 0, 0));
   CaloDetDescriptor *desc = new CaloDetDescriptor(reg_id, (AtlasDetectorID *)m_cell_id, m_cell_id, sample,0); 
   int ieta, iphi;
    for(int i=0; i<nctc; ++i) {
//       std::cout<<i<<" "<<int(ctcpar[i][0])<<" "<<std::hex<<int(ctcpar[i][0])<<std::dec<<std::endl;
       if(i<16){
          ieta = i/8 + 8;
          iphi = i%8;
       } else {
          ieta = (i-16)%8;
          iphi = (i-16)/8 + 8;
       }
       Identifier cellid = m_cell_id->cell_id((int)CaloCell_ID::LARFCAL, 2, 3, 0, ieta, iphi);
       //Identifier reg_id = m_cell_id->region_id(cellid);
       int subcalo = (int)CaloCell_ID::FCAL2;
       IdentifierHash sub_hash = m_cell_id->subcalo_cell_hash(cellid, subcalo);

//       std::cout<<std::hex<<reg_id<<" "<<cellid<<" "<<std::dec<<sub_hash<<" "<< m_cell_id->show_to_string(cellid)<<std::endl;
       DummyDetDescrElement *el = new DummyDetDescrElement(sub_hash, 0, 0, desc);
       el->set_cylindric(ctcpar[i][1],ctcpar[i][2],ctcpar[i][5]);
       el->set_cylindric_raw(ctcpar[i][1],ctcpar[i][2],ctcpar[i][5]);
       if(i<16) {
         el->set_cylindric_size(0.42,0.2,180.);
       } else {
         el->set_cylindric_size(0.12,0.8,30.);
       }
       cDDEvec->push_back(el);
    }
    
    StatusCode sc = m_detectorStore->record(cDDEvec,"ColdTCDDE");
   std::cout<<"Recording: "<<sc<<std::endl;
   if(sc != StatusCode::SUCCESS) {
       std::cout<<"LArDetectorFactoryH62004::storeDDE could not record the ColdTC caloDDE !!!"<<std::endl;
   }
   return sc;   
}

