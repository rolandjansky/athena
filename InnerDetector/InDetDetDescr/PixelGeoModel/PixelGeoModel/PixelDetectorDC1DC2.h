/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "Identifier/Identifier.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "AthenaKernel/MsgStreamMember.h"

#include <string>
#include <iostream>
#include <sstream>
#include <map>

class GeoLogVol;
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


// fwd declaration
//namespace InDetDD {class PixelDetectorManager;}

class GeoVPixelFactory {
 public:
  GeoVPixelFactory();
  virtual ~GeoVPixelFactory();
  virtual GeoVPhysVol* Build( )=0;
  static void SetDDMgr(InDetDD::PixelDetectorManager* mgr);
 protected:
  PixelGeometryManager* gmt_mgr;
  StoredMaterialManager* mat_mgr;
  static InDetDD::PixelDetectorManager* DDmgr;
  const double m_epsilon;

 private:
};


#endif


#ifndef GEOPIXELBARREL_H
#define GEOPIXELBARREL_H

//#include "PixelGeoModel/GeoVPixelFactory.h"


class GeoPixelBarrel : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif
#ifndef GEOPIXELCABLE_H
#define GEOPIXELCABLE_H

//#include "PixelGeoModel/GeoVPixelFactory.h"

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

//#include "PixelGeoModel/GeoVPixelFactory.h"

class GeoPixelChip : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELDISK_H
#define GEOPIXELDISK_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;

class GeoPixelDisk : public GeoVPixelFactory {
 public:
  GeoPixelDisk();
  virtual GeoVPhysVol* Build();
  double Thickness();
  double RMax();
  double RMin();
 private:
  const GeoLogVol* theDisk;
  int getPhiId();
};

#endif

#ifndef GEOPIXELDISKSUPPORTS_H
#define GEOPIXELDISKSUPPORTS_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;

class GeoPixelDiskSupports : public GeoVPixelFactory {
 public:
  GeoPixelDiskSupports();
  virtual GeoVPhysVol* Build();
  int NCylinders(){return _rmin.size();}
  void SetCylinder(int _n) {_nframe = _n;}
  double ZPos() {return _zpos[_nframe];}
 private:
  std::vector<double> _rmin,_rmax,_halflength,_zpos;
  std::vector<std::string> material;
  int _nframe;

};

#endif

#ifndef GEOPIXELECCABLE_H
#define GEOPIXELECCABLE_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;

class GeoPixelECCable : public GeoVPixelFactory {
 public:
  GeoPixelECCable();
  virtual GeoVPhysVol* Build();
 private:
  const GeoLogVol* theECCable;
};

#endif

#ifndef GEOPIXELENDCAP_H
#define GEOPIXELENDCAP_H

//#include "PixelGeoModel/GeoVPixelFactory.h"

class GeoPixelEndCap : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELENVELOPE_H
#define GEOPIXELENVELOPE_H

//#include "PixelGeoModel/GeoVPixelFactory.h"

class GeoPixelEnvelope : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELHYBRID_H
#define GEOPIXELHYBRID_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;

class GeoPixelHybrid : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELLADDER_H
#define GEOPIXELLADDER_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;
class GeoPixelSiCrystal;

class GeoPixelLadder : public GeoVPixelFactory {
 public:
  GeoPixelLadder(GeoPixelSiCrystal& theSensor);
  virtual GeoVPhysVol* Build();
  double Thickness();
 private:
  const GeoLogVol* theLadder;
  GeoPixelSiCrystal& m_theSensor;
};

#endif

#ifndef GEOPIXELLADDERSTRUCT_H
#define GEOPIXELLADDERSTRUCT_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;

class GeoPixelLadderStruct : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELLAYER_H
#define GEOPIXELLAYER_h

//#include "PixelGeoModel/GeoVPixelFactory.h"

class GeoPixelLayer : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif

