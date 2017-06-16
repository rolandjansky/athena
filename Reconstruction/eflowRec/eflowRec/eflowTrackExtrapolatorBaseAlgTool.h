/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTRACKEXTRAPOLATORBASEALGTOOL_H
#define EFLOWTRACKEXTRAPOLATORBASEALGTOOL_H

/********************************************************************

NAME:     eflowTrackExtrapolatorBaseAlgTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, T.Velz
CREATED:  24th January, 2005

********************************************************************/

#include "GaudiKernel/IAlgTool.h"

#include "eflowRec/eflowCaloRegions.h"
#include "xAODTracking/TrackParticle.h"

class eflowTrackCaloPoints;

class eflowTrackExtrapolatorBaseAlgTool : virtual public IAlgTool {
 public:
  virtual std::unique_ptr<eflowTrackCaloPoints> execute(const xAOD::TrackParticle* track) const = 0;
};

#endif
