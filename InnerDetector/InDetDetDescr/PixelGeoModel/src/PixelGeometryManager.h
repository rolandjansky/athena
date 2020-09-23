/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeometryManager_H
#define PixelGeometryManager_H

#include <string>
#include <iostream>
#include <map>
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "PixelGeoModelAthenaComps.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class InDetMaterialManager;
class IGeoSubDetTool;
class PixelLegacyManager;
class PixelID;

namespace InDetDD {
  class PixelDetectorManager;
  class DistortedMaterialManager;
  class SiCommonItems;
}

class PixelGeometryManager {

public:
  
  PixelGeometryManager(PixelGeoModelAthenaComps * athenaComps);
  virtual ~PixelGeometryManager();

  //
  // GET THE OTHER MANAGERS FROM STOREGATE
  // -------------------------------------

  // Get the material manager:
  virtual InDetMaterialManager* getMaterialManager()=0;

  // PixelDetectorManager
  virtual InDetDD::PixelDetectorManager *GetPixelDDManager()=0;

  // DistortedMaterialManager
  virtual const InDetDD::DistortedMaterialManager * distortedMatManager() const = 0;

  // Legacy tables
  virtual PixelLegacyManager * legacyManager() const;

 // 
  // VERSION INFORMATION
  // -------------------
  virtual std::string versionTag() const = 0;
  virtual std::string versionName() const = 0;
  virtual std::string versionDescription() const= 0;
  virtual std::string versionLayout() const = 0;


  //
  // BUILDING DEFINITIONS
  // --------------------

  // Do I want the services?
  virtual void SetServices(bool isservice)=0;
  virtual bool DoServices()=0;

  // Do I want the services on ladder ? (Omega + Al tube + cables T0 + pigtails + connectors)
  virtual void SetServicesOnLadder(bool isservice)=0;
  virtual bool DoServicesOnLadder()=0;

  // Initial layout (2nd layer missing)
  virtual void SetInitialLayout(bool flag)=0;
  virtual bool InitialLayout() const=0;

  // DC1 Geometry. 300 um long pixels and 200 um thick sensor in B layer. 
  virtual void SetDC1Geometry(bool flag)=0;
  virtual bool DC1Geometry() const=0;

  // Control whether callbacks get registered
  virtual void SetAlignable(bool flag)=0;
  virtual bool Alignable() const=0;

  // SLHC
  virtual void SetSLHC(bool flag) = 0;
  virtual bool slhc() const = 0;

  // IBL
  virtual void SetIBL(bool flag) = 0;
  virtual bool ibl() const = 0;
  virtual void SetIBLPlanarModuleNumber(int nbPlanarModule) = 0;
  virtual void SetIBL3DModuleNumber(int nb3DModule) = 0;

  // DBM
  //virtual void SetDBMFlag()=0;
  virtual void SetDBMFlag(bool flag)=0;
  virtual bool dbm() const = 0;

  //
  // BUILDER HELPERS
  // ----------------

  virtual void SetEta(int eta)=0;
  virtual void SetPhi(int phi)=0;
  virtual int Eta()=0;
  virtual int Phi()=0;

  // What am I building?
  virtual bool isBarrel()=0;
  virtual bool isEndcap()=0;
  virtual void SetBarrel()=0;
  virtual void SetEndcap()=0;

  // building DBM
  virtual bool isDBM()=0;
  virtual void SetPartsDBM()=0;

  // The layer/disk barrel/endcap can be changed by these function.
  virtual void SetCurrentLD(int i)=0;
  virtual int GetLD()=0;
  virtual std::string getLD_Label()=0;

  // Which layers/disks are present?
  virtual bool isLDPresent()=0;

  // The side
  virtual void SetPos()=0;
  virtual void SetNeg()=0;
  virtual int GetSide()=0;

  //
  // DETECTOR PARAMETERS
  // -------------------

  // DB version 
  virtual int dbVersion()=0;


  // Version Number, for the Barrel/EndCap
  virtual int PixelBarrelMajorVersion()=0;
  virtual int PixelBarrelMinorVersion()=0;
  virtual int PixelEndcapMajorVersion()=0;
  virtual int PixelEndcapMinorVersion()=0;  

