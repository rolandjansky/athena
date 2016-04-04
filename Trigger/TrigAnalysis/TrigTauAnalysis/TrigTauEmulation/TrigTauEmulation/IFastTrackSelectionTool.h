/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFASTTRACKSELECTIONTOOL_FASTTRACKSELECTIONTOOL_H
#define IFASTTRACKSELECTIONTOOL_FASTTRACKSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

//EDM include
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTracking/TrackParticle.h"

class IFastTrackSelectionTool : public virtual asg::IAsgTool
{

   ASG_TOOL_INTERFACE(IFastTrackSelectionTool)
    
 public:

  virtual const Root::TAccept& accept(const xAOD::TauJet * hlttau, const DataVector<xAOD::TrackParticle> *preselTracksIso, const DataVector<xAOD::TrackParticle> *preselTracksCore) const = 0;
  virtual const Root::TAccept& accept(const xAOD::TauJet * presel_tau) const = 0;

  virtual ~IFastTrackSelectionTool() {};


};

#endif
