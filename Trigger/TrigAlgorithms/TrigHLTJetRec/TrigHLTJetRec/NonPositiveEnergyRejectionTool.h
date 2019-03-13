// JtRecToolTrigger.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_NEGATIVEENERGYREJECTIONTOOL_H
#define JETREC_NEGATIVEENERGYREJECTIONTOOL_H

// P Sherwood March 2018

/// \class NonPositiveEnergyRejectionTool
///
/// Tool to select IParticles.

#include "TrigHLTJetRec/IIParticleRejectionTool.h"
#include "AsgTools/AsgTool.h"

class NonPositiveEnergyRejectionTool : 
  public asg::AsgTool, virtual public IIParticleRejectionTool{
  ASG_TOOL_CLASS(NonPositiveEnergyRejectionTool, IIParticleRejectionTool)
  
  public:
  
  NonPositiveEnergyRejectionTool(const std::string& name);
  virtual ~NonPositiveEnergyRejectionTool() override {}
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual bool operator()(const xAOD::IParticle*) override;

  virtual std::size_t tested() const override;
  virtual std::size_t rejected() const override;

private:

  std::size_t m_tested{0};
  std::size_t m_rejected{0};

};

#endif
