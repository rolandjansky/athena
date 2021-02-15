/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SummaryToolStub.h"

#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include <vector>
#include <bitset>

SummaryToolStub::SummaryToolStub(const std::string& t, const std::string& n, const IInterface*  p ):base_class(t,n,p){
  declareInterface<ITrackSummaryTool>(this);
  //just for fun, lets make a TrackSummary out of its components
  //I'm using 19 components, which is not a full set, but encompasses the element which 
  //would give SCT Shared Hits (which I use in the test)
  std::vector<int> information{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18, -1};
  std::vector<float> eProbability{0.1,0.1,0.1};
  std::bitset<Trk::DetectorType::numberOfDetectorTypes> hitPattern{};
  hitPattern.set(3);
  float dedx=0.2;
  int nhitsuseddedx=3;
  int nhitsoverflowdedx=1;
  m_pTrackSummaryRef = std::make_unique<Trk::TrackSummary>(information, eProbability, hitPattern, dedx, nhitsuseddedx, nhitsoverflowdedx);
}

StatusCode 
SummaryToolStub::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode 
SummaryToolStub::finalize() {
  return StatusCode::SUCCESS;
}
//methods inherited from the IExtendedTrackSummaryTool and the ITrackSummaryTool.
//
void 
SummaryToolStub::updateSharedHitCount(Trk::Track& ) const {}
//
void 
SummaryToolStub::computeAndReplaceTrackSummary(const EventContext&  ,Trk::Track& track,const Trk::PRDtoTrackMap* , bool ) const {
  //copy the reference summary
  auto p = std::make_unique<Trk::TrackSummary>(*m_pTrackSummaryRef);
  track.setTrackSummary(std::move(p));
}
//
void 
SummaryToolStub::updateTrackSummary(const EventContext&  ,Trk::Track&) const {

}
//
void 
SummaryToolStub::updateTrackSummary(const EventContext&  ,Trk::Track&, const Trk::PRDtoTrackMap* , bool) const {

}
//
void 
SummaryToolStub::updateTrack(const EventContext&  ,Trk::Track& ) const {

}
//
std::unique_ptr<Trk::TrackSummary> 
SummaryToolStub::summaryNoHoleSearch( const EventContext&  ,const Trk::Track& , const Trk::PRDtoTrackMap* ) const {
 return std::make_unique<Trk::TrackSummary>();
}
//
std::unique_ptr<Trk::TrackSummary> 
SummaryToolStub::summaryNoHoleSearch(const EventContext&  ,const Trk::Track&)const {
 return std::make_unique<Trk::TrackSummary>();
}
//
std::unique_ptr<Trk::TrackSummary> 
SummaryToolStub::summary(const EventContext&  ,const Trk::Track& ) const {
 return std::make_unique<Trk::TrackSummary>();
}
//
std::unique_ptr<Trk::TrackSummary> 
SummaryToolStub::summary(const EventContext&  ,const Trk::Track&, const Trk::PRDtoTrackMap*) const {
 return std::make_unique<Trk::TrackSummary>();
}
//
void 
SummaryToolStub::updateSharedHitCount(const Trk::Track& ,const Trk::PRDtoTrackMap* , Trk::TrackSummary& ) const {}
//
void 
SummaryToolStub::updateSharedHitCount(Trk::Track& track,const Trk::PRDtoTrackMap* ) const  {
  auto updatedSummary = *m_pTrackSummaryRef;
  updatedSummary.update(Trk::SummaryType::numberOfSCTSharedHits, 20);
  auto p = std::make_unique<Trk::TrackSummary>(updatedSummary);
  track.setTrackSummary(std::move(p));
}
//
void 
SummaryToolStub::updateAdditionalInfo(const Trk::Track& , Trk::TrackSummary& ) const {
  //
}
//
void 
SummaryToolStub::updateAdditionalInfo(Trk::Track& track) const {
  //just for fun, lets make a TrackSummary out of its components
  std::vector<int> information{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18, -1};
  std::vector<float> eProbability{0.1,0.1,0.1};
  std::bitset<Trk::DetectorType::numberOfDetectorTypes> hitPattern{};
  hitPattern.set(3);
  float dedx=0.2;
  int nhitsuseddedx=4;
  int nhitsoverflowdedx=1;
  auto t = std::make_unique<Trk::TrackSummary>(information, eProbability, hitPattern, dedx, nhitsuseddedx, nhitsoverflowdedx);
  track.setTrackSummary(std::move(t));
}