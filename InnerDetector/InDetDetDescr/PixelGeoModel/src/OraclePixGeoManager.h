/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OraclePixelGeoManager_H
#define OraclePixelGeoManager_H

#include "PixelGeometryManager.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include <string>
#include <map>
#include <vector>
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "CxxUtils/checker_macros.h"

class IRDBRecord;
class IRDBAccessSvc;
class InDetMaterialManager;
class TopLevelPlacements;
class PixelMaterialMap;
class PixelStaveTypes;
class GeoMaterial;
class PixelID;
class IGeometryDBSvc;

namespace InDetDD {
  class PairIndexMap;
  class PixelDetectorManager;
  class SiCommonItems;
}

class OraclePixGeoManager : public PixelGeometryManager {
 private:


  // Database tables
  IRDBRecordset_ptr m_PixelSwitches;
  IRDBRecordset_ptr m_PixelBarrelGeneral;
  IRDBRecordset_ptr m_PixelBarrelService;
  IRDBRecordset_ptr m_PixelSimpleService;
  IRDBRecordset_ptr m_PixelCommon;
  IRDBRecordset_ptr m_PixelEnvelope;
  IRDBRecordset_ptr m_PixelDisk;
  IRDBRecordset_ptr m_PixelDiskRing;
  IRDBRecordset_ptr m_PixelRing;
  IRDBRecordset_ptr m_PixelEndcapGeneral;
  IRDBRecordset_ptr m_PixelEndcapService;
  IRDBRecordset_ptr m_PixelEnvelopeService;
  IRDBRecordset_ptr m_PixelLayer;
  IRDBRecordset_ptr m_PixelModule;
  IRDBRecordset_ptr m_PixelModuleSvc;
  IRDBRecordset_ptr m_PixelStave;
  IRDBRecordset_ptr m_PixelStaveZ;
  IRDBRecordset_ptr m_PixelTopLevel;
  IRDBRecordset_ptr m_PixelReadout;
  IRDBRecordset_ptr m_PixelGangedPixels;
  IRDBRecordset_ptr m_PixelBarrelCable;
  IRDBRecordset_ptr m_PixelTMT;
  IRDBRecordset_ptr m_PixelOmega;
  IRDBRecordset_ptr m_PixelOmegaGlue;
  IRDBRecordset_ptr m_PixelAlTube;
  IRDBRecordset_ptr m_PixelFluid;
  IRDBRecordset_ptr m_PixelConnector;
  IRDBRecordset_ptr m_PixelPigtail;
  IRDBRecordset_ptr m_PixelFrame;
  IRDBRecordset_ptr m_PixelFrameSect;
  IRDBRecordset_ptr m_PixelIBLStave;
  IRDBRecordset_ptr m_PixelIBLSupport;
  IRDBRecordset_ptr m_PixelIBLFlex;
  IRDBRecordset_ptr m_PixelIBLFlexMaterial;
  IRDBRecordset_ptr m_PixelIBLGlueGrease;
  IRDBRecordset_ptr m_PixelConicalStave;
  IRDBRecordset_ptr m_weightTable;
  IRDBRecordset_ptr m_scalingTable;
  IRDBRecordset_ptr m_materialTable;
  IRDBRecordset_ptr m_staveTypeTable;
  
  //DBM 
  IRDBRecordset_ptr m_DBMTelescope;
  //IRDBRecordset_ptr DBMSpacing;
  IRDBRecordset_ptr m_DBMBracket;
  IRDBRecordset_ptr m_DBMCage;
  //IRDBRecordset_ptr DBMRod;
  //IRDBRecordset_ptr DBMMainPlate;
  //IRDBRecordset_ptr DBMVslide;
  IRDBRecordset_ptr m_DBMModule;
  //IRDBRecordset_ptr DBMFlex;
  //IRDBRecordset_ptr DBMServices;
  IRDBRecordset_ptr m_dbmWeightTable;

  // eta, phi, layer/disk, side (EC)
  int m_eta,m_phi;
  int m_currentLD;
  int m_BarrelEndcap;
  int m_side;
  int m_diskFrontBack;

  // number or general service elements
  int m_barrelInFrames, m_endcapInFrames;

  // flag to build also the services
  bool m_servicesOnLadder;
  bool m_services;

  // flag to indicate initial layout (2nd layer and disk missing)
  bool m_initialLayout;
  
  // flag to force B-layer to have 300um pixels and be 200um thick
  bool m_dc1Geometry;

  // control whether callbacks get registered 
  bool m_alignable;

  // SLHC
  bool m_slhc;

