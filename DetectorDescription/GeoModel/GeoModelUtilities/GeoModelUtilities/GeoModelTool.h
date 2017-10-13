/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_GEOMODELTOOL_H
#define GEOMODELSVC_GEOMODELTOOL_H

#include "GeoModelInterfaces/IGeoModelTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class GeoVDetectorManager;

class GeoModelTool : public AthAlgTool, public virtual IGeoModelTool {

public:

    // Standard Constructor
    GeoModelTool( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~GeoModelTool();

    virtual GeoVDetectorManager* manager() const;

    virtual StatusCode clear() override;	
    virtual StatusCode registerCallback() override;
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override;

protected:

    GeoVDetectorManager*   m_detector;        		// The corresponding 
};

#endif // GEOMODELSVC_DETDESCRTOOL_H



