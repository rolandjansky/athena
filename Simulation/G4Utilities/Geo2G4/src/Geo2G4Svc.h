/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4Svc_H
#define GEO2G4_Geo2G4Svc_H

#include "IGeo2G4Svc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

#include <string>
#include <map>

class VolumeBuilder;
typedef std::map< std::string, VolumeBuilder*,std::less<std::string> > BuilderMap;

class IAlgorithm;
class ISvcLocator;
class IIncident;
class IIncidentSvc;

//template <class TYPE> class SvcFactory;

class Geo2G4Svc: virtual public IGeo2G4Svc, virtual public IIncidentListener, public AthService
{
public:
  Geo2G4Svc(const std::string& , ISvcLocator *);
  virtual ~Geo2G4Svc();
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
  //protected:
  //friend class SvcFactory<Geo2G4Svc>;
private:
  IIncidentSvc* m_pIncSvc;
  VolumeBuilder *defaultBuilder;
  BuilderMap m_builders ;
  bool m_getTopTransform;
};

#endif
