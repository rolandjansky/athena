/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/



#include "Identifier/Identifier.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "RDBAccessSvc/IRDBRecord.h" //IRDBRecord used in code in the header
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h" //for IRDBRecordset_ptr typedef
#include "AthenaKernel/MsgStreamMember.h"

#include <string>
#include <vector>

class GeoLogVol;
class GeoVPhysVol;
class StoredMaterialManager;
class StoreGateSvc;
class PixelID;

namespace InDetDD {
  class PixelDetectorManager;
  class SiDetectorDesign;
  class SiCommonItems;
}



namespace PixelGeoDC2 {

class PixelGeometryManager;
//
// This is the base class for all the pieces of the Pixel detector.
//
//
#ifndef GEOVPIXELFACTORY_H
#define GEOVPIXELFACTORY_H



class GeoVPixelFactory {
 public:
  GeoVPixelFactory();
  virtual ~GeoVPixelFactory();
  virtual GeoVPhysVol* Build( )=0;
  static void SetDDMgr(InDetDD::PixelDetectorManager* mgr);
 protected:
  PixelGeometryManager* m_gmt_mgr;
  const StoredMaterialManager* m_mat_mgr;
  static InDetDD::PixelDetectorManager* m_DDmgr;
  const double m_epsilon;

 private:
};


#endif


#ifndef GEOPIXELBARREL_H
#define GEOPIXELBARREL_H



class GeoPixelBarrel : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif
#ifndef GEOPIXELCABLE_H
#define GEOPIXELCABLE_H


class GeoPixelCable : public GeoVPixelFactory {
 public:
  GeoPixelCable():m_moduleNumber(0){}
  virtual GeoVPhysVol* Build();
  double Thickness();
  double Length();
  void SetModuleNumber(int moduleNumber) {m_moduleNumber = moduleNumber;}
  int GetModuleNumber() {return m_moduleNumber;}
 private:
  //
  // ModuleNumber is going from 0 to Numberofmodules/2+1 for symmetry...
  // 0 is the central module, 6 is the one to the left/right
  int m_moduleNumber;
};

#endif
#ifndef GEOPIXELCHIP_H
#define GEOPIXELCHIP_H

//#include "GeoVPixelFactory.h"

class GeoPixelChip : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELDISK_H
#define GEOPIXELDISK_H


class GeoPixelDisk : public GeoVPixelFactory {
 public:
  GeoPixelDisk();
  virtual ~GeoPixelDisk();
  virtual GeoVPhysVol* Build();
  double Thickness();
  double RMax();
  double RMin();
 private:
  const GeoLogVol* m_theDisk;
  int getPhiId();
};

#endif

#ifndef GEOPIXELDISKSUPPORTS_H
#define GEOPIXELDISKSUPPORTS_H


class GeoPixelDiskSupports : public GeoVPixelFactory {
 public:
  GeoPixelDiskSupports();
  virtual GeoVPhysVol* Build();
  int NCylinders(){return m_rmin.size();}
  void SetCylinder(int n) {m_nframe = n;}
  double ZPos() {return m_zpos[m_nframe];}
 private:
  std::vector<double> m_rmin,m_rmax,m_halflength,m_zpos;
  std::vector<std::string> m_material;
  int m_nframe;

};

#endif

#ifndef GEOPIXELECCABLE_H
#define GEOPIXELECCABLE_H


class GeoPixelECCable : public GeoVPixelFactory {
 public:
  GeoPixelECCable();
  virtual GeoVPhysVol* Build();
  virtual ~GeoPixelECCable();
 private:
  const GeoLogVol* m_theECCable;
};

#endif

#ifndef GEOPIXELENDCAP_H
#define GEOPIXELENDCAP_H


class GeoPixelEndCap : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELENVELOPE_H
#define GEOPIXELENVELOPE_H


class GeoPixelEnvelope : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELHYBRID_H
#define GEOPIXELHYBRID_H


class GeoPixelHybrid : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELLADDER_H
#define GEOPIXELLADDER_H

class GeoPixelSiCrystal;

class GeoPixelLadder : public GeoVPixelFactory {
 public:
  GeoPixelLadder(GeoPixelSiCrystal& theSensor);
  virtual ~GeoPixelLadder();
  virtual GeoVPhysVol* Build();
  double Thickness();
 private:
  const GeoLogVol* m_theLadder ;
  GeoPixelSiCrystal& m_theSensor;
};

#endif

#ifndef GEOPIXELLADDERSTRUCT_H
#define GEOPIXELLADDERSTRUCT_H



class GeoPixelLadderStruct : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELLAYER_H
#define GEOPIXELLAYER_h


class GeoPixelLayer : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELMODULE_H
#define GEOPIXELMODULE_H

class GeoPixelSiCrystal;

class GeoPixelModule : public GeoVPixelFactory {
 public:
  GeoPixelModule(GeoPixelSiCrystal &theSensor);
  virtual ~GeoPixelModule();
  virtual GeoVPhysVol* Build();
  double Thickness();
  double Width();
  double Length();
  Identifier getID();
 private:
  const GeoLogVol* m_theModule;
  Identifier m_id;
  GeoPixelSiCrystal& m_theSensor;
};

#endif

#ifndef GEOPIXELSERVICES_H
#define GEOPIXELSERVICES_H

class GeoPixelServices : public GeoVPixelFactory {
 public:
  GeoPixelServices(std::string);
  virtual GeoVPhysVol* Build();
  int NCylinders(){return m_rmin.size();}
  void SetCylinder(int n) {m_nframe = n;}
  double ZPos() {return m_zpos[m_nframe];}
  void initialize(std::string);
 private:
  std::vector<double> m_rmin,m_rmax,m_halflength,m_zpos;
  std::vector<std::string> m_material;
  std::string m_zone;
  int m_nframe;


};

#endif

#ifndef GEOPIXELSICRYSTAL_H
#define GEOPIXELSICRYSTAL_H



class GeoPixelSiCrystal : public GeoVPixelFactory {
 public:
  GeoPixelSiCrystal(bool isBLayer);
  virtual GeoVPhysVol* Build();
  inline Identifier getID();
 private:
  Identifier m_id;
  InDetDD::SiDetectorDesign* m_design;
  bool m_isBLayer;
};
//
// Add this method to store the ID in the factory. This is used by the
// module factory to pass it to the alignment class.
//
Identifier GeoPixelSiCrystal::getID() {return m_id;}
#endif

#ifndef GEOPIXELSUBDISK_H
#define GEOPIXELSUBDISK_H

class GeoPixelSiCrystal;

class GeoPixelSubDisk : public GeoVPixelFactory {
 public:
  GeoPixelSubDisk(GeoPixelSiCrystal &theSensor);
  virtual ~GeoPixelSubDisk();
  virtual GeoVPhysVol* Build();
  double Thickness();
  double RMax();
  double RMin();
 private:
  GeoPixelSiCrystal & m_theSensor;
  const GeoLogVol* m_theSubDisk;
};

#endif

#ifndef GEOPIXELTUBECABLES_H
#define GEOPIXELTUBECABLES_H


class GeoPixelTubeCables : public GeoVPixelFactory {
 public:
  GeoPixelTubeCables();
  virtual ~GeoPixelTubeCables();
  virtual GeoVPhysVol* Build();
  double Thickness();
 private:
  const GeoLogVol* m_theBox;
};

#endif


#ifndef PixelGeometryManager_H
#define PixelGeometryManager_H

class PixelGeometryManager {


