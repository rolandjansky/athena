/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "HECConstructionH62002.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoVFullPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoSerialDenominator.h"  
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoShapeUnion.h"  
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "StoreGate/StoreGateSvc.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include <string>
#include <cmath>
#include <iostream>



using namespace GeoGenfun;
using namespace GeoXF;



LArGeo::HECConstructionH62002::HECConstructionH62002()
   :m_h6Phys(0)
{
 // access source of detector parameters
  // m_parameters = LArGeo::VDetectorParameters::GetInstance();
}


LArGeo::HECConstructionH62002::~HECConstructionH62002()
{
}


GeoVFullPhysVol* LArGeo::HECConstructionH62002::GetEnvelope()
{

  if (m_h6Phys) return (m_h6Phys);


  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in HECConstruction, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "HECConstruction");  
  log << MSG::INFO;  
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         Start of HEC TB GeoModel definition      +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;


  StoreGateSvc *detectorStore;
  if (svcLocator->service("DetectorStore", detectorStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in HECConstruction, cannot access DetectorStore");
  }


  StatusCode sc;
  IRDBAccessSvc *pAccessSvc;
  sc=svcLocator->service("RDBAccessSvc",pAccessSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }


  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    return NULL; 
  } 

  
   
  //-----------------------------------------------------------------------------------//  
  // Get the materials that we shall use.                                              //  
  // ----------------------------------------------------------------------------------//  
  
  //const GeoMaterial *air        = materialManager->getMaterial("std::Air");

  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in HECConstruction, std::LiquidArgon is not found.");
  
  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in HECConstruction, std::Iron is not found.");
  
  const GeoMaterial *Copper  = materialManager->getMaterial("std::Copper");
  if (!Copper) throw std::runtime_error("Error in HECConstruction, std::Copper is not found.");
  
  const GeoMaterial *Kapton  = materialManager->getMaterial("std::Kapton");
  if (!Kapton) throw std::runtime_error("Error in HECConstruction, std::Kapton is not found.");
  

  
  DecodeVersionKey larVersionKey("LAr");
  std::string detectorKey  =larVersionKey.tag();
  std::string detectorNode = larVersionKey.node();

  IRDBRecordset_ptr hadronicEndcap       = pAccessSvc->getRecordsetPtr("HadronicEndcap",detectorKey, detectorNode); 
  IRDBRecordset_ptr hecLongitudinalBlock = pAccessSvc->getRecordsetPtr("HecLongitudinalBlock",detectorKey, detectorNode); 
  if(!hadronicEndcap) throw std::runtime_error("Error in HECConstruction:  hadronicEendcap not found");
  if(!hecLongitudinalBlock) throw std::runtime_error("Error in HECConstruction:  hecLongitudinalBlock not found");
  

  //------------------------------------------------------------------
  // First some sort of a mother
  //-----------------------------------------------------------------------------------//  

  // This creates a "mother wedge", which we can then place into the cryostat.
  // (Not the greatest way of doing it... Needs to be fixed.)
  // Maybe the best way would be to declare a union of front and rear module shapes?

  //  std::string tempHECName = "LAr::H62002::HEC::MotherVolume";
  std::string tempHECName = "LAr::Endcap::Cryostat::MotherVolume";
  double HECMotherZplan[] = {    0. , 1350. };
  double HECMotherRin[]   = {  372. ,  372. };
  double HECMotherRout[]  = { 2030. , 2030. };  
  int lastPlaneHEC = ( sizeof(HECMotherZplan) / sizeof(double) );


  double moduleDeltaPhi =  2.*M_PI/32. ;  //  = 11.25*Gaudi::Units::deg; 
  double phiStart []  = {-19. , -18. } ;
  double hecPhistart[]  = { phiStart[0]*M_PI/32  ,   phiStart[1]*M_PI/32 } ;
  double modulePhistart[]  = { (phiStart[0]+2.)*M_PI/32  ,   (phiStart[1]+2.)*M_PI/32 } ;

  
  GeoPcon* HECMotherShape = new GeoPcon(hecPhistart[0],  3.*M_PI/16. );
             
  for ( int i = 0; i != lastPlaneHEC; ++i )
    {
      HECMotherShape->addPlane( HECMotherZplan[i], HECMotherRin[i], HECMotherRout[i]);          
    }

  const GeoLogVol   *h6Log    = new GeoLogVol("HECMother", HECMotherShape, LAr); 
  m_h6Phys   = new GeoFullPhysVol(h6Log);                          

  std::string tag = std::string("HEC1_POS");
  StatusCode status;
  
  StoredPhysVol *sPhysVol = new StoredPhysVol(m_h6Phys);
  status=detectorStore->record(sPhysVol,tag);
  if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());
  


  //----------------------------------------------------------------
  // LAr HEC GEOMETRY
  //----------------------------------------------------------------
  // 
  // .... different sets of indeces:
  // --   iwheel  --     0,1  --  Front and Rear Wheels
  // --   islice  --   0,1,2  --  slices refer to the LAr gaps
  // --   idepth  --     0-4  --  longitudinal samplings (0-2 Front;3,4 Rear)
  // --   itie    --     0-3  --  radial tierod position 


  // Some volumes and related items we have to declare here.

  // Those with index - iwheel:
  // First Absorber
  GeoTubs*           solidFirstAbsorber[2];
  const GeoLogVol*   logiFirstAbsorber[2];
  GeoPhysVol*        physiFirstAbsorber[2];
  double             firstAbsorber[5];
  // TieRod in the gap
  GeoTubs*           solidTieRod[2];
  const GeoLogVol*   logiTieRod[2];
  GeoPhysVol*        physiTieRod[2];
  // TieRod in absorbers
  GeoTubs*           solidAbsorberTieRod[2];
  const GeoLogVol*   logiAbsorberTieRod[2];
  GeoPhysVol*        physiAbsorberTieRod[2];
  GeoTubs*           solidAbsorberTieRodRear[2];
  const GeoLogVol*   logiAbsorberTieRodRear[2];
  GeoPhysVol*        physiAbsorberTieRodRear[2];
  double             tieRodPositionX[4]; // 4 radial tie rod locations
  double             tieRodPositionY[4];
  double             tieRodDiameter[2];  // 2 sizes for the two wheels
  double             spacerDiameter[2];

  double ztie[2];   // This is the +/- z length of the tie rod in the LAr gap
  ztie[0]=-0.227825*Gaudi::Units::cm;
  ztie[1]= 0.227825*Gaudi::Units::cm;
  double rodSize = 0.39435*Gaudi::Units::cm;




  // Those with index - islice:
  // Slice
  GeoTubs*           solidSlice[3];
  const GeoLogVol*   logiSlice[3];
  GeoPhysVol*        physiSlice[3];
  // Absorber
  double             radialShift = 1.02*Gaudi::Units::cm;  // absorbers are adjusted by this amount          
  GeoTubs*           solidFrontAbsorber[2];
  const GeoLogVol*   logiFrontAbsorber[2];
  GeoPhysVol*        physiFrontAbsorber[2];
  GeoTubs*           solidRearAbsorber;
  const GeoLogVol*   logiRearAbsorber;
  GeoPhysVol*        physiRearAbsorber;


  // Those with index - idepth:
  // Depth  = longitudinal samplings  
  int                gapNumber[5];
  GeoTubs*           solidDepth[5];
  const GeoLogVol*   logiDepth[5];
  GeoPhysVol*        physiDepth[5];


  // And some that are just more convenient here:
  // EstBoard
  GeoTubs*           solidEstBoard;
  const GeoLogVol*   logiEstBoard;
  GeoPhysVol*        physiEstBoard;
  // PadBoard
  GeoTubs*           solidPadBoard;
  const GeoLogVol*   logiPadBoard;
  GeoPhysVol*        physiPadBoard;



  // And here are some numbers:

  int                numberZplane = 6;        // total number of z-planes needed for HEC volume
  int                numberFrontZplane = 4;   // number of z-planes needed for front HEC volume
  int                depthNumber = 5;         // number of longitudinal readout segments
  //int                sliceNumber = 3;         // LAr gap segmentation
  double             depthSize[5];            // z dimension of longitudinal readout segments
  double             kaptonPosition[3];
  double             kaptonWidth[3];

  log << MSG::DEBUG << " In the H6 2002 HEC Constr. GetEnvelope - Here we get database things " << std::endl;



  // set the number of Front and Rear Modules in the testbeam:
  int    moduleNumberFront   = 3;             
  int    moduleNumberRear    = 2;              
  // radial dimensions of the modules:
  double moduleRinner1  = (*hecLongitudinalBlock)[0]->getDouble("BLRMN")*Gaudi::Units::cm;   
  double moduleRinner2  = (*hecLongitudinalBlock)[1]->getDouble("BLRMN")*Gaudi::Units::cm;   
  double moduleRouter   = (*hecLongitudinalBlock)[0]->getDouble("BLRMX")*Gaudi::Units::cm;   
  // thickness of Cu pads, LAr gaps and inter-wheel gap:
  double copperPad      = (*hadronicEndcap)[0]->getDouble("COPPER")*Gaudi::Units::cm;
  double gapSize        = (*hadronicEndcap)[0]->getDouble("LARG")*Gaudi::Units::cm;
  double betweenWheel   = (*hadronicEndcap)[0]->getDouble("GAPWHL")*Gaudi::Units::cm; 


  for (int idepth=0; idepth < depthNumber; ++idepth)
    {
      depthSize[idepth]    = (*hecLongitudinalBlock)[idepth]->getDouble("BLDPTH")*Gaudi::Units::cm;
      firstAbsorber[idepth]= (*hecLongitudinalBlock)[idepth]->getDouble("PLATE0")*Gaudi::Units::cm;
      gapNumber[idepth]    = (int) (*hecLongitudinalBlock)[idepth]->getDouble("BLMOD");
    }

  for (int ikapton=0; ikapton < 3; ++ikapton)
    {
      std::ostringstream A0STR;
      A0STR << "_" << ikapton;
      const std::string A0 = A0STR.str();
      kaptonPosition[ikapton] = (*hadronicEndcap)[0]->getDouble("KPTPOS"+A0)*Gaudi::Units::cm;
      kaptonWidth[ikapton]    = (*hadronicEndcap)[0]->getDouble("KPTWID"+A0)*Gaudi::Units::cm;
    }

  for (int itie=0; itie < 4; ++itie)
    {
      std::ostringstream A0STR;
      A0STR << "_" << itie;
      const std::string A0 = A0STR.str(); 
      tieRodPositionX[itie] = (*hadronicEndcap)[0]->getDouble("RODPOSX"+A0)*Gaudi::Units::cm;
      tieRodPositionY[itie] = (*hadronicEndcap)[0]->getDouble("RODPOSR"+A0)*Gaudi::Units::cm;
    }

  for (int i=0; i < 2; ++i)
    {
      std::ostringstream A0STR;
      A0STR << "_" << i;
      const std::string A0 = A0STR.str();  
      tieRodDiameter[i] = (*hadronicEndcap)[0]->getDouble("RODDIM"+A0)*Gaudi::Units::cm;
      spacerDiameter[i] = (*hadronicEndcap)[0]->getDouble("SPCDIM"+A0)*Gaudi::Units::cm;
    }


  double frontAbsThickness = (*hadronicEndcap)[0]->getDouble("PLATE_0")*Gaudi::Units::cm;
  double rearAbsThickness  = (*hadronicEndcap)[0]->getDouble("PLATE_1")*Gaudi::Units::cm;
  
  // Radial dimensions and z-plane locations
  double zCoordinate[] = {0.0*Gaudi::Units::cm, depthSize[0], depthSize[0], 816.51*Gaudi::Units::mm, 816.51*Gaudi::Units::mm, 1350.*Gaudi::Units::mm };
  double innerRadius[] = {moduleRinner1,moduleRinner1,
                         moduleRinner2,moduleRinner2,moduleRinner2,moduleRinner2,};   

  double absorberPositionZ [5];
  //double    slicePositionZ [5];
  int sliceCopyNo[]={0,0,0,0,0};
  for(int i=0; i<depthNumber; i++)  // Testbeam:  Front Module: i=0,1,2   Rear Module: i=3,4
    {
      if (i<3) absorberPositionZ[i]=firstAbsorber[i]+ gapSize+ frontAbsThickness/2.0 -depthSize[i]/2.0;
      if (i>2) absorberPositionZ[i]=firstAbsorber[i]+ gapSize+  rearAbsThickness/2.0 -depthSize[i]/2.0;
      //slicePositionZ[i]=firstAbsorber[i]+ gapSize/2.0 -depthSize[i]/2.0;  

      if (i>0) sliceCopyNo[i] += sliceCopyNo[i-1]+gapNumber[i-1];
    }
 



  // And here come the important names...:  
  std::string hecFrontName = "LAr::HEC::LiquidArgon";
  std::string hecRearName  = "LAr::HEC::LiquidArgon";

  std::string frontmoduleName = "LAr::HEC::Module";
  std::string rearmoduleName  = "LAr::HEC::Module";

  std::string depthFrontName = frontmoduleName + "::Depth";
  std::string depthRearName  = rearmoduleName  + "::Depth";
  std::string sliceFrontName = depthFrontName  + "::Slice";
  std::string sliceRearName  = depthRearName   + "::Slice";
  std::string absorberFrontName = depthFrontName + "::Absorber";
  std::string absorberRearName  = depthRearName  + "::Absorber";
  std::string firstFrontAbsorberName= depthFrontName     + "::FirstFrontAbsorber";
  std::string firstRearAbsorberName = depthRearName      + "::FirstRearAbsorber";
  std::string electrodeFrontName    = sliceFrontName     + "::Electrode";
  std::string electrodeRearName     = sliceRearName      + "::Electrode";
  std::string copperFrontName       = electrodeFrontName + "::Copper";
  std::string copperRearName        = electrodeRearName  + "::Copper";
  std::string tieRodName = sliceFrontName + "::TieRod";
  std::string tieRodFrontName = sliceFrontName + "::TieRod";
  std::string tieRodRearName = sliceRearName + "::TieRod";



  log << MSG::DEBUG << " In the H6 2002 HEC Constr. GetEnvelope - Start to assemble HEC " << std::endl;


  //----------------------------------------------------------------
  //  Front HEC 
  //----------------------------------------------------------------
  
  // setting up the overal Front HEC volume:

  GeoPcon* solidFrontHEC = new GeoPcon( hecPhistart[0], moduleNumberFront*moduleDeltaPhi);
  for (int i=0; i< numberFrontZplane; i++) 
     {
        solidFrontHEC->addPlane(zCoordinate[i],innerRadius[i], moduleRouter);
     }
  const GeoLogVol* logicFrontHEC = new GeoLogVol(hecFrontName, solidFrontHEC , LAr);
  GeoPhysVol*  physiFrontHEC = new GeoPhysVol(logicFrontHEC);
  m_h6Phys->add(physiFrontHEC);
  

  //----------------------------------------------------------------
  //  Rear HEC 
  //----------------------------------------------------------------
  
  // setting up the overal Rear HEC volume:

  GeoPcon* solidRearHEC = new GeoPcon( hecPhistart[1], moduleNumberRear*moduleDeltaPhi);
  for (int i=numberFrontZplane; i< numberZplane; i++) 
     {
        solidRearHEC->addPlane(zCoordinate[i],innerRadius[i], moduleRouter);
     }

  const GeoLogVol* logicRearHEC = new GeoLogVol(hecRearName, solidRearHEC , LAr);
  GeoPhysVol* physiRearHEC = new GeoPhysVol(logicRearHEC);
  m_h6Phys->add(physiRearHEC);
  


  // Define the generic Front and Rear modules.

  //----------------------------------------------------------------
  //   HEC FrontModule
  //----------------------------------------------------------------
  // FrontModule        

  GeoPcon* solidFrontModule = new GeoPcon(modulePhistart[0], moduleDeltaPhi);

  for (int i=0; i< numberFrontZplane; i++) 
     {
        solidFrontModule->addPlane(zCoordinate[i],innerRadius[i], moduleRouter);
     }
  const GeoLogVol* logicFrontModule = new GeoLogVol(frontmoduleName, solidFrontModule , LAr);
  GeoPhysVol* physiFrontModule = new GeoPhysVol(logicFrontModule);

  //----------------------------------------------------------------
  //   HEC RearModule
  //----------------------------------------------------------------

  GeoPcon* solidRearModule = new GeoPcon(modulePhistart[0], moduleDeltaPhi);
  for (int i=numberFrontZplane; i< numberZplane; i++) 
     {
       double tempZ=zCoordinate[i];
       if (i==4) (tempZ = tempZ + betweenWheel) ;  // start the rear module after the inter-wheel gap.
       solidRearModule->addPlane(tempZ, innerRadius[i], moduleRouter);
     }
  const GeoLogVol* logicRearModule = new GeoLogVol(rearmoduleName, solidRearModule , LAr);
  GeoPhysVol* physiRearModule = new GeoPhysVol(logicRearModule);



  // At this point we have a physiFrontHEC and a physiRearHEC
  // as well as a physiFrontModule and a physiRearModule  ..............................

  // Now "multiply" the modules and insert them into the LAr HEC volumes

  //----------------------------------------------------------------
  //   Place FrontModules into FrontHEC wheel and RearModule into RearHEC wheel
  //----------------------------------------------------------------
  GeoGenfun::Variable    Index;
  GeoGenfun::GENFUNCTION ModuleRotationAngle = -moduleDeltaPhi + moduleDeltaPhi*Index; 
  GeoXF::TRANSFUNCTION tf = GeoXF::Pow(GeoTrf::RotateZ3D(1.0),ModuleRotationAngle);


  GeoSerialIdentifier  *sIF = new GeoSerialIdentifier(1);
  GeoSerialTransformer *sTF = new GeoSerialTransformer (physiFrontModule,&tf, moduleNumberFront);
  physiFrontHEC->add(sIF);
  physiFrontHEC->add(sTF);
    
  GeoGenfun::GENFUNCTION ModuleRotationAngleR = -moduleDeltaPhi/2. + moduleDeltaPhi*Index; 
  GeoXF::TRANSFUNCTION tr = GeoXF::Pow(GeoTrf::RotateZ3D(1.0),ModuleRotationAngleR);

  GeoSerialIdentifier  *sIR = new GeoSerialIdentifier(2);
  GeoSerialTransformer *sTR = new GeoSerialTransformer (physiRearModule,&tr, moduleNumberRear);
  physiRearHEC->add(sIR);
  physiRearHEC->add(sTR);

  
  //----------------------------------------------------------------
  //    Sensitive slicegap - Front
  //----------------------------------------------------------------
  for(int islice=0; islice<2; islice++)
    {
      if (islice==0) 
	{solidSlice[islice] = new GeoTubs(moduleRinner1,moduleRouter,gapSize/2.,
						modulePhistart[0],moduleDeltaPhi); }
      else
	{solidSlice[islice] = new GeoTubs(moduleRinner2,moduleRouter,gapSize/2.,
						modulePhistart[0],moduleDeltaPhi); }
      logiSlice[islice]  = new GeoLogVol(sliceFrontName, solidSlice[islice], LAr);
      physiSlice[islice] = new GeoPhysVol(logiSlice[islice]);
    }

  //----------------------------------------------------------------
  //    Sensitive slicegap - Rear
  //----------------------------------------------------------------
  for(int islice=2; islice<3; islice++)
    {
      solidSlice[islice] = new GeoTubs(moduleRinner2,moduleRouter,gapSize/2.,
					    modulePhistart[0],moduleDeltaPhi);   
      logiSlice[islice]  = new GeoLogVol(sliceRearName, solidSlice[islice], LAr);
      physiSlice[islice] = new GeoPhysVol(logiSlice[islice]);
    }


  //----------------------------------------------------------------
  //    Absorbers , the inner and outer Radius are smaller by radialShift
  //                but positionned in the center of depth. this alows
  //                to have 2 Gaudi::Units::mm gap between the copper plates of neighbor FrontModules 
  //----------------------------------------------------------------

  // Two different Absorbers for the front depths: 
  solidFrontAbsorber[0] = new GeoTubs(moduleRinner1-radialShift,moduleRouter-radialShift,frontAbsThickness/2.,
				      modulePhistart[0],moduleDeltaPhi);                   
  solidFrontAbsorber[1] = new GeoTubs(moduleRinner2-radialShift,moduleRouter-radialShift,frontAbsThickness/2.,
				      modulePhistart[0],moduleDeltaPhi);                   

  for (int frontabsorberNo=0;frontabsorberNo<2;frontabsorberNo++)
    {
      logiFrontAbsorber[frontabsorberNo]  = new GeoLogVol(absorberFrontName, 
							  solidFrontAbsorber[frontabsorberNo], Copper);
      physiFrontAbsorber[frontabsorberNo] = new GeoPhysVol(logiFrontAbsorber[frontabsorberNo]);
    }

  // there is only one rearabsorber for the two rear depths:
  solidRearAbsorber = new GeoTubs(moduleRinner2-radialShift,moduleRouter-radialShift,rearAbsThickness/2.,
				  modulePhistart[0],moduleDeltaPhi);                   
  logiRearAbsorber  = new GeoLogVol(absorberRearName,solidRearAbsorber, Copper);
  physiRearAbsorber = new GeoPhysVol(logiRearAbsorber);


  //----------------------------------------------------------------
  //    First Absorbers at the "entrance" of each wheel:
  //----------------------------------------------------------------

  solidFirstAbsorber[0] = new GeoTubs(moduleRinner1-radialShift,moduleRouter-radialShift,
					firstAbsorber[0]/2., modulePhistart[0],moduleDeltaPhi);
  logiFirstAbsorber[0]  = new GeoLogVol(firstFrontAbsorberName, solidFirstAbsorber[0], Copper);
  physiFirstAbsorber[0] = new GeoPhysVol(logiFirstAbsorber[0]);
  physiFirstAbsorber[0]->add(new GeoIdentifierTag(50));


  solidFirstAbsorber[1] = new GeoTubs(moduleRinner2-radialShift,moduleRouter-radialShift,
				       firstAbsorber[3]/2., modulePhistart[0],moduleDeltaPhi);     
  logiFirstAbsorber[1]  = new GeoLogVol(firstRearAbsorberName, solidFirstAbsorber[1], Copper);
  physiFirstAbsorber[1] = new GeoPhysVol(logiFirstAbsorber[1]);
  physiFirstAbsorber[1]->add(new GeoIdentifierTag(51));



  //----------------------------------------------------------------
  //   Place 3 depths into 1 generic FrontModule of HEC
  //----------------------------------------------------------------

  double depthPositionZ = 0.;  // initialized here - it increases as we work through the depths


  for(int idepth=0; idepth<3; idepth++)  // Front Module: idepth=0,1,2
    {
      if (idepth==0) 
	{solidDepth[idepth]=new GeoTubs(moduleRinner1,moduleRouter,depthSize[idepth]/2.,
						 modulePhistart[0],moduleDeltaPhi);}
      else
	{solidDepth[idepth]=new GeoTubs(moduleRinner2,moduleRouter,depthSize[idepth]/2.,
						 modulePhistart[0],moduleDeltaPhi);}

      logiDepth[idepth] =new GeoLogVol(depthFrontName, solidDepth[idepth], LAr);
      physiDepth[idepth]=new GeoPhysVol(logiDepth[idepth]);


      // Placement of the actual depth:
      depthPositionZ +=depthSize[idepth]/2.;
      physiFrontModule->add(new GeoIdentifierTag(100+idepth));
      physiFrontModule->add(new GeoTransform(GeoTrf::Translate3D(0,0,depthPositionZ)));     
      physiFrontModule->add(physiDepth[idepth]);     




      // Now put absorbers into the depth:
      depthPositionZ +=depthSize[idepth]/2.;       
      if (idepth==2)  depthPositionZ +=betweenWheel; 
      


      // Position of 8 sensitive gaps and absorbers in 3 depths of a Front Module
      int islice = 1;
      int frontabsorberNo = 1;
      if(idepth==0) {  islice=0;  frontabsorberNo=0;  } 
      else          {  islice=2;  frontabsorberNo=1;  } 
      
      double slicePositionZ=firstAbsorber[idepth]+ gapSize/2.0 -depthSize[idepth]/2.0;
      GeoGenfun::Variable Index;
      GeoXF::TRANSFUNCTION TS = GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),slicePositionZ + (frontAbsThickness+gapSize)*Index);
      GeoXF::TRANSFUNCTION TA = GeoTrf::TranslateY3D(-radialShift)*GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),
						absorberPositionZ[idepth] + (frontAbsThickness+gapSize)*Index);
      GeoSerialIdentifier *sI = new GeoSerialIdentifier(sliceCopyNo[idepth]);
      GeoSerialTransformer *sTS = new GeoSerialTransformer(physiSlice[islice],   &TS, gapNumber[idepth]);
      GeoSerialTransformer *sTA = new GeoSerialTransformer(physiFrontAbsorber[frontabsorberNo],&TA, gapNumber[idepth]);
      physiDepth[idepth]->add(sI);
      physiDepth[idepth]->add(sTS);
      physiDepth[idepth]->add(sI);
      physiDepth[idepth]->add(sTA);

    } //for idepth     
  

  //----------------------------------------------------------------
  //   Place 2 depths into 1 generic Rear Module of HEC
  //----------------------------------------------------------------
  
  for(int idepth=3; idepth<5; idepth++)
    {
      depthPositionZ +=depthSize[idepth]/2.;

      // Now depth
      solidDepth[idepth] = new GeoTubs(moduleRinner2,moduleRouter,depthSize[idepth]/2.,
					       modulePhistart[0],moduleDeltaPhi);
      logiDepth[idepth]  = new GeoLogVol(depthRearName, solidDepth[idepth], LAr);
      physiDepth[idepth] = new GeoPhysVol(logiDepth[idepth]);

      physiRearModule->add(new GeoIdentifierTag(100+idepth));
      physiRearModule->add(new GeoTransform(GeoTrf::Translate3D(0,0,depthPositionZ)));     
      physiRearModule->add(physiDepth[idepth]);

     
      depthPositionZ +=depthSize[idepth]/2.;


      // Position 4 sensitive gaps and rear absorbers in 2 depths of a Rear Module:
      int islice=2;
      double slicePositionZ=firstAbsorber[idepth]+ gapSize/2.0 -depthSize[idepth]/2.0;      
      GeoGenfun::Variable Index;
      GeoXF::TRANSFUNCTION TS = GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),slicePositionZ + (rearAbsThickness+gapSize)*Index);
      GeoXF::TRANSFUNCTION TA = GeoTrf::TranslateY3D(-radialShift)*GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),
					        absorberPositionZ[idepth] + (rearAbsThickness+gapSize)*Index);
      GeoSerialIdentifier *sI = new GeoSerialIdentifier(sliceCopyNo[idepth]);
      GeoSerialTransformer *sTS = new GeoSerialTransformer(physiSlice[islice],   &TS, gapNumber[idepth]);
      GeoSerialTransformer *sTA = new GeoSerialTransformer(physiRearAbsorber,&TA, gapNumber[idepth]);
      physiDepth[idepth]->add(sI);
      physiDepth[idepth]->add(sTS);
      physiDepth[idepth]->add(sI);
      physiDepth[idepth]->add(sTA);

    } //for idepth     



  // Add the 1/2 thickness front plates to the two modules:

  double firstAbsorberPositionZ = firstAbsorber[0]/2.- depthSize[0]/2.0;
  physiDepth[0]->add(new GeoIdentifierTag(50));
  physiDepth[0]->add(new GeoTransform(GeoTrf::Translate3D(0,-radialShift,firstAbsorberPositionZ)));     
  physiDepth[0]->add(physiFirstAbsorber[0]);     

  
  firstAbsorberPositionZ = firstAbsorber[3]/2.- depthSize[3]/2.0;
  physiDepth[3]->add(new GeoIdentifierTag(51));
  physiDepth[3]->add(new GeoTransform(GeoTrf::Translate3D(0,-radialShift,firstAbsorberPositionZ)));     
  physiDepth[3]->add(physiFirstAbsorber[1]);     
  


  //----------------------------------------------------------------
  //    Electronic boards of Front/Rear Slice
  //       0, 1: front; 2: rear
  //----------------------------------------------------------------

  for(int islice=0; islice<3; islice++)
    {
      int indexKapton=1;  
      if (islice==0)
	{
	  solidPadBoard = new GeoTubs(moduleRinner1,moduleRouter,copperPad/2.,
				      modulePhistart[0],moduleDeltaPhi);                   
	  solidEstBoard = new GeoTubs(moduleRinner1,moduleRouter,(kaptonWidth[indexKapton]/2.+kaptonWidth[0]),
				      modulePhistart[0],moduleDeltaPhi);
	}
      else
	{
	  solidPadBoard = new GeoTubs(moduleRinner2,moduleRouter,copperPad/2.,
				      modulePhistart[0],moduleDeltaPhi);                   
	  solidEstBoard = new GeoTubs(moduleRinner2,moduleRouter,(kaptonWidth[indexKapton]/2.+kaptonWidth[0]),
				      modulePhistart[0],moduleDeltaPhi);
	}

      std::string copperName, electrodeName;
      if (islice == 2) {
        // Rear
        copperName = copperRearName;
        electrodeName = electrodeRearName;
      }
      else {
        // Front
        copperName = copperFrontName;
        electrodeName = electrodeFrontName;
      }
      logiPadBoard  = new GeoLogVol(copperName, solidPadBoard, Copper);
      logiEstBoard  = new GeoLogVol(electrodeName, solidEstBoard, Kapton );


      double kaptonPositionZ = kaptonPosition[indexKapton]-gapSize/2.;

      physiEstBoard = new GeoPhysVol(logiEstBoard);
      physiSlice[islice]->add(new GeoIdentifierTag(indexKapton));
      physiSlice[islice]->add(new GeoTransform(GeoTrf::Translate3D(0,0,kaptonPositionZ)));     
      physiSlice[islice]->add(physiEstBoard);     


      physiPadBoard = new GeoPhysVol(logiPadBoard);
      physiEstBoard->add(new GeoIdentifierTag(indexKapton));
      physiEstBoard->add(new GeoTransform(GeoTrf::Translate3D(0,0,0)));     
      physiEstBoard->add(physiPadBoard);     

    }//for islice


  //----------------------------------------------------------------
  //    Tie rods in Slice
  //----------------------------------------------------------------

  //  double rodSize = 0.85*Gaudi::Units::cm;
  for (int iwheel=0; iwheel<2; iwheel++) 
    { 
      solidTieRod[iwheel] = new GeoTubs(0.*Gaudi::Units::cm,spacerDiameter[iwheel]/2.,rodSize/2., 0.*Gaudi::Units::deg,360.*Gaudi::Units::deg); 
      logiTieRod[iwheel] = new GeoLogVol(tieRodName, solidTieRod[iwheel], Iron);     
    }

  for(int islice=0; islice<3; islice++) { // loop over the 3 longitdinal depths
    int numberTie=0;
    if(islice==2) numberTie=1;       // for the rear module

    int indexRod;
    for(indexRod=1; indexRod<4; indexRod++) {  // place all 7 rods: 3 pairs, one single 
      for(int iz=0;iz<2;iz++){

	physiTieRod[numberTie] = new GeoPhysVol(logiTieRod[numberTie]);
        physiSlice[islice]->add(new GeoIdentifierTag(indexRod));
        physiSlice[islice]->add(new GeoTransform(GeoTrf::Translate3D(tieRodPositionX[indexRod],
                                                      -tieRodPositionY[indexRod],ztie[iz])));     
        physiSlice[islice]->add(physiTieRod[numberTie]);     


 	physiTieRod[numberTie] = new GeoPhysVol(logiTieRod[numberTie]);
        physiSlice[islice]->add(new GeoIdentifierTag(indexRod));
        physiSlice[islice]->add(new GeoTransform(GeoTrf::Translate3D(-tieRodPositionX[indexRod],
                                                      -tieRodPositionY[indexRod],ztie[iz])));     
        physiSlice[islice]->add(physiTieRod[numberTie]);     

      }//for iz (dealt with the rods that come in pairs)
    } // for indexRod     
    for( int iz1=0;iz1<2;iz1++)
      {   

	physiTieRod[numberTie] = new GeoPhysVol(logiTieRod[numberTie]);
        physiSlice[islice]->add(new GeoIdentifierTag(indexRod));
        physiSlice[islice]->add(new GeoTransform(GeoTrf::Translate3D(tieRodPositionX[0],-tieRodPositionY[0],ztie[iz1]))); 
        physiSlice[islice]->add(physiTieRod[numberTie]);     

      }//for iz1  (this dealt with the one last un-paired rod at the module's narrow end)
  } // for islice     

  //----------------------------------------------------------------
  //    Tie rods in Absorbers
  //----------------------------------------------------------------
  solidAbsorberTieRod[0] = new GeoTubs(0.*Gaudi::Units::cm,tieRodDiameter[0]/2.,frontAbsThickness/2.,0.*Gaudi::Units::deg,360.*Gaudi::Units::deg);
  solidAbsorberTieRod[1] = new GeoTubs(0.*Gaudi::Units::cm,tieRodDiameter[1]/2.,rearAbsThickness/2.,0.*Gaudi::Units::deg,360.*Gaudi::Units::deg); 
  logiAbsorberTieRod[0]  = new GeoLogVol(tieRodName,solidAbsorberTieRod[0],Iron);  //,0,0,0);
  logiAbsorberTieRod[1]  = new GeoLogVol(tieRodName,solidAbsorberTieRod[1],Iron);  //,0,0,0);
  solidAbsorberTieRodRear[0] = new GeoTubs(0.*Gaudi::Units::cm,tieRodDiameter[0]/2.,frontAbsThickness/2.,0.*Gaudi::Units::deg,360.*Gaudi::Units::deg);
  solidAbsorberTieRodRear[1] = new GeoTubs(0.*Gaudi::Units::cm,tieRodDiameter[1]/2.,rearAbsThickness/2.,0.*Gaudi::Units::deg,360.*Gaudi::Units::deg); 
  logiAbsorberTieRodRear[0]  = new GeoLogVol(tieRodRearName,solidAbsorberTieRodRear[0],Iron);  //,0,0,0);
  logiAbsorberTieRodRear[1]  = new GeoLogVol(tieRodRearName,solidAbsorberTieRodRear[1],Iron);  //,0,0,0);


  for(int islice=0; islice<2; islice++)
    {  
      int indexR=0;
      //if(islice>1) indexR=1;
      int indexRod;
      for(indexRod=1; indexRod<4; indexRod++)
        {
          physiAbsorberTieRod[indexR] = new GeoPhysVol(logiAbsorberTieRod[indexR]);
          physiFrontAbsorber[islice]->add(new GeoIdentifierTag(indexRod));
          physiFrontAbsorber[islice]->add(new GeoTransform(GeoTrf::Translate3D(tieRodPositionX[indexRod],
                                                      -(tieRodPositionY[indexRod]-radialShift), 0)));     
          physiFrontAbsorber[islice]->add(physiAbsorberTieRod[indexR]);     
          physiAbsorberTieRod[indexR] = new GeoPhysVol(logiAbsorberTieRod[indexR]);
          physiFrontAbsorber[islice]->add(new GeoIdentifierTag(indexRod));
          physiFrontAbsorber[islice]->add(new GeoTransform(GeoTrf::Translate3D(-tieRodPositionX[indexRod],
                                                      -(tieRodPositionY[indexRod]-radialShift), 0)));     
          physiFrontAbsorber[islice]->add(physiAbsorberTieRod[indexR]);     
	}     
    physiAbsorberTieRod[indexR] = new GeoPhysVol(logiAbsorberTieRod[indexR]);
    physiFrontAbsorber[islice]->add(new GeoIdentifierTag(indexRod));
    physiFrontAbsorber[islice]->add(new GeoTransform(GeoTrf::Translate3D(tieRodPositionX[0],
                                                -(tieRodPositionY[0]-radialShift),0)));     


    physiFrontAbsorber[islice]->add(physiAbsorberTieRod[indexR]);     

    }


  int indexRod=1;
  for(indexRod=1; indexRod<4; indexRod++)
        {
          physiAbsorberTieRodRear[1] = new GeoPhysVol(logiAbsorberTieRodRear[1]);
          physiRearAbsorber->add(new GeoIdentifierTag(indexRod));
          physiRearAbsorber->add(new GeoTransform(GeoTrf::Translate3D(tieRodPositionX[indexRod],
                                                      -(tieRodPositionY[indexRod]-radialShift), 0)));     
          physiRearAbsorber->add(physiAbsorberTieRodRear[1]);     

          physiAbsorberTieRodRear[1] = new GeoPhysVol(logiAbsorberTieRodRear[1]);
          physiRearAbsorber->add(new GeoIdentifierTag(indexRod));
          physiRearAbsorber->add(new GeoTransform(GeoTrf::Translate3D(-tieRodPositionX[indexRod],
                                                      -(tieRodPositionY[indexRod]-radialShift), 0)));     


          physiRearAbsorber->add(physiAbsorberTieRodRear[1]);     
	}   


    //now the single tie rod at the narrow end:
    physiAbsorberTieRodRear[1] = new GeoPhysVol(logiAbsorberTieRodRear[1]);
    physiRearAbsorber->add(new GeoIdentifierTag(indexRod));
    physiRearAbsorber->add(new GeoTransform(GeoTrf::Translate3D(tieRodPositionX[0],
                                                -(tieRodPositionY[0]-radialShift),0)));     


    physiRearAbsorber->add(physiAbsorberTieRodRear[1]);     

    
     

  return m_h6Phys;

}