#ifndef GEOPIXELMODULE_H
#define GEOPIXELMODULE_H
//#include "Identifier/Identifier.h"
//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;
class GeoPixelSiCrystal;

class GeoPixelModule : public GeoVPixelFactory {
 public:
  GeoPixelModule(GeoPixelSiCrystal &theSensor);
  virtual GeoVPhysVol* Build();
  double Thickness();
  double Width();
  double Length();
  Identifier getID();
 private:
  const GeoLogVol* theModule;
  Identifier _id;
  GeoPixelSiCrystal& m_theSensor;
};

#endif

#ifndef GEOPIXELSERVICES_H
#define GEOPIXELSERVICES_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;

class GeoPixelServices : public GeoVPixelFactory {
 public:
  GeoPixelServices(std::string);
  virtual GeoVPhysVol* Build();
  int NCylinders(){return _rmin.size();}
  void SetCylinder(int _n) {_nframe = _n;}
  double ZPos() {return _zpos[_nframe];}
  void initialize(std::string);
 private:
  std::vector<double> _rmin,_rmax,_halflength,_zpos;
  std::vector<std::string> material;
  std::string m_zone;
  int _nframe;


};

#endif

#ifndef GEOPIXELSICRYSTAL_H
#define GEOPIXELSICRYSTAL_H

//#include "Identifier/Identifier.h"
//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;

//namespace InDetDD {class SiDetectorDesign;}

class GeoPixelSiCrystal : public GeoVPixelFactory {
 public:
  GeoPixelSiCrystal(bool isBLayer);
  virtual GeoVPhysVol* Build();
  inline Identifier getID();
 private:
  Identifier _id;
  InDetDD::SiDetectorDesign* m_design;
  bool m_isBLayer;
};
//
// Add this method to store the ID in the factory. This is used by the
// module factory to pass it to the alignement class.
//
Identifier GeoPixelSiCrystal::getID() {return _id;}
#endif

#ifndef GEOPIXELSUBDISK_H
#define GEOPIXELSUBDISK_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;
class GeoPixelSiCrystal;

class GeoPixelSubDisk : public GeoVPixelFactory {
 public:
  GeoPixelSubDisk(GeoPixelSiCrystal &theSensor);
  virtual GeoVPhysVol* Build();
  double Thickness();
  double RMax();
  double RMin();
 private:
  GeoPixelSiCrystal & m_theSensor;
  const GeoLogVol* theSubDisk;
};

#endif

#ifndef GEOPIXELTUBECABLES_H
#define GEOPIXELTUBECABLES_H

//#include "PixelGeoModel/GeoVPixelFactory.h"
//class GeoLogVol;

class GeoPixelTubeCables : public GeoVPixelFactory {
 public:
  GeoPixelTubeCables();
  virtual GeoVPhysVol* Build();
  double Thickness();
 private:
  const GeoLogVol* theBox;
};

#endif


#ifndef PixelGeometryManager_H
#define PixelGeometryManager_H

// #include <string>
// #include <iostream>
// #include <map>
// #include "GeoModelKernel/GeoMaterial.h"
// #include "CLHEP/Geometry/Vector3D.h"
// #include "AthenaKernel/MsgStreamMember.h"

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
  virtual StoredMaterialManager* getMaterialManager()=0;

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
  virtual const HepGeom::Vector3D<double> & magneticField(bool isBLayer = false) const=0;
  virtual void setMagneticField(const HepGeom::Vector3D<double> & field)=0;
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

// #include <string>
// #include <iostream>
// #include <sstream>
// #include <map>
// #include "GeoModelKernel/GeoMaterial.h"
// #include "CLHEP/Geometry/Vector3D.h"
// #include "PixelGeoModel/PixelGeometryManager.h"
// #include "RDBAccessSvc/IRDBRecord.h"
// #include "RDBAccessSvc/IRDBRecordset.h"

//class StoredMaterialManager;
//class StoreGateSvc;



