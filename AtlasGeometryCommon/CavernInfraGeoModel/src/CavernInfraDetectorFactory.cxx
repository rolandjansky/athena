/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CavernInfraDetectorFactory.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include <string>
#include <map>

CavernInfraDetectorFactory::CavernInfraDetectorFactory(StoreGateSvc *detStore,
						       IRDBAccessSvc *pAccess)
  :m_detectorManager(NULL),
   m_detectorStore(detStore),
   m_access(pAccess)
{
}

CavernInfraDetectorFactory::~CavernInfraDetectorFactory()
{
}

void CavernInfraDetectorFactory::create(GeoPhysVol *world)
{ 
  m_detectorManager=new CavernInfraDetectorManager();

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    return; 
  } 

   // Get the materials  
  const GeoMaterial *air        = materialManager->getMaterial("std::Air");
  const GeoMaterial *rock       = materialManager->getMaterial("std::Rock");
  const GeoMaterial *conc       = materialManager->getMaterial("std::Concrete");
  const GeoMaterial *shieldSteel      = materialManager->getMaterial("shield::ShieldSteel");

  // Get Recordset CavernElements.
  IRDBRecordset_ptr cavernElements = m_access->getRecordsetPtr("CavernElements",m_versionTag,m_versionNode);
 
  std::map<std::string, unsigned int> elementMap;
 
  for (unsigned int i=0; i<cavernElements->size(); i++) {
    std::string key = (*cavernElements)[i]->getString("ELEMENT");
    elementMap[key] = i;
  }
 
  unsigned int recordIndex;
 //// -------------------------Build mother volume Point1--------------------------------

 // Create variables for Point1
 // -- first get index of the record in cavernElements recordset
 // which corresponds to Point1 element
 // -- get numbers from record
  
  recordIndex = elementMap["Point1"]; 

  GeoNameTag *tag = new GeoNameTag("CavernInfra");

 //// -----------------------------------Build SX1------------------------------------------
 
  recordIndex = elementMap["SX1"];
  double SX1Xpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m;
  double SX1Ypos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::m;
  double SX1Zpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m;
  double SX1HalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m;
  double SX1HalfWidth  = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;
  double SX1HalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  
  recordIndex = elementMap["SX1Air"];
  double SX1AirYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::m;
  double SX1AirHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m;  
  double SX1AirHalfWidth  = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;   
  double SX1AirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;

  GeoTransform *s1XYZt = new GeoTransform(GeoTrf::Translate3D(SX1Xpos,SX1Ypos,SX1Zpos)); //1.7  -8.7
  world->add(s1XYZt);
  GeoBox     *SX1        = new GeoBox(SX1HalfWidth ,SX1HalfHeight, SX1HalfLength); //  11.7, 6.5, 42.3
  GeoLogVol  *lvsx1s     = new GeoLogVol("SX1Steel",SX1,shieldSteel);
  GeoPhysVol *sx1        = new GeoPhysVol(lvsx1s);
  world->add(tag);                                                                             
  world->add(sx1);
   

  GeoTransform *s1z = new GeoTransform(GeoTrf::TranslateY3D(SX1AirYpos));
  sx1->add(s1z);
  GeoBox     *SX1Air     = new GeoBox( SX1AirHalfWidth, SX1AirHalfHeight, SX1AirHalfLength);
  GeoLogVol  *lvsx1a     = new GeoLogVol("SX1Air",SX1Air,air);
  GeoPhysVol *sx1a       = new GeoPhysVol(lvsx1a);
  sx1->add(sx1a);
 
 
 //// ------------------First: Build cavern with concrete-----------------------------

  recordIndex = elementMap["WallACConc"];
  double WallACConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double WallACConcMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double WallACConcMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;
  double WallACConcSphi= (*cavernElements)[recordIndex]->getDouble("SPHI");
  double WallACConcDphi= (*cavernElements)[recordIndex]->getDouble("DPHI"); 

  recordIndex = elementMap["BoxConc"];
  double BoxConcHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m; 
  double BoxConcHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;  
  double BoxConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;

  recordIndex = elementMap["VaultConc"];
  double VaultConcZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double VaultConcYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");
  double VaultConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double VaultConcMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double VaultConcMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;
  double VaultConcSphi= (*cavernElements)[recordIndex]->getDouble("SPHI");
  double VaultConcDphi= (*cavernElements)[recordIndex]->getDouble("DPHI");

  recordIndex = elementMap["HallConc"];
  double HallConcHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m; 
  double HallConcHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;  
  double HallConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;

  recordIndex = elementMap["BoxConcW"];
  double BoxConcWZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double BoxConcWYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  recordIndex = elementMap["BoxConcVleft"];
  double BoxConcVleftZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double BoxConcVleftYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  recordIndex = elementMap["BoxConcVright"];
  double BoxConcVrightZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double BoxConcVrightYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  recordIndex = elementMap["SurfaceFloor"];
  double SurfaceFloorXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m;
  double SurfaceFloorZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double SurfaceFloorHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m; 
  double SurfaceFloorHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;  
  double SurfaceFloorHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;

   
  GeoTubs *WallACConcrete      = new GeoTubs(WallACConcMinR, WallACConcMaxR, WallACConcHalfHeight, WallACConcSphi, WallACConcDphi);    
  GeoBox  *BoxConc             = new GeoBox(BoxConcHalfLength, BoxConcHalfWidth, BoxConcHalfHeight);
  GeoTubs *VaultConcrete       = new GeoTubs(VaultConcMinR, VaultConcMaxR, VaultConcHalfHeight , VaultConcSphi, VaultConcDphi);        
  GeoBox  *HallConcrete        = new GeoBox(HallConcHalfLength, HallConcHalfWidth, HallConcHalfHeight);
  GeoBox  *SurfaceFloor        = new GeoBox(SurfaceFloorHalfLength, SurfaceFloorHalfWidth, SurfaceFloorHalfHeight);

  GeoTrf::Transform3D BoxCutWallConc          = GeoTrf::TranslateZ3D(BoxConcWZpos)*GeoTrf::RotateY3D(BoxConcWYrotate);
  GeoTrf::Transform3D BoxCutVaultLeftConc     = GeoTrf::TranslateZ3D(BoxConcVleftZpos)*GeoTrf::RotateY3D(BoxConcVleftYrotate);
  GeoTrf::Transform3D BoxCutVaulRightConc     = GeoTrf::TranslateZ3D(BoxConcVrightZpos)*GeoTrf::RotateY3D(BoxConcVrightYrotate);

  recordIndex = elementMap["HoleJN"];
  double HoleJNrmin = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::mm;
  double HoleJNrmax = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::mm;
  double HoleJNdz = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::mm;
  GeoTubs *HoleJN              = new GeoTubs(HoleJNrmin,HoleJNrmax,HoleJNdz, 0.,2.*M_PI); // should be 2300mm in radius

  recordIndex = elementMap["WallAAir"];
  double WallAAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m;
  double WallAAirXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m;

  recordIndex = elementMap["WallCAir"];
  double WallCAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m;
  double WallCAirXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m;
  double WallCAirZrotate = (*cavernElements)[recordIndex]->getDouble("ZROTATE");

  //---------------- Cut the vault and side walls----------------------------
  const GeoShape & CutWallConc   = (*WallACConcrete).subtract((*BoxConc) << GeoTrf::Transform3D(BoxCutWallConc)).
        subtract((*HoleJN) << GeoTrf::RotateY3D(M_PI*0.5) << GeoTrf::TranslateZ3D(-0.5*WallAAirZpos)*GeoTrf::TranslateY3D(-WallCAirXpos*1.0) );
  const GeoShape & CutVaultConc  = (*VaultConcrete).subtract((*BoxConc) << GeoTrf::Transform3D(BoxCutVaultLeftConc)).
                                                    subtract((*BoxConc) << GeoTrf::Transform3D(BoxCutVaulRightConc));
 
  recordIndex = elementMap["ShaftCut"];
  double ShaftCutZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double ShaftCutYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");
  double ShaftCutHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double ShaftCutMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double ShaftCutMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;

  recordIndex = elementMap["PX14Conc"];
  double PX14ConcXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m; 
  double PX14ConcYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::m;
  double PX14ConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double PX14ConcMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double PX14ConcMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;

  recordIndex = elementMap["PX16Conc"];
  double PX16ConcXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m; 
  double PX16ConcYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::m;
  double PX16ConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double PX16ConcMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double PX16ConcMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;

  //--------------- Build concrete shafts------------------------------.
  GeoTube     *ShaftCut  = new GeoTube(ShaftCutMinR, ShaftCutMaxR, ShaftCutHalfHeight);
  GeoTube     *PX14Conc  = new GeoTube(PX14ConcMinR, PX14ConcMaxR, PX14ConcHalfHeight);
  GeoTube     *PX16Conc  = new GeoTube(PX16ConcMinR, PX16ConcMaxR, PX16ConcHalfHeight);
      
  const GeoShape & ShaftsConc=
	((*PX14Conc)           << GeoTrf::TranslateX3D(PX14ConcXpos)*GeoTrf::TranslateY3D(PX14ConcYpos)).
	add((*PX16Conc)        << GeoTrf::TranslateX3D(PX16ConcXpos)*GeoTrf::TranslateY3D(PX16ConcYpos)).
	subtract((*ShaftCut)   << GeoTrf::TranslateZ3D(ShaftCutZpos)*GeoTrf::RotateY3D(ShaftCutYrotate));
 
  recordIndex = elementMap["WallCConc"];
  double WallCConcZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m;
  double WallCConcXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m; 
  double WallCConcZrotate = (*cavernElements)[recordIndex]->getDouble("ZROTATE");

  recordIndex = elementMap["WallAConc"];
  double WallAConcZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m;
  double WallAConcXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m; 

  recordIndex = elementMap["ShaftsConc"];
  double ShaftsConcZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m;

  //--------------- Build concrete nose ------------------------------.
  recordIndex =elementMap["BoxConcJN"];
  double BoxConcJNDX = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::mm;
  double BoxConcJNDY = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::mm;
  double BoxConcJNDZ = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::mm;
  double BoxConcJNXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::mm;
  recordIndex =elementMap["BoxConcExtraJN"];
  double BoxConcExtraJNDX = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::mm;
  double BoxConcExtraJNDY = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::mm;
  double BoxConcExtraJNDZ = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::mm;
  double BoxConcExtraJNXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::mm;
  double BoxConcExtraJNYpos = (*cavernElements)[recordIndex]->getDouble("YPOS");
  recordIndex =elementMap["StrutConcJN"];
  double StrutConcJNDX = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::mm;
  double StrutConcJNDY = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::mm;
  double StrutConcJNDZ = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::mm;
  double StrutConcJNXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::mm;
  double StrutConcJNYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::mm;
  double StrutConcJNZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::mm;

  GeoBox  *BoxConcJNC          = new GeoBox(BoxConcJNDX,BoxConcJNDY,BoxConcJNDZ);
  GeoBox  *BoxConcJNA          = new GeoBox(BoxConcJNDX,BoxConcJNDY,BoxConcJNDZ);
  GeoBox  *BoxConcStupidExtraJNC = new GeoBox(BoxConcExtraJNDX,BoxConcExtraJNDY,BoxConcExtraJNDZ);
  GeoBox  *BoxConcStupidExtraJNA = new GeoBox(BoxConcExtraJNDX,BoxConcExtraJNDY,BoxConcExtraJNDZ);
  const GeoBox  *StrutConcJN   = new GeoBox(StrutConcJNDX,StrutConcJNDY,StrutConcJNDZ);

  recordIndex = elementMap["CavernAir"];
  double CavernAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m;

  //----------------Assemble everything--------------------------------.
  const GeoShape & Concrete= (*HallConcrete).
        add((CutWallConc     << GeoTrf::TranslateZ3D(WallCConcZpos)*GeoTrf::TranslateX3D(WallCConcXpos)*GeoTrf::RotateZ3D(WallCConcZrotate))).
        add((ShaftsConc      << GeoTrf::TranslateZ3D(ShaftsConcZpos))).
        add((CutVaultConc    << GeoTrf::TranslateZ3D(VaultConcZpos)*GeoTrf::RotateY3D(VaultConcYrotate))).
        add((*SurfaceFloor)  << GeoTrf::TranslateZ3D(SurfaceFloorZpos)*GeoTrf::TranslateX3D(SurfaceFloorXpos)).
        add((CutWallConc     << GeoTrf::TranslateZ3D(WallAConcZpos)*GeoTrf::TranslateX3D(WallAConcXpos))).

        add((*BoxConcJNC)    << GeoTrf::TranslateX3D(BoxConcJNXpos) * GeoTrf::TranslateZ3D(-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
        add((*BoxConcStupidExtraJNC) << GeoTrf::TranslateX3D(BoxConcExtraJNXpos) * GeoTrf::TranslateZ3D(-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) * GeoTrf::TranslateY3D(-BoxConcExtraJNYpos) ).
        add((*BoxConcJNA)    << GeoTrf::TranslateX3D(-BoxConcJNXpos-15*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
        add((*BoxConcStupidExtraJNA) << GeoTrf::TranslateX3D(-BoxConcExtraJNXpos-15*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) * GeoTrf::TranslateY3D(BoxConcExtraJNYpos) ).

        add((*StrutConcJN)  << GeoTrf::TranslateX3D(StrutConcJNXpos) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
        add((*StrutConcJN)  << GeoTrf::TranslateX3D(StrutConcJNXpos) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(StrutConcJNYpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
        add((*StrutConcJN)  << GeoTrf::TranslateX3D(StrutConcJNXpos) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(-StrutConcJNYpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
        add((*StrutConcJN)  << GeoTrf::TranslateX3D(-StrutConcJNXpos-15.*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
        add((*StrutConcJN)  << GeoTrf::TranslateX3D(-StrutConcJNXpos-15.*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(StrutConcJNYpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
        add((*StrutConcJN)  << GeoTrf::TranslateX3D(-StrutConcJNXpos-15.*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5+CavernAirZpos) * GeoTrf::TranslateY3D(-StrutConcJNYpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) );

//        subtract((*HoleJN) << GeoTrf::RotateY3D(M_PI*0.5) << GeoTrf::TranslateZ3D(-0.5*WallAAirZpos+CavernAirZpos)*GeoTrf::TranslateY3D(-WallCAirXpos*1.0) );
//        subtract((*HoleJN) << GeoTrf::RotateY3D(M_PI*0.5) << GeoTrf::TranslateZ3D(-0.5*WallAAirZpos)*GeoTrf::TranslateY3D(-WallCAirXpos*1.0) );

	
 //// -----------------------Second: Build cavern with air-------------------------------

  // Create variables for Concrete Cavern and get index of the record in cavernElements
   recordIndex = elementMap["WallACAir"];
  double WallACAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double WallACAirMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double WallACAirMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;
  double WallACAirSphi= (*cavernElements)[recordIndex]->getDouble("SPHI");
  double WallACAirDphi= (*cavernElements)[recordIndex]->getDouble("DPHI"); 
 
  recordIndex = elementMap["BoxAir"];
  double BoxAirHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m; 
  double BoxAirHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;  
  double BoxAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
 
  recordIndex = elementMap["VaultAir"];
  double VaultAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double VaultAirYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");
  double VaultAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double VaultAirMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double VaultAirMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;
  double VaultAirSphi= (*cavernElements)[recordIndex]->getDouble("SPHI");
  double VaultAirDphi= (*cavernElements)[recordIndex]->getDouble("DPHI");
 
  recordIndex = elementMap["HallAir"];
  double HallAirHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m; 
  double HallAirHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;  
  double HallAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
 
  recordIndex = elementMap["BaseAir"];
  double BaseAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double BaseAirHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m; 
  double BaseAirHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;  
  double BaseAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
 
  recordIndex = elementMap["BoxAirW"];
  double BoxAirWZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double BoxAirWYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  recordIndex = elementMap["BoxAirVleft"];
  double BoxAirVleftZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double BoxAirVleftYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");
 
  recordIndex = elementMap["BoxAirVright"];
  double BoxAirVrightZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m; 
  double BoxAirVrightYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  GeoTubs *WallACAir         = new GeoTubs(WallACAirMinR, WallACAirMaxR, WallACAirHalfHeight, WallACAirSphi, WallACAirDphi);        
  GeoBox  *BoxAir            = new GeoBox(BoxAirHalfLength, BoxAirHalfWidth, BoxAirHalfHeight);
  GeoTubs *VaultAir          = new GeoTubs(VaultAirMinR, VaultAirMaxR, VaultAirHalfHeight, VaultAirSphi, VaultAirDphi);      
  GeoBox  *HallAir           = new GeoBox(HallAirHalfLength, HallAirHalfWidth, HallAirHalfHeight);
  GeoBox  *BaseAir           = new GeoBox(BaseAirHalfLength, BaseAirHalfWidth, BaseAirHalfHeight);
  GeoTrf::Transform3D BoxCutWallAir      = GeoTrf::TranslateZ3D(BoxAirWZpos)*GeoTrf::RotateY3D(BoxAirWYrotate);
  GeoTrf::Transform3D BoxCutVaultLeftAir = GeoTrf::TranslateZ3D(BoxAirVleftZpos)*GeoTrf::RotateY3D(BoxAirVleftYrotate);
  GeoTrf::Transform3D BoxCutVaulRightAir = GeoTrf::TranslateZ3D(BoxAirVrightZpos)*GeoTrf::RotateY3D(BoxAirVrightYrotate);

  //------------------Cut the vault and side walls----------------------------
  const GeoShape & CutWallAir   = (*WallACAir).subtract((*BoxAir) << GeoTrf::Transform3D(BoxCutWallAir));
  const GeoShape & CutVaultAir  = (*VaultAir).subtract((*BoxAir) << GeoTrf::Transform3D(BoxCutVaultLeftAir)).
                                              subtract((*BoxAir) << GeoTrf::Transform3D(BoxCutVaulRightAir));

  //--------------- Build air shafts------------------------------.
  recordIndex = elementMap["PX14Air"];
  double PX14AirXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m; 
  double PX14AirYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::m;
  double PX14AirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double PX14AirMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double PX14AirMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;
 
  recordIndex = elementMap["PX16Air"];
  double PX16AirXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m; 
  double PX16AirYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::m;
  double PX16AirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double PX16AirMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*Gaudi::Units::m;     
  double PX16AirMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*Gaudi::Units::m;

  GeoTube     *PX14Air   = new GeoTube(PX14AirMinR, PX14AirMaxR, PX14AirHalfHeight);  
  GeoTube     *PX16Air   = new GeoTube(PX16AirMinR, PX16AirMaxR, PX16AirHalfHeight);

  recordIndex = elementMap["CavShaftsConc"];
  double CavShaftsYPos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::m;
  double CavShaftsXrotate = (*cavernElements)[recordIndex]->getDouble("XROTATE");
  double CavShaftsYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  const GeoShape & ShaftsAir= 
	  ((*PX14Air)     << GeoTrf::TranslateX3D(PX14AirXpos)*GeoTrf::TranslateY3D(PX14AirYpos)).
	  add((*PX16Air)  << GeoTrf::TranslateX3D(PX16AirXpos)*GeoTrf::TranslateY3D(PX16AirYpos));
 
  //----------------Assemble everything--------------------------------.
  const GeoShape & CavernAir = (*HallAir).
//      add((*AirJN) << GeoTrf::RotateY3D(M_PI*0.5) << GeoTrf::TranslateZ3D(-0.5*WallAAirZpos)*GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
	  add(CutWallAir     << GeoTrf::TranslateZ3D(WallAAirZpos)*GeoTrf::TranslateX3D(WallAAirXpos)).
	  add(CutWallAir     << GeoTrf::TranslateZ3D(WallCAirZpos)*GeoTrf::TranslateX3D(WallCAirXpos)*GeoTrf::RotateZ3D(WallCAirZrotate)).
	  add(CutVaultAir    << GeoTrf::TranslateZ3D(VaultAirZpos)*GeoTrf::RotateY3D(VaultAirYrotate)).
	  add((*BaseAir)     << GeoTrf::TranslateZ3D(BaseAirZpos)).

      subtract((*BoxConcJNC)    << GeoTrf::TranslateX3D(BoxConcJNXpos) * GeoTrf::TranslateZ3D(-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
      subtract((*BoxConcStupidExtraJNC) << GeoTrf::TranslateX3D(BoxConcExtraJNXpos) * GeoTrf::TranslateZ3D(-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) * GeoTrf::TranslateY3D(-BoxConcExtraJNYpos) ).
      subtract((*BoxConcJNA)    << GeoTrf::TranslateX3D(-BoxConcJNXpos-15*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
      subtract((*BoxConcStupidExtraJNA) << GeoTrf::TranslateX3D(-BoxConcExtraJNXpos-15*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) * GeoTrf::TranslateY3D(BoxConcExtraJNYpos) ).

      subtract((*StrutConcJN)  << GeoTrf::TranslateX3D(StrutConcJNXpos) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
      subtract((*StrutConcJN)  << GeoTrf::TranslateX3D(StrutConcJNXpos) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(StrutConcJNYpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
      subtract((*StrutConcJN)  << GeoTrf::TranslateX3D(StrutConcJNXpos) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(-StrutConcJNYpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
      subtract((*StrutConcJN)  << GeoTrf::TranslateX3D(-StrutConcJNXpos-15.*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
      subtract((*StrutConcJN)  << GeoTrf::TranslateX3D(-StrutConcJNXpos-15.*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(StrutConcJNYpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
      subtract((*StrutConcJN)  << GeoTrf::TranslateX3D(-StrutConcJNXpos-15.*Gaudi::Units::mm) * GeoTrf::TranslateZ3D(-StrutConcJNZpos-WallAAirZpos*0.5) * GeoTrf::TranslateY3D(-StrutConcJNYpos) * GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
      add((*HoleJN) << GeoTrf::RotateY3D(M_PI*0.5) << GeoTrf::TranslateZ3D(-0.5*WallAAirZpos+CavernAirZpos)*GeoTrf::TranslateY3D(-WallCAirXpos*1.0) << GeoTrf::TranslateY3D(CavShaftsYPos)*GeoTrf::RotateY3D(CavShaftsYrotate)*GeoTrf::RotateX3D(CavShaftsXrotate) );


  recordIndex = elementMap["ShaftsAir"];
  double ShaftsAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*Gaudi::Units::m;

  const GeoShape & AirHall=
          ((CavernAir     << GeoTrf::TranslateZ3D(CavernAirZpos))).
          add((ShaftsAir  << GeoTrf::TranslateZ3D(ShaftsAirZpos)));     //allows for a built-in 2m thick plugs for both PX14 & PX16
 
 //// -----------------------Build BedRock for Shafts and Cavern-------------------------------
 
  recordIndex = elementMap["Rock"];
  double RockHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*Gaudi::Units::m; 
  double RockHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*Gaudi::Units::m;  
  double RockHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*Gaudi::Units::m;
  double RockXPos = (*cavernElements)[recordIndex]->getDouble("XPOS")*Gaudi::Units::m;
  double RockYPos = (*cavernElements)[recordIndex]->getDouble("YPOS")*Gaudi::Units::m;
  
  GeoBox     *Rockbox     = new GeoBox( RockHalfWidth, RockHalfHeight, RockHalfLength); // 300, 50.75, 300
  const GeoShape & BedRock = ((*Rockbox) .
			      subtract((AirHall) <<GeoTrf::TranslateY3D(CavShaftsYPos)*GeoTrf::RotateY3D(CavShaftsYrotate)*GeoTrf::RotateX3D(CavShaftsXrotate))).
                  subtract((*HoleJN) << GeoTrf::RotateY3D(M_PI*0.5) << GeoTrf::TranslateZ3D(-0.5*WallAAirZpos+CavernAirZpos)*GeoTrf::TranslateY3D(-WallCAirXpos*1.0) << GeoTrf::TranslateY3D(CavShaftsYPos)*GeoTrf::RotateY3D(CavShaftsYrotate)*GeoTrf::RotateX3D(CavShaftsXrotate) );

  GeoTransform *rZt  = new GeoTransform(GeoTrf::TranslateY3D(RockYPos)*GeoTrf::TranslateX3D(RockXPos));
  world->add(rZt);
  GeoLogVol  *lvr        = new GeoLogVol("BEDROCK",&BedRock,rock);
  GeoPhysVol *bedrock    = new GeoPhysVol(lvr);
  world->add(tag);
  world->add(bedrock);


  const GeoShape & CavConcrete = ((Concrete).
//        add((*HoleJN) << GeoTrf::RotateY3D(M_PI*0.5) << GeoTrf::TranslateZ3D(-0.5*WallAAirZpos+CavernAirZpos)*GeoTrf::TranslateY3D(-WallCAirXpos*1.0) ).
        subtract(AirHall));
  
  GeoTransform* xfTop = new GeoTransform(GeoTrf::TranslateY3D(CavShaftsYPos)*GeoTrf::RotateY3D(CavShaftsYrotate)*GeoTrf::RotateX3D(CavShaftsXrotate)); 
  bedrock->add(xfTop);
  GeoLogVol  *lvcsc              = new GeoLogVol("CAV_SHAFTS_CONC",&CavConcrete,conc);
  GeoPhysVol *CavShaftsConc      = new GeoPhysVol(lvcsc);
  bedrock->add(CavShaftsConc);
  

  //// NOW INSERT ALL OF THIS INTO THE WORLD.
  m_detectorManager->addTreeTop(sx1);
  m_detectorManager->addTreeTop(bedrock);  
  
}

const CavernInfraDetectorManager * CavernInfraDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

void CavernInfraDetectorFactory::setTagNode(std::string tag, std::string node)
{
  m_versionTag = tag;
  m_versionNode = node;
}
