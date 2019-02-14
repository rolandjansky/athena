/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_DetFactory.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoSerialDenominator.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"

// Author Soumya Mohapatra
// soumya@cern.ch

// This is the ZDC Geomodel description :: see some details at
// https://atlasop.cern.ch/atlas-point1/twiki/pub/Main/ZDCOperationManualShifter/zdc_layout.png
 
//I mave assumed that each module is 154mm which is 4 mm longer than the ones shown in the above webpage
// thus my modules are 13.4cm tungsten +1cm steel on either side

//I have assumed that the cavity is 1016mm (4*4=16mm larger than the one in the above webpage)
//Ionization chamber material is air currently

using namespace GeoGenfun;
using namespace GeoXF;

ZDC_DetFactory::ZDC_DetFactory(StoreGateSvc* detStore) : m_detectorManager(NULL) , m_detectorStore(detStore) {}

ZDC_DetFactory::~ZDC_DetFactory() {}

void ZDC_DetFactory::create(GeoPhysVol* world)
{
  m_detectorManager = new ZDC_DetManager();

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) return;

  //------------------------------------------------------------------------------------------------------------
  //List of materials

  const GeoMaterial* air = materialManager->getMaterial("std::Air");

  GeoElement*  Oxygen   = new GeoElement ("Oxygen"  ,"O"  ,  8.0 ,  16.0*GeoModelKernelUnits::g/Gaudi::Units::mole);
  GeoElement*  Sillicon = new GeoElement ("Sillicon","Si" , 14.0 ,  28.085*GeoModelKernelUnits::g/Gaudi::Units::mole);
  GeoElement*  Tung     = new GeoElement ("Tungsten","W"  , 74.0 , 183.84*GeoModelKernelUnits::g/Gaudi::Units::mole);
  GeoElement*  Iron     = new GeoElement ("Iron"    ,"Fe" , 26.0 ,  55.845 *GeoModelKernelUnits::g/Gaudi::Units::mole);
  GeoElement*  Carbon   = new GeoElement ("Carbon"  ,"C"  ,  6.0 ,  12.0107*GeoModelKernelUnits::g/Gaudi::Units::mole);
  GeoElement*  Nickel   = new GeoElement ("Nickel"  ,"Ni" , 28.0 ,  58.6934*GeoModelKernelUnits::g/Gaudi::Units::mole);


  GeoMaterial* Quartz   = new GeoMaterial("Quartz",2.20*GeoModelKernelUnits::gram/Gaudi::Units::cm3);
  Quartz->add(Sillicon,0.467);
  Quartz->add(Oxygen,0.533);
  Quartz->lock();
  
  // Absorber composition:  savannah.cern.ch/task/download.php?file_id=22925
  GeoMaterial* Tungsten = new GeoMaterial("Tungsten",18.155*GeoModelKernelUnits::g/Gaudi::Units::cm3);
  Tungsten->add(Tung,   0.948);
  Tungsten->add(Nickel, 0.037);
  Tungsten->add(Iron,   0.015);
  Tungsten->lock();

  GeoMaterial* Steel  = new GeoMaterial("Steel", 7.9*GeoModelKernelUnits::gram/Gaudi::Units::cm3);
  Steel->add(Iron  , 0.98);
  Steel->add(Carbon, 0.02);
  Steel->lock();

  //------------------------------------------------------------------------------------------------------------
  //List of shapes and logical volumes
  //https://atlasop.cern.ch/atlas-point1/twiki/pub/Main/ZDCOperationManualShifter/zdc_layout.png

  GeoBox*  Envelope_Box    = new GeoBox (10.0*Gaudi::Units::cm/2.0 ,20.0*Gaudi::Units::cm/2.0  ,100.0*Gaudi::Units::cm/2.0);
  GeoBox*  Module_Box      = new GeoBox ( 9.0*Gaudi::Units::cm/2.0 ,18.0*Gaudi::Units::cm/2.0  , 13.4*Gaudi::Units::cm/2.0);
  GeoBox*  Steel_Plate_Box = new GeoBox ( 9.0*Gaudi::Units::cm/2.0 ,18.0*Gaudi::Units::cm/2.0  ,  1.0*Gaudi::Units::cm/2.0); 
  GeoTube* Pixel_Tube      = new GeoTube( 0.0*Gaudi::Units::mm     , 1.0*Gaudi::Units::mm/2.0  , 13.4*Gaudi::Units::cm/2.0);
  GeoTube* Strip_Tube      = new GeoTube( 0.0*Gaudi::Units::mm     , 1.5*Gaudi::Units::mm/2.0  , 18.0*Gaudi::Units::cm/2.0);

  GeoLogVol* Envelope_Logical    = new GeoLogVol("Envelope_Logical"   ,Envelope_Box    ,air);
  GeoLogVol* Steel_Plate_Logical = new GeoLogVol("Steel_Plate_Logical",Steel_Plate_Box ,Steel);
  GeoLogVol* Module_Logical      = new GeoLogVol("Module_Logical"     ,Module_Box      ,Tungsten);
  GeoLogVol* Pixel_Logical       = new GeoLogVol("Pixel_Logical"      ,Pixel_Tube      ,Quartz);
  GeoLogVol* Strip_Logical       = new GeoLogVol("Strip_Logical"      ,Strip_Tube      ,Quartz);
  //------------------------------------------------------------------------------------------------------------

  GeoAlignableTransform* RotateX;
  GeoAlignableTransform* ShiftZ;

  //------------------------------------------------------------------------------------------------------------
  //List of Physical Volumes

  GeoFullPhysVol* Module_Physical[2][4];   //First Index is for A-side or C-side, 2nd index labels the 4 modules

  for(int I=0;I<2;I++) for(int J=0;J<4;J++)
    Module_Physical[I][J] =new GeoFullPhysVol(Module_Logical);

  //Here we add the pixels into the XY modules
  //C -Side has Pixels in both Em and HM0
  //Here we are adding to EM module only (so only C-Side)

  GeoAlignableTransform* ShiftX;
  GeoAlignableTransform* ShiftY;

  for(int K=1;K<=8;K++) for(int L=1;L<=8;L++) { //8*8 Grid of pixels
      
    int K1 = 8-K; 
    int L1 = L-1;

    //So the pixel is numbered 1000+X where "X" is the channel number from the ZDC-twiki (0-63)
    GeoIdentifierTag* Pixel_ID       = new GeoIdentifierTag( 11000+L1*8+K1);  
    GeoFullPhysVol*   Pixel_Physical = new GeoFullPhysVol(Pixel_Logical);

    ShiftX = new GeoAlignableTransform(GeoTrf::TranslateX3D((4.5-K)*Gaudi::Units::cm));
    ShiftY = new GeoAlignableTransform(GeoTrf::TranslateY3D((4.5-L)*Gaudi::Units::cm));
	
    Module_Physical[0][0]->add(Pixel_ID);
    Module_Physical[0][0]->add(ShiftX);
    Module_Physical[0][0]->add(ShiftY);
    Module_Physical[0][0]->add(Pixel_Physical);
  }

  // Here we add 8*10 grid of pixels to the First Hadonic Module on both sides 
  for(int I=1; I<=2; I++)      // A side or C side
    for(int K=1;K<=8;K++)      // 8*10 Grid of pixels ( 8 along X-Axis)  
      for(int L=1;L<=10;L++) { // 8*10 Grid of pixels(10 along Y-Axis)

	int K1,L1;
	  
	// The pixel location is the same in Y for the two sides but the X is the opposite
	if (I==1) {K1=8-K; L1=L-1;}
	else      {K1=K-1; L1=L-1;}
          
	// So the Pixel is numbered 10000+X or 20000+X (10000 for A-side) and (20000 for C-Side)
	int Pix_id_raw = (L1*8+K1);  
	int Pix_id=Make_Pixel_ID(Pix_id_raw); // this merges the 80 pixels into 24 readouts
  
	GeoIdentifierTag* Pixel_ID       = new GeoIdentifierTag( I*10000+2000+ Pix_id);
	GeoFullPhysVol*   Pixel_Physical = new GeoFullPhysVol(Pixel_Logical);

	ShiftX = new GeoAlignableTransform(GeoTrf::TranslateX3D( (4.5-K)*Gaudi::Units::cm ));
	ShiftY = new GeoAlignableTransform(GeoTrf::TranslateY3D( (5.5-L)*Gaudi::Units::cm ));
	
	Module_Physical[I-1][1]->add(Pixel_ID);
	Module_Physical[I-1][1]->add(ShiftX);
	Module_Physical[I-1][1]->add(ShiftY);
	Module_Physical[I-1][1]->add(Pixel_Physical);
      }	

  //Here we add the strips into the XY modules

  for(int I=1;I<=2;I++) {         // A side or C side
    for(int J=1;J<=2;J++) {       // XY Modules (Only the first two modules have gaps for Pixels, for these there must be a 1mm gap after 6 stirps)
      for(int K=1;K<=12;K++) {    // 12 layers of strips 
	for(int L=1;L<=9;L++) {   // each layer has 9 sets of strips
	  for(int M=1;M<=6;M++) { // each set has 6 strips
		    
	    GeoFullPhysVol*   Strip_Physical = new GeoFullPhysVol  (Strip_Logical);
	    GeoIdentifierTag* Strip_ID       = new GeoIdentifierTag(I*10000+J*1000+K*12+L*10+M);
	    
	    RotateX = new GeoAlignableTransform(GeoTrf::RotateX3D   (90*Gaudi::Units::deg));
	    ShiftX  = new GeoAlignableTransform(GeoTrf::TranslateX3D((L-5.5)*Gaudi::Units::cm + (M-0.75)*1.5*Gaudi::Units::mm +0.75*Gaudi::Units::mm));
	    ShiftZ  = new GeoAlignableTransform(GeoTrf::TranslateZ3D((K*1.2 - 7.8)*Gaudi::Units::cm));
	
	    Module_Physical[I-1][J-1]->add(Strip_ID);
	    Module_Physical[I-1][J-1]->add(ShiftZ);
	    Module_Physical[I-1][J-1]->add(RotateX);
	    Module_Physical[I-1][J-1]->add(ShiftX);
	    Module_Physical[I-1][J-1]->add(Strip_Physical);
	  }  
	}	
      }  
    }
  }

  //Here we add the strips into the Hadronic modules
  for(int I=1;I<=2;I++) {         // A side or C side
    for(int J=3;J<=4;J++) {       // XY Modules (Only the first two modules have gaps for Pixels, for these there must be a 1mm gap after 6 stirps)
      for(int K=1;K<=12;K++) {    // 12 layers of strips 
	for(int L=1;L<=9;L++) {   // each layer has 9 sets of strips
	  for(int M=1;M<=6;M++) { // each set has 6 strips

	    GeoFullPhysVol*   Strip_Physical = new GeoFullPhysVol(Strip_Logical);
	    GeoIdentifierTag* Strip_ID       = new GeoIdentifierTag(I*10000 + J*1000 + K*12 + L*10 + M);

	    RotateX = new GeoAlignableTransform(GeoTrf::RotateX3D   (90*Gaudi::Units::deg));
	    ShiftX  = new GeoAlignableTransform(GeoTrf::TranslateX3D((L-5.5)*Gaudi::Units::cm + (M-0.75)*1.5*Gaudi::Units::mm + 0.75*Gaudi::Units::mm));
	    ShiftZ  = new GeoAlignableTransform(GeoTrf::TranslateZ3D((K*1.2-7.8)*Gaudi::Units::cm ));
	
	    Module_Physical[I-1][J-1]->add(Strip_ID);
	    Module_Physical[I-1][J-1]->add(ShiftZ);
	    Module_Physical[I-1][J-1]->add(RotateX);
	    Module_Physical[I-1][J-1]->add(ShiftX);
	    Module_Physical[I-1][J-1]->add(Strip_Physical);
	  }  
	}	
      }  
    }
  }
  
  //------------------------------------------------------------------------------------------------------------
  //The Top Volumes
  GeoFullPhysVol* Envelope_Physical[2]; //First Index is for A-side or C-side
  GeoFullPhysVol* Steel_Plate_Physical;   
  GeoNameTag*     tag;

  for (int I=0;I<2;I++) {

    std::string Vol_Name, Vol_Name_append;
    float sgn = 1.0;
   
    if (I==0) {sgn =-1.0; Vol_Name_append="_A"; }
    else      {sgn = 1.0; Vol_Name_append="_C"; }

    Envelope_Physical[I] = new GeoFullPhysVol(Envelope_Logical);
    Steel_Plate_Physical = new GeoFullPhysVol(Steel_Plate_Logical);

    ShiftZ  = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 0.5)*Gaudi::Units::cm*sgn));
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Steel_Plate_Physical);

    Vol_Name = "EM_XY"; Vol_Name = Vol_Name + Vol_Name_append;

    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D ((-47.2 + 1 + 6.7)*Gaudi::Units::cm*sgn));

    tag = new GeoNameTag(Vol_Name.c_str());
    Envelope_Physical[I]->add(tag);
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Module_Physical[I][0]);

    Steel_Plate_Physical = new GeoFullPhysVol(Steel_Plate_Logical);
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 +.5)*Gaudi::Units::cm*sgn));
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Steel_Plate_Physical);
  
    Steel_Plate_Physical = new GeoFullPhysVol(Steel_Plate_Logical);
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + .5)*Gaudi::Units::cm*sgn));
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Steel_Plate_Physical);

    Vol_Name = "HM_XY"; Vol_Name = Vol_Name + Vol_Name_append;
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + 1 + 6.7)*Gaudi::Units::cm*sgn));
    tag = new GeoNameTag(Vol_Name.c_str());
    Envelope_Physical[I]->add(tag);
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Module_Physical[I][1]);
  
    Steel_Plate_Physical = new GeoFullPhysVol(Steel_Plate_Logical);
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + 1 + 13.4 + .5)*Gaudi::Units::cm*sgn));
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Steel_Plate_Physical);
 
    Steel_Plate_Physical = new GeoFullPhysVol(Steel_Plate_Logical);
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + 1 + 13.4 + 1 + 3 +.5)*Gaudi::Units::cm*sgn));
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Steel_Plate_Physical);

    Vol_Name = "HM_01"; Vol_Name = Vol_Name + Vol_Name_append;
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + 1 + 13.4 + 1 + 3 + 1 + 6.7)*Gaudi::Units::cm*sgn ));
    tag = new GeoNameTag(Vol_Name.c_str());
    Envelope_Physical[I]->add(tag);
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Module_Physical[I][2]);
  
    Steel_Plate_Physical = new GeoFullPhysVol(Steel_Plate_Logical);
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + 1 + 13.4 + 1 + 3 + 1 + 13.4 + .5)*Gaudi::Units::cm*sgn));
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Steel_Plate_Physical);

    Steel_Plate_Physical = new GeoFullPhysVol(Steel_Plate_Logical);
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + 1 + 13.4 + 1 + 3 + 1 + 13.4 + 1 + .5)*Gaudi::Units::cm*sgn));
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Steel_Plate_Physical);

    Vol_Name = "HM_02"; Vol_Name = Vol_Name + Vol_Name_append;
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + 1 + 13.4 + 1 + 3 + 1 + 13.4 + 1 + 1 + 6.7)*Gaudi::Units::cm*sgn ));
    tag = new GeoNameTag(Vol_Name.c_str());
    Envelope_Physical[I]->add(tag);
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Module_Physical[I][3]);
  
    Steel_Plate_Physical = new GeoFullPhysVol(Steel_Plate_Logical);
    ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D((-47.2 + 1 + 13.4 + 1 + 6 + 10 + 1 + 13.4 + 1 + 3 + 1 + 13.4 + 1 + 1 + 13.4 + .5)*Gaudi::Units::cm*sgn));
    Envelope_Physical[I]->add(ShiftZ);
    Envelope_Physical[I]->add(Steel_Plate_Physical);
  }
 
  //------------------------------------------------------------------------------------------------------------
  //Insert into World
  
  tag = new GeoNameTag("ZDC_A");

  world->add(tag);

  ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D(-141.580*Gaudi::Units::m));

  world->add(ShiftZ);
  world->add(Envelope_Physical[0]);

  m_detectorManager->addTreeTop(Envelope_Physical[0]);

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " ZDC DetFactory ADDED TOP VOLUME "
					    << Envelope_Physical[0]->getAbsoluteName() 
					    << endmsg;
  
  tag = new GeoNameTag("ZDC_C");
  
  world->add(tag);
  
  ShiftZ = new GeoAlignableTransform(GeoTrf::TranslateZ3D(141.580 *Gaudi::Units::m));

  world->add(ShiftZ);
  world->add(Envelope_Physical[1]);

  m_detectorManager->addTreeTop(Envelope_Physical[1]);

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << " ZDC DetFactory ADDED TOP VOLUME "
					    << Envelope_Physical[1]->getAbsoluteName() 
					    << endmsg;
}