  // IBL
  bool m_ibl;
  int m_PlanarModuleNumber, m_3DModuleNumber;

  // DBM
  bool m_dbm;

  // Flag set to true if no parts are listed in placement table - ie indicates actully all are present.
  bool m_allPartsPresent;

  // Class holding items that only one instance is needed for all detector elements.
  InDetDD::SiCommonItems * m_commonItems;

  // the det descr manager...
  InDetDD::PixelDetectorManager *m_pDDmgr;

  //the material manager
  InDetMaterialManager * m_pMatMgr;

  // Distorted material manager. Access to tables for distorting
  // the material. Extra volumes, modified volumes, etc
  const InDetDD::DistortedMaterialManager * m_distortedMatManager;

  // Legacy tables
  PixelLegacyManager * m_legacyManager;

  // version tag
  std::string m_versionTag;

  // top level placements
  TopLevelPlacements * m_placements;

  // material map
  PixelMaterialMap * m_materialMap;

  // Stave types
  PixelStaveTypes * m_pixelStaveTypes; 

  // ganged pixel index map 
  std::map<int,std::vector<int> > * m_gangedIndexMap;

  // frame element index map 
  std::map<int,std::vector<int> > * m_frameElementMap;


  // Map between disk,ring and index in PixelDiskRing table
  InDetDD::PairIndexMap * m_diskRingIndexMap;

  // Map between etaModule,type and index in PixelStaveZ table
  InDetDD::PairIndexMap * m_zPositionMap;

  // db version
  int m_dbVersion;

  // default length unit set according to db version (Gaudi::Units::mm or Gaudi::Units::cm)
  double m_defaultLengthUnit;

 public:

  OraclePixGeoManager(PixelGeoModelAthenaComps * athenaComps);
  ~OraclePixGeoManager();
  
  void init();

  //
  // GET THE OTHER MANAGERS FROM STOREGATE
  // -------------------------------------

  // Get the material manager:
  virtual InDetMaterialManager* getMaterialManager() override;

  // PixelDetectorManager
  virtual InDetDD::PixelDetectorManager *GetPixelDDManager() override;

  // DistortedMaterialManager
  virtual const InDetDD::DistortedMaterialManager * distortedMatManager() const override;
  
  // Access to legacy tables
  virtual PixelLegacyManager * legacyManager() override;


  // 
  // VERSION INFORMATION
  // -------------------
  virtual std::string versionTag() const override {return m_versionTag;}
  virtual std::string versionName() const override;
  virtual std::string versionDescription() const override;
  virtual std::string versionLayout() const override;

  //
  // BUILDING DEFINITIONS
  // --------------------

  // Do I want the services?
  virtual void SetServices(bool isservice) override {m_services = isservice;}
  virtual bool DoServices() override;

  // Do I want the services on ladder ? (Omega + Al tube + cables T0 + pigtails + connectors)
  virtual void SetServicesOnLadder(bool isservice) override {m_servicesOnLadder = isservice;}
  virtual bool DoServicesOnLadder() override;

  // Initial layout (2nd layer missing)
  virtual void SetInitialLayout(bool flag) override {m_initialLayout = flag;}
  virtual bool InitialLayout() const override;

  // DC1 Geometry. 300 um long pixels and 200 um thick sensor in B layer. 
  virtual void SetDC1Geometry(bool flag) override;
  virtual bool DC1Geometry() const override;

  // Control whether callbacks get registered
  virtual void SetAlignable(bool flag) override {m_alignable = flag;}
  virtual bool Alignable() const override;

  // SLHC
  virtual void SetSLHC(bool flag) override {m_slhc = flag;}
  virtual bool slhc() const override {return m_slhc;}

  // IBL
  virtual void SetIBL(bool flag) override {m_ibl = flag;}
  virtual bool ibl() const override {return m_ibl;}

  // DBM
  virtual void SetDBMFlag(bool flag) override {m_dbm = flag;}
  virtual bool dbm() const override {return m_dbm;}

  virtual void SetIBLPlanarModuleNumber(int nbPlanarModule) override { m_PlanarModuleNumber=nbPlanarModule; };
  virtual void SetIBL3DModuleNumber(int nb3DModule) override { m_3DModuleNumber=nb3DModule; };

  //
  // BUILDER HELPERS
  // ----------------

  virtual void SetEta(int eta) override {m_eta = eta;}
  virtual void SetPhi(int phi) override {m_phi = phi;}
  virtual int Eta() override {return m_eta;}
  virtual int Phi() override {return m_phi;}

  // What am I building?
  virtual bool isBarrel() override;
  virtual bool isEndcap() override;
  virtual void SetBarrel() override;
  virtual void SetEndcap() override;

