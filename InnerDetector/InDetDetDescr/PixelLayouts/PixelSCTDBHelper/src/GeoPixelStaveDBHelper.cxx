/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Ladder services (Omega + Al tube + glue + part of pigtail + connector)
//
//
#include "PixelSCTDBHelper/GeoPixelStaveDBHelper.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "InDetGeoModelUtils/PairIndexMap.h"

#include <cmath>
#include <sstream>
#include <vector>


GeoPixelStaveDBHelper::GeoPixelStaveDBHelper( PixelGeoBuilderBasics* basics, int iLayer, bool bBarrel):
  PixelGeoBuilder(basics), m_layer(iLayer), m_bBarrel(bBarrel)
{
  layerTable_ptr = geoAccessor().getTable("PixelLayer");
  staveTable_ptr = geoAccessor().getTable("PixelStave");
  staveZTable_ptr = geoAccessor().getTable("PixelStaveZ");
  ladderTable_ptr = geoAccessor().getTable("PixelLadder");
  moduleTable_ptr = geoAccessor().getTable("PixelModule");
  readoutTable_ptr = geoAccessor().getTable("PixelReadout");
  TMTTable_ptr = geoAccessor().getTable("PixelTMT");

  PixelIBLStave      = geoAccessor().getTable("PixelIBLStave");
  PixelIBLSupport    = geoAccessor().getTable("PixelIBLSupport");
  PixelIBLFlex       = geoAccessor().getTable("PixelIBLFlex");
  PixelIBLFlexMaterial  = geoAccessor().getTable("PixelIBLFlexMaterial");
  PixelIBLGlueGrease    = geoAccessor().getTable("PixelIBLGlueGrease");

  m_staveIndex=PixelStaveIndex();
  m_moduleType=PixelModuleType();
  m_zPositionMap=0;

  InitIBLflags();

}

GeoPixelStaveDBHelper::~GeoPixelStaveDBHelper()
{
  if(m_zPositionMap)delete m_zPositionMap;

}

void GeoPixelStaveDBHelper::ResetLayerOrDisk(int iLayer, bool bBarrel)
{ 
  m_layer=iLayer;
  m_bBarrel = bBarrel;

  m_staveIndex=PixelStaveIndex();
  m_moduleType=PixelModuleType();
  m_zPositionMap=0;

  m_nbPlanarModule=0;
  m_nb3DModule=0;

  InitIBLflags();
}

void GeoPixelStaveDBHelper::InitIBLflags()
{
  IRDBRecordset_ptr switchTable_ptr = geoAccessor().getTable("PixelSwitches");
  
  m_isIBLGeometry = false;
  m_isIBLplanar=false;
  m_isIBLhybrid=false;
  m_layout = 0;
  std::string versionName = geoAccessor().getString(switchTable_ptr,"VERSIONNAME");
  if(versionName=="IBL")
    {
      m_isIBLGeometry = true;
      int layout = geoAccessor().getInt(staveTable_ptr,"LAYOUT",m_staveIndex);
      if(layout==4){
	m_isIBLplanar = true; m_nbPlanarModule=PixelNModule();
      }
      if(layout==5) {
	m_isIBLhybrid = true;
	int nbPixelModule = geoAccessor().getInt(staveTable_ptr,"NMODULE",m_staveIndex);
	m_nbPlanarModule = nbPixelModule*.75;
	m_nb3DModule = (nbPixelModule-m_nbPlanarModule)*2;
      }
      m_layout=layout;
    }


}

void GeoPixelStaveDBHelper::dump() const 
{

  std::cout<<"StaveDBHelper : layer/stave/barrel "<<m_layer<<" "<<m_staveIndex<<" "<<m_bBarrel<<"  module type : ";
  std::cout<<PixelModuleType()<<"  IBL : "<<isIBLGeometry()<<" "<<isIBLhybrid()<<std::endl;

}

double GeoPixelStaveDBHelper::PixelLadderLength() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"ENVLENGTH",m_staveIndex)*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelStaveX() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"SERVICEOFFSETX",m_staveIndex) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelStaveY() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"SERVICEOFFSETY",m_staveIndex) * CLHEP::mm;
}

int GeoPixelStaveDBHelper::PixelStaveIndex() const
{
  if (!isIBLGeometry()) return 0;
  if (!geoAccessor().testField(layerTable_ptr,"STAVEINDEX",m_layer)) return 0;
  return geoAccessor().getInt(layerTable_ptr,"STAVEINDEX",m_layer);
}

int GeoPixelStaveDBHelper::PixelStaveLayout() const
{
  return m_layout;
}


