/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatGeometryManager.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "CLHEP/Units/SystemOfUnits.h"

InDetServMatGeometryManager::InDetServMatGeometryManager(const InDetDD::AthenaComps * athenaComps)   
  : m_athenaComps(athenaComps),
    m_matMgr(0)
{
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initializing InDetServMatGeometryManager" << endreq;
   
  IRDBAccessSvc *rdbSvc = m_athenaComps->rdbAccessSvc();
  IGeoModelSvc *geoModel = m_athenaComps->geoModelSvc();;

  // Get version tag and node for Pixel.
  DecodeVersionKey pixelVersionKey(geoModel, "Pixel");
  std::string pixelDetectorKey  = pixelVersionKey.tag();
  std::string pixelDetectorNode = pixelVersionKey.node();

  // Get version tag and node for SCT.
  DecodeVersionKey sctVersionKey(geoModel, "SCT");
  std::string sctDetectorKey  = sctVersionKey.tag();
  std::string sctDetectorNode = sctVersionKey.node();

  //// Get version tag and node for InnerDetector.
  DecodeVersionKey indetVersionKey(geoModel, "InnerDetector");
  std::string indetDetectorKey  = indetVersionKey.tag();
  std::string indetDetectorNode = indetVersionKey.node();
  
/////////////////////////////////////////////////////////
//
// Gets the record sets
//
/////////////////////////////////////////////////////////

  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Retrieving Pixel Record Sets from database ..." << endreq;
    msg(MSG::DEBUG) << "Pixel: Key = " << pixelDetectorKey << " Node = " << pixelDetectorNode << endreq;
    msg(MSG::DEBUG) << "SCT:   Key = " << sctDetectorKey   << " Node = " << sctDetectorNode << endreq;
    msg(MSG::DEBUG) << "InDet: Key = " << indetDetectorKey << " Node = " << indetDetectorNode << endreq;
  }
 
  m_InDetWeights       = rdbSvc->getRecordsetPtr("InDetWeights",       indetDetectorKey, indetDetectorNode);

  m_PixelBarrelGeneral = rdbSvc->getRecordsetPtr("PixelBarrelGeneral", pixelDetectorKey, pixelDetectorNode);
  m_PixelEndcapGeneral = rdbSvc->getRecordsetPtr("PixelEndcapGeneral", pixelDetectorKey, pixelDetectorNode);
  m_PixelLayer         = rdbSvc->getRecordsetPtr("PixelLayer",         pixelDetectorKey, pixelDetectorNode);
  m_PixelDisk          = rdbSvc->getRecordsetPtr("PixelDisk",          pixelDetectorKey, pixelDetectorNode);
  m_PixelDiskRing      = rdbSvc->getRecordsetPtr("PixelDiskRing",      pixelDetectorKey, pixelDetectorNode);
  m_PixelStave         = rdbSvc->getRecordsetPtr("PixelStave",         pixelDetectorKey, pixelDetectorNode);
  m_PixelRing          = rdbSvc->getRecordsetPtr("PixelRing",          pixelDetectorKey, pixelDetectorNode);
  m_PixelModule        = rdbSvc->getRecordsetPtr("PixelModule",        pixelDetectorKey, pixelDetectorNode);
  m_PixelReadout       = rdbSvc->getRecordsetPtr("PixelReadout",       pixelDetectorKey, pixelDetectorNode);
  m_PixelWeights       = rdbSvc->getRecordsetPtr("PixelWeights",       pixelDetectorKey, pixelDetectorNode);
  m_PixelEnvelope      = rdbSvc->getRecordsetPtr("PixelEnvelope",      pixelDetectorKey, pixelDetectorNode);

  m_SctBrlGeneral      = rdbSvc->getRecordsetPtr("SSctBrlGeneral",      sctDetectorKey, sctDetectorNode);
  m_SctBrlLayer        = rdbSvc->getRecordsetPtr("SSctBrlLayer",        sctDetectorKey, sctDetectorNode);
  m_SctBrlLadder       = rdbSvc->getRecordsetPtr("SSctBrlLadder",       sctDetectorKey, sctDetectorNode);
  m_SctFwdGeneral      = rdbSvc->getRecordsetPtr("SSctFwdGeneral",      sctDetectorKey, sctDetectorNode);
  m_SctFwdWheel        = rdbSvc->getRecordsetPtr("SSctFwdWheel",        sctDetectorKey, sctDetectorNode);
  m_SctFwdDiscSupport  = rdbSvc->getRecordsetPtr("SSctFwdDiscSupport",  sctDetectorKey, sctDetectorNode);
  m_SctBrlServPerLayer = rdbSvc->getRecordsetPtr("SSctBrlServPerLayer", sctDetectorKey, sctDetectorNode);
  m_SctWeights         = rdbSvc->getRecordsetPtr("SctWeights",         sctDetectorKey, sctDetectorNode);

  m_InDetSimpleServices = rdbSvc->getRecordsetPtr("InDetSimpleServices", indetDetectorKey, indetDetectorNode);

  m_scalingTable = rdbSvc->getRecordsetPtr("InDetServMatScaling", indetDetectorKey, indetDetectorNode);

  m_switches = rdbSvc->getRecordsetPtr("InDetServSwitches", indetDetectorKey, indetDetectorNode);

  m_matMgr = new InDetMaterialManager("InDetServMatMaterialManager", m_athenaComps);
  m_matMgr->addWeightTable(m_InDetWeights, "indet");
  m_matMgr->addWeightTable(m_PixelWeights, "pix");
  m_matMgr->addWeightTable(m_SctWeights,   "sct");
  m_matMgr->addScalingTable(m_scalingTable);

}

