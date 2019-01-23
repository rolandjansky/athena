/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/InDetServMatGeometryManager.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GaudiKernel/SystemOfUnits.h"

InDetServMatGeometryManager::InDetServMatGeometryManager(const InDetDD::AthenaComps * athenaComps)   
  : m_athenaComps(athenaComps),
    m_matMgr(0)
{
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initializing InDetServMatGeometryManager" << endmsg;
   
  IGeoDbTagSvc *geoDbTag = m_athenaComps->geoDbTagSvc();
  IRDBAccessSvc *rdbSvc = m_athenaComps->rdbAccessSvc();

  // Get version tag and node for Pixel.
  DecodeVersionKey pixelVersionKey(geoDbTag,"Pixel");
  std::string pixelDetectorKey  = pixelVersionKey.tag();
  std::string pixelDetectorNode = pixelVersionKey.node();

  // Get version tag and node for SCT.
  DecodeVersionKey sctVersionKey(geoDbTag,"SCT");
  std::string sctDetectorKey  = sctVersionKey.tag();
  std::string sctDetectorNode = sctVersionKey.node();

  //// Get version tag and node for InnerDetector.
  DecodeVersionKey indetVersionKey(geoDbTag,"InnerDetector");
  std::string indetDetectorKey  = indetVersionKey.tag();
  std::string indetDetectorNode = indetVersionKey.node();
  
/////////////////////////////////////////////////////////
//
// Gets the record sets
//
/////////////////////////////////////////////////////////

  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Retrieving Pixel Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << "Pixel: Key = " << pixelDetectorKey << " Node = " << pixelDetectorNode << endmsg;
    msg(MSG::DEBUG) << "SCT:   Key = " << sctDetectorKey   << " Node = " << sctDetectorNode << endmsg;
    msg(MSG::DEBUG) << "InDet: Key = " << indetDetectorKey << " Node = " << indetDetectorNode << endmsg;
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
  m_PixelSvcRoute      = rdbSvc->getRecordsetPtr("PixelServiceRoute",  pixelDetectorKey, pixelDetectorNode);

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

int InDetServMatGeometryManager::SupportTubeIndex(std::string name) const 
{
  for (unsigned int i = 0; i < db()->getTableSize(m_InDetSimpleServices); i++) 
  {
    if (db()->getString(m_InDetSimpleServices,"NAME",i)  == name) return i;
  }
  return -1;
}

double InDetServMatGeometryManager::SupportTubeRMin(std::string name) const 
{
  int ind = SupportTubeIndex(name);
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "RMIN", ind);
  return 0;
}

double InDetServMatGeometryManager::SupportTubeRMax(std::string name) const 
{
  int ind = SupportTubeIndex(name);
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "RMAX", ind);
  return 0;
}

double InDetServMatGeometryManager::SupportTubeZMin(std::string name) const 
{
  int ind = SupportTubeIndex(name);
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "ZMIN", ind);
  return 0;
}

double InDetServMatGeometryManager::SupportTubeZMax(std::string name) const 
{
  int ind = SupportTubeIndex(name);
  if (ind >= 0) return db()->getDouble(m_InDetSimpleServices, "ZMAX", ind);
  return 0;
}

int InDetServMatGeometryManager::SupportTubeExists(std::string name) const 
{
  if (SupportTubeIndex(name) != -1) return 1;
  return 0;
}


// number of layers
int InDetServMatGeometryManager::pixelNumLayers() const
{
  return db()->getInt(m_PixelBarrelGeneral,"NLAYER");
}

// layer radius 
double InDetServMatGeometryManager::pixelLayerRadius(int layer) const
{
  return db()->getDouble(m_PixelLayer,"RLAYER",layer) * Gaudi::Units::mm;
}

// layer length
double InDetServMatGeometryManager::pixelLayerLength(int layer) const
{
  int staveIndex = db()->getInt(m_PixelLayer,"STAVEINDEX",layer);
  return db()->getDouble(m_PixelStave,"ENVLENGTH",staveIndex) * Gaudi::Units::mm;
}

// Number of staves/sectors per barrel layer 
int InDetServMatGeometryManager::pixelNumSectorsForLayer(int layer) const
{
  return db()->getInt(m_PixelLayer,"NSECTORS",layer);
}

// Number of modules per stave
int InDetServMatGeometryManager::pixelModulesPerStave(int layer) const
{
  //msg(MSG::INFO) << "Entering InDetServMatGeometryManager::pixelModulesPerStave for layer " << layer << endmsg;

  int staveIndex = db()->getInt(m_PixelLayer,"STAVEINDEX",layer);

  //msg(MSG::INFO) << "staveIndex for the layer is " << staveIndex << endmsg;
  //msg(MSG::INFO) << "modules per stave is " << db()->getInt(m_PixelStave,"NMODULE",staveIndex) << endmsg;

  return db()->getInt(m_PixelStave,"NMODULE",staveIndex);
}


