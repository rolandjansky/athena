/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardRegionGeoModel_ForwardRegionGeoModelTool_H
#define ForwardRegionGeoModel_ForwardRegionGeoModelTool_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "ForwardRegionGeoModelFactory.h"

class ForwardRegionGeoModelTool : public GeoModelTool {

public:

    // Standard Constructor
    ForwardRegionGeoModelTool( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~ForwardRegionGeoModelTool() override final;

    virtual StatusCode create() override final;
private:
    // property configuration
    FWD_CONFIGURATION m_Config;
};


#endif // ForwardRegionGeoModel_ForwardRegionGeoModelTool_H