  // building DBM
  virtual bool isDBM() override;
  virtual void SetPartsDBM() override;

  // The layer/disk barrel/endcap can be changed by these function.
  virtual void SetCurrentLD(int i) override;
  virtual int GetLD() override {return m_currentLD;}
  virtual std::string getLD_Label() override;

  // Which layers/disks are present?
  virtual bool isLDPresent() override;

  // The side
  virtual void SetPos() override {m_side = 1;}
  virtual void SetNeg() override {m_side = -1;}
  virtual int GetSide() override {return m_side;}
  virtual bool isAside() override {return m_side>0;}
  virtual bool isCside() override {return m_side<0;}
  virtual void setDiskFront() override { m_diskFrontBack &= 0x3; m_diskFrontBack |= 0x4; }
  virtual void setDiskBack() override { m_diskFrontBack &= 0x3; m_diskFrontBack |= 0x8; }
  virtual bool isDiskFront() const override { return m_diskFrontBack & 0x4; }
  virtual bool isDiskBack() const override  { return m_diskFrontBack & 0x8; }

  bool isInnermostPixelLayer() {return (isBarrel() && m_currentLD == 0);}

  //
  // DETECTOR PARAMTERS
  // ------------------


  // Version Number, for the Barrel/EndCap
  virtual int PixelBarrelMajorVersion() override;
  virtual int PixelBarrelMinorVersion() override;
  virtual int PixelEndcapMajorVersion() override;
  virtual int PixelEndcapMinorVersion() override;

  
  virtual int dbVersion() override {return m_dbVersion;}
  bool useLegacy() {return !slhc() && (m_dbVersion < 4);}

  // Si Board
  virtual double PixelBoardWidth(bool isModule3D=false) override;
  virtual double PixelBoardLength(bool isModule3D=false) override;
  virtual double PixelBoardThickness(bool isModule3D=false) override;
  virtual double PixelBoardActiveLength(bool isModule3D=false) override;

  // Hybrid
  virtual double PixelHybridWidth(bool isModule3D=false) override;
  virtual double PixelHybridLength(bool isModule3D=false) override;
  virtual double PixelHybridThickness(bool isModule3D=false) override;

  //  Fe Chips (PixelModule)
  virtual double PixelChipWidth(bool isModule3D=false) override;
  virtual double PixelChipLength(bool isModule3D=false) override;
  virtual double PixelChipGap(bool isModule3D=false) override;
  virtual double PixelChipOffset(bool isModule3D=false) override;
  virtual double PixelChipThickness(bool isModule3D=false) override;

  // Module services
  virtual int PixelModuleServiceNumber() override;
  virtual double PixelModuleServiceLength(int svc) override;
  virtual double PixelModuleServiceWidth(int svc) override;
  virtual double PixelModuleServiceThick(int svc) override;
  virtual double PixelModuleServiceOffsetX(int svc) override;
  virtual double PixelModuleServiceOffsetY(int svc) override;
  virtual double PixelModuleServiceOffsetZ(int svc) override;
  virtual int PixelModuleServiceModuleType(int svc) override;
  virtual int PixelModuleServiceFullSize(int svc) override;
  virtual std::string PixelModuleServiceName(int svc) override;
  virtual std::string PixelModuleServiceMaterial(int svc) override;

  // Disk Carbon Structure
  // Being replaced by PixelDiskSupportRMin etc methods
  virtual double PixelECCarbonRMin(std::string) override;
  virtual double PixelECCarbonRMax(std::string) override;
  virtual double PixelECCarbonThickness(std::string) override;
  virtual int PixelECCarbonMaterialTypeNum(std::string) override;

  // Services
  virtual std::string PixelServiceName(const std::string &, int) override;
  virtual bool PixelServiceZsymm(const std::string &, int) override;
  virtual double PixelServiceRMin(const std::string &, int) override;
  virtual double PixelServiceRMax(const std::string &, int) override;
  virtual double PixelServiceRMin2(const std::string &, int) override;
  virtual double PixelServiceRMax2(const std::string &, int) override;
  virtual double PixelServiceZMin(const std::string &, int) override;
  virtual double PixelServiceZMax(const std::string &, int) override;
  virtual double PixelServicePhiLoc(const std::string &a, int _n) override;
  virtual double PixelServiceWidth(const std::string &a, int _n) override;
  virtual int PixelServiceRepeat(const std::string &a, int _n) override;
  virtual std::string PixelServiceShape(const std::string &a, int _n) override;
  virtual std::string PixelServiceMaterial(const std::string &, int) override;
  virtual int PixelServiceLD(const std::string &, int) override;
  virtual int PixelServiceElements(const std::string &) override;
  virtual int PixelServiceFrameNum(const std::string &, int) override;
  virtual int PixelServiceEnvelopeNum(const std::string & type, int index) override;
  virtual int PixelServiceParentEnvelopeNum(const std::string & type, int index) override;
  virtual int PixelServiceShift(const std::string & type, int index) override;


