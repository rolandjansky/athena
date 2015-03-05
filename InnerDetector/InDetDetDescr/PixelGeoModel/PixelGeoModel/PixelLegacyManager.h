/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelLegacyManager_H
#define PixelLegacyManager_H

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include <string>

class IRDBRecordset;
class IRDBAccessSvc;

namespace InDetDD {
  class PixelDetectorManager;
  class SiCommonItems;
}

class PixelLegacyManager {
 private:



  //
  // Description of Pixel Frame (M. Zdrazil + AR)
  //
  IRDBRecordset_ptr pfba;
  IRDBRecordset_ptr pbba;
  IRDBRecordset_ptr ptba;
  IRDBRecordset_ptr pfec;
  IRDBRecordset_ptr pbec;
  IRDBRecordset_ptr ptec;
  IRDBRecordset_ptr pecn;
  IRDBRecordset_ptr pecf;
  IRDBRecordset_ptr pecb;
  IRDBRecordset_ptr pect;
  //
  // Design
  //
  IRDBRecordset_ptr pxbi;
  IRDBRecordset_ptr pdch;
  IRDBRecordset_ptr pxbd;
  //
  // Detailed ladder and services on ladder
  //
  IRDBRecordset_ptr ptla;
  IRDBRecordset_ptr pctr;
  IRDBRecordset_ptr pftr;
  IRDBRecordset_ptr pttr;
  IRDBRecordset_ptr pome;
  IRDBRecordset_ptr poti;
  IRDBRecordset_ptr pobi;
  IRDBRecordset_ptr poai;
  IRDBRecordset_ptr poci;
  IRDBRecordset_ptr posi;
  IRDBRecordset_ptr pccf;
  IRDBRecordset_ptr pcff;
  
  bool m_BarrelInSFrame;
  bool m_EndcapInSFrame;
  bool m_EndConeSFrame;

  bool m_dc1Geometry; 

 public:

  PixelLegacyManager(IRDBAccessSvc * rdbSvc, const std::string & detectorKey, const std::string & detectorNode);

  // DC1 Geometry. 300 um long pixels and 200 um thick sensor in B layer. 
  void SetDC1Geometry(bool flag) {m_dc1Geometry = flag;}

  // For the new Support Frame description
  bool isBarrelInSFrame()        {return m_BarrelInSFrame;}
  void setBarrelInSFrame()  {m_BarrelInSFrame = true;  m_EndcapInSFrame = false; m_EndConeSFrame = false;}
  bool isEndcapInSFrame()        {return m_EndcapInSFrame;}
  void setEndcapInSFrame()  {m_BarrelInSFrame = false; m_EndcapInSFrame = true;  m_EndConeSFrame = false;}
  bool isEndConeSFrame()        {return m_EndConeSFrame;}
  void setEndConeSFrame()   {m_BarrelInSFrame = false; m_EndcapInSFrame = false; m_EndConeSFrame = true;}
  void setBarrelOutSFrame() {m_BarrelInSFrame = false; m_EndcapInSFrame = false; m_EndConeSFrame = false;}


  // Determine if should use the old description of the frame. In later geometries the frame is described in the general services.
  bool oldFrame();

  // Barrel
  int PixelBarrelNBFrame();
  int PixelBarrelNTFrame();
  double PixelBarrelBFrameWidth();
  double PixelBarrelTFrameWidth();
  double PixelBarrelFrameOffset();
  double PixelBarrelFrameLength();
  // EndCap
  int PixelEndcapNBFrame();
  int PixelEndcapNTFrame();
  double PixelEndcapBFrameWidth();
  double PixelEndcapTFrameWidth();
  double PixelEndcapFrameOffset();
  double PixelEndcapFrameLength();

  double PixelBFrameHalfLength();
  double PixelBFrameHalfWidth();
  double PixelBFrameHalfThickness();
  double PixelTFrameHalfLength();
  double PixelTFrameHalfWidthY();
  double PixelTFrameHalfWidthXzn();
  double PixelTFrameHalfWidthXzp();
  double PixelTFrameDzDr();
 
  // Endcone
  double PixelBarrelFrameECRadius();
  double PixelBarrelFrameECZPos();
  double PixelBarrelFrameECAlphaX();
  double PixelBarrelFrameECAlphaY();

  // Ladder
  double PixelLadderLength();
  double PixelLadderThickness();
  double PixelLadderServicesX();
  double PixelLadderServicesY();
  double PixelLadderCableOffsetX();
  double PixelLadderCableOffsetY();
  double PixelLadderConnectorOffsetX();
  double PixelLadderPigtailOffsetY();

  // Barrel LAYER CABLES 
  int PixelCableElements();
  int PixelCableLayerNum(int index);
  int PixelCableBiStaveNum(int index);
  double PixelCableZStart(int index);
  double PixelCableZEnd(int index);
  double PixelCableWidth(int index);
  double PixelCableThickness(int index);
  double PixelCableStackOffset(int index);
  double PixelCableWeight(int index);
  std::string PixelCableLabel(int index);

  
  // TMT
  int PixelTMTNumParts();

