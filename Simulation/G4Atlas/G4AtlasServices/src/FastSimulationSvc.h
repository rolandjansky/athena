/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_FASTSIMULATIONSVC_H
#define G4ATLASSERVICES_FASTSIMULATIONSVC_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IFastSimulationSvc.h"

// Members
#include "GaudiKernel/ToolHandle.h" // For tool handle array
#include "G4AtlasInterfaces/IFastSimulation.h" // Thing we have a handle on

class FastSimulationSvc : public AthService , public virtual IFastSimulationSvc {
 public:
  // Standard constructor and destructor
  FastSimulationSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~FastSimulationSvc() {}

  // Gaudi methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID& interfaceID() { return IFastSimulationSvc::interfaceID(); }

 private:
  ToolHandleArray<IFastSimulation> m_FastSimList; ///!< Private array of tool handles pointing to all fast simulations

};

#endif