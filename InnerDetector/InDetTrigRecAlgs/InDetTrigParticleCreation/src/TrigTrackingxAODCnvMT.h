/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: TrigTrackingxAODCnvMT.h
//
// author: Jiri Masik
//         Jiri.Masik@manchester.ac.uk
//
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGPARTICLECREATION_TRACKINGXAODCNVMT_H
#define INDETTRIGPARTICLECREATION_TRACKINGXAODCNVMT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"

#include <string>

namespace InDet
{

  class TrigTrackingxAODCnvMT : public AthReentrantAlgorithm
  {
  public:
    TrigTrackingxAODCnvMT(const std::string &name, ISvcLocator *pSvcLocator);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;

    SG::ReadHandleKey<TrackCollection> m_trackKey;
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey;

  };

}

#endif
