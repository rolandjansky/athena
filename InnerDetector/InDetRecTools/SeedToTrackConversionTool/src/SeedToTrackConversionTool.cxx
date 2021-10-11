/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SeedToTrackConversionTool.cxx
///////////////////////////////////////////////////////////////////

#include "SeedToTrackConversionTool/SeedToTrackConversionTool.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "SiSPSeededTrackFinderData/SeedToTrackConversionData.h"
#include "StoreGate/WriteHandle.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include <iomanip>
#include <iostream>
#include <utility>

//================ Constructor =================================================

InDet::SeedToTrackConversionTool::SeedToTrackConversionTool(const std::string& t,
                                                            const std::string& n,
                                                            const IInterface* p)
  : base_class(t, n, p)
{
}

//================ Initialisation =================================================

StatusCode InDet::SeedToTrackConversionTool::initialize()
{

  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;

  if (m_extrapolator.retrieve().isFailure()) {
    ATH_MSG_FATAL ("Could not retrieve "<< m_extrapolator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE( "initialize() Retrieved service " << m_extrapolator);
  }

  // Retrieve the Track RotCreator tool
  if (m_rotcreator.retrieve().isFailure()) {
    ATH_MSG_FATAL ("Could not retrieve "<< m_rotcreator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE( "initialize() Retrieved service " << m_rotcreator);
  }  

  ATH_CHECK(m_seedsegmentsOutput.initialize());

  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::SeedToTrackConversionTool::finalize()
{
  return AlgTool::finalize();
}

void InDet::SeedToTrackConversionTool::newEvent(SeedToTrackConversionData& data, const Trk::TrackInfo& info, const std::string& patternName) const
{
  data.seedSegmentsCollection() = std::make_unique<TrackCollection>();
  m_totseed = 0;
  m_survived = 0;

  data.trackInfo() = info;
  data.patternName() = patternName;
  if (static_cast<int>(data.patternName().find("Forward"))>-1) {
    data.trackInfo().setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_ForwardTracks);
  }
}

void InDet::SeedToTrackConversionTool::endEvent(SeedToTrackConversionData& data) const
{
  // Print event information
  //
  if (msgLevel()<=0) {
    m_nprint=1;
    dump(data, msg(MSG::DEBUG));
  }

  ATH_MSG_INFO(" Check SiSPSeedSegments Collection " << data.seedSegmentsCollection()->size() << " trackinfo: "
               << data.trackInfo());
  SG::WriteHandle<TrackCollection> seedsegmentsOutput{m_seedsegmentsOutput};
  if (seedsegmentsOutput.record(std::move(data.seedSegmentsCollection())).isFailure()) {
    ATH_MSG_ERROR("Could not save converted SiSPSeedSegments tracks");
  }
}

void InDet::SeedToTrackConversionTool::executeSiSPSeedSegments(SeedToTrackConversionData& data,
                                                               const Trk::TrackParameters* Tp,
                                                               const int& mtrk,
                                                               const std::vector<const Trk::SpacePoint*>& Sp) const
{
  ++m_totseed; // accumulate all seeds
  if (mtrk>0) ++m_survived; // survided seeds 
  std::vector<const Trk::PrepRawData*> prdsInSp;
  for (const Trk::SpacePoint* s: Sp) {
    const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>& prds = s->clusterList();
    if (prds.first) prdsInSp.push_back(prds.first);
    if (prds.second && prds.first != prds.second) prdsInSp.push_back(prds.second);
  }
  Trk::PerigeeSurface persurf;
  std::unique_ptr<const Trk::TrackParameters> per(m_extrapolator->extrapolate(*Tp, persurf, Trk::anyDirection, false, Trk::nonInteracting));
  std::unique_ptr<const Trk::TrackParameters> prevpar(Tp->uniqueClone());
  if (per) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    const Trk::TrackStateOnSurface* pertsos = new Trk::TrackStateOnSurface(nullptr, std::move(per), nullptr, nullptr, typePattern);
    auto traj = DataVector<const Trk::TrackStateOnSurface>();
    traj.push_back(pertsos);
    for (const Trk::PrepRawData* prd: prdsInSp) {
      const Trk::Surface& surf = prd->detectorElement()->surface(prd->identify());
      std::unique_ptr<const Trk::TrackParameters> thispar(m_extrapolator->extrapolate(*prevpar, surf, Trk::alongMomentum, false, Trk::nonInteracting));
      if (thispar) {
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
        std::unique_ptr<const Trk::RIO_OnTrack> rot(m_rotcreator->correct(*prd, *thispar));
        if (rot) {
          const Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface(std::move(rot), thispar->uniqueClone(), nullptr, nullptr, typePattern);
          traj.push_back(tsos);
          prevpar = std::move(thispar);
        }
      }
    }
    if (mtrk>0) { // survived seeds set as
      data.trackInfo().setTrackFitter(Trk::TrackInfo::xKalman); // xk seedfinder
    }
    Trk::Track* t = new Trk::Track(data.trackInfo(), std::move(traj), nullptr);
    if (t) data.seedSegmentsCollection()->push_back(t);
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////
 
MsgStream& InDet::SeedToTrackConversionTool::dump(SeedToTrackConversionData& data, MsgStream& out) const
{
  out << std::endl;
  if (m_nprint) dumpevent(data, out);
  return dumpconditions(data, out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SeedToTrackConversionTool::dumpconditions(SeedToTrackConversionData& data, MsgStream& out) const
{
  out << "|----------------------------------------------------------------------"
      << "-------------------|"
      << std::endl;
  out << "| Output Collection Name   | " << m_seedsegmentsOutput << std::endl;
  out << "} Name of pattern recognition | " << data.patternName() << std::endl;
  out << "|----------------------------------------------------------------------"
      << "-------------------|"
      << std::endl;
  return out;
}
///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SeedToTrackConversionTool::dumpevent(SeedToTrackConversionData& data, MsgStream& out) const
{
  out << "|---------------------------------------------------------------------|"
      << std::endl;
  out << "| Name of SeedFinder          | " << data.patternName()
      << "                              | " << std::endl;
  out << "| Number of All seeds         | " << std::setw(12) << m_totseed 
      << "                              | " << std::endl;
  out << "| Number of survived seeds    | " << std::setw(12) << m_survived  
      << "                              | " << std::endl;
  out << "|---------------------------------------------------------------------|"
      << std::endl;
  return out;
}

//============================================================================================