  virtual IRDBRecordset_ptr  getPixelServiceRecordset(const std::string & type) override;
  std::string getPixelServiceRecordString(const std::string & name, const std::string & type, int index);
  int getPixelServiceRecordInt(const std::string & name, const std::string & type, int index);
  double getPixelServiceRecordDouble(const std::string & name, const std::string & type, int index);
  bool getPixelServiceRecordTestField(const std::string & name, const std::string & type, int index);

  //  Pixel container
  virtual double PixelRMin() override;
  virtual double PixelRMax() override;
  virtual double PixelHalfLength() override;

  // Pixel Envelope
  virtual bool PixelSimpleEnvelope() override;
  virtual unsigned int PixelEnvelopeNumPlanes() override; 
  virtual double PixelEnvelopeZ(int i) override;
  virtual double PixelEnvelopeRMin(int i) override;
  virtual double PixelEnvelopeRMax(int i) override;

  // Pixel Barrel  (from PixelBarrelGeneral)
  virtual int PixelBarrelNLayer() override;
  virtual double PixelBarrelRMin() override;
  virtual double PixelBarrelRMax() override;
  virtual double PixelBarrelHalfLength() override;


  virtual bool oldFrame() override;
  virtual bool detailedFrame() override;
  virtual int PixelFrameSections() override;
  virtual double PixelFrameRMinSide(int sectionIndex) override;
  virtual double PixelFrameRMaxSide(int sectionIndex) override;
  virtual double PixelFrameSideWidth(int sectionIndex) override;
  virtual double PixelFrameZMin(int sectionIndex) override;
  virtual double PixelFrameZMax(int sectionIndex) override;
  virtual double PixelFramePhiStart(int sectionIndex) override;
  virtual int    PixelFrameNumSides(int sectionIndex) override;
  virtual bool   PixelFrameMirrorSides(int sectionIndex) override;
  virtual std::string  PixelFrameSideMaterial(int sectionIndex) override;
  virtual std::string  PixelFrameCornerMaterial(int sectionIndex) override;
  
  virtual int PixelFrameNumSideElements(int sectionIndex) override;
  virtual double PixelFrameElementZMin1(int sectionIndex, int element) override;
  virtual double PixelFrameElementZMin2(int sectionIndex, int element) override;
  virtual double PixelFrameElementZMax1(int sectionIndex, int element) override;
  virtual double PixelFrameElementZMax2(int sectionIndex, int element) override;

  // helper functions
  int PixelFrameSectionFromIndex(int sectionIndex);
  void makeFrameIndexMap();
  int getFrameElementIndex(int sectionIndex, int element);

  // Pixel Layers/Ladder Geometry
  virtual int PixelStaveIndex(int layer) override;
  virtual int PixelStaveLayout() override;
  virtual int PixelStaveAxe() override;
  virtual double PixelLayerRadius() override;
  virtual double PixelLayerGlobalShift() override;
  virtual double PixelLadderLength() override;
  virtual double PixelLadderWidthClearance() override;
  virtual double PixelLadderThicknessClearance() override;
  virtual double PixelLadderThickness() override;
  virtual double PixelLadderTilt() override;
  virtual double PixelLadderServicesX() override;
  virtual double PixelLadderServicesY() override;
  virtual double PixelLadderCableOffsetX() override;
  virtual double PixelLadderCableOffsetY() override;