 public:

  //enum TYPE {NOVA, Oracle};
  
  // Do this first!!  Specify your data source!!
  static void init();
  // virtual TYPE managerType()=0;
  
  PixelGeometryManager();
  virtual ~PixelGeometryManager();
  static PixelGeometryManager* GetPointer(); 

  //
  // GET THE OTHER MANAGERS FROM STOREGATE
  // -------------------------------------

  // Get the material manager:
  virtual const StoredMaterialManager* getMaterialManager()=0;

  // PixelDetectorManager
  virtual InDetDD::PixelDetectorManager *GetPixelDDManager()=0;

  // 
  // VERSION TAG
  // -----------
  virtual std::string versionTag() const = 0;

  //
  // DESIGN AND ELEMENT NAMES
  // ------------------------
  virtual void SetDetElementName(std::string)=0;
  virtual std::string GetDetElementName()=0;
  virtual void SetDesignName(std::string)=0;
  virtual std::string GetDesignName()=0;

  virtual void SetBarrelModuleName(std::string)=0;
  virtual std::string GetBarrelModuleName()=0;
  virtual void SetEndcapModuleName(std::string)=0;
  virtual std::string GetEndcapModuleName()=0;
  virtual void SetLayer0ModuleName(std::string)=0;
  virtual std::string GetLayer0ModuleName()=0;

  //
  // BUILDING DEFINITIONS
  // --------------------

