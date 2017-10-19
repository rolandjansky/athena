/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CavernInfraDetectorFactory01.h"


#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoSerialDenominator.h"  
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"


#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include <string>
#include <map>
using namespace Genfun;
using namespace GeoXF;

CavernInfraDetectorFactory01::CavernInfraDetectorFactory01(StoreGateSvc *detStore,
				       IRDBAccessSvc *pAccess)
  :detectorManager(NULL),
   detectorStore(detStore),
   access(pAccess)
 
{
}

CavernInfraDetectorFactory01::~CavernInfraDetectorFactory01()
{
}

void CavernInfraDetectorFactory01::create(GeoPhysVol *world)
{ 
  detectorManager=new CavernInfraDetectorManager();

  const DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    return; 
  } 

   // Get the materials  
  const GeoMaterial *air        = materialManager->getMaterial("std::Air");
  const GeoMaterial *rock       = materialManager->getMaterial("std::Rock");
  const GeoMaterial *conc       = materialManager->getMaterial("std::Concrete");
  const GeoMaterial *shieldSteel      = materialManager->getMaterial("shield::ShieldSteel");
 
  // Get Recordset CavernElements.
  IRDBRecordset_ptr cavernElements = access->getRecordsetPtr("CavernElements", "CavernElements-02");  
 
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
  double SX1Xpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m;
  double SX1Ypos = (*cavernElements)[recordIndex]->getDouble("YPOS")*CLHEP::m;
  double SX1HalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m;
  double SX1HalfWidth  = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;
  double SX1HalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  
  recordIndex = elementMap["SX1Air"];
  double SX1AirHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m;  
  double SX1AirHalfWidth  = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;   
  double SX1AirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;

  GeoTransform *s1XYZt = new GeoTransform(HepGeom::Translate3D(SX1Ypos,91.75*CLHEP::m, SX1Xpos)); //1.7  -8.7
  world->add(s1XYZt);
  GeoBox     *SX1        = new GeoBox(SX1HalfWidth ,SX1HalfHeight, SX1HalfLength); //  11.7, 6.5, 42.3
  GeoLogVol  *lvsx1s     = new GeoLogVol("SX1Steel",SX1,shieldSteel);
  GeoPhysVol *sx1        = new GeoPhysVol(lvsx1s);
  world->add(tag);                                                                             
  world->add(sx1);
   
  GeoTransform *s1z = new GeoTransform(HepGeom::TranslateY3D(-.005*CLHEP::m));
  sx1->add(s1z);
  GeoBox     *SX1Air     = new GeoBox( SX1AirHalfWidth, SX1AirHalfHeight, SX1AirHalfLength);
  GeoLogVol  *lvsx1a     = new GeoLogVol("SX1Air",SX1Air,air);
  GeoPhysVol *sx1a       = new GeoPhysVol(lvsx1a);
  sx1->add(sx1a);
 
 
 //// ------------------First: Build cavern with concrete-----------------------------

  recordIndex = elementMap["WallACConc"];
  double WallACConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double WallACConcMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double WallACConcMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;
  double WallACConcSphi= (*cavernElements)[recordIndex]->getDouble("SPHI");
  double WallACConcDphi= (*cavernElements)[recordIndex]->getDouble("DPHI"); 

  recordIndex = elementMap["BoxConc"];
  double BoxConcHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m; 
  double BoxConcHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;  
  double BoxConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;

  recordIndex = elementMap["VaultConc"];
  double VaultConcZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double VaultConcYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");
  double VaultConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double VaultConcMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double VaultConcMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;
  double VaultConcSphi= (*cavernElements)[recordIndex]->getDouble("SPHI");
  double VaultConcDphi= (*cavernElements)[recordIndex]->getDouble("DPHI");

  recordIndex = elementMap["HallConc"];
  double HallConcHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m; 
  double HallConcHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;  
  double HallConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;

  recordIndex = elementMap["BoxConcW"];
  double BoxConcWZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double BoxConcWYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  recordIndex = elementMap["BoxConcVleft"];
  double BoxConcVleftZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double BoxConcVleftYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  recordIndex = elementMap["BoxConcVright"];
  double BoxConcVrightZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double BoxConcVrightYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  recordIndex = elementMap["SurfaceFloor"];
  double SurfaceFloorXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m;
  double SurfaceFloorZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double SurfaceFloorHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m; 
  double SurfaceFloorHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;  
  double SurfaceFloorHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;

   
  GeoTubs *WallACConcrete      = new GeoTubs(WallACConcMinR, WallACConcMaxR, WallACConcHalfHeight, WallACConcSphi, WallACConcDphi);    
  GeoBox  *BoxConc             = new GeoBox(BoxConcHalfLength, BoxConcHalfWidth, BoxConcHalfHeight);
  GeoTubs *VaultConcrete       = new GeoTubs(VaultConcMinR, VaultConcMaxR, VaultConcHalfHeight , VaultConcSphi, VaultConcDphi);        
  GeoBox  *HallConcrete        = new GeoBox(HallConcHalfLength, HallConcHalfWidth, HallConcHalfHeight);
  GeoBox  *SurfaceFloor        = new GeoBox(SurfaceFloorHalfLength, SurfaceFloorHalfWidth, SurfaceFloorHalfHeight);

  HepGeom::Transform3D BoxCutWallConc          = HepGeom::TranslateZ3D(BoxConcWZpos)*HepGeom::RotateY3D(BoxConcWYrotate);
  HepGeom::Transform3D BoxCutVaultLeftConc     = HepGeom::TranslateZ3D(BoxConcVleftZpos)*HepGeom::RotateY3D(BoxConcVleftYrotate);
  HepGeom::Transform3D BoxCutVaulRightConc     = HepGeom::TranslateZ3D(BoxConcVrightZpos)*HepGeom::RotateY3D(BoxConcVrightYrotate);

  //---------------- Cut the vault and side walls----------------------------
  const GeoShape & CutWallConc   = (*WallACConcrete).subtract((*BoxConc) << HepGeom::Transform3D(BoxCutWallConc));
  const GeoShape & CutVaultConc  = (*VaultConcrete).subtract((*BoxConc) << HepGeom::Transform3D(BoxCutVaultLeftConc)).
                                                    subtract((*BoxConc) << HepGeom::Transform3D(BoxCutVaulRightConc));
 
  recordIndex = elementMap["ShaftCut"];
  double ShaftCutZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double ShaftCutYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");
  double ShaftCutHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double ShaftCutMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double ShaftCutMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;

  recordIndex = elementMap["PX14Conc"];
  double PX14ConcXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m; 
  double PX14ConcYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*CLHEP::m;
  double PX14ConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double PX14ConcMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double PX14ConcMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;

  recordIndex = elementMap["PX16Conc"];
  double PX16ConcXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m; 
  double PX16ConcYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*CLHEP::m;
  double PX16ConcHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double PX16ConcMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double PX16ConcMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;

  //--------------- Build concrete shafts------------------------------.
  GeoTube     *ShaftCut  = new GeoTube(ShaftCutMinR, ShaftCutMaxR, ShaftCutHalfHeight);
  GeoTube     *PX14Conc  = new GeoTube(PX14ConcMinR, PX14ConcMaxR, PX14ConcHalfHeight);
  GeoTube     *PX16Conc  = new GeoTube(PX16ConcMinR, PX16ConcMaxR, PX16ConcHalfHeight);
      
  const GeoShape & ShaftsConc=
	((*PX14Conc)           << HepGeom::TranslateX3D(PX14ConcXpos)*HepGeom::TranslateY3D(PX14ConcYpos)).
	add((*PX16Conc)        << HepGeom::TranslateX3D(PX16ConcXpos)*HepGeom::TranslateY3D(PX16ConcYpos)).
	subtract((*ShaftCut)   << HepGeom::TranslateZ3D(ShaftCutZpos)*HepGeom::RotateY3D(ShaftCutYrotate));
 
  recordIndex = elementMap["WallCConc"];
  double WallCConcZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m;
  double WallCConcXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m; 
  double WallCConcZrotate = (*cavernElements)[recordIndex]->getDouble("ZROTATE");

  recordIndex = elementMap["WallAConc"];
  double WallAConcZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m;
  double WallAConcXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m; 

  recordIndex = elementMap["ShaftsConc"];
  double ShaftsConcZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m;

  //----------------Assemble everything--------------------------------.
  const GeoShape & Concrete= (*HallConcrete).
	add((CutWallConc     << HepGeom::TranslateZ3D(WallCConcZpos)*HepGeom::TranslateX3D(WallCConcXpos)*HepGeom::RotateZ3D(WallCConcZrotate))).
        add((ShaftsConc      << HepGeom::TranslateZ3D(ShaftsConcZpos))).
	add((CutVaultConc    << HepGeom::TranslateZ3D(VaultConcZpos)*HepGeom::RotateY3D(VaultConcYrotate))).
        add((*SurfaceFloor)  << HepGeom::TranslateZ3D(SurfaceFloorZpos)*HepGeom::TranslateX3D(SurfaceFloorXpos)).
    add((CutWallConc     << HepGeom::TranslateZ3D(WallAConcZpos)*HepGeom::TranslateX3D(WallAConcXpos)));
	
 //// -----------------------Second: Build cavern with air-------------------------------

  // Create variables for Concrete Cavern and get index of the record in cavernElements
   recordIndex = elementMap["WallACAir"];
  double WallACAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double WallACAirMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double WallACAirMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;
  double WallACAirSphi= (*cavernElements)[recordIndex]->getDouble("SPHI");
  double WallACAirDphi= (*cavernElements)[recordIndex]->getDouble("DPHI"); 
 
  recordIndex = elementMap["BoxAir"];
  double BoxAirHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m; 
  double BoxAirHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;  
  double BoxAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
 
  recordIndex = elementMap["VaultAir"];
  double VaultAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double VaultAirYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");
  double VaultAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double VaultAirMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double VaultAirMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;
  double VaultAirSphi= (*cavernElements)[recordIndex]->getDouble("SPHI");
  double VaultAirDphi= (*cavernElements)[recordIndex]->getDouble("DPHI");
 
  recordIndex = elementMap["HallAir"];
  double HallAirHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m; 
  double HallAirHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;  
  double HallAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
 
  recordIndex = elementMap["BaseAir"];
  double BaseAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double BaseAirHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m; 
  double BaseAirHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;  
  double BaseAirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
 
  recordIndex = elementMap["BoxAirW"];
  double BoxAirWZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double BoxAirWYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  recordIndex = elementMap["BoxAirVleft"];
  double BoxAirVleftZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double BoxAirVleftYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");
 
  recordIndex = elementMap["BoxAirVright"];
  double BoxAirVrightZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m; 
  double BoxAirVrightYrotate = (*cavernElements)[recordIndex]->getDouble("YROTATE");

  GeoTubs *WallACAir         = new GeoTubs(WallACAirMinR, WallACAirMaxR, WallACAirHalfHeight, WallACAirSphi, WallACAirDphi);        
  GeoBox  *BoxAir            = new GeoBox(BoxAirHalfLength, BoxAirHalfWidth, BoxAirHalfHeight);
  GeoTubs *VaultAir          = new GeoTubs(VaultAirMinR, VaultAirMaxR, VaultAirHalfHeight, VaultAirSphi, VaultAirDphi);      
  GeoBox  *HallAir           = new GeoBox(HallAirHalfLength, HallAirHalfWidth, HallAirHalfHeight);
  GeoBox  *BaseAir           = new GeoBox(BaseAirHalfLength, BaseAirHalfWidth, BaseAirHalfHeight);
  HepGeom::Transform3D BoxCutWallAir      = HepGeom::TranslateZ3D(BoxAirWZpos)*HepGeom::RotateY3D(BoxAirWYrotate);
  HepGeom::Transform3D BoxCutVaultLeftAir = HepGeom::TranslateZ3D(BoxAirVleftZpos)*HepGeom::RotateY3D(BoxAirVleftYrotate);
  HepGeom::Transform3D BoxCutVaulRightAir = HepGeom::TranslateZ3D(BoxAirVrightZpos)*HepGeom::RotateY3D(BoxAirVrightYrotate);

  //------------------Cut the vault and side walls----------------------------
  const GeoShape & CutWallAir   = (*WallACAir).subtract((*BoxAir) << HepGeom::Transform3D(BoxCutWallAir));
  const GeoShape & CutVaultAir  = (*VaultAir).subtract((*BoxAir) << HepGeom::Transform3D(BoxCutVaultLeftAir)).
                                              subtract((*BoxAir) << HepGeom::Transform3D(BoxCutVaulRightAir));

  //--------------- Build air shafts------------------------------.
  recordIndex = elementMap["PX14Air"];
  double PX14AirXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m; 
  double PX14AirYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*CLHEP::m;
  double PX14AirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double PX14AirMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double PX14AirMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;
 
  recordIndex = elementMap["PX16Air"];
  double PX16AirXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m; 
  double PX16AirYpos = (*cavernElements)[recordIndex]->getDouble("YPOS")*CLHEP::m;
  double PX16AirHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;
  double PX16AirMinR = (*cavernElements)[recordIndex]->getDouble("MINR")*CLHEP::m;     
  double PX16AirMaxR = (*cavernElements)[recordIndex]->getDouble("MAXR")*CLHEP::m;

  GeoTube     *PX14Air   = new GeoTube(PX14AirMinR, PX14AirMaxR, PX14AirHalfHeight);  
  GeoTube     *PX16Air   = new GeoTube(PX16AirMinR, PX16AirMaxR, PX16AirHalfHeight);

  const GeoShape & ShaftsAir= 
	  ((*PX14Air)     << HepGeom::TranslateX3D(PX14AirXpos)*HepGeom::TranslateY3D(PX14AirYpos)).
	  add((*PX16Air)  << HepGeom::TranslateX3D(PX16AirXpos)*HepGeom::TranslateY3D(PX16AirYpos));
 
  //----------------Assemble everything--------------------------------.
  recordIndex = elementMap["WallAAir"];
  double WallAAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m;
  double WallAAirXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m; 
 
  recordIndex = elementMap["WallCAir"];
  double WallCAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m;
  double WallCAirXpos = (*cavernElements)[recordIndex]->getDouble("XPOS")*CLHEP::m; 
  double WallCAirZrotate = (*cavernElements)[recordIndex]->getDouble("ZROTATE");

  const GeoShape & CavernAir = (*HallAir).
	  add(CutWallAir     << HepGeom::TranslateZ3D(WallAAirZpos)*HepGeom::TranslateX3D(WallAAirXpos)).
	  add(CutWallAir     << HepGeom::TranslateZ3D(WallCAirZpos)*HepGeom::TranslateX3D(WallCAirXpos)*HepGeom::RotateZ3D(WallCAirZrotate)).
	  add(CutVaultAir    << HepGeom::TranslateZ3D(VaultAirZpos)*HepGeom::RotateY3D(VaultAirYrotate)).
	  add((*BaseAir)     << HepGeom::TranslateZ3D(BaseAirZpos));
 
  recordIndex = elementMap["ShaftsAir"];
  double ShaftsAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m;
 
  recordIndex = elementMap["CavernAir"];
  double CavernAirZpos = (*cavernElements)[recordIndex]->getDouble("ZPOS")*CLHEP::m;

  const GeoShape & AirHall=
	  ((CavernAir     << HepGeom::TranslateZ3D(CavernAirZpos))).
          add((ShaftsAir  << HepGeom::TranslateZ3D(ShaftsAirZpos)));     //allows for a built-in 2m thick plugs for both PX14 & PX16


 
 //// -----------------------Build BedRock for Shafts and Cavern-------------------------------
 
  recordIndex = elementMap["Rock"];
  double RockHalfLength = (*cavernElements)[recordIndex]->getDouble("HALFLENGTH")*CLHEP::m; 
  double RockHalfWidth = (*cavernElements)[recordIndex]->getDouble("HALFWIDTH")*CLHEP::m;  
  double RockHalfHeight = (*cavernElements)[recordIndex]->getDouble("HALFHEIGHT")*CLHEP::m;

  
  GeoBox     *Rockbox     = new GeoBox( RockHalfWidth, RockHalfHeight, RockHalfLength); // 300, 50.75, 300
  const GeoShape & BedRock = ((*Rockbox) .
			      subtract((AirHall) <<HepGeom::TranslateY3D(-34.5*CLHEP::m)*HepGeom::RotateY3D(-1.57)*HepGeom::RotateX3D(-1.57)));

  GeoTransform *rZt  = new GeoTransform(HepGeom::TranslateY3D(34.5*CLHEP::m)*HepGeom::TranslateX3D(-1.7*CLHEP::m));
  world->add(rZt);
  GeoLogVol  *lvr        = new GeoLogVol("BEDROCK",&BedRock,rock);
  GeoPhysVol *bedrock    = new GeoPhysVol(lvr);
  world->add(tag);
  world->add(bedrock);


  const GeoShape & CavConcrete = ((Concrete).subtract(AirHall));
  
  GeoTransform* xfTop = new GeoTransform(HepGeom::TranslateY3D(-34.5*CLHEP::m)*HepGeom::RotateY3D(-1.57)*HepGeom::RotateX3D(-1.57)); 
  bedrock->add(xfTop);
  GeoLogVol  *lvcsc              = new GeoLogVol("CAV_SHAFTS_CONC",&CavConcrete,conc);
  GeoPhysVol *CavShaftsConc      = new GeoPhysVol(lvcsc);
  bedrock->add(CavShaftsConc);
  

  //// NOW INSERT ALL OF THIS INTO THE WORLD.
  detectorManager->addTreeTop(sx1);
  detectorManager->addTreeTop(bedrock);  

  
}

const CavernInfraDetectorManager * CavernInfraDetectorFactory01::getDetectorManager() const
{
  return detectorManager;
}

void CavernInfraDetectorFactory01::setTagNode(std::string tag, std::string node)
{
  m_versionTag = tag;
  m_versionNode = node;
}

