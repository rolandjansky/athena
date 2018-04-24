/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLADDERSERVICESDBHELPER_H
#define GEOPIXELLADDERSERVICESDBHELPER_H

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <sstream>

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 


  class GeoPixelHelperStaveTypes;
  class GeoPixelHelperMaterialMap;

  class GeoPixelLadderServicesDBHelper :  public PixelGeoBuilder {
  public:
    GeoPixelLadderServicesDBHelper( PixelGeoBuilderBasics* basics);
    ~GeoPixelLadderServicesDBHelper();
    
    void ResetLayerAndStave(int layer, int stave) { m_layer=layer; m_staveIndex=stave; };
    double getEpsilon() const { return m_epsilon; }

    int PixelNModule() const; 
    int PixelModuleEtaFromIndex(int index) const;
    int PixelModuleIndexFromEta(int etaModule) const; 
    double PixelModuleZPosition(int etaModule) const;

    double PixelLadderLength() const; 
    double PixelLadderServicesX() const; 
    double PixelLadderServicesY() const; 
    double PixelLadderCableOffsetX() const; 
    double PixelLadderCableOffsetY() const; 

    double PixelLayerRadius() const ;

    std::string getMaterialName(const std::string & volumeName, int layerdisk=0, int typenum=0) const;

    int PixelNumConnectorElements() const;
    double PixelConnectorWidthX(int index) const;
    double PixelConnectorWidthY(int index) const;
    double PixelConnectorWidthZ(int index) const;
    double PixelConnectorPosX(int index) const;
    double PixelConnectorPosY(int index) const;
    double PixelConnectorPosZ(int index) const;

    double PixelOmegaUpperBendX() const;
    double PixelOmegaUpperBendY() const;
    double PixelOmegaUpperBendRadius() const;
    double PixelOmegaLowerBendX() const;
    double PixelOmegaLowerBendY() const;
    double PixelOmegaLowerBendRadius() const;
    double PixelOmegaWallThickness() const;
    double PixelOmegaLength() const;
    double PixelOmegaStartY() const;
    double PixelOmegaEndY() const;

    double PixelAlTubeUpperBendX() const;
    double PixelAlTubeUpperBendY() const;
    double PixelAlTubeUpperBendRadius() const;
    double PixelAlTubeLowerBendX() const;
    double PixelAlTubeLowerBendY() const;
    double PixelAlTubeLowerBendRadius() const;
    double PixelAlTubeWallThickness() const;
    double PixelAlTubeLength() const;

    int PixelNumOmegaGlueElements() const;
    double PixelOmegaGlueStartX(int index) const;
    double PixelOmegaGlueThickness(int index) const;
    double PixelOmegaGlueStartY(int index) const;
    double PixelOmegaGlueEndY(int index) const;
    double PixelOmegaGlueLength(int index) const;
    double PixelOmegaGluePosZ(int index) const;
    int PixelOmegaGlueTypeNum(int index) const;

    double PixelPigtailThickness() const;
    double PixelPigtailStartY() const;
    double PixelPigtailEndY() const;
    double PixelPigtailWidthZ() const;
    double PixelPigtailFlatWidthZ() const;
    double PixelPigtailPosX() const;
    double PixelPigtailPosZ() const;
    double PixelPigtailBendX() const;
    double PixelPigtailBendY() const;
    double PixelPigtailBendRMin() const;
    double PixelPigtailBendRMax() const;
    double PixelPigtailBendPhiMin() const;
    double PixelPigtailBendPhiMax() const;
    double PixelPigtailEnvelopeLength() const;
    
    int PixelCableElements() const;
    int PixelBiStaveType(int layer, int phi) const;
    int PixelCableLayerNum(int index) const;
    int PixelCableBiStaveNum(int index) const;
    double PixelCableZStart(int index) const;
    double PixelCableZEnd(int index) const;
    double PixelCableWidth(int index) const;
    double PixelCableThickness(int index) const;
    double PixelCableStackOffset(int index) const;
    double PixelCableWeight(int index) const;
    std::string PixelCableLabel(int index) const;

    double PixelFluidZ1(int index) const;
    double PixelFluidZ2(int index) const;
    double PixelFluidThick1(int index) const;
    double PixelFluidThick2(int index) const;
    double PixelFluidWidth(int index) const;
    double PixelFluidX(int index) const;
    double PixelFluidY(int index) const;
    int PixelFluidType(int index) const;
    int PixelFluidNumTypes() const;
    int PixelFluidIndex(int type) const;
    std::string PixelFluidMat(int index) const;
    int PixelFluidOrient(int layer, int phi) const; 

  private:
    int m_layer;
    int m_sector;
    int m_ladderType;
    int m_staveIndex;
    double m_epsilon;
    
    GeoPixelHelperStaveTypes* m_pixelStaveTypes;
    GeoPixelHelperMaterialMap* m_materialMap;

    IRDBRecordset_ptr layerTable_ptr;
    IRDBRecordset_ptr staveTable_ptr;
    IRDBRecordset_ptr ladderTable_ptr;
    IRDBRecordset_ptr connectorTable_ptr;
    IRDBRecordset_ptr omegaTable_ptr;
    IRDBRecordset_ptr omegaGlueTable_ptr;
    IRDBRecordset_ptr tubeTable_ptr;
    IRDBRecordset_ptr pigtailTable_ptr;
    IRDBRecordset_ptr barrelCableTable_ptr;
    IRDBRecordset_ptr fluidTable_ptr;

  };


#endif