  // Do I want the services?
  virtual void SetServices(bool isservice)=0;
  virtual bool DoServices()=0;

  // Make elements compatible with G3 digits
  virtual void SetG3CompatibleDigits(bool flag)=0;
  virtual bool G3CompatibleDigits() const=0;

  // Initial layout (2nd layer missing)
  virtual void SetInitialLayout(bool flag)=0;
  virtual bool InitialLayout() const=0;

  // DC1 Geometry. 300 um long pixels and 200 um thick sensor in B layer. 
  virtual void SetDC1Geometry(bool flag)=0;
  virtual bool DC1Geometry() const=0;

  // Control whether callbacks get registered
  virtual void SetAlignable(bool flag)=0;
  virtual bool Alignable() const=0;

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

  // The layer/disk barrel/endcap can be changed by these function.
  virtual void SetCurrentLD(int i)=0;
  virtual int GetLD()=0;

  // Which layers/disks are present?
  virtual bool isLDPresent()=0;

  // The side
  virtual void SetPos()=0;
  virtual void SetNeg()=0;
  virtual int GetSide()=0;

  //
  // DETECTOR PARAMTERS
  // ------------------


  // Version Number, for the Barrel/EndCap
  virtual int PixelBarrelMajorVersion()=0;
  virtual int PixelBarrelMinorVersion()=0;
  virtual int PixelEndcapMajorVersion()=0;
  virtual int PixelEndcapMinorVersion()=0;  

  // Si Board
  virtual double PixelBoardWidth()=0;
  virtual double PixelBoardLength()=0;
  virtual double PixelBoardThickness()=0;
  virtual double PixelBoardActiveLen()=0;

  // Hybrid
  virtual double PixelHybridWidth()=0;
  virtual double PixelHybridLength()=0;
  virtual double PixelHybridThickness()=0;

  //  Fe Chips (PBRN)
  virtual double PixelChipWidth()=0;
  virtual double PixelChipLength()=0;
  virtual double PixelChipGap()=0;
  virtual double PixelChipThickness()=0;

  // Disk Carbon Structure
  virtual double PixelECCarbonRMin(std::string)=0;
  virtual double PixelECCarbonRMax(std::string)=0;
  virtual double PixelECCarbonThickness(std::string)=0;
  virtual std::string PixelECCarbonMaterial(std::string)=0;

  // Services
  virtual double* PixelServiceR(std::string, int )=0;
  virtual double* PixelServiceZ(std::string, int )=0;
  virtual std::string PixelServiceMaterial(std::string, int )=0;
  virtual int PixelServiceLD(std::string, int )=0;
  virtual int PixelServiceNFrame(std::string)=0;

  //  Atlas Global volume (from ATLS)
  virtual double GetATLSRadius()=0;
  virtual double GetATLSRmin()=0;
  virtual double GetATLSLength()=0;

  //  Pixel container (from PXBG)
  virtual double PixelRMin()=0;
  virtual double PixelRMax()=0;
  virtual double PixelHalfLength()=0;
  virtual int PixelBarrelNLayer()=0;

  // Pixel Barrel  (from PXBO)
  virtual double PixelBarrelRMin()=0;
  virtual double PixelBarrelRMax()=0;
  virtual double PixelBarrelHalfLength()=0;
  
  // Pixel Layers Geomtry (PXBI)
  virtual double PixelLayerRadius()=0;
  virtual double PixelLadderHalfLength()=0;
  virtual double PixelLadderWidth()=0;
  virtual double PixelLadderThickness()=0;
  virtual double PixelLadderTilt()=0;
  virtual int NPixelSectors()=0;
  virtual double PixelBalcony()=0;
  virtual int PixelNModule()=0;
  virtual double PixelModuleDrDistance()=0;
  virtual double PixelModuleAngle()=0;
  virtual double PixelModulePosition(int)=0;
  virtual double PixelModuleShiftFlag(int)=0;
  virtual double PixelModuleAngleSign(int)=0;

  // Barrel LAYER CABLES (PBAC)
  virtual double PixelCableWidth()=0;
  virtual double PixelCableThickness()=0;
  virtual double PixelCableZMax()=0;
  virtual double PixelCableZMin()=0;
  virtual double PixelCableDeltaZ()=0;

  // Pixel Endcap PXEG
  virtual int PixelEndcapNDisk()=0;

