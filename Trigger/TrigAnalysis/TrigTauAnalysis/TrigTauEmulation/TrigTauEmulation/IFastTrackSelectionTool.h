/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFASTTRACKSELECTIONTOOL_FASTTRACKSELECTIONTOOL_H
#define IFASTTRACKSELECTIONTOOL_FASTTRACKSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

//EDM include
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTracking/TrackParticle.h"

class IFastTrackSelectionTool : public virtual asg::IAsgTool
{

   ASG_TOOL_INTERFACE(IFastTrackSelectionTool)
    
 public:

  virtual const asg::AcceptInfo& getAcceptInfo() const = 0;
  virtual asg::AcceptData accept(const xAOD::TauJet * hlttau, const DataVector<xAOD::TrackParticle> *preselTracksIso, const DataVector<xAOD::TrackParticle> *preselTracksCore) const = 0;
  virtual asg::AcceptData accept(const xAOD::TauJet * presel_tau) const = 0;

  virtual ~IFastTrackSelectionTool() {};


};

#endif