  // Stave support (IBL)
  virtual GeoTrf::Vector3D IBLStaveRotationAxis() override;
  virtual double IBLStaveRadius() override;
  virtual double IBLStaveFacePlateThickness() override; 
  virtual double IBLStaveMechanicalStaveWidth() override;
  virtual double IBLStaveMechanicalStaveEndBlockLength() override;
  virtual double IBLStaveMechanicalStaveEndBlockFixPoint() override;
  virtual double IBLStaveMechanicalStaveEndBlockOmegaOverlap() override;
  virtual double IBLStaveLength() override;
  virtual double IBLStaveMechanicalStaveOffset(bool isModule3D=false) override;
  virtual double IBLStaveMechanicalStaveModuleOffset() override;
  virtual double IBLStaveTubeOuterDiameter() override;
  virtual double IBLStaveTubeInnerDiameter() override;
  virtual double IBLStaveTubeMiddlePos() override;
  virtual double IBLStaveFlexLayerThickness() override;
  virtual double IBLStaveFlexBaseThickness() override;
  virtual double IBLStaveFlexWidth() override;
  virtual double IBLStaveFlexOffset() override;
  virtual double IBLStaveOmegaThickness() override;
  virtual double IBLStaveOmegaEndCenterX() override;
  virtual double IBLStaveOmegaEndCenterY() override;
  virtual double IBLStaveOmegaEndRadius() override;
  virtual double IBLStaveOmegaEndAngle() override;
  virtual double IBLStaveOmegaMidCenterX() override;
  virtual double IBLStaveOmegaMidRadius() override;
  virtual double IBLStaveOmegaMidAngle() override;
  virtual int IBLStaveModuleNumber() override;
  virtual int IBLStaveModuleNumber_AllPlanar() override;
  virtual double IBLStaveModuleGap() override;
  virtual int IBLStaveModuleType() override;
  virtual double IBLStaveFacePlateGreaseThickness() override;
  virtual double IBLStaveFacePlateGlueThickness() override;
  virtual double IBLStaveTubeGlueThickness() override;
  virtual double IBLStaveOmegaGlueThickness() override;
  virtual double IBLSupportRingWidth() override;
  virtual double IBLSupportRingInnerRadius() override;
  virtual double IBLSupportRingOuterRadius() override;
  virtual double IBLSupportMechanicalStaveRingFixPoint() override;
  virtual double IBLSupportMidRingWidth() override;
  virtual double IBLSupportMidRingInnerRadius() override;
  virtual double IBLSupportMidRingOuterRadius() override;
  virtual double IBLFlexMiddleGap() override;
  virtual bool IBLFlexAndWingDefined() override;
  virtual double IBLFlexDoglegLength() override;
  virtual double IBLStaveFlexWingWidth() override;
  virtual double IBLStaveFlexWingThick() override;
  virtual double IBLFlexDoglegRatio() override;
  virtual double IBLFlexDoglegHeight(int iHeight) override;
  virtual double IBLFlexDoglegDY() override;
  virtual double IBLFlexPP0Z(int iPos) override;
  virtual double IBLFlexPP0Rmin(int iPos) override;
  virtual double IBLFlexPP0Rmax(int iPos) override;
  virtual std::string IBLFlexMaterial(int iPos, const std::string& flexType) override;
  virtual double IBLServiceGetMinRadialPosition(const std::string& srvName, const std::string& srvType, 
                                                double srvZmin, double srvZmax) override;
  virtual double IBLServiceGetMaxRadialPosition(const std::string& srvName, const std::string& srvType, 
                                                double srvZmin, double srvZmax) override;

  // Simple ladder services (SLHC)
  virtual double PixelLadderSupportThickness() override; 
  virtual double PixelLadderSupportWidth() override; 
  virtual double PixelLadderSupportLength() override; 

  // Bent modules
  virtual double PixelLadderBentStaveAngle() override;
  virtual int PixelBentStaveNModule() override;
  virtual double PixelLadderModuleDeltaZ() override;

  // Layer support (SLHC)
  // if rmin is not found or <=0 the calculate as Rmin = layer radius + roffset 
  virtual bool PixelLayerSupportCylPresent() override; 
  virtual double PixelLayerSupportRMin() override;
  virtual double PixelLayerSupportROffset() override;
  virtual double PixelLayerSupportThick() override;

  virtual int PixelBiStaveType(int layer, int phi) override;
  virtual int NPixelSectors() override;
  virtual double PhiOfModuleZero() override;
  
  virtual int PixelNModule() override;
  virtual double PixelModuleDrDistance() override;
  virtual double PixelModuleAngle() override;
  virtual double PixelModuleZPosition(int) override;
  virtual double PixelModuleShiftFlag(int) override;
  virtual double PixelModuleAngleSign(int) override;

  virtual double PixelModuleStaggerDistance() override;
  virtual int PixelModuleStaggerSign(int etaModule) override;
  virtual int PixelModuleEtaFromIndex(int index) override;
  virtual int PixelModuleIndexFromEta(int etaModule) override;
  virtual bool allowSkipEtaZero() override;
  virtual bool centerModule(int etaModule) override;

  double PixelModuleZPositionTabulated(int etaModule, int type);