class OraclePixGeoManager : public PixelGeometryManager {
 private:
  //
  // NovaObjects: PixbGeo
  //
  IRDBRecordset_ptr atls;
  IRDBRecordset_ptr PixelBarrelGeneral;
  IRDBRecordset_ptr PixelBarrelService;
  IRDBRecordset_ptr PixelCommon;
  IRDBRecordset_ptr PixelDisk;
  IRDBRecordset_ptr PixelEndcapGeneral;
  IRDBRecordset_ptr PixelEndcapService;
  IRDBRecordset_ptr PixelLayer;
  IRDBRecordset_ptr PixelModule;
  IRDBRecordset_ptr PixelStave;
  IRDBRecordset_ptr pxbo;
  IRDBRecordset_ptr pxbi;

  //
  // NovaObjects: PixeGeo
  //
  IRDBRecordset_ptr pxei;
  IRDBRecordset_ptr pefi;
  IRDBRecordset_ptr peoi;
  //
  // NovaObjects: Pixbdig
  //
  IRDBRecordset_ptr pdch;
  IRDBRecordset_ptr pxbd;
  IRDBRecordset_ptr plor;
  IRDBRecordset_ptr plrn;
  
  // eta, phi, layer/disk, side (EC)
  int m_eta,m_phi;
  int currentLD;
  int BarrelEndcap;
  int _side;

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
  mutable HepGeom::Vector3D<double> m_magField;
  
  // Class holding items that only one instance is needed for all detector elements.
  InDetDD::SiCommonItems * m_commonItems;

  // the det descr manager...
  InDetDD::PixelDetectorManager *m_pDDmgr;

  //the material manager
  StoredMaterialManager* m_pMatMgr;

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

  //PixelGeometryManager::TYPE managerType() {return PixelGeometryManager::Oracle;};

  //
  // GET THE OTHER MANAGERS FROM STOREGATE
  // -------------------------------------

  // Get the material manager:
  StoredMaterialManager* getMaterialManager();

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
  int GetLD() {return currentLD;}

  // Which layers/disks are present?
  bool isLDPresent();

  // The side
  void SetPos() {_side = 1;}
  void SetNeg() {_side = -1;}
  int GetSide() {return _side;}

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
  const HepGeom::Vector3D<double> & magneticField(bool isBLayer = false) const;
  void setMagneticField(const HepGeom::Vector3D<double> & field);
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
  return (*atls)[0]->getDouble("RMAX")*CLHEP::cm;
 }

double OraclePixGeoManager::GetATLSRmin() 
{
 return (*atls)[0]->getDouble("RMIN")*CLHEP::cm;
}
double OraclePixGeoManager::GetATLSLength() 
{
  return (*atls)[0]->getDouble("ZMAX")*CLHEP::cm;
}

//
// Version of the Geometry
//

// PXPA
int OraclePixGeoManager::PixelBarrelMajorVersion() { return static_cast<int>((*PixelBarrelGeneral)[0]->getDouble("VERSION"));}
int OraclePixGeoManager::PixelBarrelMinorVersion()
  { return static_cast<int>(((*PixelBarrelGeneral)[0]->getDouble("VERSION") - PixelBarrelMajorVersion())*10 + 0.5);}
int OraclePixGeoManager::PixelEndcapMajorVersion() { return static_cast<int>((*PixelEndcapGeneral)[0]->getDouble("VERSION"));}
int OraclePixGeoManager::PixelEndcapMinorVersion() 
  { return static_cast<int>(((*PixelEndcapGeneral)[0]->getDouble("VERSION") - PixelEndcapMajorVersion())*10 + 0.5);}

