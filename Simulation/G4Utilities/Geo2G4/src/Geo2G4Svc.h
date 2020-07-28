/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4Svc_H
#define GEO2G4_Geo2G4Svc_H

#include "G4AtlasInterfaces/IGeo2G4Svc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

#include "Geo2G4AssemblyFactory.h"

#include "VolumeBuilder.h"

#include <string>
#include <map>
#include <memory>

typedef std::unordered_map<std::string, std::unique_ptr<VolumeBuilder>> BuilderMap;

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
  virtual void SetDefaultBuilder(std::string n) override final {m_defaultBuilder=n;}
  virtual VolumeBuilder* GetVolumeBuilder(std::string s) const override final;
  virtual VolumeBuilder* GetDefaultBuilder() const override final {return m_builders.at(m_defaultBuilder).get();}
  virtual bool UseTopTransforms() const override final {return m_getTopTransform;}
  virtual void ListVolumeBuilders() const override final;
private:
  std::string m_defaultBuilder;
  BuilderMap m_builders;
  bool m_getTopTransform;
  std::unique_ptr<Geo2G4AssemblyFactory> m_G4AssemblyFactory;
};

#endif
