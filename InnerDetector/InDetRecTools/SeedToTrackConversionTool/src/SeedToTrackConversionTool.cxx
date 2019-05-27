/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SeedToTrackConversionTool.cxx
///////////////////////////////////////////////////////////////////

#include "SeedToTrackConversionTool/SeedToTrackConversionTool.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"
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

void InDet::SeedToTrackConversionTool::newEvent(const Trk::TrackInfo& info, const std::string& patternName) const
{
  EventData& data{getEventData()};
  data.m_seedSegmentsCollection = std::make_unique<TrackCollection>();
  m_totseed = 0;
  m_survived = 0;

  data.m_trackInfo = info;
  data.m_patternName = patternName;
  if (static_cast<int>(data.m_patternName.find("Forward"))>-1) {
    data.m_trackInfo.setPatternRecognitionInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_ForwardTracks);
  }
}

void InDet::SeedToTrackConversionTool::endEvent() const
{
  EventData& data{getEventData()};

  // Print event information
  //
  if (msgLevel()<=0) {
    m_nprint=1;
    ATH_MSG_DEBUG(*this);
  }

  ATH_MSG_INFO(" Check SiSPSeedSegments Collection " << data.m_seedSegmentsCollection->size() << " trackinfo: "
               << data.m_trackInfo);
  SG::WriteHandle<TrackCollection> seedsegmentsOutput{m_seedsegmentsOutput};
  if (seedsegmentsOutput.record(std::move(data.m_seedSegmentsCollection)).isFailure()) {
    ATH_MSG_ERROR("Could not save converted SiSPSeedSegments tracks");
  }
}

void  InDet::SeedToTrackConversionTool::executeSiSPSeedSegments(const Trk::TrackParameters* Tp, const int& mtrk, const std::list<const Trk::SpacePoint*>& Sp) const
{
  EventData& data{getEventData()};
  ++m_totseed; // accumulate all seeds
  if (mtrk>0) ++m_survived; // survided seeds 
  std::vector<const Trk::PrepRawData*> prdsInSp;
  for (const Trk::SpacePoint* s: Sp) {
    const std::pair<const Trk::PrepRawData*, const Trk::PrepRawData*>& prds = s->clusterList();
    if (prds.first) prdsInSp.push_back(prds.first);
    if (prds.second && prds.first != prds.second) prdsInSp.push_back(prds.second);
  }
  Trk::PerigeeSurface persurf;
  const Trk::TrackParameters* per = m_extrapolator->extrapolate(*Tp, persurf, Trk::anyDirection, false, Trk::nonInteracting);
  const Trk::TrackParameters* prevpar = Tp;
  if (per) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    const Trk::TrackStateOnSurface* pertsos = new Trk::TrackStateOnSurface(0, per, 0, 0, typePattern);
    DataVector<const Trk::TrackStateOnSurface>* traj = new DataVector<const Trk::TrackStateOnSurface>;
    traj->push_back(pertsos);
    for (const Trk::PrepRawData* prd: prdsInSp) {
      const Trk::Surface& surf = prd->detectorElement()->surface(prd->identify());
      const Trk::TrackParameters* thispar = m_extrapolator->extrapolate(*prevpar, surf, Trk::alongMomentum, false, Trk::nonInteracting);
      if (thispar) {
        const Trk::TrackParameters* tmppar = thispar->clone();
        delete thispar;
        thispar = tmppar;
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
        const Trk::RIO_OnTrack* rot = m_rotcreator->correct(*prd, *thispar);
        if (rot) {
          const Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface(rot, thispar, 0, 0, typePattern);
          traj->push_back(tsos);
          prevpar = thispar;
        }
      }
    }
    if (mtrk>0) { // survived seeds set as
      data.m_trackInfo.setTrackFitter(Trk::TrackInfo::xKalman); // xk seedfinder
    }
    Trk::Track* t = new Trk::Track(data.m_trackInfo, traj, 0);
    if (t) data.m_seedSegmentsCollection->push_back(t);
  }
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////
 
MsgStream& InDet::SeedToTrackConversionTool::dump(MsgStream& out) const
{
  out << std::endl;
  if (m_nprint) dumpevent(out);
  return dumpconditions(out);
}

InDet::SeedToTrackConversionTool::EventData& InDet::SeedToTrackConversionTool::getEventData() const
{
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cache.size()) { // Need to extend vectors
    static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement
    m_eventData.resize(slot+1);
  }
  if (m_cache[slot]!=evt) { // New event
    m_cache[slot] = evt;
    // Initialization
    delete m_eventData[slot].m_seedSegmentsCollection.release();
    m_eventData[slot].m_trackInfo = Trk::TrackInfo();
    m_eventData[slot].m_patternName = "";
  }
  return m_eventData[slot];
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SeedToTrackConversionTool::dumpconditions(MsgStream& out) const
{
  EventData& data{getEventData()};
  out << "|----------------------------------------------------------------------"
      << "-------------------|"
      << std::endl;
  out << "| Output Collection Name   | " << m_seedsegmentsOutput << std::endl;
  out << "} Name of pattern recognition | " << data.m_patternName << std::endl;
  out << "|----------------------------------------------------------------------"
      << "-------------------|"
      << std::endl;
  return out;
}
///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SeedToTrackConversionTool::dumpevent(MsgStream& out) const
{
  EventData& data{getEventData()};
  out << "|---------------------------------------------------------------------|"
      << std::endl;
  out << "| Name of SeedFinder          | " << data.m_patternName
      << "                              | " << std::endl;
  out << "| Number of All seeds         | " << std::setw(12) << m_totseed 
      << "                              | " << std::endl;
  out << "| Number of survived seeds    | " << std::setw(12) << m_survived  
      << "                              | " << std::endl;
  out << "|---------------------------------------------------------------------|"
      << std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////
 
std::ostream& InDet::SeedToTrackConversionTool::dump(std::ostream& out) const
{
  return out;
}
 
///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator << 
(MsgStream& sl, const InDet::SeedToTrackConversionTool& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
(std::ostream& sl, const InDet::SeedToTrackConversionTool& se)
{
  return se.dump(sl);
}   

//============================================================================================
