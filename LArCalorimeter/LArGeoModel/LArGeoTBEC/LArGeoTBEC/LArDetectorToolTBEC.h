/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOMODEL_LARDETECTORTOOLTBEC_H
#define LARGEOMODEL_LARDETECTORTOOLTBEC_H

#include "GeoModelUtilities/GeoModelTool.h"

class LArDetectorToolTBEC : public GeoModelTool {

public:

    // Standard Constructor
    LArDetectorToolTBEC( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~LArDetectorToolTBEC();

    // Create Method:
    virtual StatusCode create( StoreGateSvc* detStore );
	
 private:
    
    int  m_ecVisLimit;

};

#endif 
