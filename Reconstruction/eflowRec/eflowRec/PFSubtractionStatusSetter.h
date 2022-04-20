/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFSUBTRACTIONSTATUSSETTER_H
#define PFSUBTRACTIONSTATUSSETTER_H

#include "AsgMessaging/AsgMessaging.h"

#include "xAODCaloEvent/CaloCluster.h"
class eflowCaloObject;

#include <vector>
#include <utility>

/** This class contains a few functions to set the amount of energy removed from a xAOD::CaloCluster by a xAOD::Track in eflowRec.
It can either set this for a specific track attached to an eflowCaloObject or set it for all tracks in an eflowCaloObject **/

class PFSubtractionStatusSetter : public asg::AsgMessaging
{

public:
  PFSubtractionStatusSetter();

  /** Set the ratio of new to old cluster energy for each cluster matched to a
   * track with trackIndex */
  void markSubtractionStatus(
    const std::vector<std::pair<xAOD::CaloCluster*, bool>>& clusterList,
    std::vector<std::pair<float, float>>& clusterSubtractedEnergyRatios,
    eflowCaloObject& thisEflowCaloObject,
    unsigned int trackIndex) const;
  /** Set the ratio of new to old cluster energy, to zero, for all cluster
   * matched to all tracks attached to the eflowCaloObject */
  void markAllTracksAnnihStatus(eflowCaloObject& thisEflowCaloObject) const;
};
#endif