  // Si Board
  virtual double PixelBoardWidth(bool isModule3D=false)=0;
  virtual double PixelBoardLength(bool isModule3D=false)=0;
  virtual double PixelBoardThickness(bool isModule3D=false)=0;
  virtual double PixelBoardActiveLength(bool isModule3D=false)=0;

  // Hybrid
  virtual double PixelHybridWidth(bool isModule3D=false)=0;
  virtual double PixelHybridLength(bool isModule3D=false)=0;
  virtual double PixelHybridThickness(bool isModule3D=false)=0;

  //  Fe Chips
  virtual double PixelChipWidth(bool isModule3D=false)=0;
  virtual double PixelChipLength(bool isModule3D=false)=0;
  virtual double PixelChipGap(bool isModule3D=false)=0;
  virtual double PixelChipOffset(bool isModule3D=false)=0; 
  virtual double PixelChipThickness(bool isModule3D=false)=0;

  // Module services
  virtual int PixelModuleServiceNumber()=0;
  virtual double PixelModuleServiceLength(int svc)=0;
  virtual double PixelModuleServiceWidth(int svc)=0;
  virtual double PixelModuleServiceThick(int svc)=0;
  virtual double PixelModuleServiceOffsetX(int svc)=0;
  virtual double PixelModuleServiceOffsetY(int svc)=0;
  virtual double PixelModuleServiceOffsetZ(int svc)=0;
  virtual int PixelModuleServiceModuleType(int svc)=0;
  virtual int PixelModuleServiceFullSize(int svc)=0;
  virtual std::string PixelModuleServiceName(int svc)=0;
  virtual std::string PixelModuleServiceMaterial(int svc)=0;

  // Disk Carbon Structure
  virtual double PixelECCarbonRMin(std::string)=0;
  virtual double PixelECCarbonRMax(std::string)=0;
  virtual double PixelECCarbonThickness(std::string)=0;
  virtual int PixelECCarbonMaterialTypeNum(std::string)=0;

  // Services
  // In principle there shouldn't be access to oracle db stuff in the interface,
  // but the tables are needed by GeoPixelServices.
  virtual IRDBRecordset_ptr getPixelServiceRecordset(const std::string & type)=0;
  virtual std::string PixelServiceName(const std::string &, int)=0;
  virtual bool PixelServiceZsymm(const std::string &, int)=0;
  virtual double PixelServiceRMin(const std::string &, int)=0;
  virtual double PixelServiceRMax(const std::string &, int)=0;
  virtual double PixelServiceRMin2(const std::string &, int)=0;
  virtual double PixelServiceRMax2(const std::string &, int)=0;
  virtual double PixelServiceZMin(const std::string &, int)=0;
  virtual double PixelServiceZMax(const std::string &, int)=0;
  virtual double PixelServicePhiLoc(const std::string &a, int _n)=0;
  virtual double PixelServiceWidth(const std::string &a, int _n)=0;
  virtual int PixelServiceRepeat(const std::string &a, int _n)=0;
  virtual std::string PixelServiceShape(const std::string &a, int _n)=0;
  virtual std::string PixelServiceMaterial(const std::string &, int)=0;
  virtual int PixelServiceLD(const std::string &, int)=0;
  virtual int PixelServiceElements(const std::string &)=0;
  virtual int PixelServiceFrameNum(const std::string &, int)=0;
  virtual int PixelServiceEnvelopeNum(const std::string & type, int index)=0;
  virtual int PixelServiceParentEnvelopeNum(const std::string & type, int index)=0;
  virtual int PixelServiceShift(const std::string & type, int index)=0;

  // Pixel container
  virtual double PixelRMin()=0;
  virtual double PixelRMax()=0;
  virtual double PixelHalfLength()=0;

  // Pixel Envelope
  virtual bool PixelSimpleEnvelope()=0;
  virtual unsigned int PixelEnvelopeNumPlanes()=0; 
  virtual double PixelEnvelopeZ(int i)=0;
  virtual double PixelEnvelopeRMin(int i)=0; 
  virtual double PixelEnvelopeRMax(int i)=0;

  // Pixel Barrel
  virtual int PixelBarrelNLayer()=0;
  virtual double PixelBarrelRMin()=0;
  virtual double PixelBarrelRMax()=0;
  virtual double PixelBarrelHalfLength()=0;