  // Barrel LAYER CABLES 
  virtual int PixelCableElements() override;
  virtual int PixelCableLayerNum(int index) override;
  virtual int PixelCableBiStaveNum(int index) override;
  virtual double PixelCableZStart(int index) override;
  virtual double PixelCableZEnd(int index) override;
  virtual double PixelCableWidth(int index) override;
  virtual double PixelCableThickness(int index) override;
  virtual double PixelCableStackOffset(int index) override;
  virtual double PixelCableWeight(int index) override;
  virtual std::string PixelCableLabel(int index) override;


  // Pixel Endcap 
  virtual int PixelEndcapNDisk() override;

  // Pixel Endcap Container 
  virtual double PixelEndcapRMin() override;
  virtual double PixelEndcapRMax() override;
  virtual double PixelEndcapZMin() override;
  virtual double PixelEndcapZMax() override;
  virtual int PixelEndcapNSupportFrames() override;

  // Pixel Disks 
  virtual double PixelDiskZPosition() override;
  virtual double PixelECSiDz1() override;
  virtual double PixelECSiDz2() override;
  //double PixelDiskRMin();
  virtual int PixelECNSectors1() override;
  virtual int PixelECNSectors2() override;

  // Endcap CABLES 
  virtual double PixelECCablesRMin() override;
  virtual double PixelECCablesRMax() override;
  virtual double PixelECCablesThickness() override;
  virtual double PixelECCablesDistance() override;

  
  // TMT
  virtual int PixelTMTNumParts() override;
  virtual double PixelTMTWidthX1(int iPart) override;
  virtual double PixelTMTWidthX2(int iPart) override;
  virtual double PixelTMTWidthY(int iPart) override;
  virtual double PixelTMTBaseX1(int iPart) override;
  virtual double PixelTMTBaseX2(int iPart) override;
  virtual double PixelTMTPosY(int iPart) override;
  virtual double PixelTMTPosZ1(int iPart) override;
  virtual double PixelTMTPosZ2(int iPart) override;
  virtual bool PixelTMTPerModule(int iPart) override;

  // Omega parameters
  virtual double PixelOmegaUpperBendX() override;
  virtual double PixelOmegaUpperBendY() override;
  virtual double PixelOmegaUpperBendRadius() override;
  virtual double PixelOmegaLowerBendX() override;
  virtual double PixelOmegaLowerBendY() override;
  virtual double PixelOmegaLowerBendRadius() override;
  virtual double PixelOmegaWallThickness() override;
  virtual double PixelOmegaLength() override;
  virtual double PixelOmegaStartY() override;
  virtual double PixelOmegaEndY() override;
  
  // Al Tube
  virtual double PixelAlTubeUpperBendX() override;
  virtual double PixelAlTubeUpperBendY() override;
  virtual double PixelAlTubeUpperBendRadius() override;
  virtual double PixelAlTubeLowerBendX() override;
  virtual double PixelAlTubeLowerBendY() override;
  virtual double PixelAlTubeLowerBendRadius() override;
  virtual double PixelAlTubeWallThickness() override;
  virtual double PixelAlTubeLength() override;
  
  // Glue
  virtual int PixelNumOmegaGlueElements() override;
  virtual double PixelOmegaGlueStartX(int index) override;
  virtual double PixelOmegaGlueThickness(int index) override;
  virtual double PixelOmegaGlueStartY(int index) override;
  virtual double PixelOmegaGlueEndY(int index) override;
  virtual double PixelOmegaGlueLength(int index) override;
  virtual double PixelOmegaGluePosZ(int index) override;
  virtual int PixelOmegaGlueTypeNum(int index) override;
  
  // Fluid
  virtual double PixelFluidZ1(int index) override;
  virtual double PixelFluidZ2(int index) override;
  virtual double PixelFluidThick1(int index) override;
  virtual double PixelFluidThick2(int index) override;
  virtual double PixelFluidWidth(int index) override;
  virtual double PixelFluidX(int index) override;
  virtual double PixelFluidY(int index) override;
  virtual int PixelFluidType(int index) override;
  virtual int PixelFluidNumTypes() override;
  virtual int PixelFluidIndex(int type) override;
  virtual std::string PixelFluidMat(int index) override;
  virtual int PixelFluidOrient(int layer, int phi) override;
  
  // Pigtail
  virtual double PixelPigtailThickness() override;
  virtual double PixelPigtailStartY() override;
  virtual double PixelPigtailEndY() override;
  virtual double PixelPigtailWidthZ() override;
  virtual double PixelPigtailFlatWidthZ() override;
  virtual double PixelPigtailPosX() override;
  virtual double PixelPigtailPosZ() override;
  virtual double PixelPigtailBendX() override;
  virtual double PixelPigtailBendY() override;
  virtual double PixelPigtailBendRMin() override;
  virtual double PixelPigtailBendRMax() override;
  virtual double PixelPigtailBendPhiMin() override;
  virtual double PixelPigtailBendPhiMax() override;
  virtual double PixelPigtailEnvelopeLength() override;

