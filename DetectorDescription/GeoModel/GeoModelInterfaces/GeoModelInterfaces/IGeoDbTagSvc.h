/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELINTERFACES_IGEODBTAGSVC_H
#define GEOMODELINTERFACES_IGEODBTAGSVC_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include <string>

namespace GeoModelIO {
  class ReadGeoModel;
}

namespace GeoModel {
  enum GeoConfig {
    GEO_RUN1,
    GEO_RUN2,
    GEO_RUN3,
    GEO_RUN4,
    GEO_TESTBEAM
  };
}

static const InterfaceID IID_IGeoDbTagSvc("IGeoDbTagSvc", 1, 0);

class IGeoDbTagSvc : virtual public IInterface {
 public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IGeoDbTagSvc;}

  virtual const std::string & atlasVersion()                    const =0;
  virtual const std::string & inDetVersion()                    const =0;
  virtual const std::string & pixelVersion()                    const =0;
  virtual const std::string & SCT_Version()                     const =0;
  virtual const std::string & TRT_Version()                     const =0;
  virtual const std::string & LAr_Version()                     const =0;
  virtual const std::string & tileVersion()                     const =0;
  virtual const std::string & muonVersion()                     const =0;
  virtual const std::string & caloVersion()                     const =0;
  virtual const std::string & magFieldVersion()                 const =0;
  virtual const std::string & cavernInfraVersion()              const =0;
  virtual const std::string & forwardDetectorsVersion()         const =0;
  
  virtual const std::string & inDetVersionOverride()            const =0;
  virtual const std::string & pixelVersionOverride()            const =0;
  virtual const std::string & SCT_VersionOverride()             const =0;
  virtual const std::string & TRT_VersionOverride()             const =0;
  virtual const std::string & LAr_VersionOverride()             const =0;
  virtual const std::string & tileVersionOverride()             const =0;
  virtual const std::string & muonVersionOverride()             const =0;
  virtual const std::string & caloVersionOverride()             const =0;
  virtual const std::string & magFieldVersionOverride()         const =0;
  virtual const std::string & cavernInfraVersionOverride()      const =0;
  virtual const std::string & forwardDetectorsVersionOverride() const =0;
  
  virtual GeoModel::GeoConfig geoConfig() const = 0;

  // Get the name of the IRDBAccessSvc used in the given job
  virtual const std::string & getParamSvcName() const = 0;
  
  // Are we building GeoModel description from the GeometryDB or from the SQLite file?
  // Yes - if the returned pointer in not null
  // No  - otherwise
  virtual GeoModelIO::ReadGeoModel* getSqliteReader() = 0;
};

#endif // GEOMODELINTERFACES_IGEODBTAGSVC_H
