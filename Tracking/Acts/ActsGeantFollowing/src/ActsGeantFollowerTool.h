/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEANTFOLLOWERTOOL_H
#define ACTSGEANTFOLLOWERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "ActsGeantFollower.h"


/// @class ActsGeantFollowerTool
/// @brief Tool which manages the ActsGeantFollower action
///
class ActsGeantFollowerTool : public G4UA::UserActionToolBase<ActsGeantFollower>
{

public:
  /// Standard constructor
  ActsGeantFollowerTool(const std::string& type, const std::string& name,const IInterface* parent);
protected:
  /// Create an action for this thread
  virtual std::unique_ptr<ActsGeantFollower>
  makeAndFillAction(G4UA::G4AtlasUserActions&) override final;
}; // class ActsGeantFollowerTool

#endif
