/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASTSIMULATION_SIMPLEFASTKILLERTOOL_H
#define G4FASTSIMULATION_SIMPLEFASTKILLERTOOL_H

#include "G4AtlasTools/FastSimulationBase.h"

class G4VFastSimulationModel;

class SimpleFastKillerTool: public FastSimulationBase
{
 public:
  SimpleFastKillerTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~SimpleFastKillerTool() {}

protected:
  /** Method to make the actual fast simulation model itself, which
   will be owned by the tool.  Must be implemented in all concrete
   base classes. */
  virtual G4VFastSimulationModel* makeFastSimModel() override final;
};

#endif //G4FASTSIMULATION_SIMPLEFASTKILLERTOOL_H