// PXBG
double OraclePixGeoManager::PixelRMin() {return (*PixelCommon)[0]->getDouble("RMIN")*CLHEP::cm;}
double OraclePixGeoManager::PixelRMax() {return (*PixelCommon)[0]->getDouble("RMAX")*CLHEP::cm;}
double OraclePixGeoManager::PixelHalfLength() {return (*PixelCommon)[0]->getDouble("HALFLENGTH")*CLHEP::cm;}
int OraclePixGeoManager::PixelBarrelNLayer() {return (*PixelBarrelGeneral)[0]->getInt("NLAYER");}

// PixelBarrelGeneral
double OraclePixGeoManager::PixelBarrelRMin() {return (*PixelBarrelGeneral)[0]->getDouble("RMIN")*CLHEP::cm;}
double OraclePixGeoManager::PixelBarrelRMax() {return (*PixelBarrelGeneral)[0]->getDouble("RMAX")*CLHEP::cm;}
double OraclePixGeoManager::PixelBarrelHalfLength() {return (*PixelBarrelGeneral)[0]->getDouble("HALFLENGTH")*CLHEP::cm;}

// PXBI
double OraclePixGeoManager::PixelLayerRadius() 
{
  return (*PixelLayer)[currentLD]->getDouble("RLAYER")*CLHEP::cm;
}

double OraclePixGeoManager::PixelLadderHalfLength() 
{
  return (*PixelStave)[0]->getDouble("SUPPORTHLENGTH")*CLHEP::cm;
}

double OraclePixGeoManager::PixelLadderWidth() 
{
  return (*PixelStave)[0]->getDouble("SUPPORTWIDTH")*CLHEP::cm;
}

double OraclePixGeoManager::PixelLadderTilt() 
{
  return (*PixelLayer)[currentLD]->getDouble("STAVETILT")*CLHEP::deg;
}
int OraclePixGeoManager::NPixelSectors() 
{
  return (*PixelLayer)[currentLD]->getInt("NSECTORS");
}

double OraclePixGeoManager::PixelBalcony() 
{
  return (*pxbi)[currentLD]->getDouble("DXELEB")*CLHEP::cm;
}

int OraclePixGeoManager::PixelNModule() 
{
  return (*PixelStave)[0]->getInt("NMODULE");
}

double OraclePixGeoManager::PixelModuleAngle() 
{
  return (*PixelStave)[0]->getDouble("MODULETILT")*CLHEP::deg;
}

double OraclePixGeoManager::PixelModuleDrDistance() 
{
  return (*PixelStave)[0]->getDouble("CENTRMODULESHIFT")*CLHEP::cm;
}

double OraclePixGeoManager::PixelModulePosition(int im) 
{
  return (*PixelStave)[0]->getDouble("MODULEDZ")*CLHEP::cm*im;
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
  return (*PixelStave)[0]->getDouble("CABLEWIDTH")*CLHEP::cm;
}

double OraclePixGeoManager::PixelCableThickness() {
  return (*PixelStave)[0]->getDouble("CABLETHICK")*CLHEP::cm;
}

double OraclePixGeoManager::PixelCableZMax() {
  return (*PixelStave)[0]->getDouble("SUPPORTHLENGTH")*CLHEP::cm;
}

double OraclePixGeoManager::PixelCableZMin() {
  return (*PixelStave)[0]->getDouble("CABLEZMIN")*CLHEP::cm;
}

double OraclePixGeoManager::PixelCableDeltaZ() {
  return (*PixelStave)[0]->getDouble("MODULEDZ")*CLHEP::cm;
}


// EndCap PXEG  Mistake in table name.
int OraclePixGeoManager::PixelEndcapNDisk() {return (*PixelEndcapGeneral)[0]->getInt("NDISK");}

// Endcap container PEVO
double  OraclePixGeoManager::PixelEndcapRMin() {return (*PixelEndcapGeneral)[0]->getDouble("RMIN")*CLHEP::cm;}

double  OraclePixGeoManager::PixelEndcapRMax() {return (*PixelEndcapGeneral)[0]->getDouble("RMAX")*CLHEP::cm;}

