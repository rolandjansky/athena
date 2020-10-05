/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetServMatGeometryManager_H
#define InDetServMatGeometryManager_H

#include "CxxUtils/checker_macros.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class IGeometryDBSvc;
class InDetMaterialManager;

class InDetServMatGeometryManager
{
public:
  InDetServMatGeometryManager(InDetDD::AthenaComps * athenaComps);
 
  ~InDetServMatGeometryManager();
  
  ///Delete assignment
  InDetServMatGeometryManager & operator = (const InDetServMatGeometryManager &) = delete;
  
  ///Delete copy c'tor
  InDetServMatGeometryManager(const InDetServMatGeometryManager &) = delete;
  
  // Access to geometry database
  const IGeometryDBSvc * db() const {return m_athenaComps->geomDB();}

  // Access to material manager 
  InDetMaterialManager * matMgr() {return m_matMgr;}
  
  
  // Access to message stream
  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  // Message strean verbosity level
  bool msgLvl (MSG::Level lvl) const { return m_athenaComps->msgLvl(lvl); }


  ////////////////////////////////////////////////////////////////////////////
  // General parameters
  ////////////////////////////////////////////////////////////////////////////
  // Check if services are to be build
  bool buildServices() const;

  // Simple service table
  IRDBRecordset_ptr simpleServiceTable() const {return m_InDetSimpleServices;}

  ////////////////////////////////////////////////////////////////////////////
  // Pixel parameters
  ////////////////////////////////////////////////////////////////////////////

  // number of layers
  int pixelNumLayers() const;

  // layer radius 
  double pixelLayerRadius(int layer) const;

  // layer length
  double pixelLayerLength(int layer) const;

  // Number of staves/sectors per barrel layer 
  int pixelNumSectorsForLayer(int layer) const;

  // Number of modules per stave
  int pixelModulesPerStave(int layer) const;
  double pixelLadderBentStaveAngle(int layer) const;
  int pixelBentStaveNModule(int layer) const;
  double pixelLadderModuleDeltaZ(int layer) const;

  // Number of staves/sectors per endcap layer 
  int pixelEndcapNumSectorsForLayer(int layer) const;

  int pixelModulesPerRing( int ring) const;

  int pixelModulesPerEndcapSector( int disk) const;

  // number of disks
  int pixelNumDisks() const;

  // disk Z position
  double pixelDiskZ(int disk) const; 

  // disk min radius
  double pixelDiskRMin(int disk) const; 

  // disk max radius
  double pixelDiskRMax(int disk) const; 

 
  // Z-axis Offset for EOS services
  double pixelDiskEOSZOffset(int disk) const ;

  std::string pixelDiskServiceRoute(int disk) const;

  // pixel envelpoe radius
  double pixelEnvelopeRMax() const;

  int pixelBarrelModuleType( int layer) const;

  int pixelDesignType( int moduleType) const;

  int pixelChipsPerModule( int moduleType) const;

  int pixelChipsPerModuleForDisk( int disk) const;

  ////////////////////////////////////////////////////////////////////////////
  // Strip detector (SCT) parameters
  ////////////////////////////////////////////////////////////////////////////

  // number of layers
  int sctNumLayers() const;

  // layer radius 
  double sctLayerRadius(int layer) const;

  // layer length
  double sctLayerLength(int layer) const;

  // layer length
  int sctLayerType(int layer) const;

  // Number of staves/sectors per barrel layer 
  int sctNumSectorsForLayer(int layer) const;
  
  int sctModulesPerLadder(int layer) const;

  // Number of staves/sectors per endcap layer 
  int sctEndcapNumSectorsForLayer(int layer) const;

  // number of disks
  int sctNumDisks() const;

  // disk Z position
  double sctDiskZ(int disk) const; 

  // disk Z position
  double sctDiskRMax(int disk) const; 

  // Inner radius of first SCT support
  double sctInnerSupport() const;

  // Pixel support tubes dimensions: specify name from simple services table
  double SupportTubeRMin(const std::string& name) const;
  double SupportTubeRMax(const std::string& name) const;
  double SupportTubeZMin(const std::string& name) const;
  double SupportTubeZMax(const std::string& name) const;
  int    SupportTubeExists(const std::string& name) const;

private:
  // Record sets
  IRDBRecordset_ptr m_InDetWeights;

  IRDBRecordset_ptr m_PixelBarrelGeneral;
  IRDBRecordset_ptr m_PixelEndcapGeneral;
  IRDBRecordset_ptr m_PixelLayer;
  IRDBRecordset_ptr m_PixelDisk;
  IRDBRecordset_ptr m_PixelDiskRing;
  IRDBRecordset_ptr m_PixelStave;
  IRDBRecordset_ptr m_PixelRing;
  IRDBRecordset_ptr m_PixelModule;
  IRDBRecordset_ptr m_PixelReadout;
  IRDBRecordset_ptr m_PixelWeights;
  IRDBRecordset_ptr m_PixelEnvelope;
  IRDBRecordset_ptr m_PixelSvcRoute;

  IRDBRecordset_ptr m_SctBrlGeneral;
  IRDBRecordset_ptr m_SctBrlLayer;
  IRDBRecordset_ptr m_SctBrlLadder;
  IRDBRecordset_ptr m_SctFwdGeneral;
  IRDBRecordset_ptr m_SctFwdWheel;
  IRDBRecordset_ptr m_SctFwdDiscSupport;
  IRDBRecordset_ptr m_SctBrlServPerLayer;
  IRDBRecordset_ptr m_SctWeights;

  IRDBRecordset_ptr m_InDetSimpleServices;

  IRDBRecordset_ptr m_switches;

  IRDBRecordset_ptr m_scalingTable;

  // Access to athena components
  InDetDD::AthenaComps * m_athenaComps;

  // Material Manager
  InDetMaterialManager * m_matMgr;

  // index of named support tube in  m_InDetSimpleServices table, -1 if not found. Could be cached for speed.
  int SupportTubeIndex(const std::string& name) const;

};

#endif //InDetServMatGeometryManager_H
