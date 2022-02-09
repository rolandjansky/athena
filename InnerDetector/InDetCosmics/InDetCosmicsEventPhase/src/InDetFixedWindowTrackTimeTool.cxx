/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// InDetFixedWindowTrackTimeTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetCosmicsEventPhase/InDetFixedWindowTrackTimeTool.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"

#include "InDetIdentifier/TRT_ID.h"
#include "StoreGate/ReadCondHandle.h"
//================ Constructor =================================================

InDet::InDetFixedWindowTrackTimeTool::InDetFixedWindowTrackTimeTool(std::string const& t
                                                                    , std::string const& n
                                                                    , IInterface const* p
                                                                    ) : AthAlgTool(t, n, p)
  , m_caldbtool("TRT_CalDbTool", this) {
  declareInterface<IInDetCosmicsEventPhaseTool>(this);
  declareProperty("TRTCalDbTool", m_caldbtool);
  declareProperty("GlobalOffset", m_globalOffset = 10.);
  declareProperty("UseNewEP", m_useNewEP = true);
  declareProperty("WindowCenter", m_windowCenter = -8.5);
  declareProperty("WindowSize", m_windowSize = 7.);
}

//================ Destructor =================================================

InDet::InDetFixedWindowTrackTimeTool::~InDetFixedWindowTrackTimeTool()
{}


//================ Initialisation =================================================

StatusCode InDet::InDetFixedWindowTrackTimeTool::initialize() {
  ATH_CHECK(m_caldbtool.retrieve());
  // Read key
  ATH_CHECK( m_T0ReadKey.initialize() );

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::InDetFixedWindowTrackTimeTool::finalize() {
  return StatusCode::SUCCESS;
}

//============================================================================================


double InDet::InDetFixedWindowTrackTimeTool::findPhase(Trk::Track const* track) const {
  ATH_MSG_DEBUG("Finding phase...");

  SG::ReadCondHandle<TRTCond::AverageT0> rhl{m_T0ReadKey};
  const TRTCond::AverageT0* avgT0{*rhl};

  TRTCond::RtRelation const* rtr = nullptr;

  double timeresidualsum = 0;
  size_t ntrthits = 0;

  for (const Trk::TrackStateOnSurface* state : *track->trackStateOnSurfaces()) {
    Trk::MeasurementBase const* mesb = state->measurementOnTrack();
    if (!mesb
        || !state->type(Trk::TrackStateOnSurface::Measurement)
        ) continue;

    InDet::TRT_DriftCircleOnTrack const* trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const*>(mesb);

    if (!trtcirc) continue;

    InDet::TRT_DriftCircle const* rawhit = trtcirc->prepRawData();
    if (!rawhit) continue;

    if (!rawhit->driftTimeValid()
        || rawhit->firstBinHigh()
        ) continue;

    Identifier const& ident = trtcirc->identify();
    double rawdrifttime = rawhit->rawDriftTime();

    double t0 = m_caldbtool->getT0(ident);
    ATH_MSG_DEBUG("T0 : " << t0);
    rtr = m_caldbtool->getRtRelation(ident);

    Trk::TrackParameters const* tparp = state->trackParameters();
    if (!tparp) continue;

    double trkdistance = tparp->parameters()[Trk::driftRadius];
    double trkdrifttime = rtr->drifttime(std::abs(trkdistance));
    double timeresidual = rawdrifttime - t0 + avgT0->get() - trkdrifttime;

    ATH_MSG_DEBUG("trkdistance=" << trkdistance
                                 << "  trkdrifttime=" << trkdrifttime
                                 << "  timeresidual=" << timeresidual
                                 << " rawdrifttime=" << rawdrifttime);

    if (std::abs(timeresidual - m_windowCenter) < m_windowSize
        && std::abs(trkdistance) < 2.8) {
      timeresidualsum += timeresidual;
      ++ntrthits;
    }
  }

  ATH_MSG_DEBUG("timeresidualsum = " << timeresidualsum);
  ATH_MSG_DEBUG("ntrtrhits = " << ntrthits);

  if (ntrthits > 1) return timeresidualsum / ntrthits + m_globalOffset;

  return 0.;
}

double InDet::InDetFixedWindowTrackTimeTool::findPhase(Trk::Segment const* segment) const {

  double sum_tr = 0.;
  double sum_goodhits = 0.;

  TRTCond::RtRelation const* rtr = nullptr;

  int nhits = segment->numberOfMeasurementBases();
  for (int i = 0; i < nhits; ++i) {
    Trk::RIO_OnTrack const* rio = dynamic_cast<Trk::RIO_OnTrack const*>(segment->measurement(i));

    InDet::TRT_DriftCircleOnTrack const* trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const*>(rio);
    if (!trtcirc) continue;

    InDet::TRT_DriftCircle const* rawhit = trtcirc->prepRawData();
    if (!rawhit) continue;

    if (!rawhit->lastBinHigh() && !rawhit->isNoise()) {
      Identifier const& ident = trtcirc->identify();
      rtr = m_caldbtool->getRtRelation(ident);
  
      if (not rtr) {
        ATH_MSG_WARNING("Rt relation pointer is null!");
        return 0.;
      }
      double full_drifttime = rtr->drifttime(2.0);

      sum_tr += rawhit->trailingEdge() * 3.125 - full_drifttime;

      ATH_MSG_VERBOSE("Hit " << sum_goodhits
                             << " : " << rawhit->trailingEdge() * 3.125
                             << "  fulldrifttime=" << full_drifttime);

      sum_goodhits += 1;
    } else {
      ATH_MSG_VERBOSE("Hit has lastbin high");
    }
  }


  if (sum_goodhits > 1) return sum_tr / sum_goodhits + m_globalOffset;

  return 0;
}

double InDet::InDetFixedWindowTrackTimeTool::findPhaseFromTE(Trk::Track const* track) const {
  ATH_MSG_DEBUG("Finding phase...");

  SG::ReadCondHandle<TRTCond::AverageT0> rhl{m_T0ReadKey};
  const TRTCond::AverageT0* avgT0{*rhl};

  double timeresidualsum = 0;
  size_t ntrthits = 0;

  for (const Trk::TrackStateOnSurface* state : *track->trackStateOnSurfaces()) {
    Trk::MeasurementBase const* mesb = state->measurementOnTrack();
    if (!mesb
        || !state->type(Trk::TrackStateOnSurface::Measurement)
        ) continue;

    InDet::TRT_DriftCircleOnTrack const* trtcirc = dynamic_cast<InDet::TRT_DriftCircleOnTrack const*>(mesb);
    if (!trtcirc) continue;

    InDet::TRT_DriftCircle const* rawhit = trtcirc->prepRawData();
    if (!rawhit) continue;

    Identifier const& ident = trtcirc->identify();
    double rawtrailingedge = rawhit->trailingEdge() * 3.125;

    double t0 = m_caldbtool->getT0(ident);

    ATH_MSG_DEBUG("T0 : " << t0);

    double timeresidual = rawtrailingedge - t0 + avgT0->get();

    ATH_MSG_DEBUG("timeresidual=" << timeresidual);

    if (timeresidual < 2000) {
      timeresidualsum += timeresidual;
      ++ntrthits;
    }
  }

  if (ntrthits > 1) return timeresidualsum / ntrthits + m_globalOffset;

  return 0.;
}