double GeoPixelStaveDBHelper::PixelLadderCableOffsetX() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"CABLEOFFSETX",m_staveIndex) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelLadderCableOffsetY() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"CABLEOFFSETY",m_staveIndex) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelLadderWidthClearance() const
{
  return geoAccessor().getDouble(staveTable_ptr,"CLEARANCEY",m_staveIndex)*CLHEP::mm;  
}

double GeoPixelStaveDBHelper::PixelLadderThicknessClearance() const
{
  int index = m_staveIndex;
  if (geoAccessor().testField(staveTable_ptr,"CLEARANCEX",index)) {
    return geoAccessor().getDouble(staveTable_ptr,"CLEARANCEX",index)*CLHEP::mm;  
  }
  return 0.1*CLHEP::mm;
}




double GeoPixelStaveDBHelper::PixelLadderTilt() const 
{
  return geoAccessor().getDouble(layerTable_ptr,"STAVETILT",m_layer)*CLHEP::deg;
}

int GeoPixelStaveDBHelper::PixelLayerNSectors() const 
{
  int nSectors = geoAccessor().getInt( layerTable_ptr, "NSECTORS", m_layer);
  return nSectors;
}

double GeoPixelStaveDBHelper::PixelLayerPhiOfModuleZero() const 
{
  return geoAccessor().getDouble(layerTable_ptr,"PHIOFMODULEZERO",m_layer)*CLHEP::deg;
}

double GeoPixelStaveDBHelper::PixelLayerRadius() const 
{
  double radius = geoAccessor().getDouble(layerTable_ptr,"RLAYER",m_layer)*CLHEP::mm;
  return radius;
}

double GeoPixelStaveDBHelper::PixelLayerThickness() const 
{
  double thick = -1;

  try{
    thick = geoAccessor().getDouble(layerTable_ptr,"THICKLAYER",m_layer)*CLHEP::mm;
  }
  catch(...)
    {
    }

  return thick;
}


int GeoPixelStaveDBHelper::NPixelSectors() const 
{
  return PixelLayerNSectors();
}

double GeoPixelStaveDBHelper::PhiOfModuleZero() const
{
  // For backward compatibilty first module is at 1/2 a module division
  if (!geoAccessor().testField(layerTable_ptr,"PHIOFMODULEZERO",m_layer)){
    return 180.0*CLHEP::degree/NPixelSectors();
  } else { 
    return geoAccessor().getDouble(layerTable_ptr,"PHIOFMODULEZERO",m_layer) * CLHEP::degree;
  }
}


//--------------------------------------
// Modules
//--------------------------------------

int GeoPixelStaveDBHelper::PixelNModule() const
{
  if(isIBLGeometry() && PixelStaveLayout()>3 && PixelStaveLayout()<7 && m_layer==0)
    return IBLStaveModuleNumber();
 
  return geoAccessor().getInt(staveTable_ptr,"NMODULE",m_staveIndex);
}

bool GeoPixelStaveDBHelper::centerModule(int etaModule) const
{
  // There is only a center module if there are an odd number
  // of modules. In that case it will be etaModule = 0.
  return (etaModule == 0 && PixelNModule()%2);
}

bool GeoPixelStaveDBHelper::allowSkipEtaZero() const
{
  bool allowSkip = true;
  if (isIBLGeometry() || isSLHC()){
    int staveIndex = PixelStaveIndex();
    if (geoAccessor().testField(staveTable_ptr,"NOSKIPZERO",staveIndex)) {
      if (geoAccessor().getInt(staveTable_ptr,"NOSKIPZERO",staveIndex)) allowSkip = false;
    }
  }
  return allowSkip;
}

int GeoPixelStaveDBHelper::PixelModuleEtaFromIndex(int index) const 
{
  int nModules = PixelNModule();
  int etaModule = index-nModules/2;
  // If even number of modules skip eta = 0.
  // For IBL or SLHC this behaviour can be disabled.
  if (allowSkipEtaZero() && (etaModule >= 0) && !(nModules%2)) etaModule++; 

  return etaModule;
}

int GeoPixelStaveDBHelper::PixelModuleIndexFromEta(int etaModule) const
{
  int nModules = PixelNModule();  
  int index = etaModule + nModules/2;
  // If even number of modules skip eta = 0.
  // For IBL or SLHC this behaviour can be disabled.
  if (allowSkipEtaZero() && (etaModule >= 0) && (nModules%2 == 0)) index--; 
  return index;
}

