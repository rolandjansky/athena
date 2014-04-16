/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardRegionGeoModel_ForwardRegionGeoModelTool_H
#define ForwardRegionGeoModel_ForwardRegionGeoModelTool_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "ForwardRegionGeoModel/ForwardRegionGeoModelFactory.h"

class ForwardRegionGeoModelTool : public GeoModelTool {

public:

    // Standard Constructor
    ForwardRegionGeoModelTool( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~ForwardRegionGeoModelTool();

    virtual StatusCode create( StoreGateSvc* detStore );
private:
    // property configuration
    FWD_CONFIGURATION m_Config;
};


#endif // ForwardRegionGeoModel_ForwardRegionGeoModelTool_H
