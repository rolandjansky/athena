/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServicesTrackerBuilder_H
#define ServicesTrackerBuilder_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PixelInterfaces/IPixelModuleSvc.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"

class ServicesDynTracker;

class ServicesTrackerBuilder { 
public:

  ServicesTrackerBuilder();
  ServicesDynTracker* buildGeometry(const PixelGeoBuilderBasics* basics, bool bSvcDynAuto=true, bool bBarrelModuleMaterial=false) const;

 private: 
  double ComputeRMax(double rMin, double safety, double moduleLength, double moduleWidth) const;

  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader; 
  ServiceHandle<IPixelModuleSvc> m_pixelModuleSvc;

};

#endif