double GeoPixelStaveDBHelper::PixelModuleZPosition(int etaModule) const
{
  /*  int zPosType = 0;*/
  // Equi-distant modules
  int moduleIndex =  PixelModuleIndexFromEta(etaModule);  
  return geoAccessor().getDouble(staveTable_ptr,"MODULEDZ",m_staveIndex)*CLHEP::mm * (moduleIndex - 0.5*(PixelNModule()-1));

}

int GeoPixelStaveDBHelper::PixelModuleType() const
{
  int type = -1;
  
  if(isBarrel()){
    try {
      type = geoAccessor().getInt(layerTable_ptr,"MODULETYPE",m_layer);
      return type;
    }
    catch(...)
      {
      }
  }
  else {
    // if endcap
    type=0;
  }
  
  return type;
}

//   if (isSLHC() || isIBLGeometry()) {
//     if (isBarrel()) 
//       {
// 	type = geoAccessor().getInt(PixelLayer,"MODULETYPE",currentLD);
//       }
//     if (isEndcap() && isSLHC()) 
//       {
// 	// Not in DB yet.
// 	int ringType = getDiskRingType(currentLD,m_eta);
// 	if (ringType>=0) {
// 	  type = geoAccessor().getInt(PixelRing,"MODULETYPE",ringType);
// 	}
//       }
//     } 
//   else {
//     if(isBarrel()) type = currentLD;
//     if(isEndcap()) type = currentLD+PixelBarrelNLayer();
//   }
//   return type;


int GeoPixelStaveDBHelper::PixelModuleType3D() const
{
  int type = -1;

  try {
    type = geoAccessor().getInt(layerTable_ptr,"MODULETYPE3D",m_layer);
    return type;
  }
  catch(...)
    {
      type = PixelModuleType()+1;
    }
  return type;
}


double GeoPixelStaveDBHelper::PixelModuleAngle() const
{
  return geoAccessor().getDouble(staveTable_ptr,"MODULETILT",m_staveIndex)*CLHEP::deg;
}

double GeoPixelStaveDBHelper::PixelModuleAngleSign(int etaModule) const
{
  if (centerModule(etaModule)) return 0;
  if(etaModule < 0) return 1.;
  return -1.;
}

double GeoPixelStaveDBHelper::PixelModuleDrDistance() const
{
  return geoAccessor().getDouble(staveTable_ptr,"CENTRMODULESHIFT",m_staveIndex)*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelModuleZPositionTabulated(int etaModule, int type)
{ 
  if (!m_zPositionMap) 
    {
      m_zPositionMap = new InDetDD::PairIndexMap;
      for (unsigned int indexTmp = 0; indexTmp < geoAccessor().getTableSize(staveZTable_ptr); ++indexTmp) {
	int etaModule = geoAccessor().getInt(staveZTable_ptr,"ETAMODULE",indexTmp);
	int type      = geoAccessor().getInt(staveZTable_ptr,"TYPE",indexTmp);
	m_zPositionMap->add(type,etaModule,indexTmp);
      }
    }
  int index = m_zPositionMap->find(type, etaModule);
  if (index < 0) {
    msg(MSG::ERROR) << "Z position not found for etaModule,type =  " << etaModule << ", " << type << endreq;
    return 0;
  }
  return geoAccessor().getDouble(staveZTable_ptr,"ZPOS",index) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelModuleShiftFlag(int etaModule) const
{
  if (centerModule(etaModule)) return 1;
  return 0.;
}

double GeoPixelStaveDBHelper::PixelModuleStaggerDistance() const
{
  if (!(isSLHC() || isIBLGeometry()) || !geoAccessor().testField(staveTable_ptr,"STAGGERDIST",m_staveIndex)) return 0; 
  return geoAccessor().getDouble(staveTable_ptr,"STAGGERDIST",m_staveIndex) * CLHEP::mm;
}

int GeoPixelStaveDBHelper::PixelModuleStaggerSign(int etaModule) const
{
  if (!(isSLHC() || isIBLGeometry()) || !geoAccessor().testField(staveTable_ptr,"FIRSTSTAGGER",m_staveIndex)) return 0;  
  // FIRSTSTAGGER refers to whether the first module (lowest etavalue) is staggered up (+1) or down(-1)

  int firstStagger =  geoAccessor().getInt(staveTable_ptr,"FIRSTSTAGGER",m_staveIndex);
  int moduleIndex = PixelModuleIndexFromEta(etaModule);
  return firstStagger * (moduleIndex%2 ? -1 : 1);
}


/////////////////////////////////////////////////////////
//
// Si Boards Parameters:
//
/////////////////////////////////////////////////////////
double GeoPixelStaveDBHelper::PixelBoardWidth(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"BOARDWIDTH",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"BOARDWIDTH",PixelModuleType())*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelBoardLength(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"BOARDLENGTH",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"BOARDLENGTH",PixelModuleType())*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelBoardThickness(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"BOARDTHICK",PixelModuleType3D())*CLHEP::mm;
  return geoAccessor().getDouble(moduleTable_ptr,"BOARDTHICK",PixelModuleType())*CLHEP::mm;
}

double  GeoPixelStaveDBHelper::PixelBoardActiveLength(bool isModule3D) const 
{
  return DesignZActiveArea(isModule3D); 
}


/////////////////////////////////////////////////////////
//
// Hybrid Parameters:
//
/////////////////////////////////////////////////////////
double GeoPixelStaveDBHelper::PixelHybridWidth(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"HYBRIDWIDTH",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"HYBRIDWIDTH",PixelModuleType())*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelHybridLength(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"HYBRIDLENGTH",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"HYBRIDLENGTH",PixelModuleType())*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelHybridThickness(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"HYBRIDTHICK",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"HYBRIDTHICK",PixelModuleType())*CLHEP::mm;
}
 
