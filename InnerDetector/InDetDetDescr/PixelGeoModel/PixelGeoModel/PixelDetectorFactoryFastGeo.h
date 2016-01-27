/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelDetectorFactoryFastGeo_h
#define PixelDetectorFactoryFastGeo_h 

#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// readout includes:
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "PixelInterfaces/IGeoPixelEnvelopeTool.h"

class PixelSwitches;
class PixelGeometryManager;
class PixelGeoModelAthenaComps;




class PixelDetectorFactoryFastGeo : public InDetDD::DetectorFactoryBase {

 public:
  // Constructor:
  PixelDetectorFactoryFastGeo(const PixelGeoModelAthenaComps * athenaComps,
			      const PixelSwitches & switches,
			      std::string geoBuilderName,
			      bool bConfigGeoAlgTool = false);
  
  // Destructor:
  ~PixelDetectorFactoryFastGeo();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const InDetDD::PixelDetectorManager * getDetectorManager() const;

 private:  

  // Illegal operations:
  const PixelDetectorFactoryFastGeo & operator=(const PixelDetectorFactoryFastGeo &right);
  PixelDetectorFactoryFastGeo(const PixelDetectorFactoryFastGeo &right);
  // private data
  InDetDD::PixelDetectorManager     *m_detectorManager;
  PixelGeometryManager * m_geometryManager;

  std::string m_geomLayoutName;
  ToolHandle<IGeoPixelEnvelopeTool> m_geomBuilderTool;
  bool m_bConfigGeoAlgTool;

  void doChecks();

};


#endif 
