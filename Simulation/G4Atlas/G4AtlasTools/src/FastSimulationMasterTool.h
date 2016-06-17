/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_FASTSIMULATIONMASTERTOOL_H
#define G4ATLASSERVICES_FASTSIMULATIONMASTERTOOL_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"

// Members
#include "GaudiKernel/ToolHandle.h" // For tool handle array
#include "G4AtlasInterfaces/IFastSimulation.h" // Thing we have a handle on

class FastSimulationMasterTool : public AthAlgTool , public virtual IFastSimulationMasterTool {
 public:
  // Standard constructor and destructor
  FastSimulationMasterTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~FastSimulationMasterTool() {}

  // Gaudi methods
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;
  static const InterfaceID& interfaceID() { return IFastSimulationMasterTool::interfaceID(); }

  // Base class methods
  StatusCode initializeFastSims() override final; ///!< Base class method to initialize all the fast simulation models
  StatusCode EndOfAthenaEvent() override final; ///!< Base class method that calls EventOfAthenaEvent for all fast simulation models.

 private:
  ToolHandleArray<IFastSimulation> m_FastSimList; ///!< Private array of tool handles pointing to all fast simulations

};

#endif
