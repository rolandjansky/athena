// JtRecToolTrigger.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_NEGATIVEENERGYREJECTIONTOOL_H
#define JETREC_NEGATIVEENERGYREJECTIONTOOL_H

// P Sherwood March 2018

/// \class NegativeEnergyRejectionTool
///
/// Tool to select IParticles.

#include "TrigHLTJetRec/IIParticleRejectionTool.h"
#include "AsgTools/AsgTool.h"

class NegativeEnergyRejectionTool : 
  public asg::AsgTool, virtual public IIParticleRejectionTool{
  ASG_TOOL_CLASS(NegativeEnergyRejectionTool, IIParticleRejectionTool)
  
  public:
  
  NegativeEnergyRejectionTool(const std::string& name);
  virtual ~NegativeEnergyRejectionTool() override {}
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual bool operator()(const xAOD::IParticle*) override;

  virtual std::size_t tested() const override;
  virtual std::size_t rejected() const override;

private:
  bool  m_skipNegativeEnergy{true};

  std::size_t m_tested{0};
  std::size_t m_rejected{0};

};

#endif
