/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSREFITTERALG_H
#define ACTSGEOMETRY_ACTSREFITTERALG_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"
#include "TrkTrack/TrackCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// PACKAGE
#include "ActsGeometry/ATLASSourceLink.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>

class EventContext;
namespace Trk {
  class ITrackFitter;
}

class ActsReFitterAlg : public AthReentrantAlgorithm {
public:
  ActsReFitterAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

private:

  ToolHandle<Trk::ITrackFitter> m_actsKalmanFitter{this, "ActsKalmanFitter", "ActsKalmanFitter", "the Acts Kalman Fitter"};
  // --- job options
  SG::ReadHandleKey<TrackCollection>  m_trackName{this, "TrackName", "Tracks", "Collection name for tracks to be refitted"};
  SG::WriteHandleKey<TrackCollection> m_newTrackName{this, "NewTrackName", "ReFitted_Tracks", "Collection name for output tracks"};

  mutable std::mutex m_writeMutex{};

};

#endif 