  // Pixel Endcap Container PEVO
  virtual double PixelEndcapRMin()=0;
  virtual double PixelEndcapRMax()=0;
  virtual double PixelEndcapZMin()=0;
  virtual double PixelEndcapZMax()=0;
  virtual int PixelEndcapNSupportFrames()=0;

  // Pixel Disks PXEI
  virtual double PixelDiskPosition()=0;
  virtual double PixelECSiDz1()=0;
  virtual double PixelECSiDz2()=0;
  virtual double PixelDiskRMin()=0;
  virtual int PixelECNSectors1()=0;
  virtual int PixelECNSectors2()=0;

  // Endcap CABLES (PEAC)
  virtual double PixelECCablesRMin()=0;
  virtual double PixelECCablesRMax()=0;
  virtual double PixelECCablesThickness()=0;
  virtual double PixelECCablesDistance()=0;

  // Design Parameter
  //
  virtual double DesignRPActiveArea()=0;
  virtual double DesignZActiveArea()=0;

  virtual int DesignCircuitsPerColumn()=0;
  virtual int DesignCircuitsPerRow()=0;

  virtual int DesignCellColumnsPerCircuit(bool isBLayer)=0;
  virtual int DesignCellRowsPerCircuit(bool isBLayer)=0;
  virtual int DesignDiodeColumnsPerCircuit(bool isBLayer)=0;
  virtual int DesignDiodeRowsPerCircuit(bool isBLayer)=0;

  virtual double DesignPitchRP(bool isBLayer)=0;
  virtual double DesignGapRP()=0;
  virtual double DesignPitchZ(bool isBLayer)=0;
  virtual double DesignGapZ()=0;

  // Ganged Pixels
  virtual int NumberOfEmptyRows()=0;
  virtual int EmptyRows(int index)=0;
  virtual int EmptyRowConnections(int index)=0;

  // 
  // conditions stuff (probably it shouldnt be here)
  virtual double Voltage(bool isBLayer)=0;
  virtual double Temperature(bool isBLayer)=0;

  // Magnetic field
  virtual const GeoTrf::Vector3D & magneticField(bool isBLayer = false) const=0;
  virtual void setMagneticField(const GeoTrf::Vector3D & field)=0;
  virtual bool useMagneticFieldSvc() const=0;
  virtual void setUseMagneticFieldSvc(bool flag)=0;  

  // CommonItems for Det Elements
  virtual InDetDD::SiCommonItems * commonItems() const=0;
  virtual void setCommonItems(InDetDD::SiCommonItems * commonItems)=0; 

  // ID helper
  virtual const PixelID * getIdHelper()=0;

  //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) { return m_msg.get().level() <= lvl; }

 private:
  
  static PixelGeometryManager *s_geometry_manager;

  //Declaring private message stream member.
  mutable Athena::MsgStreamMember m_msg;


};
#endif


#ifndef OraclePixelGeoManager_H
#define OraclePixelGeoManager_H

class OraclePixGeoManager : public PixelGeometryManager {
 private:
  //
  // NovaObjects: PixbGeo
  //
  IRDBRecordset_ptr m_atls;
  IRDBRecordset_ptr m_PixelBarrelGeneral;
  IRDBRecordset_ptr m_PixelBarrelService;
  IRDBRecordset_ptr m_PixelCommon;
  IRDBRecordset_ptr m_PixelDisk;
  IRDBRecordset_ptr m_PixelEndcapGeneral;
  IRDBRecordset_ptr m_PixelEndcapService;
  IRDBRecordset_ptr m_PixelLayer;
  IRDBRecordset_ptr m_PixelModule;
  IRDBRecordset_ptr m_PixelStave;
  IRDBRecordset_ptr m_pxbo;
  IRDBRecordset_ptr m_pxbi;

  //
  // NovaObjects: PixeGeo
  //
  IRDBRecordset_ptr m_pxei;
  IRDBRecordset_ptr m_pefi;
  IRDBRecordset_ptr m_peoi;
  //
  // NovaObjects: Pixbdig
  //
  IRDBRecordset_ptr m_pdch;
  IRDBRecordset_ptr m_pxbd;
  IRDBRecordset_ptr m_plor;
  IRDBRecordset_ptr m_plrn;
  
  // eta, phi, layer/disk, side (EC)
  int m_eta,m_phi;
  int m_currentLD;
  int m_BarrelEndcap;
  int m_side;

  int m_barrelInFrames, m_endcapInFrames;

  // flag to build also the services
  bool m_services;

  // flag to make elements compatible with G3 digits
  bool m_g3CompatibleDigits;

