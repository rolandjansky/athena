// -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ACTSSIMTOOL_h
#define ISF_ACTSSIMTOOL_h

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"  // for ToolHandleArray

// ISF
#include "ISF_Interfaces/BaseSimulatorTool.h"
#include "ISF_Interfaces/IParticleFilter.h"
#include "ISF_Interfaces/IParticleProcessor.h"

namespace ISF {

class ActsSimTool : public BaseSimulatorTool {
 public:
  ActsSimTool(const std::string& type, const std::string& name,
              const IInterface* parent);

  ~ActsSimTool();

  virtual StatusCode initialize() override;

  virtual StatusCode simulate(const ISFParticle& isp, ISFParticleContainer&,
                              McEventCollection*) const override;

  virtual StatusCode setupEvent() override { return StatusCode::SUCCESS; };

  virtual StatusCode releaseEvent() override { return StatusCode::SUCCESS; };

  virtual ISF::SimulationFlavor simFlavor() const override {
    return ISF::Fatras;
  };

 private:
  /** Track Creation & transport */
  PublicToolHandle<ISF::IParticleProcessor> m_IDsimulationTool{
      this, "IDSimulationTool", "", ""};  //!< Pointer to the transport AlgTool
  bool m_useExtrapolator{
      true};  //!< Boolean used to run with the old extrapolator setup
  PublicToolHandle<ISF::IParticleProcessor> m_simulationTool{
      this, "SimulationTool", "", ""};  //!< Pointer to the transport AlgTool
  PublicToolHandle<ISF::IParticleFilter> m_particleFilter{
      this, "ParticleFilter", "",
      ""};  //!< the particle filter concerning kinematic cuts, etc.
};

}  // namespace ISF

#endif
