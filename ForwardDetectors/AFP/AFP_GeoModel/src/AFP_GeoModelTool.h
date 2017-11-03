/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GeoModelTool_H
#define AFP_GeoModelTool_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AFP_GeoModelFactory.h"
#include "AFP_Geometry/AFP_Geometry.h"

#define COOLFOLDER_BPM "/TDAQ/OLC/ALFA"

class AFP_GeoModelTool : public GeoModelTool
{

 public:
    AFP_GeoModelTool( const std::string& type, const std::string& name, const IInterface* parent );
    virtual ~AFP_GeoModelTool() override final;
    
    virtual StatusCode create() override final;
    
 private:
    AFP_CONFIGURATION m_CfgParams;
    AFP_Geometry* m_pGeometry;
    AFP_GeoModelFactory* m_pAFPDetectorFactory;
    ServiceHandle< IIOVDbSvc > m_iovSvc;
    std::vector<double> m_vecAFP00XStaggering, m_vecAFP00YStaggering;
    std::vector<double> m_vecAFP01XStaggering, m_vecAFP01YStaggering;
    std::vector<double> m_vecAFP02XStaggering, m_vecAFP02YStaggering;
    std::vector<double> m_vecAFP03XStaggering, m_vecAFP03YStaggering;
    
 private:
    StatusCode CheckPropertiesSettings();
    //virtual StatusCode registerCallback(StoreGateSvc* detStore);
    //virtual StatusCode align(IOVSVC_CALLBACK_ARGS);
};

#endif // AFP_GeoModelTool_H
