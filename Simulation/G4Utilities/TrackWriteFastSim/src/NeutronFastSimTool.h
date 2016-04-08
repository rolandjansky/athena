/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKWRITEFASTSIM_NeutronFastSimTool_h
#define TRACKWRITEFASTSIM_NeutronFastSimTool_h

#include "G4AtlasTools/FastSimulationBase.h"

#include "GaudiKernel/ToolHandle.h"
#include "G4AtlasInterfaces/ISensitiveDetector.h" // What we have a handle on

class G4VFastSimulationModel;

class NeutronFastSimTool: public FastSimulationBase
{
public:

  NeutronFastSimTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~NeutronFastSimTool() {}
  StatusCode initialize() override final;
protected:
  /** Method to make the actual fast simulation model itself, which
   will be owned by the tool.  Must be implemented in all concrete
   base classes. */
  virtual G4VFastSimulationModel* makeFastSimModel() override final;
private:
  ToolHandle<ISensitiveDetector> m_trackFastSimSDTool;
  double m_etaCut; ///!< Eta cut for primaries
  double m_timeCut; ///!< Time cut for neutrons
};

#endif // TRACKWRITEFASTSIM_NeutronFastSimTool_h