InDetServMatGeometryManager::~InDetServMatGeometryManager()
{
  delete m_matMgr;
}

// flag for whether or not to build services
bool InDetServMatGeometryManager::buildServices() const
{
  if (db()->testField("","BUILDSERVICES")) {
    return db()->getInt("","BUILDSERVICES");
  } 
  if (db()->testField(m_switches,"BUILDSERVICES")) {
    return db()->getInt(m_switches,"BUILDSERVICES");
  } 
  return false;
}

int InDetServMatGeometryManager::pstIndex() const 
{
  for (unsigned int i = 0; i < db()->getTableSize(m_InDetSimpleServices); i++) {
    std::string name = db()->getString(m_InDetSimpleServices,"NAME",i);
    if (name == "PST") return i;
  }
  return -1;
}

double InDetServMatGeometryManager::pstRMin() const 
{
  int ind = pstIndex();
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "RMIN", ind);
  else          return 0;
}

double InDetServMatGeometryManager::pstRMax() const 
{
  int ind = pstIndex();
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "RMAX", ind);
  else          return 0;
}

double InDetServMatGeometryManager::pstZMax() const 
{
  int ind = pstIndex();
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "ZMAX", ind);
  else          return 0;
}

int InDetServMatGeometryManager::istIndex() const 
{
  for (unsigned int i = 0; i < db()->getTableSize(m_InDetSimpleServices); i++) {
    std::string name = db()->getString(m_InDetSimpleServices,"NAME",i);
    if (name == "IST") return i;
  }
  return -1;
}

double InDetServMatGeometryManager::istRMin() const 
{
  int ind = istIndex();
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "RMIN", ind);
  else          return 0;
}

double InDetServMatGeometryManager::istRMax() const 
{
  int ind = istIndex();
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "RMAX", ind);
  else          return 0;
}

double InDetServMatGeometryManager::istZMax() const 
{
  int ind = istIndex();
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "ZMAX", ind);
  else          return 0;
}

// number of layers
int InDetServMatGeometryManager::pixelNumLayers() const
{
  return db()->getInt(m_PixelBarrelGeneral,"NLAYER");
}

// layer radius 
double InDetServMatGeometryManager::pixelLayerRadius(int layer) const
{
  return db()->getDouble(m_PixelLayer,"RLAYER",layer) * CLHEP::mm;
}