  virtual bool oldFrame()=0;
  virtual bool detailedFrame()=0;
  virtual int PixelFrameSections()=0;
  virtual double PixelFrameRMinSide(int section)=0;
  virtual double PixelFrameRMaxSide(int section)=0;
  virtual double PixelFrameSideWidth(int section)=0;
  virtual double PixelFrameZMin(int section)=0;
  virtual double PixelFrameZMax(int section)=0;
  virtual double PixelFramePhiStart(int section)=0;
  virtual int    PixelFrameNumSides(int section)=0;
  virtual bool   PixelFrameMirrorSides(int sectionIndex)=0;
  virtual std::string  PixelFrameSideMaterial(int section)=0;
  virtual std::string  PixelFrameCornerMaterial(int section)=0;
  
  virtual int PixelFrameNumSideElements(int section)=0;
  virtual double PixelFrameElementZMin1(int section, int element)=0;
  virtual double PixelFrameElementZMin2(int section, int element)=0;
  virtual double PixelFrameElementZMax1(int section, int element)=0;
  virtual double PixelFrameElementZMax2(int section, int element)=0;

  // Pixel Layers Geomtry
  virtual int PixelStaveLayout()=0;
  virtual int PixelStaveAxe()=0;
  virtual double PixelLayerRadius()=0;
  virtual double PixelLayerGlobalShift()=0;
  virtual double PixelLadderLength()=0;
  virtual double PixelLadderWidthClearance()=0;
  virtual double PixelLadderThicknessClearance()=0;
  virtual double PixelLadderThickness()=0;
  virtual double PixelLadderTilt()=0;
  virtual double PixelLadderServicesX()=0;
  virtual double PixelLadderServicesY()=0;
  virtual double PixelLadderCableOffsetX()=0;
  virtual double PixelLadderCableOffsetY()=0;

  virtual double PixelLadderSupportThickness()=0; 
  virtual double PixelLadderSupportWidth()=0; 
  virtual double PixelLadderSupportLength()=0; 

  virtual double PixelLadderBentStaveAngle()=0;
  virtual int PixelBentStaveNModule()=0;
  virtual double PixelLadderModuleDeltaZ()=0;

  virtual int PixelStaveIndex(int layer)=0;

  // Stave support (IBL)
  virtual GeoTrf::Vector3D IBLStaveRotationAxis()=0; 
  virtual double IBLStaveRadius()=0;
  virtual double IBLStaveFacePlateThickness()=0; 
  virtual double IBLStaveMechanicalStaveWidth()=0;
  virtual double IBLStaveMechanicalStaveEndBlockLength()=0;
  virtual double IBLStaveMechanicalStaveEndBlockFixPoint()=0;
  virtual double IBLStaveMechanicalStaveEndBlockOmegaOverlap()=0;
  virtual double IBLStaveLength()=0;
  virtual double IBLStaveMechanicalStaveOffset(bool isModule3D=false)=0;
  virtual double IBLStaveMechanicalStaveModuleOffset()=0;
  virtual double IBLStaveTubeOuterDiameter()=0;
  virtual double IBLStaveTubeInnerDiameter()=0;
  virtual double IBLStaveTubeMiddlePos()=0;
  virtual double IBLStaveFlexLayerThickness()=0;
  virtual double IBLStaveFlexBaseThickness()=0;
  virtual double IBLStaveFlexWidth()=0;
  virtual double IBLStaveFlexOffset()=0;
  virtual double IBLStaveOmegaThickness()=0;
  virtual double IBLStaveOmegaEndCenterX()=0;
  virtual double IBLStaveOmegaEndCenterY()=0;
  virtual double IBLStaveOmegaEndRadius()=0;
  virtual double IBLStaveOmegaEndAngle()=0;
  virtual double IBLStaveOmegaMidCenterX()=0;
  virtual double IBLStaveOmegaMidRadius()=0;
  virtual double IBLStaveOmegaMidAngle()=0;
  virtual int IBLStaveModuleNumber()=0;
  virtual int IBLStaveModuleNumber_AllPlanar()=0;
  virtual double IBLStaveModuleGap()=0;
  virtual int IBLStaveModuleType()=0;
  virtual double IBLStaveFacePlateGreaseThickness()=0;
  virtual double IBLStaveFacePlateGlueThickness()=0;
  virtual double IBLStaveTubeGlueThickness()=0;
  virtual double IBLStaveOmegaGlueThickness()=0;
  virtual double IBLSupportRingWidth()=0;
  virtual double IBLSupportRingInnerRadius()=0;
  virtual double IBLSupportRingOuterRadius()=0;
  virtual double IBLSupportMechanicalStaveRingFixPoint()=0;
  virtual double IBLSupportMidRingWidth()=0;
  virtual double IBLSupportMidRingInnerRadius()=0;
  virtual double IBLSupportMidRingOuterRadius()=0;
  virtual double IBLFlexMiddleGap()=0;
  virtual bool IBLFlexAndWingDefined()=0;
  virtual double IBLFlexDoglegLength()=0;
  virtual double IBLStaveFlexWingWidth()=0;
  virtual double IBLStaveFlexWingThick()=0;
  virtual double IBLFlexDoglegRatio()=0;
  virtual double IBLFlexDoglegHeight(int iHeight)=0;
  virtual double IBLFlexDoglegDY()=0;
  virtual double IBLFlexPP0Z(int iPos)=0;
  virtual double IBLFlexPP0Rmin(int iPos)=0;
  virtual double IBLFlexPP0Rmax(int iPos)=0;
  virtual std::string IBLFlexMaterial(int iPos, const std::string& flexType)=0;

