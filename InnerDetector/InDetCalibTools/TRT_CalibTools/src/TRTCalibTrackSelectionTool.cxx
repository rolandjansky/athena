/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

********************************************************************/

#include "TRTCalibTrackSelectionTool.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ListItem.h"

TRTCalibTrackSelectionTool::TRTCalibTrackSelectionTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_PtMin(0),
  m_EtaMin(0),
  m_EtaMax(0),
  m_PhiOffset(0.),
  m_DeltaPhi(3.3),
  m_TrackSummaryTool("Trk::TrackSummaryTool/TrackSummaryTool"),
  m_MinPixelHits(0),
  m_MinSCTHits(0),
  m_MinTRTHits(1) //20
{
  declareInterface< TRTCalibTrackSelectionTool >(this);
  // declare tool parameters
  declareProperty("PtMin",m_PtMin);
  declareProperty("PhiOffset",m_PhiOffset);
  declareProperty("DeltaPhi",m_DeltaPhi);
  declareProperty("TrackSummaryTool",m_TrackSummaryTool);
  declareProperty("EtaMin",m_EtaMin);
  declareProperty("EtaMax",m_EtaMax);
  declareProperty("MinTRTHits",m_MinTRTHits);
  declareProperty("MinPixelHits",m_MinPixelHits);
  declareProperty("MinSCTHits",m_MinSCTHits);
}
  
StatusCode TRTCalibTrackSelectionTool::initialize()
{
  // get the ToolSvc

  if (m_TrackSummaryTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Cannot get TrackSummaryTool" << endmsg;
    return StatusCode::FAILURE;
  }
  // Print input properties
  msg(MSG::INFO) << "Track selection:"
      << "\n Pt > " << m_PtMin/CLHEP::GeV << " GeV"
      << "\n |Phi - " << m_PhiOffset << "| < " << m_DeltaPhi
      << "\n " << m_EtaMin << " < eta < " << m_EtaMax
      << "\n number_of_PIXEL_hits >= " << m_MinPixelHits
      << "\n number_of_SCT_hits >= " << m_MinSCTHits
      << "\n number_of_TRT_hits >= " << m_MinTRTHits << endmsg;
  return StatusCode::SUCCESS;
}

bool TRTCalibTrackSelectionTool::isAccepted(const Trk::Track* pTrack) const
{
  // selection for alignment quality tracks
  const Trk::Perigee* mesp=(pTrack->perigeeParameters());
  
  if(!mesp) return false;
  if (m_DeltaPhi>0 && fabs(mesp->parameters()[Trk::phi]-m_PhiOffset) > m_DeltaPhi) return false;

  float theta=mesp->parameters()[Trk::theta];
  float ptinv = fabs(mesp->parameters()[Trk::qOverP])/sin(theta);
  if(m_PtMin>0 && ptinv > 1/m_PtMin) return false;
  
  float eta=-log(tan(theta/2));
  if (m_EtaMin< m_EtaMax && (eta < m_EtaMin || eta > m_EtaMax) ) return false;

  std::unique_ptr<const Trk::TrackSummary> summary(m_TrackSummaryTool->createSummary(*pTrack));
  if(summary->get(Trk::numberOfTRTHits)<m_MinTRTHits) return false;
  if(summary->get(Trk::numberOfPixelHits)<m_MinPixelHits) return false;
  if(summary->get(Trk::numberOfSCTHits)<m_MinSCTHits) return false;
  // All cuts passed...
  if (msgLvl(MSG::DEBUG)) msg() << "Track passed selection:"
      << "\n\t Pt = " << 1.0/ptinv
      << "\n\t phi = " << mesp->parameters()[Trk::phi]
      << "\n\t eta = " << eta
      << "\n\t nPixel = " << summary->get(Trk::numberOfPixelHits)
      << "\n\t nSCT = " << summary->get(Trk::numberOfSCTHits)
      << "\n\t nTRT = " << summary->get(Trk::numberOfTRTHits) << endmsg;
  return (true);
}