// layer length
double InDetServMatGeometryManager::pixelLayerLength(int layer) const
{
  int staveIndex = db()->getInt(m_PixelLayer,"STAVEINDEX",layer);
  return db()->getDouble(m_PixelStave,"ENVLENGTH",staveIndex) * CLHEP::mm;
}

// Number of staves/sectors per barrel layer 
int InDetServMatGeometryManager::pixelNumSectorsForLayer(int layer) const
{
  return db()->getInt(m_PixelLayer,"NSECTORS",layer);
}

// Number of modules per stave
int InDetServMatGeometryManager::pixelModulesPerStave(int layer) const
{
  //msg(MSG::INFO) << "Entering InDetServMatGeometryManager::pixelModulesPerStave for layer " << layer << endreq;

  int staveIndex = db()->getInt(m_PixelLayer,"STAVEINDEX",layer);

  //msg(MSG::INFO) << "staveIndex for the layer is " << staveIndex << endreq;
  //msg(MSG::INFO) << "modules per stave is " << db()->getInt(m_PixelStave,"NMODULE",staveIndex) << endreq;

  return db()->getInt(m_PixelStave,"NMODULE",staveIndex);
}

// Number of staves/sectors per endcap layer 
int InDetServMatGeometryManager::pixelEndcapNumSectorsForLayer(int layer) const
{
  return db()->getInt(m_PixelDisk,"NSECTORS",layer);  // FIXME: not yet in DB?
}

int InDetServMatGeometryManager::pixelModulesPerRing( int ring) const
{
  return db()->getInt( m_PixelRing, "NMODULE", ring);
}

int InDetServMatGeometryManager::pixelModulesPerEndcapSector( int layer) const
{
  int nModulesDisk = 0;
  for (unsigned int indexTmp = 0; indexTmp < db()->getTableSize(m_PixelDiskRing); ++indexTmp) {
    int disk = db()->getInt(m_PixelDiskRing,"DISK",indexTmp);
    if ( disk == layer) {
      int ring = db()->getInt(m_PixelDiskRing,"RING",indexTmp);
      nModulesDisk += pixelModulesPerRing( ring);
      //msg(MSG::INFO) << "Pixel Ring " << ring << " on disk " << disk << " has " << pixelModulesPerRing( ring) << " modules" << endreq;
    }
  }
  return nModulesDisk / pixelEndcapNumSectorsForLayer(layer);
}

int InDetServMatGeometryManager::pixelChipsPerModuleForDisk( int layer) const
{
  int sumChips = 0;
  int sumModules = 0;
  for (unsigned int indexTmp = 0; indexTmp < db()->getTableSize(m_PixelDiskRing); ++indexTmp) {
    int disk = db()->getInt(m_PixelDiskRing,"DISK",indexTmp);
    if ( disk == layer) {
      int ring = db()->getInt(m_PixelDiskRing,"RING",indexTmp);
      int moduleType = db()->getInt(m_PixelRing,"MODULETYPE",ring);
      int nModules = pixelModulesPerRing( ring);
      sumModules += nModules;
      sumChips += nModules * pixelChipsPerModule( moduleType);
    }
  }
  if (sumChips % sumModules == 0) return sumChips/sumModules;
  else return 1 + sumChips/sumModules; // round to larger integer
}

// number of disks
int InDetServMatGeometryManager::pixelNumDisks() const
{
  return db()->getInt(m_PixelEndcapGeneral,"NDISK");
}

// disk Z position
double InDetServMatGeometryManager::pixelDiskZ(int disk) const 
{
  return db()->getDouble(m_PixelDisk,"ZDISK",disk) * CLHEP::mm;
}

// disk min radius
double InDetServMatGeometryManager::pixelDiskRMin(int disk) const 
{
  // FIXME. Number in DB doesn't take into account support
  return db()->getDouble(m_PixelDisk,"RMIN",disk) * CLHEP::mm - 11*CLHEP::mm;
}

