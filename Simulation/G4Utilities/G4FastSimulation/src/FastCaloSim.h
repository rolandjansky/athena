/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASTSIMULATION_FASTCALOSIM_H
#define G4FASTSIMULATION_FASTCALOSIM_H

// Service handle to define smart pointer to FastCaloSim parametrisation service
#include "GaudiKernel/ServiceHandle.h"
// Tool handle to define smart pointer to FastCaloSim extrapolation tool
#include "GaudiKernel/ToolHandle.h"
// Geant4 fast simulation base class
#include "G4VFastSimulationModel.hh"
// FastCaloSim parametrisation service interface
#include "ISF_FastCaloSimInterfaces/IFastCaloSimParamSvc.h"
// FastCaloSim extrapolation tool interface
#include "ISF_FastCaloSimParametrization/IFastCaloSimCaloExtrapolation.h"
// Random generator service interface
#include "AthenaKernel/IAthRNGSvc.h"
// FastCaloSim tool
#include "FastCaloSimTool.h"


class CaloCellContainerSD;

class FastCaloSim: public G4VFastSimulationModel
{
 public:

  FastCaloSim(const std::string& name, 
  ServiceHandle<IAthRNGSvc> rndmGenSvc, Gaudi::Property<std::string> randomEngineName,
  PublicToolHandle<IFastCaloSimCaloExtrapolation> FastCaloSimCaloExtrapolation, ServiceHandle<ISF::IFastCaloSimParamSvc> FastCaloSimSvc,
  Gaudi::Property<std::string> m_CaloCellContainerSDName,
  FastCaloSimTool * FastCaloSimTool);
  ~FastCaloSim() {}

  G4bool IsApplicable(const G4ParticleDefinition&) override final;
  void DoIt(const G4FastTrack&, G4FastStep&) override final;
  void StartOfAthenaEvent(const EventContext& ctx);

  /** Determines the applicability of the fast sim model to this particular track.
  Checks that geometric location, energy, and particle type are within bounds.  Also checks for
  containment of the particle's shower within a specific detector region. **/
  G4bool ModelTrigger(const G4FastTrack &) override final;

  /** Retrieves the associated sensitive detector responsible for writing out the CaloCellContainer **/
  CaloCellContainerSD * getCaloCellContainerSD();

  /** Check if the particle is located at the proper ID-Calo parametrization boundary and is travelling outwards from the ID to the CALO **/
  G4bool passedIDCaloBoundary(const G4FastTrack& fastTrack);
  
 private:

  // Random generator services
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc;
  Gaudi::Property<std::string> m_randomEngineName;
  ATHRNG::RNGWrapper* m_rngWrapper;
  // FastCaloSimCaloExtrapolation tool to extrapolate particle shower positions to layers
  PublicToolHandle<IFastCaloSimCaloExtrapolation> m_FastCaloSimCaloExtrapolation;
  // Main FastCaloSim service
  ServiceHandle<ISF::IFastCaloSimParamSvc> m_FastCaloSimSvc;
  // Name of associated CaloCellContainer sensitive detector
  Gaudi::Property<std::string> m_CaloCellContainerSDName;
  // Fast simulation FastCaloSimTool 
  FastCaloSimTool * m_FastCaloSimTool;
};

#endif //G4FASTSIMULATION_FASTCALOSIM_H

