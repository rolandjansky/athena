/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASTSIMULATION_DEADMATERIALSHOWERTOOL_H
#define G4FASTSIMULATION_DEADMATERIALSHOWERTOOL_H

#include "G4AtlasTools/FastSimulationBase.h"

class G4VFastSimulationModel;
class DeadMaterialShowerTool: public FastSimulationBase
{
 public:

  DeadMaterialShowerTool(const std::string& type, const std::string& name, const IInterface *parent);   //!< Default constructor
  ~DeadMaterialShowerTool() {}

protected:
  /** Method to make the actual fast simulation model itself, which
   will be owned by the tool.  Must be implemented in all concrete
   base classes. */
  virtual G4VFastSimulationModel* makeFastSimModel() override final;

 private:
  double m_highEnergy; ///!< Kill everything but muons up to this energy
  double m_lowEnergy; ///!< Kill only electrons and positrons up to this energy
  double m_zcutoff; ///!< Z-position to start killing everything but muons

};

#endif //G4FASTSIMULATION_DEADMATERIALSHOWERTOOL_H

