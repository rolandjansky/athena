// JtRecToolTrigger.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_EMTOPOREJECTIONTOOL_H
#define JETREC_EMTOPOREJECTIONTOOL_H

// P Sherwood March 2018

/// \class EMTopoRejectionTool
///
/// Tool to select IParticles.

#include "TrigHLTJetRec/IIParticleRejectionTool.h"
#include "AsgTools/AsgTool.h"

class EMTopoRejectionTool : 
  public asg::AsgTool, virtual public IIParticleRejectionTool{
  ASG_TOOL_CLASS(EMTopoRejectionTool, IIParticleRejectionTool)
  
  public:
  
  EMTopoRejectionTool(const std::string& name);
  virtual ~EMTopoRejectionTool() override {}
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual bool operator()(const xAOD::IParticle*) override;
  virtual std::size_t tested() const override;
  virtual std::size_t rejected() const override;

private:
  std::size_t m_tested{0};
  std::size_t m_rejected{0};


};

#endif