/////////////////////////////////////////////////////////
//
// Chip Parameters:
//
/////////////////////////////////////////////////////////

double GeoPixelStaveDBHelper::PixelChipWidth(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"CHIPWIDTH",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"CHIPWIDTH",PixelModuleType())*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelChipLength(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"CHIPLENGTH",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"CHIPLENGTH",PixelModuleType())*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelChipGap(bool isModule3D) const 
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"CHIPGAP",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"CHIPGAP",PixelModuleType())*CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelChipThickness(bool isModule3D) const  
{
  if(isIBLGeometry()&&isModule3D)
    return geoAccessor().getDouble(moduleTable_ptr,"CHIPTHICK",PixelModuleType3D())*CLHEP::mm;

  return geoAccessor().getDouble(moduleTable_ptr,"CHIPTHICK",PixelModuleType())*CLHEP::mm;
}



//////////////////////////////////////////////////////////////
//
//   Readout geometry
//
//////////////////////////////////////////////////////////////

int  GeoPixelStaveDBHelper::designType() const
{
  if (geoAccessor().getTableSize(readoutTable_ptr) == 0) {
    msg(MSG::ERROR) << "ERROR in PixelReadout size. Should not occur!" << endreq;
    return 0;
  } else if (geoAccessor().getTableSize(readoutTable_ptr) == 1 && !isSLHC() && !isIBLGeometry()) {
    return 0;
  } else { // Only in IBL and SLHC
    return geoAccessor().getInt(moduleTable_ptr,"DESIGNTYPE",PixelModuleType());
  }
}


double GeoPixelStaveDBHelper::DesignRPActiveArea(bool isModule3D) const
{
  // All layers assumed to be the same.
  return DesignPitchRP(isModule3D) * ((DesignNumRowsPerChip(isModule3D)+DesignNumEmptyRowsInGap(isModule3D)) * DesignNumChipsPhi(isModule3D) - DesignNumEmptyRowsInGap(isModule3D));

}

double GeoPixelStaveDBHelper::DesignZActiveArea(bool isModule3D) const
{
  // All layers assumed to be the same.
  return (DesignPitchZ(isModule3D) * (DesignNumColsPerChip(isModule3D) - 2) + 2 * DesignPitchZLong(isModule3D)) * DesignNumChipsEta(isModule3D) + 2 * (DesignPitchZLongEnd(isModule3D) - DesignPitchZLong(isModule3D));

}
    
