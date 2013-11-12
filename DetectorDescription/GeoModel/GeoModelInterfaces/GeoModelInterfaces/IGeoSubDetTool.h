/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELINTERFACES_IGEOSUBDETTOOL_H
#define GEOMODELINTERFACES_IGEOSUBDETTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class StoreGateSvc;
class GeoVPhysVol;

static const InterfaceID IID_IGeoSubDetTool( "IGeoSubDetTool", 1, 0 );

class IGeoSubDetTool : public virtual IAlgTool {
public:

    /// Retrieve Interface ID
    static const InterfaceID& interfaceID( ) { return IID_IGeoSubDetTool; }
  

    // Build subdetector in parent 
    virtual StatusCode build( GeoVPhysVol* parent ) = 0;

    // Register callback function on ConDB object
    virtual StatusCode registerCallback( StoreGateSvc* detStore ) = 0;
  
    // Callback function itself
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS) = 0;

};

#endif // GEOMODELINTERFACES_IGEOSUBDETTOOL_H



