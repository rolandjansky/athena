/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSTAVEDBHELPER_H
#define GEOPIXELSTAVEDBHELPER_H

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <sstream>

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

namespace InDetDD {
  class PairIndexMap;
}


  class GeoPixelStaveDBHelper :  public PixelGeoBuilder {
  public:
    GeoPixelStaveDBHelper( PixelGeoBuilderBasics* basics, int iLayer, bool bBarrel=true);
    ~GeoPixelStaveDBHelper();

    void dump() const;
    void ResetLayerOrDisk(int iLayer, bool bBarrel);
    
    bool isSLHC() const { return false; }
    bool isIBLplanar() const { return m_isIBLplanar; }
    bool isIBLhybrid() const { return m_isIBLhybrid; }
    bool isIBLGeometry() const { return m_isIBLGeometry; }

    void InitIBLflags();

    bool isBarrel() const { return m_bBarrel; }
    bool isEndcap() const { return !m_bBarrel; }

    int GetStaveIndex() const { return m_staveIndex; }

    int PixelNModule() const; 
    bool centerModule(int etaModule) const; 
    bool allowSkipEtaZero() const;
    int PixelModuleEtaFromIndex(int index) const;
    int PixelModuleIndexFromEta(int etaModule) const; 
    double PixelModuleZPosition(int etaModule) const;

    double PixelModuleAngle() const; 
    double PixelModuleAngleSign(int etaModule) const;
    double PixelModuleDrDistance() const; 
    double PixelModuleZPositionTabulated(int etaModule, int type); 
    double PixelModuleShiftFlag(int etaModule) const; 
    double PixelModuleStaggerDistance() const;
    int PixelModuleStaggerSign(int etaModule) const;


    double PixelLadderLength() const; 
    double PixelLadderServicesX() const; 
    double PixelLadderServicesY() const; 
    double PixelLadderCableOffsetX() const; 
    double PixelLadderCableOffsetY() const; 
    double PixelLadderWidthClearance() const;
    double PixelLadderThicknessClearance() const; 
    double PixelLadderTilt() const; 

    double PixelStaveX() const; 
    double PixelStaveY() const; 
    int PixelStaveIndex() const;
    int PixelStaveLayout() const;
    
    double PixelLayerRadius() const;
    double PixelLayerThickness() const;
    int PixelLayerNSectors() const ;
    double PixelLayerPhiOfModuleZero() const ;
  
    int NPixelSectors() const ;
    double PhiOfModuleZero() const;
  
    int PixelModuleType() const;
    int PixelModuleType3D() const;
    double PixelBoardWidth(bool isModule3D=false) const; 
    double PixelBoardLength(bool isModule3D=false) const; 
    double PixelBoardThickness(bool isModule3D=false) const; 
    double  PixelBoardActiveLength(bool isModule3D=false) const; 
    double PixelHybridWidth(bool isModule3D=false) const; 
    double PixelHybridLength(bool isModule3D=false) const; 
    double PixelHybridThickness(bool isModule3D=false) const; 
    double PixelChipWidth(bool isModule3D=false) const; 
    double PixelChipLength(bool isModule3D=false) const; 
    double PixelChipGap(bool isModule3D=false) const; 
    double PixelChipThickness(bool isModule3D=false) const; 
    
    int  designType() const;
    double DesignRPActiveArea(bool isModule3D = false) const;
    double DesignZActiveArea(bool isModule3D = false) const;
    double DesignPitchRP(bool isModule3D = false) const;
    double DesignPitchZ(bool isModule3D = false) const;
    double DesignPitchZLong(bool isModule3D = false) const;
    double DesignPitchZLongEnd(bool isModule3D = false) const;
    double DesignGapRP(bool isModule3D = false) const;
    double DesignGapZ(bool isModule3D = false) const;
    int DesignCircuitsPhi(bool isModule3D = false) const;
    int DesignCircuitsEta(bool isModule3D = false) const;

    int DesignReadoutSide(bool isModule3D=false) const;
    int DesignNumChipsPhi(bool isModule3D=false) const;
    int DesignNumChipsEta(bool isModule3D=false) const;
    int DesignNumRowsPerChip(bool isModule3D=false) const;
    int DesignNumColsPerChip(bool isModule3D=false) const; 
    int DesignDiodesPhiTotal(bool isModule3D=false) const;
    int DesignDiodesEtaTotal(bool isModule3D=false) const;
    int DesignCellRowsPerCircuit(bool isModule3D=false) const;
    int DesignCellColumnsPerCircuit(bool isModule3D=false) const;
    int DesignDiodeRowsPerCircuit(bool isModule3D=false) const;
    int DesignDiodeColumnsPerCircuit(bool isModule3D=false) const;
    int  DesignNumEmptyRowsInGap(bool isModule3D=false) const;
    int GangedType() const;
      
    // Stave support (IBL)
    int IBLStaveModuleNumber() const;
    int IBLStaveModuleNumber_planar() const;
    int IBLStaveModuleNumber_3D() const;
    double IBLStaveFacePlateThickness() const; 
    double IBLStaveMechanicalStaveWidth() const;
    double IBLStaveMechanicalStaveEndBlockLength() const;
    double IBLStaveMechanicalStaveEndBlockFixPoint() const;
    double IBLStaveMechanicalStaveEndBlockOmegaOverlap() const;
    double IBLStaveLength() const;
    double IBLStaveMechanicalStaveOffset(bool isModule3D=false) const;
    double IBLStaveMechanicalStaveModuleOffset() const;
    double IBLStaveTubeOuterDiameter() const;
    double IBLStaveTubeInnerDiameter() const;
    double IBLStaveTubeMiddlePos() const;
    double IBLStaveFlexLayerThickness() const;
    double IBLStaveFlexBaseThickness() const;
    double IBLStaveFlexWidth() const;
    double IBLStaveFlexOffset() const;
    double IBLStaveOmegaThickness() const;
    double IBLStaveOmegaEndCenterX() const;
    double IBLStaveOmegaEndCenterY() const;
    double IBLStaveOmegaEndRadius() const;
    double IBLStaveOmegaEndAngle() const;
    double IBLStaveOmegaMidCenterX() const;
    double IBLStaveOmegaMidRadius() const;
    double IBLStaveOmegaMidAngle() const;
    //    int IBLStaveModuleNumber() const;
    int IBLStaveModuleNumber_AllPlanar() const;
    double IBLStaveModuleGap() const;
    int IBLStaveModuleType() const;
    double IBLStaveFacePlateGreaseThickness() const;
    double IBLStaveFacePlateGlueThickness() const;
    double IBLStaveTubeGlueThickness() const;
    double IBLStaveOmegaGlueThickness() const;
    double IBLSupportRingWidth() const;
    double IBLSupportRingInnerRadius() const;
    double IBLSupportRingOuterRadius() const;
    double IBLSupportMechanicalStaveRingFixPoint() const;
    double IBLSupportMidRingWidth() const;
    double IBLSupportMidRingInnerRadius() const;
    double IBLSupportMidRingOuterRadius() const;
    double IBLFlexMiddleGap() const;
    bool IBLFlexAndWingDefined() const;
    double IBLFlexDoglegLength() const;
    double IBLStaveFlexWingWidth() const;
    double IBLStaveFlexWingThick() const;
    double IBLFlexDoglegRatio() const;
    double IBLFlexDoglegHeight(int iHeight) const;
    double IBLFlexDoglegDY() const;
    double IBLFlexPP0Z(int iPos) const;
    double IBLFlexPP0Rmin(int iPos) const;
    double IBLFlexPP0Rmax(int iPos) const;
    std::string IBLFlexMaterial(int iPos, const std::string& flexType) const;

    int PixelTMTNumParts() const;
    double PixelTMTWidthX1(int iPart) const;
    double PixelTMTWidthX2(int iPart) const;
    double PixelTMTWidthY(int iPart) const;
    double PixelTMTBaseX1(int iPart) const;
    double PixelTMTBaseX2(int iPart) const;
    double PixelTMTPosY(int iPart) const;
    double PixelTMTPosZ1(int iPart) const;
    double PixelTMTPosZ2(int iPart) const;
    bool PixelTMTPerModule(int iPart) const;

  private:
    int m_layer;
    int m_sector;
    int m_ladderType;
    int m_staveIndex;
    int m_moduleType;
    int m_moduleType3D;

    int m_nbPlanarModule;
    int m_nb3DModule;

    bool m_isIBLGeometry;
    bool m_isIBLplanar;
    bool m_isIBLhybrid;
    int m_layout;

    bool m_bBarrel;

    // Map between etaModule,type and index in PixelStaveZ table
    mutable InDetDD::PairIndexMap * m_zPositionMap;
    mutable int m_eta;

    IRDBRecordset_ptr layerTable_ptr;
    IRDBRecordset_ptr staveTable_ptr;
    IRDBRecordset_ptr staveZTable_ptr;
    IRDBRecordset_ptr ladderTable_ptr;
    IRDBRecordset_ptr moduleTable_ptr;
    IRDBRecordset_ptr readoutTable_ptr;
    IRDBRecordset_ptr TMTTable_ptr;

    IRDBRecordset_ptr PixelIBLStave;
    IRDBRecordset_ptr PixelIBLSupport;
    IRDBRecordset_ptr PixelIBLFlex;
    IRDBRecordset_ptr PixelIBLFlexMaterial;
    IRDBRecordset_ptr PixelIBLGlueGrease;

  };


#endif