  // Connector
  virtual int PixelNumConnectorElements() override;
  virtual double PixelConnectorWidthX(int index) override;
  virtual double PixelConnectorWidthY(int index) override;
  virtual double PixelConnectorWidthZ(int index) override;
  virtual double PixelConnectorPosX(int index) override;
  virtual double PixelConnectorPosY(int index) override;
  virtual double PixelConnectorPosZ(int index) override;

  //
  //DBM
  //


  // tilting angle of the telescope
  double DBMAngle() override;

  // Telescope dimension
  virtual double DBMTelescopeX() override;
  virtual double DBMTelescopeY() override;
  virtual double DBMTelescopeZ() override;

  // 3-layers unit
  virtual double DBMModuleCageY() override;
  virtual double DBMModuleCageZ() override;

  // layer spacing
  virtual double DBMSpacingZ() override;
  virtual double DBMSpacingRadial() override;

  // bracket unit
  virtual double DBMBracketX() override; // width of the bracket unit
  virtual double DBMBracketY() override; // total height of the bracket unit
  virtual double DBMBracketZ() override; // total thickness of the bracket unit,
  // back trapezoid block with window
  virtual double DBMTrapezBackTheta() override;
  virtual double DBMTrapezBackX() override;
  virtual double DBMTrapezBackY() override;
  virtual double DBMTrapezBackShortZ() override;
  // bracket window
  virtual double DBMBrcktWindowX() override; 
  virtual double DBMBrcktWindowY() override;
  // bracket front volume
  virtual double DBMBrcktTopBlockZ() override;
  virtual double DBMBrcktSideBlockX() override;
  virtual double DBMBrcktSideBlockY() override;
  // back locking box
  virtual double DBMBrcktLockZ() override;
  virtual double DBMBrcktLockY() override;
  // window offset, from bottom of back trapezoid to bottom of window; in the front side
  virtual double DBMBrcktWindowOffset() override;
  //center of trapezoid block in z-direction
  virtual double DBMBrcktWindowCenterZ() override;
  // cooling fin beside the bracket unit
  virtual double DBMBrcktFinLongZ() override;
  virtual double DBMBrcktFinHeight() override;
  virtual double DBMBrcktFinThick() override;
  virtual double DBMBrcktFinPos() override;

  // gap between V-slide and first main plate
  virtual double DBMSpace() override;

  // DBM module
  virtual double DBMDiamondX() override; // only diamond dimension
  virtual double DBMDiamondY() override; 
  virtual double DBMDiamondZ() override; 
  virtual double DBMFEI4X() override;    // FE-I4 dimension
  virtual double DBMFEI4Y() override;
  virtual double DBMFEI4Z() override;
  virtual double DBMCeramicX() override; // ceramic 
  virtual double DBMCeramicY() override;
  virtual double DBMCeramicZ() override;
  virtual double DBMAirGap() override;   // air gap between diamond and FE-I4 chip
  virtual double DBMKaptonZ() override;  //Kapton

  // main plate, on which is mounted the sensor module
  virtual double DBMMainPlateX() override; //dimension in x-direction or width
  virtual double DBMMainPlateY() override; //y-direction or height
  virtual double DBMMainPlateZ() override; //z-direction or thickness
  virtual double DBMMPlateWindowWidth() override; // window width in the main plate
  virtual double DBMMPlateWindowHeight() override; // window height
  virtual double DBMMPlateWindowPos() override; // window position from bottom of the main plate
  // cooling side Plate
  virtual double DBMCoolingSidePlateX() override;
  virtual double DBMCoolingSidePlateY() override;
  virtual double DBMCoolingSidePlateZ() override;
  // position of side plate, parallel to side plate axis,
  // measured from the back of the V-slide to the front of the side plate
  virtual double DBMCoolingSidePlatePos() override;

  // flex support
  virtual double DBMFlexSupportX() override;
  virtual double DBMFlexSupportY() override;
  virtual double DBMFlexSupportZ() override;
  virtual double DBMFlexSupportOffset() override;

  // rods
  virtual double DBMRodRadius() override;
  virtual double DBMMPlateRod2RodY() override; // vertical distance bewteen center of rods
  virtual double DBMMPlateRod2RodX() override; // horizontal distance bewteen center of rods

  
  // radius and thickness of PP0 board
  virtual double DBMPP0RIn() override;
  virtual double DBMPP0ROut() override;
  virtual double DBMPP0Thick() override;


