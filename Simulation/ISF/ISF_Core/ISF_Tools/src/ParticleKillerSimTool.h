// -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_PARTICLEKILLERSIMTOOL_h
#define ISF_PARTICLEKILLERSIMTOOL_h

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"  // for ToolHandleArray

//Athena
#include "AthenaBaseComps/AthAlgTool.h"

// ISF
#include "ISF_Interfaces/BaseSimulatorTool.h"

namespace ISF {

class ParticleKillerSimTool : public BaseSimulatorTool  {
public:
  ParticleKillerSimTool( const std::string& type, const std::string& name,  const IInterface* parent);

  ~ParticleKillerSimTool();

  virtual StatusCode initialize() override;

  virtual StatusCode simulate( const ISFParticle& isp, ISFParticleContainer&, McEventCollection* ) const override;

  /** */
  virtual StatusCode simulateVector(const ConstISFParticleVector& particles, ISFParticleContainer&, McEventCollection* ) const override;

  virtual StatusCode setupEvent(const EventContext&) override { return StatusCode::SUCCESS; };

  virtual StatusCode releaseEvent(const EventContext&) override { return StatusCode::SUCCESS; };

  virtual ISF::SimulationFlavor simFlavor() const override { return ISF::ParticleKiller; };
};

}

#endif
