/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJetUpdateJvt_H
#define IJetUpdateJvt_H

/// \class IJetUpdateJvt
/// \author David Adams
/// \date June 2015
///
/// Class interface for a tool that updates JVT.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

class IJetUpdateJvt : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetUpdateJvt)

public:

  /// Calculate the updated JVT.
  ///   jet - jet for which JVT is updated
  /// The new value for JVT is returned.
  virtual float updateJvt(const xAOD::Jet& jet) const =0;

};

#endif
