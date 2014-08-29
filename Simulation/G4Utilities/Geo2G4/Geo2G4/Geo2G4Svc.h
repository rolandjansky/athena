/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Geo2G4Svc_H
#define Geo2G4Svc_H

#include "Geo2G4/IGeo2G4Svc.h"
#include "Geo2G4/Geo2G4SvcBase.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"

//  #include "Geo2G4/VolumeBuilder.h"
#include <string>
#include <map>

class VolumeBuilder;
typedef std::map< std::string, VolumeBuilder*,std::less<std::string> > BuilderMap;

class IAlgorithm;
class ISvcLocator;
class IIncident;
class IIncidentSvc;
#include "GaudiKernel/MsgStream.h"

template <class TYPE> class SvcFactory;

class Geo2G4Svc: virtual public IGeo2G4Svc, virtual public IIncidentListener,
	         virtual public Geo2G4SvcBase, public Service
{
public:
	StatusCode initialize();
	StatusCode finalize();
	virtual StatusCode queryInterface(const InterfaceID& , void** ppvInterface );

	void handle(const Incident&);

	virtual void RegisterVolumeBuilder(VolumeBuilder* vb);
	virtual void UnregisterVolumeBuilder(VolumeBuilder* vb);
	void SetDefaultBuilder(VolumeBuilder *vb) {defaultBuilder=vb;}
	void SetDefaultBuilder(std::string n) {SetDefaultBuilder(GetVolumeBuilder(n));}
	VolumeBuilder* GetVolumeBuilder(std::string s);
	VolumeBuilder* GetDefaultBuilder() {return defaultBuilder;}
	bool UseTopTransforms() {return m_getTopTransform;}
	void ListVolumeBuilders();
protected:
	Geo2G4Svc(const std::string& , ISvcLocator *);
	virtual ~Geo2G4Svc();
	friend class SvcFactory<Geo2G4Svc>;
private:
	MsgStream log;
	IIncidentSvc* m_pIncSvc;
	VolumeBuilder *defaultBuilder;
	BuilderMap m_builders ;
	bool m_getTopTransform;
};

#endif