const ZDC_DetManager* ZDC_DetFactory::getDetectorManager() const { return m_detectorManager; }

// if you change this function then also make changes in PixelSD::EndOfEvent
// CHANGE Kmax=80 THERE, IF YOU WANT EACH PIXEL IN HMXY TO BE READ OUT SEPARATELY

int ZDC_DetFactory::Make_Pixel_ID( int Pid ) {

  if (Pid==0  || Pid==1  || Pid==8  || Pid==9 ) return  0;
  if (Pid==2  || Pid==3  || Pid==10 || Pid==11) return  1;  
  if (Pid==4  || Pid==5  || Pid==12 || Pid==13) return  2;  
  if (Pid==6  || Pid==7  || Pid==14 || Pid==15) return  3;  

  if (Pid==16 || Pid==17 || Pid==25           ) return  4;  
  if (Pid==18 || Pid==19 || Pid==27           ) return  5;  
  if (Pid==20 || Pid==21 || Pid==29           ) return  6;  
  if (Pid==22 || Pid==23 || Pid==31           ) return  7;  

  if (Pid==24 || Pid==32 || Pid==33           ) return  8;  
  if (Pid==26 || Pid==34 || Pid==35           ) return  9;  
  if (Pid==28 || Pid==36 || Pid==37           ) return 10;  
  if (Pid==30 || Pid==38 || Pid==39           ) return 11;
  
  if (Pid==40 || Pid==41 || Pid==49           ) return 12;  
  if (Pid==42 || Pid==43 || Pid==51           ) return 13;  
  if (Pid==44 || Pid==45 || Pid==53           ) return 14;  
  if (Pid==46 || Pid==47 || Pid==55           ) return 15;  

  if (Pid==48 || Pid==56 || Pid==57           ) return 16;  
  if (Pid==50 || Pid==58 || Pid==59           ) return 17;  
  if (Pid==52 || Pid==60 || Pid==61           ) return 18;  
  if (Pid==54 || Pid==62 || Pid==63           ) return 19;  

  if (Pid==64 || Pid==65 || Pid==72 || Pid==73) return 20;
  if (Pid==66 || Pid==67 || Pid==74 || Pid==75) return 21;  
  if (Pid==68 || Pid==69 || Pid==76 || Pid==77) return 22;  
  if (Pid==70 || Pid==71 || Pid==78 || Pid==79) return 23;  

  return -1;
} 
