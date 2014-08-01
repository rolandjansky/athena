/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTRACKEXTRAPOLATORBASEALGTOOL_H
#define EFLOWTRACKEXTRAPOLATORBASEALGTOOL_H

/********************************************************************

NAME:     eflowTrackExtrapolatorBaseAlgTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  24th January, 2005

********************************************************************/

namespace Rec{
  class TrackParticle;
}

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "xAODTracking/TrackParticle.h"

class eflowTrackExtrapolatorBaseAlgTool : virtual public IAlgTool {

 public:

  virtual StatusCode intialize() {return StatusCode::SUCCESS;};
  virtual Amg::Vector3D execute(const xAOD::TrackParticle* track) = 0;
  virtual std::vector<Amg::Vector3D > execute(const xAOD::TrackParticle* track, const std::vector<CaloCell_ID::CaloSample>& samples) = 0;
  virtual bool isInBarrel(const xAOD::TrackParticle* track) = 0;
  virtual StatusCode finalize() {return StatusCode::SUCCESS;};

  virtual StatusCode setGeometry(CaloCell_ID::CaloSample subCalo) = 0 ;

};


#endif