double GeoPixelStaveDBHelper::DesignPitchRP(bool isModule3D) const
{
  int type = designType();
  if(isIBLGeometry()&&isModule3D)type+=1;
  return geoAccessor().getDouble(readoutTable_ptr,"PITCHPHI",type) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::DesignPitchZ(bool isModule3D) const
{
  int type = designType();
  if(isIBLGeometry()&&isModule3D)type+=1;
  return geoAccessor().getDouble(readoutTable_ptr,"PITCHETA",type) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::DesignPitchZLong(bool isModule3D) const
{
  int type = designType();
  if(isIBLGeometry()&&isModule3D)type+=1;
  double pitch = geoAccessor().getDouble(readoutTable_ptr,"PITCHETALONG",type) * CLHEP::mm;
  if (pitch == 0) pitch = DesignPitchZ(isModule3D);
  return pitch;
}

double GeoPixelStaveDBHelper::DesignPitchZLongEnd(bool isModule3D) const
{
  // Defaults to DesignPitchZLongEnd if not specified or is zero.
  if (!isSLHC() && !isIBLGeometry()) { // This check is not really needed once the field is in the database.
    return DesignPitchZLong(isModule3D);
  } else {
    int type = designType();
    if(isIBLGeometry()&&isModule3D)type+=1;
    double pitch = 0;  
    if (geoAccessor().testField(readoutTable_ptr,"PITCHETAEND",type)) {
      pitch = geoAccessor().getDouble(readoutTable_ptr,"PITCHETAEND",type) * CLHEP::mm;
    }
    if (pitch == 0) pitch = DesignPitchZLong(isModule3D);
    return pitch;
  }
}


double GeoPixelStaveDBHelper::DesignGapRP(bool isModule3D) const
{
  return DesignNumEmptyRowsInGap(isModule3D) * DesignPitchRP(isModule3D);

}

double GeoPixelStaveDBHelper::DesignGapZ(bool isModule3D) const
{
  return 2. * (DesignPitchZLong(isModule3D) - DesignPitchZ(isModule3D));

}

int GeoPixelStaveDBHelper::DesignCircuitsPhi(bool /*isModule3D*/) const
{
  //
  // This should be (*pdch)[0]->getDouble("NRPCHIP"), but in the current
  // design we prefer to have one chip in the rphi direction
  // and define the connections for the pixels in the gap
  return 1;
}

int GeoPixelStaveDBHelper::DesignCircuitsEta(bool isModule3D) const
{
  return DesignNumChipsEta(isModule3D);
}

int GeoPixelStaveDBHelper::DesignReadoutSide(bool isModule3D) const
{
  int type = designType();
  if(isIBLGeometry()&&isModule3D) type+=1;
  
  return geoAccessor().getInt(readoutTable_ptr,"READOUTSIDE",type);
}

int GeoPixelStaveDBHelper::DesignNumChipsPhi(bool isModule3D) const
{
  int type = designType();
  if(isIBLGeometry()&&isModule3D) type+=1;
  
  return geoAccessor().getInt(readoutTable_ptr,"NCHIPSPHI",type);
}    


int GeoPixelStaveDBHelper::DesignNumChipsEta(bool isModule3D) const
 {
   int type = designType();
   if(isIBLGeometry()&&isModule3D) type+=1;
   
   return geoAccessor().getInt(readoutTable_ptr,"NCHIPSETA",type);
}

int GeoPixelStaveDBHelper::DesignNumRowsPerChip(bool isModule3D) const
{
  int type = designType();
  if(isIBLGeometry()&&isModule3D) type+=1;
  
  return geoAccessor().getInt(readoutTable_ptr,"ROWSPERCHIP",type);
}

int GeoPixelStaveDBHelper::DesignNumColsPerChip(bool isModule3D) const 
{
  int type = designType();
  if(isIBLGeometry()&&isModule3D) type+=1;
  
  return geoAccessor().getInt(readoutTable_ptr,"COLSPERCHIP",type);
}


int GeoPixelStaveDBHelper::DesignDiodesPhiTotal(bool isModule3D) const
{
  return DesignNumChipsPhi(isModule3D) * (DesignNumRowsPerChip(isModule3D)+DesignNumEmptyRowsInGap(isModule3D)) - DesignNumEmptyRowsInGap(isModule3D);
}

int GeoPixelStaveDBHelper::DesignDiodesEtaTotal(bool isModule3D) const
{
  return DesignNumChipsEta(isModule3D) * DesignNumColsPerChip(isModule3D);
}


int GeoPixelStaveDBHelper::DesignCellRowsPerCircuit(bool isModule3D) const
{
  return DesignNumChipsPhi(isModule3D) * DesignNumRowsPerChip(isModule3D);
}

int GeoPixelStaveDBHelper::DesignCellColumnsPerCircuit(bool isModule3D) const
{
  return DesignNumColsPerChip(isModule3D);
}

int GeoPixelStaveDBHelper::DesignDiodeRowsPerCircuit(bool isModule3D) const
{
  return DesignDiodesPhiTotal(isModule3D);
}

int GeoPixelStaveDBHelper::DesignDiodeColumnsPerCircuit(bool isModule3D) const
{
  return DesignNumColsPerChip(isModule3D);
}

int  GeoPixelStaveDBHelper::DesignNumEmptyRowsInGap(bool isModule3D) const
{
  // Could determine it from m_gangedIndexMap but expect it to be filled correctly in PixelReadoutTable 
  int type=designType();
  if(isIBLGeometry()&&isModule3D) type+=1;
  
  return geoAccessor().getInt(readoutTable_ptr,"EMPTYROWS",type);
}

// Ganged Pixels
int GeoPixelStaveDBHelper::GangedType() const
{
  // type 0 means no ganged pixels
  if (!isSLHC() && !isIBLGeometry()) return 1;
  if (isIBLGeometry()) {
    return geoAccessor().getInt(readoutTable_ptr,"GANGEDTYPE",designType());
  } else {
    int type = 1;
    if (geoAccessor().testField(readoutTable_ptr,"GANGEDTYPE",designType())) {
      type = geoAccessor().getInt(readoutTable_ptr,"GANGEDTYPE",designType());
    }
    return type;
  }
}
      

/////////////////////////////////////////////////////////
//
// IBL Parameters:
//
/////////////////////////////////////////////////////////
// IBL detailed stave support only

int GeoPixelStaveDBHelper::IBLStaveModuleNumber() const
{
  return m_nbPlanarModule+m_nb3DModule;
}

int GeoPixelStaveDBHelper::IBLStaveModuleNumber_planar() const
{
  return m_nbPlanarModule;
}

int GeoPixelStaveDBHelper::IBLStaveModuleNumber_3D() const
{
  return m_nb3DModule;
}

double GeoPixelStaveDBHelper::IBLStaveFacePlateThickness() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"FACEPLATETHICK",index)) {
    double thickness = geoAccessor().getDouble(PixelIBLStave,"FACEPLATETHICK",index) * CLHEP::mm;
    if (thickness > 0)  return thickness ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveMechanicalStaveWidth() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"STAVEWIDTH",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"STAVEWIDTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveMechanicalStaveEndBlockLength() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"ENDBLOCKLENGTH",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"ENDBLOCKLENGTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveMechanicalStaveEndBlockFixPoint() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"ENDBLOCKFIXINGPOS",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"ENDBLOCKFIXINGPOS",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveMechanicalStaveEndBlockOmegaOverlap() const
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (geoAccessor().testField(PixelIBLStave,"ENDBLOCKOMEGAOVERLAP",index)) {
      double value = geoAccessor().getDouble(PixelIBLStave,"ENDBLOCKOMEGAOVERLAP",index) * CLHEP::mm;
      return value ;
    } 
    return 0.0;
  }
  catch(...){}
  return 0.;
}

