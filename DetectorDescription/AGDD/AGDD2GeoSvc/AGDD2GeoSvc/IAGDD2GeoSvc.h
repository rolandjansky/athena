/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAGGDtoGeoSvc_H
#define IAGGDtoGeoSvc_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <vector>

#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/AGDDDetector.h"

// Forward declarations
class IIncidentListener;
class Incident;
class GeoFullPhysVol;

class XMLHandler;



// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IAGDDtoGeoSvc("IAGDDtoGeoSvc", 1 , 0);

#include <vector>
#include <string>

typedef std::vector<std::string>::const_iterator StrVecIterator;

class IAGDDtoGeoSvc : virtual public IInterface {
public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IAGDDtoGeoSvc; }
	
	// accessors
	
	virtual void addHandler(XMLHandler*) = 0;

};


#endif 



