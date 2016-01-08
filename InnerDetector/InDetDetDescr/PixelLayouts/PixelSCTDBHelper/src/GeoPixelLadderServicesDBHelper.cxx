/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Ladder services (Omega + Al tube + glue + part of pigtail + connector)
//
//
#include "PixelSCTDBHelper/GeoPixelLadderServicesDBHelper.h"
#include "PixelSCTDBHelper/GeoPixelHelperStaveTypes.h"
#include "PixelSCTDBHelper/GeoPixelHelperMaterialMap.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include <cmath>
#include <sstream>
#include <vector>


GeoPixelLadderServicesDBHelper::GeoPixelLadderServicesDBHelper( PixelGeoBuilderBasics* basics):
  PixelGeoBuilder(basics)
{

  m_epsilon=0.0001;

  IRDBRecordset_ptr staveTypeTable_ptr = geoAccessor().getTable("PixelStaveType");
  m_pixelStaveTypes= new GeoPixelHelperStaveTypes( &geoAccessor(), staveTypeTable_ptr);

  // Create material map
  IRDBRecordset_ptr materialMapTable_ptr = geoAccessor().getTable("PixelMaterialMap");
  m_materialMap = new GeoPixelHelperMaterialMap( &geoAccessor(), materialMapTable_ptr);

  layerTable_ptr = geoAccessor().getTable("PixelLayer");
  staveTable_ptr = geoAccessor().getTable("PixelStave");
  ladderTable_ptr = geoAccessor().getTable("PixelLadder");
  connectorTable_ptr = geoAccessor().getTable("PixelConnector");
  omegaTable_ptr = geoAccessor().getTable("PixelOmega");
  omegaGlueTable_ptr = geoAccessor().getTable("PixelOmegaGlue");
  tubeTable_ptr = geoAccessor().getTable("PixelAlTube");
  pigtailTable_ptr = geoAccessor().getTable("PixelPigtail");
  barrelCableTable_ptr = geoAccessor().getTable("PixelBarrelCable");
  fluidTable_ptr = geoAccessor().getTable("PixelFluid");

}

GeoPixelLadderServicesDBHelper::~GeoPixelLadderServicesDBHelper()
{
  delete m_pixelStaveTypes;
}

double GeoPixelLadderServicesDBHelper::PixelLayerRadius() const 
{
  double radius = geoAccessor().getDouble(layerTable_ptr,"RLAYER",m_layer)*CLHEP::mm;
  return radius;
}

double GeoPixelLadderServicesDBHelper::PixelLadderLength() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"ENVLENGTH",m_staveIndex)*CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelLadderServicesX() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"SERVICEOFFSETX",m_staveIndex) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelLadderServicesY() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"SERVICEOFFSETY",m_staveIndex) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelLadderCableOffsetX() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"CABLEOFFSETX",m_staveIndex) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelLadderCableOffsetY() const 
{
  return geoAccessor().getDouble(staveTable_ptr,"CABLEOFFSETY",m_staveIndex) * CLHEP::mm;
}

//
//  Connector
//