  virtual double IBLServiceGetMinRadialPosition(const std::string& srvName, const std::string& srvType, 
					double srvZmin, double srvZmax)=0;
  virtual double IBLServiceGetMaxRadialPosition(const std::string& srvName, const std::string& srvType, 
					double srvZmin, double srvZmax)=0;

  virtual bool PixelLayerSupportCylPresent()=0; 
  virtual double PixelLayerSupportRMin()=0;
  virtual double PixelLayerSupportROffset()=0;
  virtual double PixelLayerSupportThick()=0;

  virtual int PixelBiStaveType(int layer, int phi)=0;
  virtual int NPixelSectors()=0;
  virtual double PhiOfModuleZero()=0;
  virtual int PixelNModule()=0;
  virtual double PixelModuleDrDistance()=0;
  virtual double PixelModuleAngle()=0;
  virtual double PixelModuleZPosition(int)=0;
  virtual double PixelModuleShiftFlag(int)=0;
  virtual double PixelModuleAngleSign(int)=0;

  virtual double PixelModuleStaggerDistance()=0;
  virtual int PixelModuleStaggerSign(int etaModule)=0;
  virtual int PixelModuleEtaFromIndex(int index)=0;
  virtual int PixelModuleIndexFromEta(int etaModule)=0;
  virtual bool allowSkipEtaZero() = 0;
  virtual bool centerModule(int etaModule) = 0;

  // Barrel LAYER CABLES 
  virtual int PixelCableElements()=0;
  virtual int PixelCableLayerNum(int index)=0;
  virtual int PixelCableBiStaveNum(int index)=0;
  virtual double PixelCableZStart(int index)=0;
  virtual double PixelCableZEnd(int index)=0;
  virtual double PixelCableWidth(int index)=0;
  virtual double PixelCableThickness(int index)=0;
  virtual double PixelCableStackOffset(int index)=0;
  virtual double PixelCableWeight(int index)=0;
  virtual std::string PixelCableLabel(int index)=0;

  // Pixel Endcap
  virtual int PixelEndcapNDisk()=0;

  // Pixel Endcap Container
  virtual double PixelEndcapRMin()=0;
  virtual double PixelEndcapRMax()=0;
  virtual double PixelEndcapZMin()=0;
  virtual double PixelEndcapZMax()=0;
  virtual int PixelEndcapNSupportFrames()=0;

  // Pixel Disks
  virtual double PixelDiskZPosition()=0;
  virtual double PixelECSiDz1()=0;
  virtual double PixelECSiDz2()=0;
  //virtual double PixelDiskRMin()=0;
  virtual int PixelECNSectors1()=0;
  virtual int PixelECNSectors2()=0;

  // Endcap CABLES
  virtual double PixelECCablesRMin()=0;
  virtual double PixelECCablesRMax()=0;
  virtual double PixelECCablesThickness()=0;
  virtual double PixelECCablesDistance()=0;

