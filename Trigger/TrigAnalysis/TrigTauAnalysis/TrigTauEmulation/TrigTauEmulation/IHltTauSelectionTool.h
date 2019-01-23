/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef IHLTTAUSELECTIONTOOL_HLTTAUSELECTIONTOOL_H
#define IHLTTAUSELECTIONTOOL_HLTTAUSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

//EDM include
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTracking/TrackParticle.h"

#include "TrigTauEmulation/ISelectionTool.h"
#include "TrigTauEmulation/DecoratedHltTau.h"

class IHltTauSelectionTool : public virtual ISelectionTool
{
  ASG_TOOL_INTERFACE(IHltTauSelectionTool)

  public:

    virtual const asg::AcceptInfo& getAcceptInfo() const = 0;

    virtual asg::AcceptData accept(const DecoratedHltTau& hlttau) const = 0; 
    virtual asg::AcceptData accept(const xAOD::TauJet * hlttau, const DataVector<xAOD::TrackParticle> *preselTracksIso, const DataVector<xAOD::TrackParticle> *preselTracksCore) const = 0;
    virtual asg::AcceptData accept(const xAOD::TauJet * hlttau, const xAOD::TauJetContainer* presel_taus) const = 0;

    virtual ~IHltTauSelectionTool() {};

};

#endif