double GeoPixelStaveDBHelper::IBLStaveLength() const
{
 // try and catch (param availbale only if db tag > IBL-03-00-00)
  try
    {
      int index=0;
      if (geoAccessor().testField(PixelIBLStave,"STAVELENGTH",index)) {
	double value = geoAccessor().getDouble(PixelIBLStave,"STAVELENGTH",index) * CLHEP::mm;
	return value ;
      } 
    }
  catch(...)
    {
      // FIXME : patch for initial IBL geometry (SES)
      //           IBL stave length not eqal to other stave length 
    }  
  
  return 748.0 * CLHEP::mm;  
}

double GeoPixelStaveDBHelper:: IBLStaveMechanicalStaveOffset(bool isModule3D) const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (!isModule3D&&geoAccessor().testField(PixelIBLStave,"MODULELATERALOFFSET",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"MODULELATERALOFFSET",index) * CLHEP::mm;
    return value ;
  } 
  if (isModule3D&&geoAccessor().testField(PixelIBLStave,"MODULELATERALOFFSET3D",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"MODULELATERALOFFSET3D",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveMechanicalStaveModuleOffset() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"STAVETOMODULEGAP",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"STAVETOMODULEGAP",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveTubeOuterDiameter() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"TUBEOUTERDIAM",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"TUBEOUTERDIAM",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveTubeInnerDiameter() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"TUBEINNERDIAM",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"TUBEINNERDIAM",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveTubeMiddlePos() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"TUBEMIDDLEPOS",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"TUBEMIDDLEPOS",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveFlexLayerThickness() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"FLEXLAYERTHICK",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"FLEXLAYERTHICK",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveFlexBaseThickness() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"FLEXBASETHICK",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"FLEXBASETHICK",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveFlexWidth() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"FLEXWIDTH",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"FLEXWIDTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLStaveFlexOffset() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"FLEXOFFSET",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"FLEXOFFSET",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}


double GeoPixelStaveDBHelper::IBLStaveOmegaThickness() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"OMEGATHICK",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"OMEGATHICK",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLStaveOmegaEndCenterX() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"OMEGAENDCENTERX",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"OMEGAENDCENTERX",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}
double GeoPixelStaveDBHelper::IBLStaveOmegaEndCenterY() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"OMEGAENDCENTERY",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"OMEGAENDCENTERY",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}
double GeoPixelStaveDBHelper::IBLStaveOmegaEndRadius() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"OMEGAENDRADIUS",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"OMEGAENDRADIUS",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}
double GeoPixelStaveDBHelper::IBLStaveOmegaEndAngle() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"OMEGAENDANGLE",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"OMEGAENDANGLE",index) * CLHEP::deg;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLStaveOmegaMidCenterX() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"OMEGAMIDCENTERX",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"OMEGAMIDCENTERX",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLStaveOmegaMidRadius() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"OMEGAMIDRADIUS",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"OMEGAMIDRADIUS",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}
double GeoPixelStaveDBHelper::IBLStaveOmegaMidAngle() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"OMEGAOPENINGANGLE",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"OMEGAOPENINGANGLE",index) * CLHEP::deg;
    return value ;
  } 
  return 0.0;
}

