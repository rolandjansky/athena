/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IAGGD2GeoSvc_H
#define IAGGD2GeoSvc_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <vector>

#include "AGDD2Geo/AGDDDetectorStore.h"
#include "AGDD2Geo/AGDDDetector.h"

// Forward declarations
class IIncidentListener;
class Incident;
class GeoFullPhysVol;



// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IAGDD2GeoSvc("IAGDD2GeoSvc", 1 , 0);

#include <vector>
#include <string>

typedef std::vector<std::string>::const_iterator StrVecIterator;

class IAGDD2GeoSvc : virtual public IInterface {
public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IAGDD2GeoSvc; }
	
	virtual void Print()=0;
	
	virtual MsgStream GetMsgStream() const =0;
		// accessors
	virtual StrVecIterator GetFileBegin() const =0;
	virtual StrVecIterator GetFileEnd() const =0;
	virtual StrVecIterator GetSectionBegin() const =0;
	virtual StrVecIterator GetSectionEnd() const =0;
	virtual StrVecIterator GetVolumeBegin() const =0;
	virtual StrVecIterator GetVolumeEnd() const =0;
	virtual StrVecIterator GetStructureBegin() const =0;
	virtual StrVecIterator GetStructureEnd() const =0;
	
	virtual StatusCode SetTagInfo(std::string,std::string) const =0;
	
	virtual bool DisableSections() const =0;
	
	virtual bool IsLocked() const =0;

	virtual bool DumpAGDDString() const =0;
	
	virtual bool BuildMScomponents() const =0;

	virtual int PrintLevel() const = 0;

	virtual detectorList& GetDetectorList() const =0;
	virtual std::vector<AGDDDetector*> GetDetectorsByType(std::string) const =0;

};


#endif 



