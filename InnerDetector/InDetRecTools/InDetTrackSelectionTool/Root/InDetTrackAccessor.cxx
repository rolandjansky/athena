// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// InDetTrackAccessor.cxx
// implementations of accessor objects

#include "InDetTrackAccessor.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "AsgTools/IAsgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#ifndef XAOD_ANALYSIS
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
// includes for top/bottom Si hits (for cosmic selection)
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#endif

// ---------------- TrackAccessor ----------------
InDet::TrackAccessor::TrackAccessor(const asg::IAsgTool* tool)
  : asg::AsgMessaging::AsgMessaging(tool)
{
}

InDet::TrackAccessor::~TrackAccessor() = default;

// ---------------- SummaryAccessor ----------------
InDet::SummaryAccessor::SummaryAccessor(const asg::IAsgTool* tool)
  : TrackAccessor::TrackAccessor(tool)
  , m_summaryValue(0)
  , m_summaryType(xAOD::numberOfTrackSummaryTypes)
{
}

void InDet::SummaryAccessor::setSummaryType( xAOD::SummaryType sumType )
{
  m_summaryType = sumType;
  return;
}

StatusCode InDet::SummaryAccessor::access( const xAOD::TrackParticle& track,
					   const xAOD::Vertex* )
{
  if (!track.summaryValue(m_summaryValue, m_summaryType)) {
    ATH_MSG_DEBUG( "Failed to get SummaryType " << std::to_string(m_summaryType) << " from xAOD::TrackParticle summary. A value of zero will be used instead." );
    m_summaryValue = 0;
  }
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::SummaryAccessor::access ( const Trk::Track&,
					    const Trk::TrackParameters*,
					    const Trk::TrackSummary* summary )
{
  if (!summary) {
    ATH_MSG_ERROR( "Recieved null pointer to track summary." );
    m_summaryValue = 0;
    return StatusCode::FAILURE;
  }
  // previously casting the xAOD::SummaryType directly to a Trk::SummaryType.
  // now casting to an int in between to try to remove coverity warning.
  Int_t summaryTypeInt = static_cast<Int_t>(m_summaryType);
  Int_t checkSummaryValue = summary->get( static_cast<Trk::SummaryType>(summaryTypeInt) );
  if (checkSummaryValue < 0) {
    // Trk::TrackSummary::get() will return -1 if the data cannot be retrieved
    ATH_MSG_DEBUG( "Recieved " << checkSummaryValue << " for SummaryType " << std::to_string(m_summaryType) << " from Trk::TrackSummary. A value of zero will be used instead." );
    checkSummaryValue = 0;
  }
  m_summaryValue = checkSummaryValue;
  return StatusCode::SUCCESS;
}
#endif


// ---------------- FitQualityAccessor ----------------
InDet::FitQualityAccessor::FitQualityAccessor(const asg::IAsgTool* tool)
  : TrackAccessor(tool)
  , m_chiSq(0)
  , m_nDoF(0)
{
}

StatusCode InDet::FitQualityAccessor::access( const xAOD::TrackParticle& track,
					      const xAOD::Vertex* )
{
  m_chiSq = track.chiSquared();
  m_nDoF = track.numberDoF();
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::FitQualityAccessor::access( const Trk::Track& track,
					      const Trk::TrackParameters*,
					      const Trk::TrackSummary* )
{
  if (!track.fitQuality()) {
    ATH_MSG_WARNING( "Zero pointer to fit quality recieved." );
    m_chiSq = std::nan("");
    m_nDoF = std::nan("");
    return StatusCode::FAILURE;
  }
  m_chiSq = track.fitQuality()->chiSquared();
  m_nDoF = track.fitQuality()->doubleNumberDoF();
  return StatusCode::SUCCESS;
}
#endif


// ---------------- eProbabilityHTAccessor ----------------
InDet::eProbabilityHTAccessor::eProbabilityHTAccessor(const asg::IAsgTool* tool)
  : TrackAccessor::TrackAccessor(tool)
  , m_eProbHT(0.)
{
}

StatusCode InDet::eProbabilityHTAccessor::access( const xAOD::TrackParticle& track,
						  const xAOD::Vertex* )
{
  
  if (!track.summaryValue(m_eProbHT, xAOD::SummaryType::eProbabilityHT)) {
    ATH_MSG_DEBUG( "Failed to get eProbabilityHT from xAOD::TrackParticle summary. A value of zero will be used instead." );
    m_eProbHT = 0;
  }
  
  return StatusCode::SUCCESS;
}

#ifndef XAOD_ANALYSIS
StatusCode InDet::eProbabilityHTAccessor::access ( const Trk::Track&,
						   const Trk::TrackParameters*,
						   const Trk::TrackSummary* summary )
{
  
  if (!summary) {
    ATH_MSG_ERROR( "Recieved null pointer to track summary." );
    m_eProbHT = 0;
    return StatusCode::FAILURE;
  }
  float checkSummaryValue = summary->getPID( Trk::eProbabilityType::eProbabilityHT );
  if (checkSummaryValue < 0) {
    // Trk::TrackSummary::getkPID() will return -1 if the data cannot be retrieved
    ATH_MSG_DEBUG( "Recieved " << checkSummaryValue << " for eProbabilityHT. A value of zero will be used instead." );
    checkSummaryValue = 0;
  }
  m_eProbHT = checkSummaryValue;
  
  return StatusCode::SUCCESS;
}
#endif


// the top/bottom si hit info is likely only available in athena
#ifndef XAOD_ANALYSIS
// ---------------- SiHitsTopBottomAccessor ----------------
InDet::SiHitsTopBottomAccessor::SiHitsTopBottomAccessor(const asg::IAsgTool* tool)
  : TrackAccessor(tool)
  , m_top(0)
  , m_bottom(0)
{
}

StatusCode InDet::SiHitsTopBottomAccessor::access( const xAOD::TrackParticle& track,
						   const xAOD::Vertex* )
{
  const Trk::Track* trkTrack = track.track();
  if (!trkTrack) {
    ATH_MSG_WARNING( "xAOD::TrackParticle has a null pointer to a Trk::Track." );
    ATH_MSG_WARNING( "Unable to access top/bottom Si hits." );
    return StatusCode::FAILURE;
  }
  return access(*trkTrack);
}

StatusCode InDet::SiHitsTopBottomAccessor::access( const Trk::Track& track,
						   const Trk::TrackParameters*,
						   const Trk::TrackSummary* )
{
  m_top = 0;
  m_bottom = 0;

  const DataVector<const Trk::MeasurementBase>* trkMeasurements = track.measurementsOnTrack();
  if (!trkMeasurements) {
    ATH_MSG_WARNING( "Null pointer to Trk::MeasurementBase vector." );
    return StatusCode::FAILURE;
  }
  // code in here is mostly adapted from InDetCosmicTrackSelectorTool
  for (const Trk::MeasurementBase* measurement : *trkMeasurements) {
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
    if (!rot)
      continue;
    const InDet::SiClusterOnTrack* siclus
      = dynamic_cast<const InDet::SiClusterOnTrack*>(rot);
    if (!siclus)
      continue;

    float ypos = siclus->globalPosition().y();
    if (ypos == 0)
      continue; // neither top nor bottom

    const InDet::SCT_ClusterOnTrack* sctclus
      = dynamic_cast<const SCT_ClusterOnTrack*>(siclus);
    if (!sctclus) {
      // Pixel hit
      if (ypos > 0)
	m_top+=2;
      else
	m_bottom+=2;
    } else {
      // SCT hit
      if (ypos > 0)
	m_top++;
      else
	m_bottom++;
    }   
  }

  
  return StatusCode::SUCCESS;
}
#endif // XAOD_ANALYSIS


// template specializations for using generic xAOD::TrackParticle functions in full athena
#ifndef XAOD_ANALYSIS
namespace InDet {
  template <>
  StatusCode FuncAccessor<double, &xAOD::TrackParticle::pt>
  ::access( const Trk::Track&,
	    const Trk::TrackParameters* perigee,
	    const Trk::TrackSummary* )
  {
    if (!perigee) {
      ATH_MSG_WARNING( "Zero pointer to perigee recieved." );
      m_value = std::nan("");
      return StatusCode::FAILURE;
    }
    m_value = perigee->momentum().perp();
    return StatusCode::SUCCESS;
  }

  template <>
  StatusCode FuncAccessor<double, &xAOD::TrackParticle::eta>
  ::access( const Trk::Track&,
	    const Trk::TrackParameters* perigee,
	    const Trk::TrackSummary* )
  {
    
    if (!perigee) {
      ATH_MSG_WARNING( "Zero pointer to perigee recieved." );
      m_value = std::nan("");
      return StatusCode::FAILURE;
    }
    m_value = perigee->momentum().eta();
    
    return StatusCode::SUCCESS;
  }

  template <>
  StatusCode FuncAccessor<float, &xAOD::TrackParticle::qOverP>
  ::access( const Trk::Track&,
	    const Trk::TrackParameters* perigee,
	    const Trk::TrackSummary* )
  {
    
    if (!perigee) {
      ATH_MSG_WARNING( "Zero pointer to perigee recieved." );
      m_value = std::nan("");
      return StatusCode::FAILURE;
    }
    m_value = 1./perigee->momentum().mag();
    
    return StatusCode::SUCCESS;
  }
  
  template <>
  StatusCode FuncAccessor<uint8_t, &xAOD::TrackParticle::numberOfUsedHitsdEdx>
  ::access ( const Trk::Track&,
	     const Trk::TrackParameters*,
	     const Trk::TrackSummary* summary )
  {
  
    if (!summary) {
      ATH_MSG_ERROR( "Recieved null pointer to track summary." );
      m_value = 0;
      return StatusCode::FAILURE;
    }
    Int_t checkSummaryValue = summary->numberOfUsedHitsdEdx();
    if (checkSummaryValue < 0) {
      // Trk::TrackSummary::numberOfUsedHitsdEdx() will return -1 if the data cannot be retrieved
      ATH_MSG_DEBUG( "Recieved " << checkSummaryValue << " for numberOfUsedHitsdEdx from Trk::TrackSummary. A value of zero will be used instead." );
      checkSummaryValue = 0;
    }
    m_value = checkSummaryValue;
  
    return StatusCode::SUCCESS;
  }

  template<>
  StatusCode FuncAccessor<uint8_t, &xAOD::TrackParticle::numberOfIBLOverflowsdEdx>
  ::access ( const Trk::Track&,
	     const Trk::TrackParameters*,
	     const Trk::TrackSummary* summary )
  {
  
    if (!summary) {
      ATH_MSG_ERROR( "Recieved null pointer to track summary." );
      m_value = 0;
      return StatusCode::FAILURE;
    }
    Int_t checkSummaryValue = summary->numberOfOverflowHitsdEdx();
    if (checkSummaryValue < 0) {
      // Trk::TrackSummary::numberOfOverflowHitsdEdx() will return -1 if the data cannot be retrieved
      ATH_MSG_DEBUG( "Recieved " << checkSummaryValue << " for numberOfOverflowHitsdEdx from Trk::TrackSummary. A value of zero will be used instead." );
      checkSummaryValue = 0;
    }
    m_value = checkSummaryValue;
  
    return StatusCode::SUCCESS;
  }

} // namespace InDet

#endif // XAOD_ANALYSIS
