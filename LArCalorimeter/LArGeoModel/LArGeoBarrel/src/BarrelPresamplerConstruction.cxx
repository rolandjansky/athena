/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoBarrel/BarrelPresamplerConstruction.h"
#include "LArGeoCode/VDetectorParameters.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoXF.h"

// volumes used:  Pcon, Tubs, Cons, Box, Trap 
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

// For transforms:
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"

// For units:
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

// For Functions:

// For functions:
#include "GeoGenericFunctions/Abs.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "GeoGenericFunctions/Sqrt.h"
#include "GeoGenericFunctions/ATan.h"
#include "GeoGenericFunctions/Rectangular.h"
#include "GeoGenericFunctions/Mod.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/FixedConstant.h"

// Constructor;
LArGeo::BarrelPresamplerConstruction ::BarrelPresamplerConstruction(bool fullGeo, int itb):
  m_parameters(LArGeo::VDetectorParameters::GetInstance()),
  m_psPhysicalPos(NULL),
  m_psPhysicalNeg(NULL),
  m_fullGeo(fullGeo)
{

  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in BarrelPresamplerConstruction, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "BarrelPresamplerConstruction"); 
  

  log << MSG::INFO;
  
  log  << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         Start of Barrel PS GeoModel definition   +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorFactory, cannot access DetectorStore");
  }
  

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) {
    throw std::runtime_error("Error in BarrelPresamplerConstruction, stored MaterialManager is not found.");
  }
  
  const GeoMaterial *Copper  = materialManager->getMaterial("std::Copper");
  if (!Copper) throw std::runtime_error("Error in BarrelPresamplerConstruction, std::Copper is not found.");
  
  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in BarrelPresamplerConstruction, std::Iron is not found.");
  
  const GeoMaterial *Lead  = materialManager->getMaterial("std::Lead");
  if (!Lead) throw std::runtime_error("Error in BarrelPresamplerConstruction, std::Lead is not found.");
  
  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in BarrelPresamplerConstruction, std::LiquidArgon is not found.");
  
  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in BarrelPresamplerConstruction, std::Air is not found.");
  
  const GeoMaterial *Kapton  = materialManager->getMaterial("std::Kapton");
  if (!Kapton) throw std::runtime_error("Error in BarrelPresamplerConstruction, std::Kapton is not found."); 
  const GeoMaterial *Glue  = materialManager->getMaterial("LAr::Glue");
  if (!Glue) throw std::runtime_error("Error in BarrelPresamplerConstruction, LAr::Glue is not found.");
  
  const GeoMaterial *G10  = materialManager->getMaterial("LAr::G10");
  if (!G10) throw std::runtime_error("Error in BarrelPresamplerConstruction, LAr::G10 is not found.");
  
  const GeoMaterial *FR4  = materialManager->getMaterial("LAr::FR4");
  if (!FR4) throw std::runtime_error("Error in BarrelPresamplerConstruction, LAr::FR4 is not found.");

  const GeoMaterial *MBMat  = materialManager->getMaterial("LAr::MBMat");
  if (!MBMat) throw std::runtime_error("Error in BarrelPresamplerConstruction, LAr::MBMat is not found.");

  const GeoMaterial *AnodeMat  = materialManager->getMaterial("LAr::AnodeMat");
  if (!AnodeMat) throw std::runtime_error("Error in BarrelPresamplerConstruction, LAr::AnodeMat is not found.");

  const GeoMaterial *CathodeMat  = materialManager->getMaterial("LAr::CathodeMat");
  if (!CathodeMat) throw std::runtime_error("Error in BarrelPresamplerConstruction, LAr::CathodeMat is not found.");

  const GeoMaterial *ConnecMat  = materialManager->getMaterial("LAr::ConnecMat");
  if (!ConnecMat) throw std::runtime_error("Error in BarrelPresamplerConstruction, LAr::ConnecMat is not found.");
  //  double rMinPresamplerMother   =1385*Gaudi::Units::mm;
  double rMinPresamplerMother   =1410*Gaudi::Units::mm;
  double rMaxPresamplerMother   =1447*Gaudi::Units::mm-0.001*Gaudi::Units::mm;
  double presamplerMother_length=1549*Gaudi::Units::mm;
  double Phi_min=0.*Gaudi::Units::deg;
  double Phi_span=360.*Gaudi::Units::deg;
  int nbsectors=32;

  if (itb==1) {
     Phi_min=-0.5*Gaudi::Units::deg;
     Phi_span=23.5*Gaudi::Units::deg;
     nbsectors=2;
  }

  double mod[8][6];

  mod[0][0] = 286.4; 
  mod[0][1] = 56;    
  mod[0][2] = 56;
  mod[0][3] = -25.;
  mod[0][4] = 4.987;
  mod[0][5] = 4.2;  
 
  mod[1][0] = 295.74;  
  mod[1][1] = 64;
  mod[1][2] = 64;
  mod[1][3] = -12.;
  mod[1][4] = 4.621;
  mod[1][5] = 0.3;
 
  mod[2][0] = 321.1;
  mod[2][1] = 72;
  mod[2][2] = 72;
  mod[2][3] = 0;
  mod[2][4] = 4.46;
  mod[2][5] = 0.9;
 
  mod[3][0] = 356.8;
  mod[3][1] = 80;
  mod[3][2] = 80;
  mod[3][3] = 0;
  mod[3][4] = 4.46;
  mod[3][5] = 0.9; 
 
  mod[4][0] = 404.8;
  mod[4][1] = 88;
  mod[4][2] = 88;
  mod[4][3] = 0;
  mod[4][4] = 4.6;
  mod[4][5] = 0.9;
 
  mod[5][0] = 478.4;
  mod[5][1] = 104;
  mod[5][2] = 104;
  mod[5][3] = 0;
  mod[5][4] = 4.6;
  mod[5][5] = 0.95;
 
  mod[6][0] = 563.2;
  mod[6][1] = 128;
  mod[6][2] = 128;
  mod[6][3] = 0;
  mod[6][4] = 4.4;
  mod[6][5] = 1.05;
 
  mod[7][0] = 380.6;
  mod[7][1] = 86;
  mod[7][2] = 87;
  mod[7][3] = 0;
  mod[7][4] = 4.4;
  mod[7][5] = 0.95;

  // Make a presampler:
  std::string basename = "LAr::Barrel::Presampler";
  {
    GeoTubs *tubs = new GeoTubs(rMinPresamplerMother, rMaxPresamplerMother, presamplerMother_length,Phi_min, Phi_span);
    GeoLogVol* logVol = new GeoLogVol(basename,tubs,LAr);
    m_psPhysicalPos = new GeoFullPhysVol(logVol);
    m_psPhysicalNeg = new GeoFullPhysVol(logVol);
    m_psPhysicalPos->ref();
    m_psPhysicalNeg->ref();
  }

  // Make a presampler sector:
  if(m_fullGeo){
    // ?    
    double  epsil = 0.007*Gaudi::Units::mm;

    //  contraction factor 
    double  cmm = (1-0.0026)*Gaudi::Units::mm;
 
    double  mod_leng[8];
    for(int ii=0; ii<8; ii++ ) mod_leng[ii]=mod[ii][0]*cmm+2*epsil;

    double  mod_heig[8];
    double larheight = 13*Gaudi::Units::mm;

    double prep1_th = 1.*Gaudi::Units::mm;                 // bottom prepreg layer 
    double prep2_th = 4.5*Gaudi::Units::mm;   
    double smallLength = 275.6*Gaudi::Units::mm;
    double bigLength = 277.5;
    double prep1_height = (smallLength/2+1.)*cmm;
    double larheight2 = larheight*cos(-mod[1][3]*Gaudi::Units::deg)*Gaudi::Units::mm;
    mod_heig[0]= (larheight+prep1_th+prep2_th)*cmm+4*epsil;
    mod_heig[1]= (larheight2+prep1_th+prep2_th)*cmm+5.*epsil;
    for(int  i=2; i<8; i++ ) mod_heig[i] = mod_heig[0];

    double shell_th = 0.4*Gaudi::Units::mm; 
    double rail_th  = 8.6*Gaudi::Units::mm;
    double mech_clear = 0.5*Gaudi::Units::mm;


    double  mb_length = 3100.3;
    double  sector_length =  mb_length*cmm +9.*epsil;
    double  sector_height =  mod_heig[0]+(shell_th+rail_th)*cmm+mech_clear*Gaudi::Units::mm+3*epsil; 

    unsigned int nsectors=32;
    double mod_xm = prep1_height+epsil;
    double mod_xp = (bigLength/2+1.+prep2_th*tan((360./(2*nsectors))*Gaudi::Units::deg))*cmm;
    double sect_xm = mod_xm+epsil;
    double sect_xp = sect_xm+sector_height*tan((360./(2*nsectors))*Gaudi::Units::deg);	    
    double rpres = 1426.*Gaudi::Units::mm;

    double zpres = -presamplerMother_length+sector_length/2+epsil;

    GeoTrd      *trd           = new GeoTrd(sect_xm, sect_xp, sector_length/2, sector_length/2, sector_height/2);

    GeoLogVol   *logVol        = new GeoLogVol (basename+"::Sector",trd,LAr);
    GeoPhysVol  *sectorPhysVol = new GeoPhysVol(logVol);

    GeoGenfun::Variable I;
    double dphiSector = (360.*Gaudi::Units::deg)/nsectors;
    GeoGenfun::GENFUNCTION  f = dphiSector*I+0.5*dphiSector;
    GeoXF::TRANSFUNCTION t = GeoXF::Pow(GeoTrf::RotateZ3D(1.0),f)*GeoTrf::TranslateX3D(rpres)*GeoTrf::TranslateZ3D(zpres)*GeoTrf::RotateZ3D(90*Gaudi::Units::deg)*GeoTrf::RotateX3D(90*Gaudi::Units::deg);
    GeoSerialTransformer *st = new GeoSerialTransformer(sectorPhysVol,&t, nbsectors);

    m_psPhysicalPos->add(st);
    m_psPhysicalNeg->add(st);


    // Now there appear to be eight different things in the sectors:

    // 1) Eight modules.
    // 2) A protection shell
    //-------------------------------Eight modules-------------------------------//
    // recompute length of module 0 and 1 to have avoid overshoorting of first cathode of module 1
    // into module 0  => reduce module 0 length by 0.5*lar_height*tan(tilt angle)
    //                    and increase module 1 length by same amount
    double delta01 =  0.5*larheight*tan(-mod[1][3]*Gaudi::Units::deg);   // delta01 is >0
    mod_leng[0]=mod_leng[0]-delta01; 
    mod_leng[1]=mod_leng[1]+delta01; 
    GeoPhysVol* pvModule[8];
    {

      double modYPrev=0, modLenPrev=0;
      for (int m=0;m<8;m++) {

//  	double modLen=mod[m][0]*cmm+2*epsil;
  	double modLen=mod_leng[m];
	

	double modY= (m==0) ?  -sector_length/2+modLen/2+epsil : modYPrev + modLenPrev /2 + modLen/2 +epsil;
	double modZ= (m==1) ?  -sector_height/2+shell_th*cmm+mech_clear+mod_heig[0]/2+epsil+(mod_heig[0]-mod_heig[1])/2 : -sector_height/2+shell_th*cmm+mech_clear+mod_heig[0]/2+epsil;

	GeoTrd * trd = new GeoTrd(mod_xm,mod_xp,modLen/2,modLen/2,mod_heig[m]/2);
	GeoLogVol *logVol = new GeoLogVol(basename+"::Module",trd,LAr);
	pvModule[m] = new GeoPhysVol(logVol);

	GeoTransform *xf = new GeoTransform(GeoTrf::Translate3D(0.0, modY, modZ)); 
	sectorPhysVol->add(xf);
	sectorPhysVol->add(pvModule[m]);

	modYPrev=modY;
	modLenPrev=modLen;

      }
    }

    double shell_leng = mod[0][0]+mod[1][0]+mod[2][0]+mod[3][0]+mod[4][0]+mod[5][0]+mod[6][0]+mod[7][0];
    double prot_y = (shell_leng/2)*cmm;
    double glX = 0.*Gaudi::Units::mm;
    double glY = -sector_length/2+prot_y+epsil;

    //-----------------------------A Protection Shell--------------------------//
    {
      GeoBox *box = new GeoBox((smallLength/2+1.)*cmm, (shell_leng/2)*cmm,(shell_th/2)*cmm);
      GeoLogVol *logVol=new GeoLogVol(basename+"::ProtectionShell",box,FR4);
      GeoPhysVol *physVol = new GeoPhysVol(logVol);

      double glZ = -sector_height/2+(shell_th/2)*cmm+epsil;
      
      GeoTransform *xf = new GeoTransform(GeoTrf::Translate3D(glX,glY, glZ));
      sectorPhysVol->add(xf);
      sectorPhysVol->add(physVol);
    }

    double mb_th = 2.2;
    double rail_pos = 22.;       
    double rail_width = 24.;
    double prot_th = 0.5;
    double widthFront = 2.8;
    double mb_width = 169.;
    double heightIn = 1.5;
    double heightOut = 5.;

    double modz[8], mody[8];
    mody[0] = -sector_length/2+mod_leng[0]/2+epsil;
    modz[0] = -sector_height/2+shell_th*cmm+mech_clear+mod_heig[0]/2+epsil;
    modz[1] = modz[0]+(mod_heig[0]-mod_heig[1])/2;
    for(int i=1; i<8; i++)
      mody[i] = mody[i-1]+mod_leng[i-1]/2+mod_leng[i]/2+epsil;

    for(int i=2; i<8; i++) modz[i]= modz[0];

    double mbZ = modz[0]+mod_heig[0]/2+(mb_th/2)*cmm+epsil;

    //-----------------------------Mother Board--------------------------//
    {
      GeoBox* MB = new GeoBox((mb_width/2)*cmm,(mb_length/2)*cmm,(mb_th/2)*cmm);
      GeoLogVol *logVol=new GeoLogVol(basename+"::MotherBoard",MB,MBMat);
      GeoPhysVol *physVol = new GeoPhysVol(logVol);

      GeoTransform* xf = new GeoTransform(GeoTrf::TranslateZ3D(mbZ));
      sectorPhysVol->add(xf);
      sectorPhysVol->add(physVol);
    }

    //-----------------------------A Protection Plate--------------------------//
    {
      double prot_x = (bigLength/2+1.-rail_pos-rail_width+epsil)*cmm;
      double prot_y = (shell_leng/2)*cmm;
      double prot_z = (prot_th/2)*cmm;

      GeoBox* plate = new GeoBox(prot_x,prot_y,prot_z);
      GeoLogVol *logVol=new GeoLogVol(basename+"::ProtectionPlate",plate,FR4);
      GeoPhysVol *physVol = new GeoPhysVol(logVol);

      GeoTransform* xf = new GeoTransform(GeoTrf::Translate3D(glX,glY,mbZ+(mb_th/2+heightOut+prot_th/2)*cmm+2*epsil));
      sectorPhysVol->add(xf);
      sectorPhysVol->add(physVol);
    }

    //-----------------------------Connectics--------------------------//
    {
      double conn_xm = (widthFront/2)*cmm;
      double conn_xp = (mb_width/2)*cmm;
      double conn_ym= (heightIn/2)*cmm;
      double conn_yp =(heightOut/2)*cmm;
      double conn_leng = (mb_length/2)*cmm;

      GeoTrd* connectics = new GeoTrd(conn_xm,conn_xp,conn_ym,conn_yp,conn_leng);
      GeoLogVol *logVol=new GeoLogVol(basename+"::Connectics",connectics,ConnecMat);
      GeoPhysVol *physVol = new GeoPhysVol(logVol);

      double connZ = mbZ+(mb_th/2+heightOut/2)*cmm+epsil;
      GeoTransform* xf1 = new GeoTransform(GeoTrf::TranslateZ3D(connZ));
      GeoTransform* xf2 = new GeoTransform(GeoTrf::RotateX3D(-90*Gaudi::Units::deg));

      sectorPhysVol->add(xf1);
      sectorPhysVol->add(xf2);
      sectorPhysVol->add(physVol);
    }

    //-----------------------------Rails--------------------------//
    {
      GeoBox* rail = new GeoBox((rail_width/2)*cmm,(shell_leng/2)*cmm,(rail_th/2)*cmm);
      GeoLogVol *logVol=new GeoLogVol(basename+"::Rail",rail,FR4);
      GeoPhysVol *physVol = new GeoPhysVol(logVol);

      double railX = (bigLength/2+1-rail_pos-rail_width/2)*cmm+epsil;
      double railZ = modz[0]+mod_heig[0]/2+(rail_th/2)*cmm+epsil;

      GeoTransform* xf1 = new GeoTransform(GeoTrf::Translate3D(railX,glY,railZ));
      GeoTransform* xf2 = new GeoTransform(GeoTrf::Translate3D(-railX,glY,railZ));

      sectorPhysVol->add(xf1);
      sectorPhysVol->add(physVol);

      sectorPhysVol->add(xf2);
      sectorPhysVol->add(physVol);
    }

    //
    //--- --- --- Module contents --- --- ---
    //

    // 1. Create physical volumes
    double anode_th = 0.330;
    double cathode_th = 0.270;

    double heig_elec1 = (larheight/cos(-mod[0][3]*Gaudi::Units::deg)-0.5*anode_th/cos(mod[0][3]*Gaudi::Units::deg))*cmm;
    double heig_elec3 = (larheight-0.5*cathode_th/cos(mod[1][3]*Gaudi::Units::deg))*cmm;

    GeoTrd* catho1 = new GeoTrd(smallLength/2*cmm,bigLength/2*cmm,cathode_th/2*cmm,cathode_th/2*cmm,heig_elec1/2*cmm);
    GeoLogVol* LV_catho1 = new GeoLogVol(basename+"::Cathode1",catho1,CathodeMat);
    GeoPhysVol *PV_catho1 = new GeoPhysVol(LV_catho1);

    GeoTrd* catho3 = new GeoTrd(smallLength/2 *cmm,bigLength/2 *cmm,cathode_th/2 *cmm,cathode_th/2 *cmm,heig_elec3/2);
    GeoLogVol* LV_catho3 = new GeoLogVol(basename+"::Cathode3",catho3,CathodeMat);
    GeoPhysVol *PV_catho3 = new GeoPhysVol(LV_catho3);

    GeoTrd* ano1 = new GeoTrd(smallLength/2 *cmm,bigLength/2 *cmm,anode_th/2 *cmm,anode_th/2 *cmm,heig_elec1/2);
    GeoLogVol* LV_ano1 = new GeoLogVol(basename+"::Anode1",ano1,AnodeMat);
    GeoPhysVol *PV_ano1 = new GeoPhysVol(LV_ano1);

    GeoTrd* ano3 = new GeoTrd(smallLength/2 *cmm,bigLength/2 *cmm,anode_th/2 *cmm,anode_th/2 *cmm,heig_elec3/2);
    GeoLogVol* LV_ano3 = new GeoLogVol(basename+"::Anode3",ano3,AnodeMat);
    GeoPhysVol *PV_ano3 = new GeoPhysVol(LV_ano3);

    //-- Prepreg. plates --
    double prep2_height = (bigLength/2+1.)*cmm;
    double prep1_z = (prep1_th/2)*cmm;
    double prep2_z = (prep2_th/2)*cmm;
    double prep_length[8];
    for(int i=0; i<8; i++ ) prep_length[i] = mod_leng[i]-2.*epsil;

    GeoPhysVol* PV_Prep1[8], *PV_Prep2[8];

    for(int i=0; i<8; i++ )
    {
      GeoBox* box1 =   new GeoBox(prep1_height,prep_length[i]/2,prep1_z); 
      GeoLogVol* logVol1 = new GeoLogVol(basename+"::Prep1",box1,FR4);
      PV_Prep1[i] = new GeoPhysVol(logVol1);

      GeoBox* box2 =   new GeoBox(prep2_height,prep_length[i]/2,prep2_z);
      GeoLogVol* logVol2 = new GeoLogVol(basename+"::Prep2",box2,FR4);
      PV_Prep2[i] = new GeoPhysVol(logVol2);
    }

    // 2. Fill the module contents
    double prep1_pos[8];
    double prep2_pos[8];
    double elec_trans = -2*prep2_z+mod_heig[0]/2-(larheight/2)*cmm-3*epsil; 
    for(int i=0; i<8; i++ )
    {
      prep1_pos[i] = prep1_z-mod_heig[i]/2+epsil;
      prep2_pos[i] = -prep2_z+mod_heig[i]/2-epsil;
    }

    double YStartA[8],YStartC[8];
    
    for(int i=0; i<8; i++ ) 
    {
      YStartC[i] = -mod_leng[i]/2+(mod[i][5]+cathode_th/2)*cmm;
      YStartA[i] = YStartC[i]+(mod[i][4]/2)*cmm;
    }
// for module 1, the y position has shifted 
// should add delta01 to make sure that electrodes stay at the same place
// within full sector
    YStartC[1] += delta01;
    YStartA[1] += delta01;

    for(int i=0; i<8; i++)
    {
      GeoTransform* xfPrep1 = new GeoTransform(GeoTrf::TranslateZ3D(prep1_pos[i]));
      GeoTransform* xfPrep2 = new GeoTransform(GeoTrf::TranslateZ3D(prep2_pos[i]));

      pvModule[i]->add(xfPrep1);
      pvModule[i]->add(PV_Prep1[i]);
      pvModule[i]->add(xfPrep2);
      pvModule[i]->add(PV_Prep2[i]);


      // Parameterizations for electrodes

      GeoGenfun::Variable I;
      GeoGenfun::GENFUNCTION cathoGF = YStartC[i]+I*mod[i][4]*cmm;
      GeoGenfun::GENFUNCTION anoGF = YStartA[i]+I*mod[i][4]*cmm;

      GeoXF::TRANSFUNCTION cathoTF = GeoXF::Pow(GeoTrf::TranslateY3D(1.),cathoGF)*GeoTrf::TranslateZ3D(elec_trans)*GeoTrf::RotateX3D(-mod[i][3]*Gaudi::Units::deg);
      GeoXF::TRANSFUNCTION anoTF = GeoXF::Pow(GeoTrf::TranslateY3D(1.),anoGF)*GeoTrf::TranslateZ3D(elec_trans)*GeoTrf::RotateX3D(-mod[i][3]*Gaudi::Units::deg);


      GeoSerialTransformer *cathoST,*anoST;
      if(i==0)
      {
	cathoST =  new GeoSerialTransformer(PV_catho1,&cathoTF,static_cast<unsigned int>(mod[i][2]));
	anoST = new GeoSerialTransformer(PV_ano1,&anoTF,static_cast<unsigned int>(mod[i][1]));
      }
      else
      {
	cathoST =  new GeoSerialTransformer(PV_catho3,&cathoTF,static_cast<unsigned int>(mod[i][2]));
	anoST = new GeoSerialTransformer(PV_ano3,&anoTF,static_cast<unsigned int>(mod[i][1]));
      }

      pvModule[i]->add(cathoST);
      pvModule[i]->add(anoST);
    }

  }

}

// Destructor:
LArGeo::BarrelPresamplerConstruction::~BarrelPresamplerConstruction()
{
  m_psPhysicalPos->unref();
  m_psPhysicalNeg->unref();
}
  

GeoFullPhysVol* LArGeo::BarrelPresamplerConstruction::GetPositiveEnvelope() {
  return m_psPhysicalPos;
}

GeoFullPhysVol* LArGeo::BarrelPresamplerConstruction::GetNegativeEnvelope(){
  return m_psPhysicalNeg;
}
