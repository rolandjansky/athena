/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GeoModelXml_HGTD_GMX_DETECTORTOOL_H
#define HGTD_GeoModelXml_HGTD_GMX_DETECTORTOOL_H
//
//    Create an Athena Tool; handle Athena services and Tools needed for 
//    building the HGTD geometry. 
//    This is the entry to the HGTD_GeoModelXml package.
//
#include "InDetGeoModelUtils/GeoModelXmlTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
// #include "GeoModelInterfaces/IGeoDbTagSvc.h"

#include <string>

class HGTD_DetectorManager;

namespace InDetDD {
    class SiCommonItems;
}

class HGTD_GMX_DetectorTool : public GeoModelXmlTool {
public:
    // Standard Constructor
    HGTD_GMX_DetectorTool(const std::string &type, const std::string &name, const IInterface *parent);

    virtual StatusCode create() override final;
    virtual StatusCode clear() override final;

    // Register callback function on CondDB object
    virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;

    // Callback function itself
    // virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override final;
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;

private:

    Gaudi::Property<bool> m_alignable{this, "Alignable", false, ""};
    //This should be changed to an HGTD-specific one in future, once available
    Gaudi::Property<std::string> m_alignmentFolderName{this, "AlignmentFolderName", "/Indet/AlignHGTD", ""}; // modifay it in future

    const HGTD_DetectorManager* m_detectorManager{nullptr};
    InDetDD::SiCommonItems* m_commonItems;

    HGTD_DetectorManager * createManager(GeoPhysVol * theWorld);
};

#endif // HGTD_GeoModelXml_HGTD_GMX_DETECTORTOOL_H