  // TMT
  virtual int PixelTMTNumParts()=0;
  virtual double PixelTMTWidthY(int iPart)=0;
  virtual double PixelTMTWidthX1(int iPart)=0;
  virtual double PixelTMTWidthX2(int iPart)=0;
  virtual double PixelTMTBaseX1(int iPart)=0;
  virtual double PixelTMTBaseX2(int iPart)=0;
  virtual double PixelTMTPosY(int iPart)=0;
  virtual double PixelTMTPosZ1(int iPart)=0;
  virtual double PixelTMTPosZ2(int iPart)=0;
  virtual bool PixelTMTPerModule(int iPart)=0;

  // Omega parameters
  virtual double PixelOmegaUpperBendX()=0;
  virtual double PixelOmegaUpperBendY()=0;
  virtual double PixelOmegaUpperBendRadius()=0;
  virtual double PixelOmegaLowerBendX()=0;
  virtual double PixelOmegaLowerBendY()=0;
  virtual double PixelOmegaLowerBendRadius()=0;
  virtual double PixelOmegaWallThickness()=0;
  virtual double PixelOmegaLength()=0;
  virtual double PixelOmegaStartY()=0;
  virtual double PixelOmegaEndY()=0;
  
  // Al Tube
  virtual double PixelAlTubeUpperBendX()=0;
  virtual double PixelAlTubeUpperBendY()=0;
  virtual double PixelAlTubeUpperBendRadius()=0;
  virtual double PixelAlTubeLowerBendX()=0;
  virtual double PixelAlTubeLowerBendY()=0;
  virtual double PixelAlTubeLowerBendRadius()=0;
  virtual double PixelAlTubeWallThickness()=0;
  virtual double PixelAlTubeLength()=0;
  
  // Glue
  virtual int PixelNumOmegaGlueElements()=0;
  virtual double PixelOmegaGlueStartX(int index)=0;
  virtual double PixelOmegaGlueThickness(int index)=0;
  virtual double PixelOmegaGlueStartY(int index)=0;
  virtual double PixelOmegaGlueEndY(int index)=0;
  virtual double PixelOmegaGlueLength(int index)=0;
  virtual double PixelOmegaGluePosZ(int index)=0;
  virtual int PixelOmegaGlueTypeNum(int index)=0;
  
  // Fluid
  virtual double PixelFluidZ1(int index)=0;
  virtual double PixelFluidZ2(int index)=0;
  virtual double PixelFluidThick1(int index)=0;
  virtual double PixelFluidThick2(int index)=0;
  virtual double PixelFluidWidth(int index)=0;
  virtual double PixelFluidX(int index)=0;
  virtual double PixelFluidY(int index)=0;
  virtual int PixelFluidType(int index)=0;
  virtual int PixelFluidNumTypes()=0;
  virtual int PixelFluidIndex(int type)=0;
  virtual std::string PixelFluidMat(int index)=0; 
  virtual int PixelFluidOrient(int layer, int phi)=0; 
  
  // Pigtail
  virtual double PixelPigtailThickness()=0;
  virtual double PixelPigtailStartY()=0;
  virtual double PixelPigtailEndY()=0;
  virtual double PixelPigtailWidthZ()=0;
  virtual double PixelPigtailFlatWidthZ()=0;
  virtual double PixelPigtailPosX()=0;
  virtual double PixelPigtailPosZ()=0;
  virtual double PixelPigtailBendX()=0;
  virtual double PixelPigtailBendY()=0;
  virtual double PixelPigtailBendRMin()=0;
  virtual double PixelPigtailBendRMax()=0;
  virtual double PixelPigtailBendPhiMin()=0;
  virtual double PixelPigtailBendPhiMax()=0;
  virtual double PixelPigtailEnvelopeLength()=0;


  // Connector
  virtual int PixelNumConnectorElements()=0;
  virtual double PixelConnectorWidthX(int index)=0;
  virtual double PixelConnectorWidthY(int index)=0;
  virtual double PixelConnectorWidthZ(int index)=0;
  virtual double PixelConnectorPosX(int index)=0;
  virtual double PixelConnectorPosY(int index)=0;
  virtual double PixelConnectorPosZ(int index)=0;


  //
  // DBM
  // Function to acess values in the data files
  //

  // titlting angle of the telscope
  virtual double DBMAngle()=0;