  // flag to indicate initial layout (2nd layer and disk missing)
  bool m_initialLayout;
  
  // flag to force B-layer to have 300um pixels and be 200um thick
  bool m_dc1Geometry;

  // control whether callbacks get registered 
  bool m_alignable;

  // Magnetic Field
  bool m_magFieldFromNova;
  bool m_useMagFieldSvc;
  mutable GeoTrf::Vector3D m_magField;
  
  // Class holding items that only one instance is needed for all detector elements.
  InDetDD::SiCommonItems * m_commonItems;

  // the det descr manager...
  InDetDD::PixelDetectorManager *m_pDDmgr;

  //the material manager
  const StoredMaterialManager* m_pMatMgr;

  // The Transient Detector Store Service
  StoreGateSvc* m_pDetStore;   

  // The name of the collections in the det store
  std::string m_elementsObjectName;
  std::string m_designsObjectName;

  // The design of the pixel modules. 
  std::string m_barrelDesignName;
  std::string m_endcapDesignName;
  std::string m_blayerDesignName;

  // version tag
  std::string m_versionTag;

 private:

  double CalculateThickness(double,std::string);

 public:

  OraclePixGeoManager();


  //
  // GET THE OTHER MANAGERS FROM STOREGATE
  // -------------------------------------

  // Get the material manager:
  const StoredMaterialManager* getMaterialManager();

  // PixelDetectorManager
  InDetDD::PixelDetectorManager *GetPixelDDManager();

  // 
  // VERSION TAG
  // -----------
  virtual std::string versionTag() const {return m_versionTag;}

  //
  // DESIGN AND ELEMENT NAMES
  // ------------------------
  void SetDetElementName(std::string);
  std::string GetDetElementName();
  void SetDesignName(std::string);
  std::string GetDesignName();

  void SetBarrelModuleName(std::string);
  std::string GetBarrelModuleName();
  void SetEndcapModuleName(std::string);
  std::string GetEndcapModuleName();
  void SetLayer0ModuleName(std::string);
  std::string GetLayer0ModuleName();

  //
  // BUILDING DEFINITIONS
  // --------------------

  // Do I want the services?
  void SetServices(bool isservice) {m_services = isservice;}
  bool DoServices();

  // Make elements compatible with G3 digits
  void SetG3CompatibleDigits(bool flag) {m_g3CompatibleDigits = flag;}
  bool G3CompatibleDigits() const;

  // Initial layout (2nd layer missing)
  void SetInitialLayout(bool flag) {m_initialLayout = flag;}
  bool InitialLayout() const;

  // DC1 Geometry. 300 um long pixels and 200 um thick sensor in B layer. 
  void SetDC1Geometry(bool flag) {m_dc1Geometry = flag;}
  bool DC1Geometry() const;

  // Control whether callbacks get registered
  void SetAlignable(bool flag) {m_alignable = flag;}
  bool Alignable() const;

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

  // The layer/disk barrel/endcap can be changed by these function.
  void SetCurrentLD(int i);
  int GetLD() {return m_currentLD;}

  // Which layers/disks are present?
  bool isLDPresent();

  // The side
  void SetPos() {m_side = 1;}
  void SetNeg() {m_side = -1;}
  int GetSide() {return m_side;}

  //
  // DETECTOR PARAMTERS
  // ------------------


  // Version Number, for the Barrel/EndCap
  inline int PixelBarrelMajorVersion();
  inline int PixelBarrelMinorVersion();
  inline int PixelEndcapMajorVersion();
  inline int PixelEndcapMinorVersion();  

  // Si Board
  double PixelBoardWidth();
  double PixelBoardLength();
  double PixelBoardThickness();
  double PixelBoardActiveLen();

  // Hybrid
  double PixelHybridWidth();
  double PixelHybridLength();
  double PixelHybridThickness();

  //  Fe Chips (PBRN)
  double PixelChipWidth();
  double PixelChipLength();
  double PixelChipGap();
  double PixelChipThickness();

  // Disk Carbon Structure
  double PixelECCarbonRMin(std::string);
  double PixelECCarbonRMax(std::string);
  double PixelECCarbonThickness(std::string);
  std::string PixelECCarbonMaterial(std::string);

  // Services
  double* PixelServiceR(std::string, int );
  double* PixelServiceZ(std::string, int );
  std::string PixelServiceMaterial(std::string, int );
  int PixelServiceLD(std::string, int );
  int PixelServiceNFrame(std::string);

