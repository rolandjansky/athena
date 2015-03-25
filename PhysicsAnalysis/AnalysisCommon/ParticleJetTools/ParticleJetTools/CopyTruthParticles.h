/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYTRUTHPARTICLES_H
#define COPYTRUTHPARTICLES_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "xAODTruth/TruthParticle.h"


// Do I need IAsgTool? I need AsgTool for the eventStore()
class CopyTruthParticles : public IJetExecuteTool, public asg::AsgTool {
ASG_TOOL_INTERFACE(CopyTruthParticles)
ASG_TOOL_CLASS(CopyTruthParticles, IJetExecuteTool)
public:

  /// Constructor
  CopyTruthParticles(const std::string& name);

  /// @name Event loop algorithm methods
  //@{
  virtual int execute() const;
  //@}

  /// Classifier function(s)
  virtual bool classify(const xAOD::TruthParticle* tp) const = 0;


protected:

  /// Name of output collection
  std::string m_outputname;

  /// Minimum pT for particle selection (in MeV)
  double m_ptmin;

};


#endif
