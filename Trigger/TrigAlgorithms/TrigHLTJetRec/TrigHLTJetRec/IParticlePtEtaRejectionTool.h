// JtRecToolTrigger.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_IPARTICLEPTETAREJECTIONTOOL_H
#define JETREC_IPARTICLEPTETAREJECTIONTOOL_H

// P Sherwood March 2018

/// \class TriggerJetBuildTool
///
/// Tool to select IParticles.

#include "TrigHLTJetRec/IIParticleRejectionTool.h"
#include "AsgTools/AsgTool.h"

class IParticlePtEtaRejectionTool : 
  public asg::AsgTool, virtual public IIParticleRejectionTool{
  ASG_TOOL_CLASS(IParticlePtEtaRejectionTool, IIParticleRejectionTool)
  
  public:
  
  IParticlePtEtaRejectionTool(const std::string& name);
  virtual ~IParticlePtEtaRejectionTool() override {}
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual bool operator()(const xAOD::IParticle*) override;

  virtual std::size_t tested() const override;
  virtual std::size_t rejected() const override;
  
private:
  double m_ptMin;
  double m_absEtaMax;

  std::size_t m_tested{0};
  std::size_t m_rejected{0};

};

#endif