  //  Atlas Global volume (from ATLS)
  inline double GetATLSRadius();
  inline double GetATLSRmin();
  inline double GetATLSLength();

  //  Pixel container (from PXBG)
  inline double PixelRMin();
  inline double PixelRMax();
  inline double PixelHalfLength();
  inline int PixelBarrelNLayer();

  // Pixel Barrel  (from PXBO)
  inline double PixelBarrelRMin();
  inline double PixelBarrelRMax();
  inline double PixelBarrelHalfLength();
  
  // Pixel Layers Geomtry (PXBI)
  inline double PixelLayerRadius();
  inline double PixelLadderHalfLength();
  inline double PixelLadderWidth();
  double PixelLadderThickness();
  inline double PixelLadderTilt();
  inline int NPixelSectors();
  inline double PixelBalcony();
  inline int PixelNModule();
  inline double PixelModuleDrDistance();
  inline double PixelModuleAngle();
  inline double PixelModulePosition(int);
  inline double PixelModuleShiftFlag(int);
  inline double PixelModuleAngleSign(int);

  // Barrel LAYER CABLES (PBAC)
  inline double PixelCableWidth();
  inline double PixelCableThickness();
  inline double PixelCableZMax();
  inline double PixelCableZMin();
  inline double PixelCableDeltaZ();

  // Pixel Endcap PXEG
  inline int PixelEndcapNDisk();

  // Pixel Endcap Container PEVO
  inline double PixelEndcapRMin();
  inline double PixelEndcapRMax();
  inline double PixelEndcapZMin();
  inline double PixelEndcapZMax();
  inline int PixelEndcapNSupportFrames();

  // Pixel Disks PXEI
  inline double PixelDiskPosition();
  inline double PixelECSiDz1();
  inline double PixelECSiDz2();
  inline double PixelDiskRMin();
  inline int PixelECNSectors1();
  inline int PixelECNSectors2();

  // Endcap CABLES (PEAC)
  inline double PixelECCablesRMin();
  inline double PixelECCablesRMax();
  double PixelECCablesThickness();
  inline double PixelECCablesDistance();

  // Design Parameter
  //
  inline double DesignRPActiveArea();
  inline double DesignZActiveArea();

  inline int DesignCircuitsPerColumn();
  inline int DesignCircuitsPerRow();

  int DesignCellColumnsPerCircuit(bool isBLayer);
  int DesignCellRowsPerCircuit(bool isBLayer);
  int DesignDiodeColumnsPerCircuit(bool isBLayer);
  int DesignDiodeRowsPerCircuit(bool isBLayer);

  inline double DesignPitchRP(bool isBLayer);
  inline double DesignGapRP();
  inline double DesignPitchZ(bool isBLayer);
  inline double DesignGapZ();

  // Ganged Pixels
  int NumberOfEmptyRows();
  int EmptyRows(int index);
  int EmptyRowConnections(int index);

  // 
  // conditions stuff (probably it shouldnt be here)
  double Voltage(bool isBLayer);
  double Temperature(bool isBLayer);

  // Magnetic field
  const GeoTrf::Vector3D& magneticField(bool isBLayer = false) const;
  void setMagneticField(const GeoTrf::Vector3D& field);
  bool useMagneticFieldSvc() const;
  void setUseMagneticFieldSvc(bool flag);  

  // CommonItems for Det Elements
  InDetDD::SiCommonItems * commonItems() const;
  void setCommonItems(InDetDD::SiCommonItems * commonItems); 

