/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GeoModelTool_H
#define AFP_GeoModelTool_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AFP_GeoModel/AFP_GeoModelFactory.h"
#include "AFP_Geometry/AFP_Geometry.h"

#define COOLFOLDER_BPM "/TDAQ/OLC/ALFA"

class AFP_GeoModelTool : public GeoModelTool
{

public:
    AFP_GeoModelTool( const std::string& type, const std::string& name, const IInterface* parent );
    virtual ~AFP_GeoModelTool();

	virtual StatusCode create( StoreGateSvc* detStore );

private:
	AFP_CONFIGURATION m_CfgParams;
	AFP_Geometry* m_pGeometry;
	AFP_GeoModelFactory* m_pAFPDetectorFactory;
	ServiceHandle< IIOVDbSvc > m_iovSvc;
	std::vector<double> arrAFP00XStaggering, arrAFP00YStaggering;
	std::vector<double> arrAFP01XStaggering, arrAFP01YStaggering;
	std::vector<double> arrAFP02XStaggering, arrAFP02YStaggering;
	std::vector<double> arrAFP03XStaggering, arrAFP03YStaggering;

private:
	StatusCode CheckPropertiesSettings();
	//virtual StatusCode registerCallback(StoreGateSvc* detStore);
	//virtual StatusCode align(IOVSVC_CALLBACK_ARGS);

};

#endif // AFP_GeoModelTool_H
