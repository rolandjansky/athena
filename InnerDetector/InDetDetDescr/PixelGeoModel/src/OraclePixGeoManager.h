/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

class IRDBRecord;
class IRDBAccessSvc;
class InDetMaterialManager;
class StoreGateSvc;
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

  OraclePixGeoManager(const PixelGeoModelAthenaComps * athenaComps);
  ~OraclePixGeoManager();
  
  void init();

  //
  // GET THE OTHER MANAGERS FROM STOREGATE
  // -------------------------------------

  // Get the material manager:
  InDetMaterialManager* getMaterialManager();

  // PixelDetectorManager
  InDetDD::PixelDetectorManager *GetPixelDDManager();

  // DistortedMaterialManager
  const InDetDD::DistortedMaterialManager * distortedMatManager() const;
  
  // Access to legacy tables
  PixelLegacyManager * legacyManager() const;


  // 
  // VERSION INFORMATION
  // -------------------
  virtual std::string versionTag() const {return m_versionTag;}
  virtual std::string versionName() const;
  virtual std::string versionDescription() const;
  virtual std::string versionLayout() const;

  //
  // BUILDING DEFINITIONS
  // --------------------

  // Do I want the services?
  void SetServices(bool isservice) {m_services = isservice;}
  bool DoServices();

  // Do I want the services on ladder ? (Omega + Al tube + cables T0 + pigtails + connectors)
  void SetServicesOnLadder(bool isservice) {m_servicesOnLadder = isservice;}
  bool DoServicesOnLadder();

  // Initial layout (2nd layer missing)
  void SetInitialLayout(bool flag) {m_initialLayout = flag;}
  bool InitialLayout() const;

  // DC1 Geometry. 300 um long pixels and 200 um thick sensor in B layer. 
  void SetDC1Geometry(bool flag);
  bool DC1Geometry() const;

  // Control whether callbacks get registered
  void SetAlignable(bool flag) {m_alignable = flag;}
  bool Alignable() const;

  // SLHC
  void SetSLHC(bool flag) {m_slhc = flag;}
  bool slhc() const {return m_slhc;}

  // IBL
  void SetIBL(bool flag) {m_ibl = flag;}
  bool ibl() const {return m_ibl;}

  // DBM
  void SetDBMFlag(bool flag) {m_dbm = flag;}
  bool dbm() const {return m_dbm;}

  void SetIBLPlanarModuleNumber(int nbPlanarModule) { m_PlanarModuleNumber=nbPlanarModule; };
  void SetIBL3DModuleNumber(int nb3DModule) { m_3DModuleNumber=nb3DModule; };

  //
  // BUILDER HELPERS
  // ----------------

  void SetEta(int eta) {m_eta = eta;}
  void SetPhi(int phi) {m_phi = phi;}
  int Eta() {return m_eta;}
  int Phi() {return m_phi;}

  // What am I building?
  bool isBarrel();
  bool isEndcap();
  void SetBarrel();
  void SetEndcap();

  // building DBM
  bool isDBM();
  void SetPartsDBM();

  // The layer/disk barrel/endcap can be changed by these function.
  void SetCurrentLD(int i);
  int GetLD() {return m_currentLD;}
  std::string getLD_Label();

  // Which layers/disks are present?
  bool isLDPresent();

  // The side
  void SetPos() {m_side = 1;}
  void SetNeg() {m_side = -1;}
  int GetSide() {return m_side;}
  bool isAside() {return m_side>0;}
  bool isCside() {return m_side<0;}
  void setDiskFront() { m_diskFrontBack &= 0x3; m_diskFrontBack |= 0x4; }
  void setDiskBack()  { m_diskFrontBack &= 0x3; m_diskFrontBack |= 0x8; }
  bool isDiskFront() const { return m_diskFrontBack & 0x4; }
  bool isDiskBack() const  { return m_diskFrontBack & 0x8; }

  bool isInnermostPixelLayer() {return (isBarrel() && m_currentLD == 0);}

  //
  // DETECTOR PARAMTERS
  // ------------------


  // Version Number, for the Barrel/EndCap
  int PixelBarrelMajorVersion();
  int PixelBarrelMinorVersion();
  int PixelEndcapMajorVersion();
  int PixelEndcapMinorVersion();  

  
  int dbVersion() {return m_dbVersion;}
  bool useLegacy() {return !slhc() && (m_dbVersion < 4);}

  // Si Board
  double PixelBoardWidth(bool isModule3D=false);
  double PixelBoardLength(bool isModule3D=false);
  double PixelBoardThickness(bool isModule3D=false);
  double PixelBoardActiveLength(bool isModule3D=false);

  // Hybrid
  double PixelHybridWidth(bool isModule3D=false);
  double PixelHybridLength(bool isModule3D=false);
  double PixelHybridThickness(bool isModule3D=false);

  //  Fe Chips (PixelModule)
  double PixelChipWidth(bool isModule3D=false);
  double PixelChipLength(bool isModule3D=false);
  double PixelChipGap(bool isModule3D=false);
  double PixelChipOffset(bool isModule3D=false);
  double PixelChipThickness(bool isModule3D=false);

  // Module services
  int PixelModuleServiceNumber();
  double PixelModuleServiceLength(int svc);
  double PixelModuleServiceWidth(int svc);
  double PixelModuleServiceThick(int svc);
  double PixelModuleServiceOffsetX(int svc);
  double PixelModuleServiceOffsetY(int svc);
  double PixelModuleServiceOffsetZ(int svc);
  int PixelModuleServiceModuleType(int svc);
  int PixelModuleServiceFullSize(int svc);
  std::string PixelModuleServiceName(int svc);
  std::string PixelModuleServiceMaterial(int svc);

  // Disk Carbon Structure
  // Being replaced by PixelDiskSupportRMin etc methods
  double PixelECCarbonRMin(std::string);
  double PixelECCarbonRMax(std::string);
  double PixelECCarbonThickness(std::string);
  int PixelECCarbonMaterialTypeNum(std::string);

  // Services
  std::string PixelServiceName(const std::string &, int);
  bool PixelServiceZsymm(const std::string &, int);
  double PixelServiceRMin(const std::string &, int);
  double PixelServiceRMax(const std::string &, int);
  double PixelServiceRMin2(const std::string &, int);
  double PixelServiceRMax2(const std::string &, int);
  double PixelServiceZMin(const std::string &, int);
  double PixelServiceZMax(const std::string &, int);
  double PixelServicePhiLoc(const std::string &a, int _n);
  double PixelServiceWidth(const std::string &a, int _n);
  int PixelServiceRepeat(const std::string &a, int _n);
  std::string PixelServiceShape(const std::string &a, int _n);
  std::string PixelServiceMaterial(const std::string &, int);
  int PixelServiceLD(const std::string &, int);
  int PixelServiceElements(const std::string &);
  int PixelServiceFrameNum(const std::string &, int);
  int PixelServiceEnvelopeNum(const std::string & type, int index);
  int PixelServiceParentEnvelopeNum(const std::string & type, int index);
  int PixelServiceShift(const std::string & type, int index);


  IRDBRecordset_ptr  getPixelServiceRecordset(const std::string & type);
  std::string getPixelServiceRecordString(const std::string & name, const std::string & type, int index);
  int getPixelServiceRecordInt(const std::string & name, const std::string & type, int index);
  double getPixelServiceRecordDouble(const std::string & name, const std::string & type, int index);
  bool getPixelServiceRecordTestField(const std::string & name, const std::string & type, int index);

  //  Pixel container
  double PixelRMin();
  double PixelRMax();
  double PixelHalfLength();

  // Pixel Envelope
  bool PixelSimpleEnvelope();
  unsigned int PixelEnvelopeNumPlanes(); 
  double PixelEnvelopeZ(int i);
  double PixelEnvelopeRMin(int i); 
  double PixelEnvelopeRMax(int i);

  // Pixel Barrel  (from PixelBarrelGeneral)
  int PixelBarrelNLayer();
  double PixelBarrelRMin();
  double PixelBarrelRMax();
  double PixelBarrelHalfLength();


  bool oldFrame();
  bool detailedFrame();
  int PixelFrameSections();
  double PixelFrameRMinSide(int sectionIndex);
  double PixelFrameRMaxSide(int sectionIndex);
  double PixelFrameSideWidth(int sectionIndex);
  double PixelFrameZMin(int sectionIndex);
  double PixelFrameZMax(int sectionIndex);
  double PixelFramePhiStart(int sectionIndex);
  int    PixelFrameNumSides(int sectionIndex);
  bool   PixelFrameMirrorSides(int sectionIndex);
  std::string  PixelFrameSideMaterial(int sectionIndex);
  std::string  PixelFrameCornerMaterial(int sectionIndex);
  
  int PixelFrameNumSideElements(int sectionIndex);
  double PixelFrameElementZMin1(int sectionIndex, int element);
  double PixelFrameElementZMin2(int sectionIndex, int element);
  double PixelFrameElementZMax1(int sectionIndex, int element);
  double PixelFrameElementZMax2(int sectionIndex, int element);

  // helper functions
  int PixelFrameSectionFromIndex(int sectionIndex);
  void makeFrameIndexMap();
  int getFrameElementIndex(int sectionIndex, int element);

  // Pixel Layers/Ladder Geometry
  int PixelStaveIndex(int layer);
  int PixelStaveLayout();
  int PixelStaveAxe();
  double PixelLayerRadius();
  double PixelLayerGlobalShift();
  double PixelLadderLength();
  double PixelLadderWidthClearance();
  double PixelLadderThicknessClearance();
  double PixelLadderThickness();
  double PixelLadderTilt();
  double PixelLadderServicesX();
  double PixelLadderServicesY();
  double PixelLadderCableOffsetX();
  double PixelLadderCableOffsetY();

  // Stave support (IBL)
  GeoTrf::Vector3D IBLStaveRotationAxis(); 
  double IBLStaveRadius();
  double IBLStaveFacePlateThickness(); 
  double IBLStaveMechanicalStaveWidth();
  double IBLStaveMechanicalStaveEndBlockLength();
  double IBLStaveMechanicalStaveEndBlockFixPoint();
  double IBLStaveMechanicalStaveEndBlockOmegaOverlap();
  double IBLStaveLength();
  double IBLStaveMechanicalStaveOffset(bool isModule3D=false);
  double IBLStaveMechanicalStaveModuleOffset();
  double IBLStaveTubeOuterDiameter();
  double IBLStaveTubeInnerDiameter();
  double IBLStaveTubeMiddlePos();
  double IBLStaveFlexLayerThickness();
  double IBLStaveFlexBaseThickness();
  double IBLStaveFlexWidth();
  double IBLStaveFlexOffset();
  double IBLStaveOmegaThickness();
  double IBLStaveOmegaEndCenterX();
  double IBLStaveOmegaEndCenterY();
  double IBLStaveOmegaEndRadius();
  double IBLStaveOmegaEndAngle();
  double IBLStaveOmegaMidCenterX();
  double IBLStaveOmegaMidRadius();
  double IBLStaveOmegaMidAngle();
  int IBLStaveModuleNumber();
  int IBLStaveModuleNumber_AllPlanar();
  double IBLStaveModuleGap();
  int IBLStaveModuleType();
  double IBLStaveFacePlateGreaseThickness();
  double IBLStaveFacePlateGlueThickness();
  double IBLStaveTubeGlueThickness();
  double IBLStaveOmegaGlueThickness();
  double IBLSupportRingWidth();
  double IBLSupportRingInnerRadius();
  double IBLSupportRingOuterRadius();
  double IBLSupportMechanicalStaveRingFixPoint();
  double IBLSupportMidRingWidth();
  double IBLSupportMidRingInnerRadius();
  double IBLSupportMidRingOuterRadius();
  double IBLFlexMiddleGap();
  bool IBLFlexAndWingDefined();
  double IBLFlexDoglegLength();
  double IBLStaveFlexWingWidth();
  double IBLStaveFlexWingThick();
  double IBLFlexDoglegRatio();
  double IBLFlexDoglegHeight(int iHeight);
  double IBLFlexDoglegDY();
  double IBLFlexPP0Z(int iPos);
  double IBLFlexPP0Rmin(int iPos);
  double IBLFlexPP0Rmax(int iPos);
  std::string IBLFlexMaterial(int iPos, const std::string& flexType);
  double IBLServiceGetMinRadialPosition(const std::string& srvName, const std::string& srvType, 
					double srvZmin, double srvZmax);
  double IBLServiceGetMaxRadialPosition(const std::string& srvName, const std::string& srvType, 
					double srvZmin, double srvZmax);

  // Simple ladder services (SLHC)
  double PixelLadderSupportThickness(); 
  double PixelLadderSupportWidth(); 
  double PixelLadderSupportLength(); 

  // Bent modules
  double PixelLadderBentStaveAngle();
  int PixelBentStaveNModule();
  double PixelLadderModuleDeltaZ();

  // Layer support (SLHC)
  // if rmin is not found or <=0 the calculate as Rmin = layer radius + roffset 
  bool PixelLayerSupportCylPresent(); 
  double PixelLayerSupportRMin();
  double PixelLayerSupportROffset();
  double PixelLayerSupportThick();

  int PixelBiStaveType(int layer, int phi);
  int NPixelSectors();
  double PhiOfModuleZero();
  
  int PixelNModule();
  double PixelModuleDrDistance();
  double PixelModuleAngle();
  double PixelModuleZPosition(int);
  double PixelModuleShiftFlag(int);
  double PixelModuleAngleSign(int);

  double PixelModuleStaggerDistance();
  int PixelModuleStaggerSign(int etaModule);
  int PixelModuleEtaFromIndex(int index);
  int PixelModuleIndexFromEta(int etaModule);
  bool allowSkipEtaZero();
  bool centerModule(int etaModule);

  double PixelModuleZPositionTabulated(int etaModule, int type);


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


  // Pixel Endcap 
  int PixelEndcapNDisk();

  // Pixel Endcap Container 
  double PixelEndcapRMin();
  double PixelEndcapRMax();
  double PixelEndcapZMin();
  double PixelEndcapZMax();
  int PixelEndcapNSupportFrames();

  // Pixel Disks 
  double PixelDiskZPosition();
  double PixelECSiDz1();
  double PixelECSiDz2();
  //double PixelDiskRMin();
  int PixelECNSectors1();
  int PixelECNSectors2();

  // Endcap CABLES 
  double PixelECCablesRMin();
  double PixelECCablesRMax();
  double PixelECCablesThickness();
  double PixelECCablesDistance();

  
  // TMT
  int PixelTMTNumParts();
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
  std::string PixelFluidMat(int index); 
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
  //DBM
  //


  // titlting angle of the telscope
  double DBMAngle();

  // Telescope dimension
  double DBMTelescopeX();
  double DBMTelescopeY();
  double DBMTelescopeZ();

  // 3-layers unit
  double DBMModuleCageY();
  double DBMModuleCageZ();

  // layer spacing
  double DBMSpacingZ();
  double DBMSpacingRadial();

  // bracket unit
  double DBMBracketX(); // width of the bracket unit
  double DBMBracketY(); // total height of the bracket unit
  double DBMBracketZ(); // total thickness of the bracket unit,
  // back trapezoid block with window
  double DBMTrapezBackTheta();
  double DBMTrapezBackX();
  double DBMTrapezBackY();
  double DBMTrapezBackShortZ();
  // bracket window
  double DBMBrcktWindowX(); 
  double DBMBrcktWindowY();
  // bracket front volume
  double DBMBrcktTopBlockZ();
  double DBMBrcktSideBlockX();
  double DBMBrcktSideBlockY();
  // back locking box
  double DBMBrcktLockZ();
  double DBMBrcktLockY();
  // window offset, from bottom of back trapezoid to bottom of window; in the front side
  double DBMBrcktWindowOffset();
  //center of trapezoid block in z-direction
  double DBMBrcktWindowCenterZ();
  // cooling fin beside the bracket unit
  double DBMBrcktFinLongZ();
  double DBMBrcktFinHeight();
  double DBMBrcktFinThick();
  double DBMBrcktFinPos();

  // gap between V-slide and first main plate
  double DBMSpace();

  // DBM module
  double DBMDiamondX(); // only diamond dimension
  double DBMDiamondY(); 
  double DBMDiamondZ(); 
  double DBMFEI4X();    // FE-I4 dimension
  double DBMFEI4Y();
  double DBMFEI4Z();
  double DBMCeramicX(); // ceramic 
  double DBMCeramicY();
  double DBMCeramicZ();
  double DBMAirGap();   // air gap between diamond and FE-I4 chip
  double DBMKaptonZ();  //Kapton

  // main plate, on which is mounted the sensor module
  double DBMMainPlateX(); //dimension in x-direction or width
  double DBMMainPlateY(); //y-direction or height
  double DBMMainPlateZ(); //z-direction or thickness
  double DBMMPlateWindowWidth(); // window width in the main plate
  double DBMMPlateWindowHeight(); // window height
  double DBMMPlateWindowPos(); // window position from bottom of the main plate
  // cooling side Plate
  double DBMCoolingSidePlateX();  
  double DBMCoolingSidePlateY();  
  double DBMCoolingSidePlateZ();  
  // position of side plate, parallel to side plate axis,
  // measured from the back of the V-slide to the front of the side plate
  double DBMCoolingSidePlatePos();

  // flex support
  double DBMFlexSupportX();
  double DBMFlexSupportY();
  double DBMFlexSupportZ();
  double DBMFlexSupportOffset();

  // rods
  double DBMRodRadius();
  double DBMMPlateRod2RodY(); // vertical distance bewteen center of rods
  double DBMMPlateRod2RodX(); // horizontal distance bewteen center of rods

  
  // radius and thickness of PP0 board
  double DBMPP0RIn();
  double DBMPP0ROut();
  double DBMPP0Thick();


  //
  // Design Parameter
  //

  // Allow for more than one readout type in the future
  int designType(bool isModule3D=false);
  int designType3D();

  int DesignReadoutSide(bool isModule3D=false);

  double DesignRPActiveArea(bool isModule3D=false);
  double DesignZActiveArea(bool isModule3D=false);

  int DesignCircuitsPhi(bool isModule3D=false);
  int DesignCircuitsEta(bool isModule3D=false);

  int DesignNumChipsPhi(bool isModule3D=false);
  int DesignNumChipsEta(bool isModule3D=false);

  int DesignDiodesPhiTotal(bool isModule3D=false);
  int DesignDiodesEtaTotal(bool isModule3D=false);

  int DesignCellColumnsPerCircuit(bool isModule3D=false);
  int DesignCellRowsPerCircuit(bool isModule3D=false);
  int DesignDiodeColumnsPerCircuit(bool isModule3D=false);
  int DesignDiodeRowsPerCircuit(bool isModule3D=false);

  int DesignNumRowsPerChip(bool isModule3D=false);
  int DesignNumColsPerChip(bool isModule3D=false);

  double DesignPitchRP(bool isModule3D=false);
  double DesignGapRP(bool isModule3D=false);
  double DesignPitchZ(bool isModule3D=false);
  double DesignPitchZLong(bool isModule3D=false);
  double DesignPitchZLongEnd(bool isModule3D=false);
  double DesignGapZ(bool isModule3D=false);

  int DesignNumEmptyRowsInGap(bool isModule3D=false);

  // Ganged Pixels
  int NumberOfEmptyRows(bool isModule3D=false);
  int EmptyRows(int index);
  int EmptyRowConnections(int index);
  int GangedType();
  int GangedTableIndex(int index, int type);

  // CommonItems for Det Elements
  InDetDD::SiCommonItems * commonItems() const;
  void setCommonItems(InDetDD::SiCommonItems * commonItems); 

  // ID helper
  const PixelID * getIdHelper();

  // Top Level placements
  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
  
  std::string getMaterialName(const std::string & volumeName, int layerdisk = 0, int typenum = 0);
  
  // Geometry DB Interface
  const IGeometryDBSvc * db() const {return athenaComps()->geomDB();}

  int    PixelDiskNRings();
  int    PixelDiskRingNModules();
  double PixelDiskRMin(bool includeSupports=false);
  double PixelDiskRMax(bool includeSupports=false);
  double PixelDiskThickness(double safety=0.01);
  double PixelRingRcenter();
  double PixelRingRMin(double safety=0.01);
  double PixelRingRMax(double safety=0.01);
  double PixelRingThickness(double safety=0.01);
  double PixelRingZpos();
  double PixelRingZoffset();
  double PixelRingStagger();
  int    PixelRingSide();
  int    PixelDiskNumSupports();
  double PixelDiskSupportRMin(int isup);
  double PixelDiskSupportRMax(int isup);
  double PixelDiskSupportThickness(int isup);
  int    PixelDiskSupportMaterialTypeNum(int isup);
  double PixelModuleThicknessN();
  double PixelModuleThicknessP();
  double PixelModuleThickness();
  double PixelModuleWidth();
  double PixelModuleLength();
  int moduleType();
  int moduleType3D();
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