// disk max radius
double InDetServMatGeometryManager::pixelDiskRMax(int disk) const 
{
  // FIXME. Number in DB doesn't take into account support
  return db()->getDouble(m_PixelDisk,"RMAX",disk) * CLHEP::mm + 11*CLHEP::mm;
}

double InDetServMatGeometryManager::pixelEnvelopeRMax() const
{
  return db()->getDouble(m_PixelEnvelope,"RMAX") * CLHEP::mm;
}

int InDetServMatGeometryManager::pixelBarrelModuleType( int layer) const 
{
  return db()->getInt( m_PixelLayer, "MODULETYPE", layer);
}
    /*
  else {
426	      // Not in DB yet.
427	      int ringType = getDiskRingType(currentLD,m_eta);
428	      if (ringType>=0) {
429	        type = db()->getInt(PixelRing,"MODULETYPE",ringType);
430	      }
431	    }
432
    */

int InDetServMatGeometryManager::pixelDesignType( int moduleType) const
{
  return db()->getInt( m_PixelModule, "DESIGNTYPE", moduleType);
}

int InDetServMatGeometryManager::pixelChipsPerModule( int moduleType) const 
{
  int nChipsEta = db()->getInt( m_PixelReadout, "NCHIPSETA", moduleType);
  int nChipsPhi = db()->getInt( m_PixelReadout, "NCHIPSPHI", moduleType);
  return nChipsEta*nChipsPhi;
}



// number of layers
int InDetServMatGeometryManager::sctNumLayers() const
{
  return db()->getInt(m_SctBrlGeneral,"NUMLAYERS");
}

// layer radius 
double InDetServMatGeometryManager::sctLayerRadius(int layer) const
{
  return db()->getDouble(m_SctBrlLayer,"RADIUS",layer) * CLHEP::mm;
}

// layer length
double InDetServMatGeometryManager::sctLayerLength(int layer) const
{
  return db()->getDouble(m_SctBrlLayer,"CYLLENGTH",layer) * CLHEP::mm;
}

// layer type. Long(0) or Short (1) strips. NEEDS CHECKING
int InDetServMatGeometryManager::sctLayerType(int layer) const
{
  int ladType =  db()->getInt(m_SctBrlLayer,"LADDERTYPE",layer);
  return db()->getInt(m_SctBrlLadder,"MODTYPE",ladType);
}

// Number of staves/sectors per barrel layer 
int InDetServMatGeometryManager::sctNumSectorsForLayer(int layer) const
{
  return db()->getInt(m_SctBrlLayer,"SKISPERLAYER",layer);
}

int InDetServMatGeometryManager::sctModulesPerLadder(int layer) const
{
  int ladType =  db()->getInt(m_SctBrlLayer,"LADDERTYPE",layer);
  return db()->getInt(m_SctBrlLadder,"NUMPERLADDER",ladType);
}

// Number of staves/sectors per endcap layer 
int InDetServMatGeometryManager::sctEndcapNumSectorsForLayer(int /*layer*/) const
{
  return 32; // FIXME: hardwired number, should go to text file and DB
  //return db()->getInt(m_SctFwdWheel,"NSECTORS",layer); // FIXME: not yet in DB or text file
}

// number of disks
int InDetServMatGeometryManager::sctNumDisks() const
{
  return db()->getInt(m_SctFwdGeneral,"NUMWHEELS");
}

// disk Z position
double InDetServMatGeometryManager::sctDiskZ(int disk) const 
{
  return db()->getDouble(m_SctFwdWheel,"ZPOSITION",disk) * CLHEP::mm;
}

// disk Z position
double InDetServMatGeometryManager::sctDiskRMax(int disk) const 
{
  return db()->getDouble(m_SctFwdDiscSupport,"OUTERRADIUS",disk) * CLHEP::mm;
}

double InDetServMatGeometryManager::sctInnerSupport() const 
{
  return db()->getDouble(m_SctBrlServPerLayer,"SUPPORTCYLINNERRAD",0) * CLHEP::mm;
}