  //
  // Design Parameter
  //

  // Allow for more than one readout type in the future
  int designType(bool isModule3D=false);
  int designType3D();

  virtual int DesignReadoutSide(bool isModule3D=false) override;

  virtual double DesignRPActiveArea(bool isModule3D=false) override;
  virtual double DesignZActiveArea(bool isModule3D=false) override;

  virtual int DesignCircuitsPhi(bool isModule3D=false) override;
  virtual int DesignCircuitsEta(bool isModule3D=false) override;

  virtual int DesignNumChipsPhi(bool isModule3D=false) override;
  virtual int DesignNumChipsEta(bool isModule3D=false) override;

  virtual int DesignDiodesPhiTotal(bool isModule3D=false) override;
  virtual int DesignDiodesEtaTotal(bool isModule3D=false) override;

  virtual int DesignCellColumnsPerCircuit(bool isModule3D=false) override;
  virtual int DesignCellRowsPerCircuit(bool isModule3D=false) override;
  virtual int DesignDiodeColumnsPerCircuit(bool isModule3D=false) override;
  virtual int DesignDiodeRowsPerCircuit(bool isModule3D=false) override;

  virtual int DesignNumRowsPerChip(bool isModule3D=false) override;
  virtual int DesignNumColsPerChip(bool isModule3D=false) override;

  virtual double DesignPitchRP(bool isModule3D=false) override;
  virtual double DesignGapRP(bool isModule3D=false) override;
  virtual double DesignPitchZ(bool isModule3D=false) override;
  virtual double DesignPitchZLong(bool isModule3D=false) override;
  virtual double DesignPitchZLongEnd(bool isModule3D=false) override;
  virtual double DesignGapZ(bool isModule3D=false) override;

  virtual int DesignNumEmptyRowsInGap(bool isModule3D=false) override;

  // Ganged Pixels
  virtual int NumberOfEmptyRows(bool isModule3D=false) override;
  virtual int EmptyRows(int index) override;
  virtual int EmptyRowConnections(int index) override;
  int GangedType();
  int GangedTableIndex(int index, int type);

  // CommonItems for Det Elements
  virtual InDetDD::SiCommonItems * commonItems() override;
  virtual const InDetDD::SiCommonItems * commonItems() const override;
  virtual void setCommonItems(InDetDD::SiCommonItems * commonItems) override; 

  // ID helper
  virtual const PixelID * getIdHelper() override;

  // Top Level placements
  virtual const GeoTrf::Transform3D & partTransform(const std::string & partName) const override;
  virtual bool partPresent(const std::string & partName) const override;
  
  virtual std::string getMaterialName(const std::string & volumeName, int layerdisk = 0, int typenum = 0) override;
  
  // Geometry DB Interface
  const IGeometryDBSvc * db() const {return athenaComps()->geomDB();}

  virtual int    PixelDiskNRings() override;
  virtual int    PixelDiskRingNModules() override;
  virtual double PixelDiskRMin(bool includeSupports=false) override;
  virtual double PixelDiskRMax(bool includeSupports=false) override;
  virtual double PixelDiskThickness(double safety=0.01) override;
  virtual double PixelRingRcenter() override;
  virtual double PixelRingRMin(double safety=0.01) override;
  virtual double PixelRingRMax(double safety=0.01) override;
  virtual double PixelRingThickness(double safety=0.01) override;
  virtual double PixelRingZpos() override;
  virtual double PixelRingZoffset() override;
  virtual double PixelRingStagger() override;
  virtual int    PixelRingSide() override;
  virtual int    PixelDiskNumSupports() override;
  virtual double PixelDiskSupportRMin(int isup) override;
  virtual double PixelDiskSupportRMax(int isup) override;
  virtual double PixelDiskSupportThickness(int isup) override;
  virtual int    PixelDiskSupportMaterialTypeNum(int isup) override;
  virtual double PixelModuleThicknessN() override;
  virtual double PixelModuleThicknessP() override;
  virtual double PixelModuleThickness() override;
  virtual double PixelModuleWidth() override;
  virtual double PixelModuleLength() override;
  virtual int moduleType() override;
  virtual int moduleType3D() override;
  int getDiskRingIndex(int disk, int eta);
  int getDiskRingType(int disk, int eta);

  /////
 private:

  double CalculateThickness(double,std::string);
  int determineDbVersion();
  void addDefaultMaterials();
  // return default length unit (Gaudi::Units::mm or Gaudi::Units::cm)
  double mmcm() {return m_defaultLengthUnit;}

};


#endif

