/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetConsumer.h

#ifndef IJetConsumer_H
#define IJetConsumer_H

/// \class IJetConsumer
/// \author David Adams
/// \date November 2014
///
/// Class interface for tools that make use of a jet container.
/// One anticipated use is to transfer the jet pseudojets to
/// another container.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

class IJetConsumer : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetConsumer)

public:

  /// Destructor.
  virtual ~IJetConsumer();

  /// Copy the pseudojets from srcjets to dstjets.
  /// Return 0 for success.
  virtual int process(const xAOD::JetContainer& srcjets) const =0;

};

#endif