int GeoPixelStaveDBHelper::IBLStaveModuleNumber_AllPlanar() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"NMODULE",index)) {
    int value = geoAccessor().getInt(PixelIBLStave,"NMODULE",index);
    if (value > 0)  return value ;
  } 
  return 0;
}

// int GeoPixelStaveDBHelper::IBLStaveModuleNumber() const
// {
//   return m_PlanarModuleNumber+m_3DModuleNumber;
// }

double GeoPixelStaveDBHelper::IBLStaveModuleGap() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"MODULEGAP",index)) {
    double value = geoAccessor().getDouble(PixelIBLStave,"MODULEGAP",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

int GeoPixelStaveDBHelper::IBLStaveModuleType() const 
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLStave,"MODULETYPE",index)) {
    int value = geoAccessor().getInt(PixelIBLStave,"MODULETYPE",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0;
}

double GeoPixelStaveDBHelper::IBLStaveFacePlateGreaseThickness() const
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (geoAccessor().testField(PixelIBLGlueGrease,"FACEPLATEGREASETHICK",index)) {
      double value = geoAccessor().getDouble(PixelIBLGlueGrease,"FACEPLATEGREASETHICK",index) * CLHEP::mm;
      return value ;
    }
    return 0.;
  }
  catch(...){}
  return 0.;
}

double GeoPixelStaveDBHelper::IBLStaveFacePlateGlueThickness() const
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (geoAccessor().testField(PixelIBLGlueGrease,"FACEPLATEGLUETHICK",index)) {
      double value = geoAccessor().getDouble(PixelIBLGlueGrease,"FACEPLATEGLUETHICK",index) * CLHEP::mm;
      return value ;
    }
    return 0.;
  }
  catch(...) {}
  return 0.;
}

double GeoPixelStaveDBHelper::IBLStaveTubeGlueThickness() const
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (geoAccessor().testField(PixelIBLGlueGrease,"TUBEGLUETHICK",index)) {
      double value = geoAccessor().getDouble(PixelIBLGlueGrease,"TUBEGLUETHICK",index) * CLHEP::mm;
      return value ;
    }
    return 0.;
  }
  catch(...) {}
  return 0.;
}

double GeoPixelStaveDBHelper::IBLStaveOmegaGlueThickness() const
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (geoAccessor().testField(PixelIBLGlueGrease,"OMEGAGLUETHICK",index)) {
      double value = geoAccessor().getDouble(PixelIBLGlueGrease,"OMEGAGLUETHICK",index) * CLHEP::mm;
      return value ;
    }
    return 0.;
  }
  catch(...){}
  return 0.;
}


double GeoPixelStaveDBHelper:: IBLSupportRingWidth() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLSupport,"STAVERINGWIDTH",index)) {
    double value = geoAccessor().getDouble(PixelIBLSupport,"STAVERINGWIDTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLSupportRingInnerRadius() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLSupport,"STAVERINGINNERRADIUS",index)) {
    double value = geoAccessor().getDouble(PixelIBLSupport,"STAVERINGINNERRADIUS",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLSupportRingOuterRadius() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLSupport,"STAVERINGOUTERRADIUS",index)) {
    double value = geoAccessor().getDouble(PixelIBLSupport,"STAVERINGOUTERRADIUS",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}


double GeoPixelStaveDBHelper:: IBLSupportMechanicalStaveRingFixPoint() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLSupport,"STAVERINGFIXINGPOS",index)) {
    double value = geoAccessor().getDouble(PixelIBLSupport,"STAVERINGFIXINGPOS",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLSupportMidRingWidth() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLSupport,"STAVEMIDRINGWIDTH",index)) {
    double value = geoAccessor().getDouble(PixelIBLSupport,"STAVEMIDRINGWIDTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLSupportMidRingInnerRadius() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLSupport,"STAVEMIDRINGINNERRADIUS",index)) {
    double value = geoAccessor().getDouble(PixelIBLSupport,"STAVEMIDRINGINNERRADIUS",index) * CLHEP::mm;
    if (value > 0)  return value;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper:: IBLSupportMidRingOuterRadius() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLSupport,"STAVEMIDRINGOUTERRADIUS",index)) {
    double value = geoAccessor().getDouble(PixelIBLSupport,"STAVEMIDRINGOUTERRADIUS",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLFlexMiddleGap() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,"FLEXMIDGAP",index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,"FLEXMIDGAP",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

bool GeoPixelStaveDBHelper::IBLFlexAndWingDefined() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,"FLEXMIDGAP",index)) return true;
  return false;
}