int GeoPixelLadderServicesDBHelper::PixelNumConnectorElements() const
{
  return geoAccessor().getTableSize(connectorTable_ptr);
}
double GeoPixelLadderServicesDBHelper::PixelConnectorWidthX(int index) const
{
  return geoAccessor().getDouble(connectorTable_ptr,"WIDTHX",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelConnectorWidthY(int index) const
{
  return geoAccessor().getDouble(connectorTable_ptr,"WIDTHY",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelConnectorWidthZ(int index) const
{
  return geoAccessor().getDouble(connectorTable_ptr,"WIDTHZ",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelConnectorPosX(int index) const
{
  return geoAccessor().getDouble(connectorTable_ptr,"X",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelConnectorPosY(int index) const
{
  return geoAccessor().getDouble(connectorTable_ptr,"Y",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelConnectorPosZ(int index) const
{
  return geoAccessor().getDouble(connectorTable_ptr,"Z",index) * CLHEP::mm;
}

//
// Omega parameters
//
double GeoPixelLadderServicesDBHelper::PixelOmegaUpperBendX() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"UPPERBENDX") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaUpperBendY() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"UPPERBENDY") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaUpperBendRadius() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"UPPERBENDR") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaLowerBendX() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"LOWERBENDX") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaLowerBendY() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"LOWERBENDY") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaLowerBendRadius() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"LOWERBENDR") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaWallThickness() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"THICK") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaLength() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"LENGTH") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaStartY() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"STARTY") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaEndY() const
{
  return geoAccessor().getDouble(omegaTable_ptr,"ENDY") * CLHEP::mm;
}

//
// Al Tube
//

double GeoPixelLadderServicesDBHelper::PixelAlTubeUpperBendX() const
{
  return geoAccessor().getDouble(tubeTable_ptr,"UPPERBENDX") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelAlTubeUpperBendY() const
{
  return geoAccessor().getDouble(tubeTable_ptr,"UPPERBENDY") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelAlTubeUpperBendRadius() const
{
  return geoAccessor().getDouble(tubeTable_ptr,"UPPERBENDR") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelAlTubeLowerBendX() const
{
  return geoAccessor().getDouble(tubeTable_ptr,"LOWERBENDX") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelAlTubeLowerBendY() const
{
  return geoAccessor().getDouble(tubeTable_ptr,"LOWERBENDY") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelAlTubeLowerBendRadius() const
{
  return geoAccessor().getDouble(tubeTable_ptr,"LOWERBENDR") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelAlTubeWallThickness() const
{
  return geoAccessor().getDouble(tubeTable_ptr,"THICK") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelAlTubeLength() const
{
  return geoAccessor().getDouble(tubeTable_ptr,"LENGTH") * CLHEP::mm;
}

//
// Glue
// 

int GeoPixelLadderServicesDBHelper::PixelNumOmegaGlueElements() const
{
  return geoAccessor().getTableSize(omegaGlueTable_ptr);
}

double GeoPixelLadderServicesDBHelper::PixelOmegaGlueStartX(int index) const
{
  return geoAccessor().getDouble(omegaGlueTable_ptr,"STARTX",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaGlueThickness(int index) const
{
  return geoAccessor().getDouble(omegaGlueTable_ptr,"THICK",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaGlueStartY(int index) const
{
  return geoAccessor().getDouble(omegaGlueTable_ptr,"STARTY",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaGlueEndY(int index) const
{
  return geoAccessor().getDouble(omegaGlueTable_ptr,"ENDY",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaGlueLength(int index) const
{
  return geoAccessor().getDouble(omegaGlueTable_ptr,"LENGTH",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelOmegaGluePosZ(int index) const
{
  return geoAccessor().getDouble(omegaGlueTable_ptr,"Z",index) * CLHEP::mm;
}

int GeoPixelLadderServicesDBHelper::PixelOmegaGlueTypeNum(int index) const
{
  return geoAccessor().getInt(omegaGlueTable_ptr,"TYPENUM",index);
}


//
// Pigtail
//
double GeoPixelLadderServicesDBHelper::PixelPigtailThickness() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"THICK") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailStartY() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"STARTY") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailEndY() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"ENDY") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailWidthZ() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"WIDTHZ") * CLHEP::mm;
}

// Different width from the curved section in old geometry
double GeoPixelLadderServicesDBHelper::PixelPigtailFlatWidthZ() const
{
  return PixelPigtailWidthZ();
}

double GeoPixelLadderServicesDBHelper::PixelPigtailPosX() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"X") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailPosZ() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"Z") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailBendX() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"BENDX") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailBendY() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"BENDY") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailBendRMin() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"BENDRMIN") * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailBendRMax() const
{
  return PixelPigtailBendRMin() + PixelPigtailThickness();
}

double GeoPixelLadderServicesDBHelper::PixelPigtailBendPhiMin() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"BENDPHIMIN") * CLHEP::deg;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailBendPhiMax() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"BENDPHIMAX") * CLHEP::deg;
}

double GeoPixelLadderServicesDBHelper::PixelPigtailEnvelopeLength() const
{
  return geoAccessor().getDouble(pigtailTable_ptr,"ENVLENGTH") * CLHEP::mm;
}

//--------------------------------------
// Modules
//--------------------------------------

int GeoPixelLadderServicesDBHelper::PixelNModule() const
{
  return geoAccessor().getInt(staveTable_ptr,"NMODULE",m_staveIndex);
}

int GeoPixelLadderServicesDBHelper::PixelModuleEtaFromIndex(int index) const 
{
  int nModules = PixelNModule();
  int etaModule = index-nModules/2;
  return etaModule;
}

int GeoPixelLadderServicesDBHelper::PixelModuleIndexFromEta(int etaModule) const
{
  int nModules = PixelNModule();  
  int index = etaModule + nModules/2;
  return index;
}

double GeoPixelLadderServicesDBHelper::PixelModuleZPosition(int etaModule) const
{
  //  int zPosType = 0;
  // Equi-distant modules
  int moduleIndex =  PixelModuleIndexFromEta(etaModule);  
  return geoAccessor().getDouble(staveTable_ptr,"MODULEDZ",m_staveIndex)*CLHEP::mm * (moduleIndex - 0.5*(PixelNModule()-1));

}

int GeoPixelLadderServicesDBHelper::PixelCableElements() const
{
  return geoAccessor().getTableSize(barrelCableTable_ptr);
}

int GeoPixelLadderServicesDBHelper::PixelCableLayerNum(int index) const
{
  return geoAccessor().getInt(barrelCableTable_ptr,"LAYER",index);
}

int GeoPixelLadderServicesDBHelper::PixelCableBiStaveNum(int index) const
{
  return geoAccessor().getInt(barrelCableTable_ptr,"BISTAVE",index);
}


double GeoPixelLadderServicesDBHelper::PixelCableZStart(int index) const
{
  return geoAccessor().getDouble(barrelCableTable_ptr,"ZSTART",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelCableZEnd(int index) const
{
  return geoAccessor().getDouble(barrelCableTable_ptr,"ZEND",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelCableWidth(int index) const
{
  return geoAccessor().getDouble(barrelCableTable_ptr,"WIDTH",index) * CLHEP::mm;
}
double GeoPixelLadderServicesDBHelper::PixelCableThickness(int index) const
{
  return geoAccessor().getDouble(barrelCableTable_ptr,"THICK",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelCableStackOffset(int index) const
{
  return geoAccessor().getDouble(barrelCableTable_ptr,"STACKPOS",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelCableWeight(int index) const
{
  return geoAccessor().getDouble(barrelCableTable_ptr,"WEIGHT",index) * CLHEP::gram;
}

std::string GeoPixelLadderServicesDBHelper::PixelCableLabel(int index) const
{
  return geoAccessor().getString(barrelCableTable_ptr,"LABEL",index);
}

  


//
// Fluid
// 
double GeoPixelLadderServicesDBHelper::PixelFluidZ1(int index) const
{
  return geoAccessor().getDouble(fluidTable_ptr,"Z1",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelFluidZ2(int index) const
{
  return geoAccessor().getDouble(fluidTable_ptr,"Z2",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelFluidThick1(int index) const
{
  return geoAccessor().getDouble(fluidTable_ptr,"THICK1",index) * CLHEP::mm;
}


double GeoPixelLadderServicesDBHelper::PixelFluidThick2(int index) const
{
  return geoAccessor().getDouble(fluidTable_ptr,"THICK2",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelFluidWidth(int index) const
{
  return geoAccessor().getDouble(fluidTable_ptr,"WIDTH",index) * CLHEP::mm;
}


double GeoPixelLadderServicesDBHelper::PixelFluidX(int index) const
{
  return geoAccessor().getDouble(fluidTable_ptr,"X",index) * CLHEP::mm;
}

double GeoPixelLadderServicesDBHelper::PixelFluidY(int index) const
{
  return geoAccessor().getDouble(fluidTable_ptr,"Y",index) * CLHEP::mm;
}

int GeoPixelLadderServicesDBHelper::PixelFluidType(int index) const
{
  return geoAccessor().getInt(fluidTable_ptr,"TYPE",index);
}

int GeoPixelLadderServicesDBHelper::PixelFluidNumTypes() const
{
  return geoAccessor().getTableSize(fluidTable_ptr);
}

int GeoPixelLadderServicesDBHelper::PixelFluidIndex(int type) const
{
  for (int i = 0; i < PixelFluidNumTypes(); i++) {
    if (type == PixelFluidType(i)) return i;
  }
  msg(MSG::ERROR) << "Unrecognized fluid volume type: " << type << endreq;
  return -1;
}
 
std::string GeoPixelLadderServicesDBHelper::PixelFluidMat(int index) const 
{
  int matType = 0;

  matType = geoAccessor().getInt(fluidTable_ptr,"MATTYPE",index);
  return getMaterialName("Fluid", 0, matType);
}

int GeoPixelLadderServicesDBHelper::PixelFluidOrient(int layer, int phi) const 
{
  return m_pixelStaveTypes->getFluidType(layer,phi);
}

int GeoPixelLadderServicesDBHelper::PixelBiStaveType(int layer, int phi) const
{
  //if (m_staveTypeTable->size() == 0) return phi % 2;       SES fixme
  return m_pixelStaveTypes->getBiStaveType(layer, phi) % 2;
}


/////////////////////////////////////////////////////
//
//  Materials
//
/////////////////////////////////////////////////////

std::string GeoPixelLadderServicesDBHelper::getMaterialName(const std::string & volumeName, int layerdisk, int typenum) const 
{
  return m_materialMap->getMaterial(layerdisk, typenum, volumeName);
}
