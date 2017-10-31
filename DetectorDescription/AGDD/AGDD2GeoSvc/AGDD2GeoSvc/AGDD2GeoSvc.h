/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDD2GeoSvc_H
#define AGDD2GeoSvc_H

#include "AGDD2GeoSvc/IAGDD2GeoSvc.h"
#include "AGDDControl/XMLHandler.h"
#include "AthenaBaseComps/AthService.h"

#include "AGDDKernel/AGDDDetectorStore.h"

#include <vector>

class ISvcLocator;
class IToolSvc;
class ITagInfoMgr;
class IRDBAccessSvc;
class IGeoModelSvc;
class GeoFullPhysVol;

template <class TYPE> class SvcFactory;

class AGDDtoGeoSvc : public AthService, virtual public IAGDDtoGeoSvc {

public:

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Query the interfaces.
    //   Input: riid, Requested interface ID
    //          ppvInterface, Pointer to requested interface
    //   Return: StatusCode indicating SUCCESS or FAILURE.
    // N.B. Don't forget to release the interface after use!!!
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
		
	void addHandler(XMLHandler* v) { m_handlerVector.push_back(v);}
	
	friend class SvcFactory<AGDDtoGeoSvc>;

    // Standard Constructor
    AGDDtoGeoSvc(const std::string& name, ISvcLocator* svc);

    // Standard Destructor
    virtual ~AGDDtoGeoSvc(); 
	
private:
	
	void localInitialization();
	
	ITagInfoMgr* m_tagInfoMgr;
	
	std::vector<XMLHandler*> m_handlerVector;
	
	std::vector<std::string> m_builders;

//	int PrintLevel() const {return m_printLevel;}
};

#endif