  // Telescope dimension
  virtual double DBMTelescopeX()=0;
  virtual double DBMTelescopeY()=0;
  virtual double DBMTelescopeZ()=0;

  // 3-layers unit
  virtual double DBMModuleCageY()=0;
  virtual double DBMModuleCageZ()=0;

  // layer spacing
  virtual double DBMSpacingZ()=0;
  virtual double DBMSpacingRadial()=0;

  // bracket unit
  virtual double DBMBracketX()=0; // width of the bracket unit
  virtual double DBMBracketY()=0; // total height of the bracket unit
  virtual double DBMBracketZ()=0; // total thickness of the bracket unit,
  // back trapezoid block with window
  virtual double DBMTrapezBackTheta()=0;
  virtual double DBMTrapezBackX()=0;
  virtual double DBMTrapezBackY()=0;
  virtual double DBMTrapezBackShortZ()=0;
  // bracket window
   virtual double DBMBrcktWindowX()=0; 
   virtual double DBMBrcktWindowY()=0;
  // bracket front volume
  virtual double DBMBrcktTopBlockZ()=0;
  virtual double DBMBrcktSideBlockX()=0;
  virtual double DBMBrcktSideBlockY()=0;
  // back locking box
  virtual double DBMBrcktLockZ()=0;
  virtual double DBMBrcktLockY()=0;
  // window offset, from bottom of back trapezoid to bottom of window; in the front side
  virtual double DBMBrcktWindowOffset()=0;
  //center of trapezoid block in z-direction
  virtual double DBMBrcktWindowCenterZ()=0;
  // cooling side plates next to the bracket
  virtual double DBMBrcktFinLongZ()=0;
  virtual double DBMBrcktFinHeight()=0;
  virtual double DBMBrcktFinThick()=0;
  virtual double DBMBrcktFinPos()=0;

  // gap between V-slide and first main plate
  virtual double DBMSpace()=0;

  // DBM module
  virtual double DBMDiamondX()=0; // only diamond dimension
  virtual double DBMDiamondY()=0; 
  virtual double DBMDiamondZ()=0; 
  virtual double DBMFEI4X()=0;    // FE-I4 dimension
  virtual double DBMFEI4Y()=0;
  virtual double DBMFEI4Z()=0;
  virtual double DBMCeramicX()=0; // ceramic 
  virtual double DBMCeramicY()=0;
  virtual double DBMCeramicZ()=0;
  virtual double DBMAirGap()=0;   // air gap between diamond and FE-I4 chip
  virtual double DBMKaptonZ()=0;  //Kapton

  // main plate, on which is mounted the sensor module
  virtual double DBMMainPlateX()=0; //dimension in x-direction or width
  virtual double DBMMainPlateY()=0; //y-direction or height
  virtual double DBMMainPlateZ()=0; //z-direction or thickness
  virtual double DBMMPlateWindowWidth()=0; // window width in the main plate
  virtual double DBMMPlateWindowHeight()=0; // window height
  virtual double DBMMPlateWindowPos()=0; // window position from bottom of the main plate
  // cooling side Plate
  virtual double DBMCoolingSidePlateX()=0;  
  virtual double DBMCoolingSidePlateY()=0;  
  virtual double DBMCoolingSidePlateZ()=0;  
  virtual double DBMCoolingSidePlatePos()=0;  

  // flex support
  virtual double DBMFlexSupportX()=0;
  virtual double DBMFlexSupportY()=0;
  virtual double DBMFlexSupportZ()=0;
  virtual double DBMFlexSupportOffset()=0;

  // rods
  virtual double DBMRodRadius()=0;
  virtual double DBMMPlateRod2RodY()=0; // vertical distance bewteen center of rods
  virtual double DBMMPlateRod2RodX()=0; // horizontal distance bewteen center of rods

  // radius and thickness of PP0 board
  virtual double DBMPP0RIn()=0;
  virtual double DBMPP0ROut()=0;
  virtual double DBMPP0Thick()=0;

  //
  // Design Parameter
  //
  virtual int DesignReadoutSide(bool isModule3D=false)=0;

  virtual double DesignRPActiveArea(bool isModule3D=false)=0;
  virtual double DesignZActiveArea(bool isModule3D=false)=0;
  
