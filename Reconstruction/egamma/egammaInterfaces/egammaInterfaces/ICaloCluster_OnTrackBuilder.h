/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICALOCLUSTER_ONTRACKBUILER_H
#define ICALOCLUSTER_ONTRACKBUILER_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaFwd.h"

namespace Trk {
class CaloCluster_OnTrack;
}
static const InterfaceID
  IID_caloCluster_OnTrackBuilder("ICaloCluster_OnTrackBuilder", 1, 0);

class ICaloCluster_OnTrackBuilder : virtual public IAlgTool
{
public:
  static const InterfaceID& interfaceID()
  {
    return IID_caloCluster_OnTrackBuilder;
  };

  ~ICaloCluster_OnTrackBuilder(){};

  virtual Trk::CaloCluster_OnTrack* buildClusterOnTrack(
    const EventContext& ctx,
    const xAOD::CaloCluster* cl,
    int charge = 0) const = 0;
};

#endif // ICALOCLUSTER_ONTRACKBUILER_H