  // ID helper
  const PixelID * getIdHelper();

};

//
// INLINE METHODS:
//

// ATLS
double OraclePixGeoManager::GetATLSRadius() 
{
  return (*m_atls)[0]->getDouble("RMAX")*Gaudi::Units::cm;
 }

double OraclePixGeoManager::GetATLSRmin() 
{
 return (*m_atls)[0]->getDouble("RMIN")*Gaudi::Units::cm;
}
double OraclePixGeoManager::GetATLSLength() 
{
  return (*m_atls)[0]->getDouble("ZMAX")*Gaudi::Units::cm;
}

//
// Version of the Geometry
//

// PXPA
int OraclePixGeoManager::PixelBarrelMajorVersion() { return static_cast<int>((*m_PixelBarrelGeneral)[0]->getDouble("VERSION"));}
int OraclePixGeoManager::PixelBarrelMinorVersion()
  { return static_cast<int>(((*m_PixelBarrelGeneral)[0]->getDouble("VERSION") - PixelBarrelMajorVersion())*10 + 0.5);}
int OraclePixGeoManager::PixelEndcapMajorVersion() { return static_cast<int>((*m_PixelEndcapGeneral)[0]->getDouble("VERSION"));}
int OraclePixGeoManager::PixelEndcapMinorVersion() 
  { return static_cast<int>(((*m_PixelEndcapGeneral)[0]->getDouble("VERSION") - PixelEndcapMajorVersion())*10 + 0.5);}

// PXBG
double OraclePixGeoManager::PixelRMin() {return (*m_PixelCommon)[0]->getDouble("RMIN")*Gaudi::Units::cm;}
double OraclePixGeoManager::PixelRMax() {return (*m_PixelCommon)[0]->getDouble("RMAX")*Gaudi::Units::cm;}
double OraclePixGeoManager::PixelHalfLength() {return (*m_PixelCommon)[0]->getDouble("HALFLENGTH")*Gaudi::Units::cm;}
int OraclePixGeoManager::PixelBarrelNLayer() {return (*m_PixelBarrelGeneral)[0]->getInt("NLAYER");}

// m_PixelBarrelGeneral
double OraclePixGeoManager::PixelBarrelRMin() {return (*m_PixelBarrelGeneral)[0]->getDouble("RMIN")*Gaudi::Units::cm;}
double OraclePixGeoManager::PixelBarrelRMax() {return (*m_PixelBarrelGeneral)[0]->getDouble("RMAX")*Gaudi::Units::cm;}
double OraclePixGeoManager::PixelBarrelHalfLength() {return (*m_PixelBarrelGeneral)[0]->getDouble("HALFLENGTH")*Gaudi::Units::cm;}

// PXBI
double OraclePixGeoManager::PixelLayerRadius() 
{
  return (*m_PixelLayer)[m_currentLD]->getDouble("RLAYER")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelLadderHalfLength() 
{
  return (*m_PixelStave)[0]->getDouble("SUPPORTHLENGTH")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelLadderWidth() 
{
  return (*m_PixelStave)[0]->getDouble("SUPPORTWIDTH")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelLadderTilt() 
{
  return (*m_PixelLayer)[m_currentLD]->getDouble("STAVETILT")*Gaudi::Units::deg;
}
int OraclePixGeoManager::NPixelSectors() 
{
  return (*m_PixelLayer)[m_currentLD]->getInt("NSECTORS");
}

double OraclePixGeoManager::PixelBalcony() 
{
  return (*m_pxbi)[m_currentLD]->getDouble("DXELEB")*Gaudi::Units::cm;
}

int OraclePixGeoManager::PixelNModule() 
{
  return (*m_PixelStave)[0]->getInt("NMODULE");
}

double OraclePixGeoManager::PixelModuleAngle() 
{
  return (*m_PixelStave)[0]->getDouble("MODULETILT")*Gaudi::Units::deg;
}

double OraclePixGeoManager::PixelModuleDrDistance() 
{
  return (*m_PixelStave)[0]->getDouble("CENTRMODULESHIFT")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelModulePosition(int im) 
{
  return (*m_PixelStave)[0]->getDouble("MODULEDZ")*Gaudi::Units::cm*im;
}

// OBSOLETE!!! TO MOVE INTO THE NEW FACTORY 
double OraclePixGeoManager::PixelModuleShiftFlag(int im) 
{
  if(im == 0) return 1.;
  return 0.;
}

// OBSOLETE!!! TO MOVE INTO THE NEW FACTORY 
double OraclePixGeoManager::PixelModuleAngleSign(int im) 
{
  if(im < 6) return 1.;
  if(im > 6) return -1.;
  return 0.;
}


// PBAC
double OraclePixGeoManager::PixelCableWidth() {
  return (*m_PixelStave)[0]->getDouble("CABLEWIDTH")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelCableThickness() {
  return (*m_PixelStave)[0]->getDouble("CABLETHICK")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelCableZMax() {
  return (*m_PixelStave)[0]->getDouble("SUPPORTHLENGTH")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelCableZMin() {
  return (*m_PixelStave)[0]->getDouble("CABLEZMIN")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelCableDeltaZ() {
  return (*m_PixelStave)[0]->getDouble("MODULEDZ")*Gaudi::Units::cm;
}


// EndCap PXEG  Mistake in table name.
int OraclePixGeoManager::PixelEndcapNDisk() {return (*m_PixelEndcapGeneral)[0]->getInt("NDISK");}

// Endcap container PEVO
double  OraclePixGeoManager::PixelEndcapRMin() {return (*m_PixelEndcapGeneral)[0]->getDouble("RMIN")*Gaudi::Units::cm;}

double  OraclePixGeoManager::PixelEndcapRMax() {return (*m_PixelEndcapGeneral)[0]->getDouble("RMAX")*Gaudi::Units::cm;}

double  OraclePixGeoManager::PixelEndcapZMin() {return (*m_PixelEndcapGeneral)[0]->getDouble("ZMIN")*Gaudi::Units::cm;}

double  OraclePixGeoManager::PixelEndcapZMax() {return (*m_PixelEndcapGeneral)[0]->getDouble("ZMAX")*Gaudi::Units::cm;}

int OraclePixGeoManager::PixelEndcapNSupportFrames() {return (int) (*m_PixelEndcapGeneral)[0]->getDouble("NFRAME");}

// Endcap Inner (PXEI)
double  OraclePixGeoManager::PixelDiskPosition() {return (*m_PixelDisk)[m_currentLD]->getDouble("ZDISK")*Gaudi::Units::cm;}

double  OraclePixGeoManager::PixelDiskRMin() {return (*m_PixelDisk)[m_currentLD]->getDouble("RIDISK")*Gaudi::Units::cm;}

double OraclePixGeoManager::PixelECSiDz1() {return (*m_PixelDisk)[m_currentLD]->getDouble("DZCOUNTER")*Gaudi::Units::cm;}

double OraclePixGeoManager::PixelECSiDz2() {return (*m_PixelDisk)[m_currentLD]->getDouble("DZCOUNTER")*Gaudi::Units::cm;}

int OraclePixGeoManager::PixelECNSectors1() {return (*m_PixelDisk)[m_currentLD]->getInt("NMODULE");}

int OraclePixGeoManager::PixelECNSectors2() {return (*m_PixelDisk)[m_currentLD]->getInt("NMODULE");}

// Endcap Cables PEAC
double OraclePixGeoManager::PixelECCablesRMin() {
  return (*m_PixelDisk)[m_currentLD]->getDouble("RMINCABLE")*Gaudi::Units::cm;
}

double OraclePixGeoManager::PixelECCablesRMax() {
  return (*m_PixelDisk)[m_currentLD]->getDouble("RMAXCABLE")*Gaudi::Units::cm;
}


double OraclePixGeoManager::PixelECCablesDistance() {
  return (*m_PixelDisk)[m_currentLD]->getDouble("ZCABLE")*Gaudi::Units::cm;
}
//
// Design
//

double OraclePixGeoManager::DesignRPActiveArea(){
  return (*m_pxbi)[0]->getDouble("DYACTIVE")*Gaudi::Units::cm;
}
double OraclePixGeoManager::DesignZActiveArea(){
  return (*m_pxbi)[0]->getDouble("DZELEB")*Gaudi::Units::cm;
}

double OraclePixGeoManager::DesignPitchRP(bool isBLayer) {
  if(isBLayer) return (*m_pxbd)[0]->getDouble("PITCHRP")*Gaudi::Units::cm;
  else return (*m_pxbd)[1]->getDouble("PITCHRP")*Gaudi::Units::cm;
}

double OraclePixGeoManager::DesignPitchZ(bool isBLayer) {
  double pitchZ;
  if(isBLayer) {
    if (m_dc1Geometry) { // Override NOVA 
      pitchZ = 300 * Gaudi::Units::micrometer; 
    } else {
      pitchZ = (*m_pxbd)[0]->getDouble("PITCHZ") * Gaudi::Units::cm;
    }
  } else {
    pitchZ = (*m_pxbd)[1]->getDouble("PITCHZ") * Gaudi::Units::cm;
  }
  return pitchZ;
}

double OraclePixGeoManager::DesignGapRP() {
  return (*m_pdch)[0]->getDouble("GAPRP")*Gaudi::Units::cm;
}

double OraclePixGeoManager::DesignGapZ() {
  return (*m_pdch)[0]->getDouble("GAPZ")*Gaudi::Units::cm;
}


int OraclePixGeoManager::DesignCircuitsPerColumn() {
  //
  // This should be (*m_pdch)[0]->getDouble("NRPCHIP"), but in the current
  // design we prefer to have one chip in the rphi direction
  // and define the connections for the pixels in the gap
  return 1;
}
int OraclePixGeoManager::DesignCircuitsPerRow() {
  return  (*m_pdch)[0]->getInt("NZCHIP");
}



#endif

}