// Bent stave (conical layout) parameters
double InDetServMatGeometryManager::pixelLadderBentStaveAngle(int layer)  const
{
  if (!db()->testFieldTxt(m_PixelStave, "BENTSTAVEANGLE")) return 0;
  int staveIndex = db()->getInt(m_PixelLayer,"STAVEINDEX", layer);
  return db()->getDouble(m_PixelStave,"BENTSTAVEANGLE", staveIndex);
}

int InDetServMatGeometryManager::pixelBentStaveNModule(int layer) const
{
  if (!db()->testFieldTxt(m_PixelStave,"BENTSTAVENMODULE")) return 0;
  int staveIndex = db()->getInt(m_PixelLayer, "STAVEINDEX", layer);
  return db()->getInt(m_PixelStave, "BENTSTAVENMODULE", staveIndex);
}

double InDetServMatGeometryManager::pixelLadderModuleDeltaZ(int layer) const
{
  int staveIndex = db()->getInt(m_PixelLayer, "STAVEINDEX", layer);
  return db()->getDouble(m_PixelStave, "MODULEDZ", staveIndex);
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
      //msg(MSG::INFO) << "Pixel Ring " << ring << " on disk " << disk << " has " << pixelModulesPerRing( ring) << " modules" << endmsg;
    }
  }
  if(pixelEndcapNumSectorsForLayer(layer)==0) return 0;
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
  if(sumModules==0) return 0;
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
  return db()->getDouble(m_PixelDisk,"ZDISK",disk) * Gaudi::Units::mm;
}

// disk min radius
double InDetServMatGeometryManager::pixelDiskRMin(int disk) const 
{
  std::string route = pixelDiskServiceRoute(disk);   
  if(route=="StdRoute")
    return db()->getDouble(m_PixelDisk,"RMIN",disk) * Gaudi::Units::mm - 11*Gaudi::Units::mm;

  // support structures - SUP1RMIN is always closest to centre
  return db()->getDouble(m_PixelDisk,"SUP1RMIN",disk) * Gaudi::Units::mm;

}

// disk max radius
double InDetServMatGeometryManager::pixelDiskRMax(int disk) const 
{
  std::string route = pixelDiskServiceRoute(disk);   
  if(route=="StdRoute")
    return db()->getDouble(m_PixelDisk,"RMAX",disk) * Gaudi::Units::mm + 11*Gaudi::Units::mm;

  // support structures - SUP3RMAX is always furthest from centre
  return db()->getDouble(m_PixelDisk,"SUP3RMAX",disk) * Gaudi::Units::mm;

}

// EOS ZOffset
double InDetServMatGeometryManager::pixelDiskEOSZOffset(int disk) const 
{
  if (!db()->testField(m_PixelSvcRoute, "EOSZOFFSET")) 
    return 0.0;
  else
    return db()->getDouble(m_PixelSvcRoute,"EOSZOFFSET",disk) * Gaudi::Units::mm;
}

// return name of support tube where 
std::string InDetServMatGeometryManager::pixelDiskServiceRoute(int disk) const 
{
  if(db()->testField(m_PixelSvcRoute,"SERVICEROUTE"))
    return db()->getString(m_PixelSvcRoute,"SERVICEROUTE",disk);
  return "StdRoute";
}

double InDetServMatGeometryManager::pixelEnvelopeRMax() const
{
  return db()->getDouble(m_PixelEnvelope,"RMAX") * Gaudi::Units::mm;
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
  return db()->getDouble(m_SctBrlLayer,"RADIUS",layer) * Gaudi::Units::mm;
}

// layer length
double InDetServMatGeometryManager::sctLayerLength(int layer) const
{
  return db()->getDouble(m_SctBrlLayer,"CYLLENGTH",layer) * Gaudi::Units::mm;
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
  return db()->getDouble(m_SctFwdWheel,"ZPOSITION",disk) * Gaudi::Units::mm;
}

// disk Z position
double InDetServMatGeometryManager::sctDiskRMax(int disk) const 
{
  return db()->getDouble(m_SctFwdDiscSupport,"OUTERRADIUS",disk) * Gaudi::Units::mm;
}

double InDetServMatGeometryManager::sctInnerSupport() const 
{
  return db()->getDouble(m_SctBrlServPerLayer,"SUPPORTCYLINNERRAD",0) * Gaudi::Units::mm;
}

