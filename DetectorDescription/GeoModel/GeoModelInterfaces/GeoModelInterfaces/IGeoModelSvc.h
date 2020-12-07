/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELINTERFACES_IGEOMODELSVC_H
#define GEOMODELINTERFACES_IGEOMODELSVC_H

// Include Files
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include <string>

// Forward declarations
class IGeoModelTool;

class IGeoModelSvc : virtual public IInterface {
public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID();

    virtual const std::string & atlasVersion()         const =0;
    virtual const std::string & inDetVersion()         const =0;
    virtual const std::string & pixelVersion()         const =0;
    virtual const std::string & SCT_Version()          const =0;
    virtual const std::string & TRT_Version()          const =0;
    virtual const std::string & LAr_Version()          const =0;
    virtual const std::string & tileVersion()          const =0;
    virtual const std::string & muonVersion()          const =0;
    virtual const std::string & caloVersion()          const =0;
    virtual const std::string & magFieldVersion()      const =0;
    virtual const std::string & cavernInfraVersion()   const =0;
    virtual const std::string & forwardDetectorsVersion()   const =0;

    virtual const std::string & inDetVersionOverride() const =0;
    virtual const std::string & pixelVersionOverride() const =0;
    virtual const std::string & SCT_VersionOverride()  const =0;
    virtual const std::string & TRT_VersionOverride()  const =0;
    virtual const std::string & LAr_VersionOverride()  const =0;
    virtual const std::string & tileVersionOverride()  const =0;
    virtual const std::string & muonVersionOverride()  const =0;
    virtual const std::string & caloVersionOverride()  const =0;
    virtual const std::string & magFieldVersionOverride()    const =0;
    virtual const std::string & cavernInfraVersionOverride() const =0;
    virtual const std::string & forwardDetectorsVersionOverride() const =0;
    
    virtual GeoModel::GeoConfig geoConfig() const = 0;

    // Access to subsystem tool for callback registration
    virtual const IGeoModelTool* getTool(std::string toolName) const = 0;

    // Release GeoModel tree from memory
    virtual StatusCode clear() = 0;
};

inline
const InterfaceID&
IGeoModelSvc::interfaceID() {

  static const InterfaceID IID(1011, 1, 1);

  return IID;
}


#endif // GEOMODELINTERFACES_GEOMODELSVC_H



