/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetUpdateJvt.h

#ifndef IJetUpdateJvt_H
#define IJetUpdateJvt_H

/// \class IJetUpdateJvt
/// \author David Adams
/// \date June 2015
///
/// Class interface for a toool that updates JVT.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

class IJetUpdateJvt : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetUpdateJvt)

public:

  /// Calculate the updated JVT.
  ///   jet - jet for which JVT is updated
  ///   sjvt - name of the existing JVT moment (and prefix for RpT and JVFcorr).
  ///   scale - name of the jet scale holding the original pT
  /// The new value for JVT is returned.
  virtual float updateJvt(const xAOD::Jet& jet) const =0;

};

#endif
