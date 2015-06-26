/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IHLTTAUSELECTIONTOOL_HLTTAUSELECTIONTOOL_H
#define IHLTTAUSELECTIONTOOL_HLTTAUSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

//EDM include
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTracking/TrackParticle.h"

class IHltTauSelectionTool : public virtual asg::IAsgTool
{

   ASG_TOOL_INTERFACE(IHltTauSelectionTool)
    
 public:


  virtual const Root::TAccept& accept(const xAOD::TauJet * hlttau, const DataVector<xAOD::TrackParticle> * tracks) const = 0;
  virtual const Root::TAccept& accept(const xAOD::TauJet * hlttau, const xAOD::TauJetContainer* presel_taus) const = 0;

  virtual ~IHltTauSelectionTool() {};


};

#endif
