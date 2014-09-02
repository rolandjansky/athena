/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _MUONGEOMODELTOOL_H
#define _MUONGEOMODELTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"

class MuonDetDescrManager;

class MuonGeoModelTool : public GeoModelTool {

public:

    // Standard Constructor
    MuonGeoModelTool( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~MuonGeoModelTool();

	// Create mechanism
	virtual StatusCode create( StoreGateSvc* detStore );

private:
		MuonDetDescrManager* m_mgr;

};

#endif // _MUONGEOMODELTOOL_H
