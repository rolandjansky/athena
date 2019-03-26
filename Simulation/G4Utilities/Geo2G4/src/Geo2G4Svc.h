/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4Svc_H
#define GEO2G4_Geo2G4Svc_H

#include "G4AtlasInterfaces/IGeo2G4Svc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

#include "Geo2G4AssemblyFactory.h"
#include "Geo2G4AssemblyVolume.h"

#include <string>
#include <map>
#include <memory>

class VolumeBuilder;
typedef std::map< std::string, VolumeBuilder*,std::less<std::string> > BuilderMap;

/// @todo NEEDS DOCUMENTATION
class Geo2G4Svc: public extends<AthService, IGeo2G4Svc, IIncidentListener>
{
public:
  Geo2G4Svc(const std::string& , ISvcLocator *);
  virtual ~Geo2G4Svc();
  /// AthService methods
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  /// IIncidentListener methods -  FIXME does this service actually need to listen for Incidents?
  virtual void handle(const Incident&) override final;
  /// Geo2G4SvcBase methods
  virtual void RegisterVolumeBuilder(VolumeBuilder* vb) override final;
  virtual void UnregisterVolumeBuilder(VolumeBuilder* vb) override final;
  virtual void SetDefaultBuilder(VolumeBuilder *vb) override final {m_defaultBuilder=vb;}
  virtual void SetDefaultBuilder(std::string n) override final {this->SetDefaultBuilder(this->GetVolumeBuilder(n));}
  virtual VolumeBuilder* GetVolumeBuilder(std::string s) const override final;
  virtual VolumeBuilder* GetDefaultBuilder() const override final {return m_defaultBuilder;}
  virtual bool UseTopTransforms() const override final {return m_getTopTransform;}
  virtual void ListVolumeBuilders() const override final;
private:
  VolumeBuilder *m_defaultBuilder;
  BuilderMap m_builders ;
  bool m_getTopTransform;
  std::unique_ptr<Geo2G4AssemblyFactory> m_G4AssemblyFactory;
};

#endif
