/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKWRITEFASTSIM_NeutronFastSimTool_h
#define TRACKWRITEFASTSIM_NeutronFastSimTool_h

#include "G4AtlasTools/FastSimulationBase.h"

#include <string>

class G4VFastSimulationModel;

class NeutronFastSimTool: public FastSimulationBase
{
public:

  NeutronFastSimTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~NeutronFastSimTool() {}
protected:
  /** Method to make the actual fast simulation model itself, which
   will be owned by the tool.  Must be implemented in all concrete
   base classes. */
  virtual G4VFastSimulationModel* makeFastSimModel() override final;
private:
  std::string m_trackFastSimSDName;
  double m_etaCut; ///!< Eta cut for primaries
  double m_timeCut; ///!< Time cut for neutrons
};

#endif // TRACKWRITEFASTSIM_NeutronFastSimTool_h