double GeoPixelStaveDBHelper::IBLFlexDoglegLength() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,"FLEXDOGLEGLENGTH",index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,"FLEXDOGLEGLENGTH",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}


double GeoPixelStaveDBHelper::IBLStaveFlexWingWidth() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,"FLEXWINGWIDTH",index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,"FLEXWINGWIDTH",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLStaveFlexWingThick() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,"FLEXWINGTHICK",index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,"FLEXWINGTHICK",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLFlexDoglegRatio() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,"FLEXDOGLEGRATIO",index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,"FLEXDOGLEGRATIO",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLFlexDoglegHeight(int iHeight) const
{
  std::ostringstream lname;
  lname << "FLEXDOGLEGHEIGHT"<<iHeight;

  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,lname.str(),index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,lname.str(),index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLFlexDoglegDY() const
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,"FLEXDOGLEGDY",index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,"FLEXDOGLEGDY",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLFlexPP0Z(int iPos) const
{
  std::ostringstream lname;
  lname << "FLEXPP0_Z"<<iPos;

  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,lname.str(),index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,lname.str(),index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}


double GeoPixelStaveDBHelper::IBLFlexPP0Rmin(int iPos) const
{
  std::ostringstream lname;
  lname << "FLEXPP0_S"<<iPos<<"RMIN";

  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,lname.str(),index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,lname.str(),index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double GeoPixelStaveDBHelper::IBLFlexPP0Rmax(int iPos) const
{
  std::ostringstream lname;
  lname << "FLEXPP0_S"<<iPos<<"RMAX";

  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (geoAccessor().testField(PixelIBLFlex,lname.str(),index)) {
    double value = geoAccessor().getDouble(PixelIBLFlex,lname.str(),index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}


std::string GeoPixelStaveDBHelper::IBLFlexMaterial(int iPos, const std::string& flexType) const
{

  int nbMaterial=geoAccessor().getTableSize(PixelIBLFlexMaterial);
  int cmptType=0;

  for(int index=0; index<nbMaterial; index++)
    {
      std::string flexTypeIdx = geoAccessor().getString(PixelIBLFlexMaterial,"TYPE",index);
      if(flexTypeIdx.compare(flexType)==0)
	{
	  cmptType++;
	  if(iPos==cmptType){
	    std::string matTypeIdx = geoAccessor().getString(PixelIBLFlexMaterial,"MATERIALNAME",index);
	    return matTypeIdx;
	  }
	}
    }
  return std::string("noMat");
      
}


//
/// TMT  (pixel ladder support)
//

int GeoPixelStaveDBHelper::PixelTMTNumParts() const
{
  return geoAccessor().getTableSize(TMTTable_ptr);
}

double GeoPixelStaveDBHelper::PixelTMTWidthX1(int iPart) const
{
  return geoAccessor().getDouble(TMTTable_ptr,"WIDTHX1",iPart) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelTMTWidthX2(int iPart) const
{
  return geoAccessor().getDouble(TMTTable_ptr,"WIDTHX2",iPart) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelTMTWidthY(int iPart) const
{
  return geoAccessor().getDouble(TMTTable_ptr,"WIDTHY",iPart) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelTMTBaseX1(int iPart) const
{
  return geoAccessor().getDouble(TMTTable_ptr,"BASEX1",iPart) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelTMTBaseX2(int iPart) const
{
  return geoAccessor().getDouble(TMTTable_ptr,"BASEX2",iPart) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelTMTPosY(int iPart) const
{
  return geoAccessor().getDouble(TMTTable_ptr,"Y",iPart) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelTMTPosZ1(int iPart) const
{
  return geoAccessor().getDouble(TMTTable_ptr,"Z1",iPart) * CLHEP::mm;
}

double GeoPixelStaveDBHelper::PixelTMTPosZ2(int iPart) const
{
  return geoAccessor().getDouble(TMTTable_ptr,"Z2",iPart) * CLHEP::mm;
}

bool GeoPixelStaveDBHelper::PixelTMTPerModule(int iPart) const
{
  return geoAccessor().getInt(TMTTable_ptr,"PERMODULE",iPart);
}


