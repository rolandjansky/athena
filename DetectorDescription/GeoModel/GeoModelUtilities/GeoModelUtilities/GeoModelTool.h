/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_GEOMODELTOOL_H
#define GEOMODELSVC_GEOMODELTOOL_H

#ifndef BUILDVP1LIGHT

#include "GeoModelInterfaces/IGeoModelTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"

class GeoVDetectorManager;

class GeoModelTool : public AthAlgTool, public virtual IGeoModelTool {

public:

    // Standard Constructor
    GeoModelTool( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~GeoModelTool();

    virtual GeoVDetectorManager* manager();
    virtual const GeoVDetectorManager* manager() const;

    virtual StatusCode clear() override;	
    virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override;
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override;

protected:

    GeoVDetectorManager*   m_detector;        		// The corresponding 
};

#endif  // BUILDVP1LIGHT

#endif // GEOMODELSVC_DETDESCRTOOL_H



