/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETDETDESCR_MUONDETDESCRTOOL_H
#define INDETDETDESCR_MUONDETDESCRTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"

class MuonDetDescrManager;
class StoreGateSvc;

class MuonDetDescrTool : public GeoModelTool
{
public:

    // Standard Constructor

    MuonDetDescrTool(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    // Standard Destructor

    virtual ~MuonDetDescrTool();

    // Standard Tool Methods

    virtual StatusCode create(StoreGateSvc* detStore);

private:

    MuonDetDescrManager* m_mgr;
	bool                 m_debugPrint;

};

#endif // INDETDETDESCR_MUONDETDESCRTOOL_H
