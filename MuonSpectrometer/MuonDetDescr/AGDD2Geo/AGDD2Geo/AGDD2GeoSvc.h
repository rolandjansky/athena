/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDD2GeoSvc_H
#define AGDD2GeoSvc_H

#include "AGDD2Geo/IAGDD2GeoSvc.h"
#include "AGDD2Geo/XMLHandler.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>

class ISvcLocator;
class IToolSvc;
class ITagInfoMgr;
class IRDBAccessSvc;
class IGeoModelSvc;
class GeoFullPhysVol;

namespace MuonGM{
  class MuonDetectorManager;
  }

template <class TYPE> class SvcFactory;

class AGDD2GeoSvc : public Service, virtual public IAGDD2GeoSvc {

public:

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Query the interfaces.
    //   Input: riid, Requested interface ID
    //          ppvInterface, Pointer to requested interface
    //   Return: StatusCode indicating SUCCESS or FAILURE.
    // N.B. Don't forget to release the interface after use!!!
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

	MsgStream GetMsgStream() const {return log;}

	// accessors
	StrVecIterator GetFileBegin() const { return m_xmlFiles.begin();}
	StrVecIterator GetFileEnd() const { return m_xmlFiles.end();}
	StrVecIterator GetSectionBegin() const { return m_sectionsToBuild.begin();}
	StrVecIterator GetSectionEnd() const { return m_sectionsToBuild.end();}
	StrVecIterator GetVolumeBegin() const { return m_volumesToBuild.begin();}
	StrVecIterator GetVolumeEnd() const { return m_volumesToBuild.end();}
	StrVecIterator GetStructureBegin() const
	{
		if (m_locked) return m_structuresFromFlags.begin();
		else return m_structuresToBuild.begin();
	}
	StrVecIterator GetStructureEnd() const 
	{ 
		if (m_locked) return m_structuresFromFlags.end();
		else return m_structuresToBuild.end();
	}
	
	StatusCode SetTagInfo(std::string,std::string) const;
	
	bool DisableSections() const {return m_disableSections;}
	
	void ReadAGDDFlags();
	
	bool IsLocked() const {return m_locked;}

	bool DumpAGDDString() const {return m_dumpAGDD;}

	std::map<std::string, GeoFullPhysVol*>* GetMSdetectors() const;
	bool BuildMScomponents() const;
	bool BuildReadoutGeometry(MuonGM::MuonDetectorManager* mgr/*, std::map<std::string, GeoFullPhysVol*>* */) const;

protected:

	friend class SvcFactory<AGDD2GeoSvc>;

    // Standard Constructor
    AGDD2GeoSvc(const std::string& name, ISvcLocator* svc);

    // Standard Destructor
    virtual ~AGDD2GeoSvc(); 
	
	void Print();
	
	void addHandler(XMLHandler* v) { handlerVector.push_back(v);}
	
	MsgStream log;
	
private:

	std::vector<std::string> m_xmlFiles;
	std::vector<std::string> m_sectionsToBuild;
	std::vector<std::string> m_volumesToBuild;
	std::vector<std::string> m_structuresToBuild;
	std::vector<std::string> m_structuresFromFlags;
	//std::vector<GeoFullPhysVol*>* m_detectors;

	int m_parserVerbosity;
	int m_builderVerbosity;
	bool m_readAGDD;
	bool m_printSections;
	bool m_disableSections;
	bool m_locked;
	bool m_dumpAGDD;
	bool m_overrideConfiguration;
	std::string m_defaultDetector;
	std::string m_navigateDetector;
	int m_printLevel;
	
	void localInitialization();
	
	ITagInfoMgr* m_tagInfoMgr;
	
	IRDBAccessSvc* p_RDBAccessSvc;
	IGeoModelSvc* p_GeoModelSvc;
	
	std::vector<XMLHandler*> handlerVector;
	
	int PrintLevel() const {return m_printLevel;}
};

#endif