double  OraclePixGeoManager::PixelEndcapZMin() {return (*PixelEndcapGeneral)[0]->getDouble("ZMIN")*CLHEP::cm;}

double  OraclePixGeoManager::PixelEndcapZMax() {return (*PixelEndcapGeneral)[0]->getDouble("ZMAX")*CLHEP::cm;}

int OraclePixGeoManager::PixelEndcapNSupportFrames() {return (int) (*PixelEndcapGeneral)[0]->getDouble("NFRAME");}

// Endcap Inner (PXEI)
double  OraclePixGeoManager::PixelDiskPosition() {return (*PixelDisk)[currentLD]->getDouble("ZDISK")*CLHEP::cm;}

double  OraclePixGeoManager::PixelDiskRMin() {return (*PixelDisk)[currentLD]->getDouble("RIDISK")*CLHEP::cm;}

double OraclePixGeoManager::PixelECSiDz1() {return (*PixelDisk)[currentLD]->getDouble("DZCOUNTER")*CLHEP::cm;}

double OraclePixGeoManager::PixelECSiDz2() {return (*PixelDisk)[currentLD]->getDouble("DZCOUNTER")*CLHEP::cm;}

int OraclePixGeoManager::PixelECNSectors1() {return (*PixelDisk)[currentLD]->getInt("NMODULE");}

int OraclePixGeoManager::PixelECNSectors2() {return (*PixelDisk)[currentLD]->getInt("NMODULE");}

// Endcap Cables PEAC
double OraclePixGeoManager::PixelECCablesRMin() {
  return (*PixelDisk)[currentLD]->getDouble("RMINCABLE")*CLHEP::cm;
}

double OraclePixGeoManager::PixelECCablesRMax() {
  return (*PixelDisk)[currentLD]->getDouble("RMAXCABLE")*CLHEP::cm;
}


double OraclePixGeoManager::PixelECCablesDistance() {
  return (*PixelDisk)[currentLD]->getDouble("ZCABLE")*CLHEP::cm;
}
//
// Design
//

double OraclePixGeoManager::DesignRPActiveArea(){
  return (*pxbi)[0]->getDouble("DYACTIVE")*CLHEP::cm;
}
double OraclePixGeoManager::DesignZActiveArea(){
  return (*pxbi)[0]->getDouble("DZELEB")*CLHEP::cm;
}

double OraclePixGeoManager::DesignPitchRP(bool isBLayer) {
  if(isBLayer) return (*pxbd)[0]->getDouble("PITCHRP")*CLHEP::cm;
  else return (*pxbd)[1]->getDouble("PITCHRP")*CLHEP::cm;
}

double OraclePixGeoManager::DesignPitchZ(bool isBLayer) {
  double pitchZ;
  if(isBLayer) {
    if (m_dc1Geometry) { // Override NOVA 
      pitchZ = 300 * CLHEP::micrometer; 
    } else {
      pitchZ = (*pxbd)[0]->getDouble("PITCHZ") * CLHEP::cm;
    }
  } else {
    pitchZ = (*pxbd)[1]->getDouble("PITCHZ") * CLHEP::cm;
  }
  return pitchZ;
}

double OraclePixGeoManager::DesignGapRP() {
  return (*pdch)[0]->getDouble("GAPRP")*CLHEP::cm;
}

double OraclePixGeoManager::DesignGapZ() {
  return (*pdch)[0]->getDouble("GAPZ")*CLHEP::cm;
}


int OraclePixGeoManager::DesignCircuitsPerColumn() {
  //
  // This should be (*pdch)[0]->getDouble("NRPCHIP"), but in the current
  // design we prefer to have one chip in the rphi direction
  // and define the connections for the pixels in the gap
  return 1;
}
int OraclePixGeoManager::DesignCircuitsPerRow() {
  return  (*pdch)[0]->getInt("NZCHIP");
}



#endif

}
