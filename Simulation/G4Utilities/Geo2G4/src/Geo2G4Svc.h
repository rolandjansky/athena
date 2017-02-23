/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4Svc_H
#define GEO2G4_Geo2G4Svc_H

#include "G4AtlasInterfaces/IGeo2G4Svc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

#include <string>
#include <map>

class VolumeBuilder;
typedef std::map< std::string, VolumeBuilder*,std::less<std::string> > BuilderMap;

/// @todo NEEDS DOCUMENTATION
class Geo2G4Svc: virtual public IGeo2G4Svc, virtual public IIncidentListener, public AthService
{
public:
  Geo2G4Svc(const std::string& , ISvcLocator *);
  virtual ~Geo2G4Svc();
  /// AthService methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;
  virtual StatusCode queryInterface(const InterfaceID& , void** ppvInterface ) override final;
  /// IIncidentListener methods -  FIXME does this service actually need to listen for Incidents?
  void handle(const Incident&) override final;
  /// Geo2G4SvcBase methods
  virtual void RegisterVolumeBuilder(VolumeBuilder* vb) override final;
  virtual void UnregisterVolumeBuilder(VolumeBuilder* vb) override final;
  void SetDefaultBuilder(VolumeBuilder *vb) override final {m_defaultBuilder=vb;}
  void SetDefaultBuilder(std::string n) override final {this->SetDefaultBuilder(this->GetVolumeBuilder(n));}
  VolumeBuilder* GetVolumeBuilder(std::string s) const override final;
  VolumeBuilder* GetDefaultBuilder() const override final {return m_defaultBuilder;}
  bool UseTopTransforms() const override final {return m_getTopTransform;}
  void ListVolumeBuilders() const override final;
private:
  IIncidentSvc* m_pIncSvc; // not used - remove?
  VolumeBuilder *m_defaultBuilder;
  BuilderMap m_builders ;
  bool m_getTopTransform;
};

#endif