  virtual int DesignCircuitsPhi(bool isModule3D=false)=0;
  virtual int DesignCircuitsEta(bool isModule3D=false)=0;

  virtual int DesignNumChipsPhi(bool isModule3D=false)=0;
  virtual int DesignNumChipsEta(bool isModule3D=false)=0;

  virtual int DesignDiodesPhiTotal(bool isModule3D=false)=0;
  virtual int DesignDiodesEtaTotal(bool isModule3D=false)=0;

  virtual int DesignCellColumnsPerCircuit(bool isModule3D=false)=0;
  virtual int DesignCellRowsPerCircuit(bool isModule3D=false)=0;
  virtual int DesignDiodeColumnsPerCircuit(bool isModule3D=false)=0;
  virtual int DesignDiodeRowsPerCircuit(bool isModule3D=false)=0;
 
  virtual int DesignNumRowsPerChip(bool isModule3D=false)=0;
  virtual int DesignNumColsPerChip(bool isModule3D=false)=0;

  virtual double DesignPitchRP(bool isModule3D=false)=0;
  virtual double DesignGapRP(bool isModule3D=false)=0;
  virtual double DesignPitchZ(bool isModule3D=false)=0;
  virtual double DesignPitchZLong(bool isModule3D=false)=0;
  virtual double DesignPitchZLongEnd(bool isModule3D=false)=0;
  virtual double DesignGapZ(bool isModule3D=false)=0;

  virtual int DesignNumEmptyRowsInGap(bool isModule3D=false)=0;

  // Ganged Pixels
  virtual int NumberOfEmptyRows(bool isModule3D=false)=0;
  virtual int EmptyRows(int index)=0;
  virtual int EmptyRowConnections(int index)=0;

  // CommonItems for Det Elements
  virtual InDetDD::SiCommonItems * commonItems() const=0;
  virtual void setCommonItems(InDetDD::SiCommonItems * commonItems)=0; 

   // ID helper
  virtual const PixelID * getIdHelper() = 0;

  // Top Level placements
  virtual const GeoTrf::Transform3D & partTransform(const std::string & partName) const = 0; 
  virtual bool partPresent(const std::string & partName) const = 0;
 
  virtual std::string getMaterialName(const std::string & volumeName, int layerdisk = 0, int typenum = 0) = 0;

  //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_athenaComps->msgLvl(lvl); }

  PixelGeoModelAthenaComps * athenaComps() {return m_athenaComps;}
  const PixelGeoModelAthenaComps * athenaComps() const {return m_athenaComps;}

  /////
  virtual int    PixelDiskNRings()=0;
  virtual int    PixelDiskRingNModules()=0;
  virtual double PixelDiskRMin(bool includeSupports=false)=0;
  virtual double PixelDiskRMax(bool includeSupports=false)=0;
  virtual double PixelDiskThickness(double safety=0.01)=0;
  virtual double PixelRingRcenter()=0;
  virtual double PixelRingRMin(double safety=0.01)=0;
  virtual double PixelRingRMax(double safety=0.01)=0;
  virtual double PixelRingThickness(double safety=0.01)=0;
  virtual double PixelRingZpos()=0;
  virtual double PixelRingZoffset()=0;
  virtual double PixelRingStagger()=0;
  virtual int    PixelRingSide()=0;
  virtual int    PixelDiskNumSupports()=0;
  virtual double PixelDiskSupportRMin(int isup)=0;
  virtual double PixelDiskSupportRMax(int isup)=0;
  virtual double PixelDiskSupportThickness(int isup)=0;
  virtual int    PixelDiskSupportMaterialTypeNum(int isup)=0;
  virtual double PixelModuleThicknessN()=0;
  virtual double PixelModuleThicknessP()=0;
  virtual double PixelModuleThickness()=0;
  virtual double PixelModuleWidth()=0;
  virtual double PixelModuleLength()=0;
  virtual int moduleType()=0;
  virtual int moduleType3D()=0;
  virtual bool isAside()=0;
  virtual bool isCside()=0;
  virtual void setDiskFront()=0; 
  virtual void setDiskBack()=0;  
  virtual bool isDiskFront() const=0;
  virtual bool isDiskBack() const =0;
  /////


private:
  
  // Access to athena components
  PixelGeoModelAthenaComps * m_athenaComps;


};
#endif