  double PixelTMTVariable(int iPart, const std::string & varName);
  double PixelTMTDzdr(int iPart);
  double PixelTMTPosX(int iPart);
  double PixelTMTPosZ(int iPart);

  double PixelTMTLength(int iPart);
  double PixelTMTWidthX1(int iPart);
  double PixelTMTWidthX2(int iPart);
  double PixelTMTWidthY(int iPart);
  double PixelTMTBaseX1(int iPart);
  double PixelTMTBaseX2(int iPart);
  double PixelTMTPosY(int iPart);
  double PixelTMTPosZ1(int iPart);
  double PixelTMTPosZ2(int iPart);
  bool PixelTMTPerModule(int iPart);

  // Omega parameters
  double PixelOmegaUpperBendX();
  double PixelOmegaUpperBendY();
  double PixelOmegaUpperBendRadius();
  double PixelOmegaLowerBendX();
  double PixelOmegaLowerBendY();
  double PixelOmegaLowerBendRadius();
  double PixelOmegaWallThickness();
  double PixelOmegaLength();
  double PixelOmegaStartY();
  double PixelOmegaEndY();
  
  // Al Tube
  double PixelAlTubeUpperBendX();
  double PixelAlTubeUpperBendY();
  double PixelAlTubeUpperBendRadius();
  double PixelAlTubeLowerBendX();
  double PixelAlTubeLowerBendY();
  double PixelAlTubeLowerBendRadius();
  double PixelAlTubeWallThickness();
  double PixelAlTubeLength();
  
  // Glue
  int PixelNumOmegaGlueElements();
  double PixelOmegaGlueStartX(int index);
  double PixelOmegaGlueThickness(int index);
  double PixelOmegaGlueStartY(int index);
  double PixelOmegaGlueEndY(int index);
  double PixelOmegaGlueLength(int index);
  double PixelOmegaGluePosZ(int index);
  int PixelOmegaGlueTypeNum(int index);
  
  // Fluid
  double PixelFluidZ1(int index);
  double PixelFluidZ2(int index);
  double PixelFluidThick1(int index);
  double PixelFluidThick2(int index);
  double PixelFluidWidth(int index);
  double PixelFluidX(int index);
  double PixelFluidY(int index);
  int PixelFluidType(int index);
  int PixelFluidNumTypes();
  int PixelFluidIndex(int type);
  int PixelFluidMatType(int index);
  int PixelFluidOrient(int layer, int phi); 
  
  // Pigtail
  double PixelPigtailThickness();
  double PixelPigtailStartY();
  double PixelPigtailEndY();
  double PixelPigtailWidthZ();
  double PixelPigtailFlatWidthZ();
  double PixelPigtailPosX();
  double PixelPigtailPosZ();
  double PixelPigtailBendX();
  double PixelPigtailBendY();
  double PixelPigtailBendRMin();
  double PixelPigtailBendRMax();
  double PixelPigtailBendPhiMin();
  double PixelPigtailBendPhiMax();
  double PixelPigtailEnvelopeLength();

  // Connector
  int PixelNumConnectorElements();
  double PixelConnectorWidthX(int index);
  double PixelConnectorWidthY(int index);
  double PixelConnectorWidthZ(int index);
  double PixelConnectorPosX(int index);
  double PixelConnectorPosY(int index);
  double PixelConnectorPosZ(int index);

  //
  // Design Parameter
  //

  // Allow for more than one readout type in the future
  int designType(bool isBlayer);

  int DesignReadoutSide();

  double DesignRPActiveArea();
  double DesignZActiveArea();

  int DesignCircuitsPhi();
  int DesignCircuitsEta();

  int DesignNumChipsPhi();
  int DesignNumChipsEta();

  int DesignDiodesPhiTotal(bool isBLayer);
  int DesignDiodesEtaTotal(bool isBLayer);

  int DesignCellColumnsPerCircuit(bool isBLayer);
  int DesignCellRowsPerCircuit(bool isBLayer);
  int DesignDiodeColumnsPerCircuit(bool isBLayer);
  int DesignDiodeRowsPerCircuit(bool isBLayer);

  int DesignNumRowsPerChip(bool isBLayer);
  int DesignNumColsPerChip(bool isBLayer);

  double DesignPitchRP(bool isBLayer);
  double DesignGapRP();
  double DesignPitchZ(bool isBLayer);
  double DesignPitchZLong(bool isBLayer);
  double DesignGapZ();

  int DesignNumEmptyRowsInGap();

  // Ganged Pixels
  int NumberOfEmptyRows();
  int EmptyRows(int index);
  int EmptyRowConnections(int index);

  

};


#endif
