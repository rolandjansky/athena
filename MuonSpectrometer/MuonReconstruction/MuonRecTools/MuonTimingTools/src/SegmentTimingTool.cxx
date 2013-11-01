/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SegmentTimingTool.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h" // this tool is not correctly interfaced
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

//Constructor

Muon::SegmentTimingTool::SegmentTimingTool(const std::string& t, 
					   const std::string& n, 
					   const IInterface* p):
  AthAlgTool(t,n,p),
  m_idHelper(0),
  m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
  m_idToFixedIdTool("MuonCalib::IdToFixedIdTool")
{
  
  declareInterface<Trk::ITrackTimingTool>(this);
  //  declareInterface<ISegmentTimingTool>(this);

  declareProperty("MatchThreshold", m_matchThreshold = 0.);
  declareProperty("NumberOfMatchedCut", m_numberOfMatchedCut = 4);
  declareProperty("MuonEDMHelperTool",m_helperTool);

  declareProperty("doMuonBoyCSCTiming", m_doMuonBoyCSCTiming = true);
}

//Destructor

Muon::SegmentTimingTool::~SegmentTimingTool() {}

//Initialization

StatusCode Muon::SegmentTimingTool::initialize() {

  StatusCode sc = AlgTool::initialize();
  if(sc.isFailure()) return sc;

  sc = detStore()->retrieve(m_idHelper, "AtlasID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }

  sc = m_helperTool.retrieve();
  if(sc.isFailure()){
    ATH_MSG_FATAL("Unable to retrieve MuonSpectro EDM helper tool "<<m_helperTool);
    return sc;
  } else ATH_MSG_DEBUG("Successfully retrieved "<<m_helperTool);

  CHECK( m_idToFixedIdTool.retrieve() );

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;

}

//Finalization

StatusCode Muon::SegmentTimingTool::finalize() {
  StatusCode sc = AlgTool::finalize();
  return sc;
}

std::vector<Trk::SpaceTimePoint*> Muon::SegmentTimingTool::timeMeasurements
  (const Trk::Track& track,
   const Trk::SegmentCollection* segs) const
{

  if(segs == NULL){
    ATH_MSG_WARNING ("timeMeasurements(track) called without segment collection,"<<
		     " this is a bug assuming wrong tool/convention");
    std::vector<Trk::SpaceTimePoint*> stp;
    return stp;
  }
    
  std::vector<const Trk::Segment*> matched_segs;
  std::vector<int> matched_segs_numberOfMatchedMeasurements;

  Trk::SegmentCollection::const_iterator seg_iter = segs->begin();
  Trk::SegmentCollection::const_iterator seg_end = segs->end();
  
  for(; seg_iter != seg_end; ++seg_iter) {
    const Trk::Segment* curr_seg = *seg_iter;

    if(!curr_seg)
      continue;
    
    int numberOfMatchedMeasurements = matchSegmentTrk(track, curr_seg);
    if(numberOfMatchedMeasurements) {
      matched_segs.push_back(curr_seg);
      matched_segs_numberOfMatchedMeasurements.push_back(numberOfMatchedMeasurements);

      //ATH_MSG_DEBUG ("Matched segment: " << *curr_seg);
      ATH_MSG_DEBUG ("numberOfMatchedMeasurements = " << numberOfMatchedMeasurements <<
          ", chi2/DoF = " << curr_seg->fitQuality()->chiSquared() / curr_seg->fitQuality()->numberDoF());
      const Muon::MuonSegment* mu_seg = dynamic_cast<const Muon::MuonSegment*>(curr_seg);
      Identifier id = m_helperTool->chamberId(*mu_seg);
      MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId(id);
      ATH_MSG_DEBUG ("station name = " << fid.stationName() << " " << fid.stationNumberToFixedStationString(fid.stationName()));
    }
  }
  ATH_MSG_DEBUG ("Number of matched space-time points = " << matched_segs.size());


  // refined selection
  std::vector<bool> matched_segs_isSelected;
  matched_segs_isSelected.assign(matched_segs.size(), bool(true));

  for(unsigned int i=0; i<matched_segs.size(); i++) {
    if(!matched_segs_isSelected[i]) continue;

    const Trk::Segment* seg_i = matched_segs.at(i);
    int nMatched_i = matched_segs_numberOfMatchedMeasurements.at(i);
    float chi2PerDof_i = seg_i->fitQuality()->chiSquared() / seg_i->fitQuality()->numberDoF();

    for(unsigned int j=i+1; j<matched_segs.size(); j++) {
      if(!matched_segs_isSelected[j]) continue;

      const Trk::Segment* seg_j = matched_segs.at(j);
      int nMatched_j = matched_segs_numberOfMatchedMeasurements.at(j);
      float chi2PerDof_j = seg_j->fitQuality()->chiSquared() / seg_j->fitQuality()->numberDoF();

      // In case the two segments have common hits:
      // 1) choose the one with higher number of matched hits to the track.
      // 2) if the numbers of matched hits are the same, choose the one with smaller chi2/DoF.
      int commonHits = this->GetNumberOfCommonHits(seg_i, seg_j);
      if(commonHits) {
        if(nMatched_j > nMatched_i) {
          matched_segs_isSelected[i] = false;
        } else if(nMatched_j < nMatched_i) {
          matched_segs_isSelected[j] = false;
        } else {
          if(chi2PerDof_j < chi2PerDof_i) matched_segs_isSelected[i] = false;
          else matched_segs_isSelected[j] = false;
        }

        ATH_MSG_DEBUG ("Segments " << i << " and " << j << " have " << commonHits << "hit(s) in common.");
        if(matched_segs_isSelected[i]) ATH_MSG_DEBUG ("Keeping segment " << i);
        else ATH_MSG_DEBUG ("Keeping segment " << j);
      }
    }
  }


  std::vector<const Trk::Segment*> selected_segs;

  for(unsigned int i=0; i<matched_segs.size(); i++) {
    if(matched_segs_isSelected[i]) {
      selected_segs.push_back(matched_segs.at(i));
      ATH_MSG_DEBUG ("Selected space-time point " << i);
    }
  }
  ATH_MSG_DEBUG ("Number of selected space-time points = " << selected_segs.size());


  return timeMeasurements(selected_segs);
}

std::vector<Trk::SpaceTimePoint*> Muon::SegmentTimingTool::timeMeasurements(const Trk::TrackParticleBase& /*trk*/) const {
  std::vector<Trk::SpaceTimePoint*> s;
  return s;
}


//NOTE: Should only be passed a vector of segments that can be cast to MuonSegment*
std::vector<Trk::SpaceTimePoint*> Muon::SegmentTimingTool::timeMeasurements(std::vector<const Trk::Segment*> segs) const 
{

  std::vector<Trk::SpaceTimePoint*> stp;

  
  //Loop over the segments
  std::vector<const Trk::Segment*>::const_iterator seg_iter = segs.begin();
  for(; seg_iter != segs.end(); ++seg_iter) {

    //Cast to a muon segment
    const Muon::MuonSegment* mu_seg = dynamic_cast<const Muon::MuonSegment*>(*seg_iter);

    if(!mu_seg) continue;

    //    float t0_offset = getTimeFromSegment(mu_seg);
    if (mu_seg->hasFittedT0()) {
    
      float t0_offset = mu_seg->time();
      
      float t0_error = mu_seg->errorTime();
      if (t0_error <= 0.0 || t0_error > 100.) t0_error = getError(mu_seg);

      Identifier chId = m_helperTool->chamberId(*mu_seg);
      Trk::TrackState::MeasurementType detectorType = 
	m_idHelper->is_csc(chId) ? Trk::TrackState::CSC : Trk::TrackState::MDT;

      // reconstruct CSC time as it is not stored in the mboy segment collection until release 17.1.0
      if (m_idHelper->is_csc(chId) && mu_seg->author() == Trk::Segment::Muonboy) MuonboyT0CSCSegment(mu_seg, t0_offset, t0_error);

      ATH_MSG_DEBUG ("Segment has fitted T0, t0 = "<<t0_offset<<", weight = "<<
		     mu_seg->weight()<<" and error = "<<
		     mu_seg->errorTime()<<" vs fixed number error = "<<t0_error);
      stp.push_back(new Trk::SpaceTimePoint(mu_seg->globalPosition(), t0_offset, t0_error, 1.0/(t0_error*t0_error), detectorType));
    } else ATH_MSG_DEBUG ("Segment does not have fitted T0, dumping segment "<<*mu_seg);
  }

  return stp;
}

int Muon::SegmentTimingTool::matchSegmentTrk(const Trk::Track& origTrk, const Trk::Segment*& seg) const {
  const DataVector<const Trk::TrackStateOnSurface>* states = origTrk.trackStateOnSurfaces();

  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
  
  int matches = 0;
  int tot = 0;

  std::vector<const Trk::MeasurementBase*>::const_iterator mb_it   = seg->containedMeasurements().begin();
  std::vector<const Trk::MeasurementBase*>::const_iterator mb_end  = seg->containedMeasurements().end();

  //Loop over measurements on the segment
  for(; mb_it!=mb_end; ++mb_it){
    const Trk::MeasurementBase* seg_meas = *(mb_it);
    Identifier id2 = m_helperTool->getIdentifier(*seg_meas);
    //    MuonCalib::MuonFixedId fId2 = m_idToFixedIdTool->idToFixedId(id2) ;
      
    if(!(m_idHelper->is_mdt(id2)) && !(m_idHelper->is_csc(id2)) )
      continue;

    tot++;
  
    //Look for matching measurement on the track
    for(; tsit!=tsit_end ; ++tsit)  {
      const Trk::MeasurementBase* measurement = (*tsit)->measurementOnTrack();
      if( (measurement == NULL) || 
	  (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;
      
      Identifier id = m_helperTool->getIdentifier(*measurement);
      
      if(!m_idHelper->is_mdt(id) && !m_idHelper->is_csc(id)) continue;
      
      //      MuonCalib::MuonFixedId fId = m_idToFixedIdTool->idToFixedId(id) ;    

      //Check if the fixed ids from segment and track match
      if(id==id2) {
	matches++;
	break;
      }
    }
  }
  ATH_MSG_DEBUG ("segment-to-track match: matched/total hits = "<<matches<<"/"<<tot<<
		 " (ratio threshold >= " << m_matchThreshold <<
     ", number of matched hits >= " << m_numberOfMatchedCut << ")");

  //Check whether the percentage of matches is at the threshold set by the user
  if(!( ((double)matches/(double)tot)>(m_matchThreshold-1e-6) && matches>=m_numberOfMatchedCut )) matches = 0;

  return matches;
}


bool Muon::SegmentTimingTool::GetNumberOfCommonHits(const Trk::Segment*& seg1, const Trk::Segment*& seg2) const
{
  int matches = 0;
    
  std::vector<const Trk::MeasurementBase*>::const_iterator mbseg1_it   = seg1->containedMeasurements().begin();
  std::vector<const Trk::MeasurementBase*>::const_iterator mbseg1_end  = seg1->containedMeasurements().end();
  std::vector<const Trk::MeasurementBase*>::const_iterator mbseg2_it   = seg2->containedMeasurements().begin();
  std::vector<const Trk::MeasurementBase*>::const_iterator mbseg2_end  = seg2->containedMeasurements().end();
    
  //Loop over measurements on segment1 
  for(; mbseg1_it!=mbseg1_end; ++mbseg1_it){
    const Trk::MeasurementBase* seg1_meas = *(mbseg1_it);
    Identifier id1 = m_helperTool->getIdentifier(*seg1_meas);
        
    if(!(m_idHelper->is_mdt(id1)) && !(m_idHelper->is_csc(id1)) ) continue;
        
    //Look for matching measurement on segment2
    for(; mbseg2_it!=mbseg2_end; ++mbseg2_it){
      const Trk::MeasurementBase* seg2_meas = *(mbseg2_it);
      Identifier id2 = m_helperTool->getIdentifier(*seg2_meas);

      if(!(m_idHelper->is_mdt(id2)) && !(m_idHelper->is_csc(id2)) )continue;

      //Check if the fixed ids from segment and track match
      if(id1==id2) matches++;
    }
  }

  return matches ;     // return 0 if seg1 and seg2 do not match 
}


float Muon::SegmentTimingTool::getError(const Muon::MuonSegment*& seg) const{
  //TO BE UPDATED FURTHER: Currently just inflates the error if the segment has a bad topology; Should ultimately
  //have the error scale with how bad the segment is. 

  std::vector<const Trk::MeasurementBase*>::const_iterator mb_it   = seg->containedMeasurements().begin();
  std::vector<const Trk::MeasurementBase*>::const_iterator mb_end  = seg->containedMeasurements().end();

  int hits_pos = 0;
  int hits_neg = 0;
  
  //Loop over measurements on the segment
  for(; mb_it!=mb_end; ++mb_it){
    const Trk::MeasurementBase* seg_meas = *(mb_it);
    Identifier id = m_helperTool->getIdentifier(*seg_meas);
    
    if( !m_idHelper->is_mdt(id)  /* || !m_idHelper->is_csc(id) WHAT ABOUT CSC? */)
      continue;

    Trk::MeasurementBase* mb = const_cast<Trk::MeasurementBase*> (seg_meas);

    Muon::MdtDriftCircleOnTrack* mdtDriftCircle = dynamic_cast<Muon::MdtDriftCircleOnTrack*> (mb);
    // what about CSC ?
    
    //Get signed radius to see which side of track the hit is on 
    if(mdtDriftCircle){
      float hit_r = mdtDriftCircle->driftRadius();
    
      if(hit_r>=0.0)
	hits_pos++;
      else
	hits_neg++;
    }
  }

  if((hits_pos > 0 && hits_neg > 0) || (hits_pos == 0 && hits_neg == 0) /*For CSC*/){
    return 2.5; //Average resolution (about 2.5 ns)
  }else{
    return 6.0; //Worse resolution (about 6 ns)
  }
}

// CSC timing for muonboy
// Code from: MboyEDMTool-07-83-41/src//MuonboyToSegmentTool.cxx

void Muon::SegmentTimingTool::MuonboyT0CSCSegment(const Muon::MuonSegment*& pMuonSegment, float& t0, float& t0Error) const{
   t0      = pMuonSegment->time()      ;
   t0Error = pMuonSegment->errorTime() ;
   
   if ( !m_doMuonBoyCSCTiming ) return;


   int   DoAlternative = 0 ;
   int   Alternative_ThereIsaCscTimeEarly  = 0 ;
   int   Alternative_ThereIsaCscTimeLate   = 0 ;
   float Alternative_LatestEarlyTime  = 0. ;
   float Alternative_EarliestLateTime = 0. ;
   float Alternative_Kounter = 0. ;
   float Alternative_t0      = 0. ;
   float Alternative_t0sqr   = 0. ;
   float Alternative_t0Error = 0. ;
   const std::vector<const Trk::RIO_OnTrack*> pRIOSet = pMuonSegment->containedROTs();
   std::vector<const Trk::RIO_OnTrack*>::const_iterator pRIO_OnTrackIter = pRIOSet.begin();
   for (; pRIO_OnTrackIter!=pRIOSet.end(); ++pRIO_OnTrackIter){
     if ( (*pRIO_OnTrackIter) ){
       const Trk::PrepRawData* pPrepRawData =(*pRIO_OnTrackIter)->prepRawData () ;
       if ( pPrepRawData ){
         const Muon::CscPrepData* pPrepData = dynamic_cast<const Muon::CscPrepData*>(pPrepRawData);
         if ( pPrepData ){
           DoAlternative = 1 ;
           double Time = pPrepData->time() ;
           if (pPrepData->timeStatus() == Muon::CscTimeSuccess){
             Alternative_Kounter = Alternative_Kounter + 1. ;
             Alternative_t0    = Alternative_t0    + Time      ;
             Alternative_t0sqr = Alternative_t0sqr + Time*Time ;
           }
           if (pPrepData->timeStatus() == Muon::CscTimeEarly) {
             if ( Alternative_ThereIsaCscTimeEarly == 0 ) Alternative_LatestEarlyTime = Time ;
             if ( Time >= Alternative_LatestEarlyTime   ) Alternative_LatestEarlyTime = Time ; 
             Alternative_ThereIsaCscTimeEarly = 1 ;
           }
           if (pPrepData->timeStatus() == Muon::CscTimeLate ) {
             if ( Alternative_ThereIsaCscTimeLate == 0 ) Alternative_EarliestLateTime = Time ;
             if ( Time <= Alternative_EarliestLateTime ) Alternative_EarliestLateTime = Time ; 
             Alternative_ThereIsaCscTimeLate  = 1 ;
           }
         }
       }
     }
   }
   
   if (DoAlternative==1){ 
     if (Alternative_Kounter){
       Alternative_t0    =  Alternative_t0    / Alternative_Kounter ;
       Alternative_t0sqr =  Alternative_t0sqr / Alternative_Kounter ;
       Alternative_t0Error =  Alternative_t0sqr - Alternative_t0*Alternative_t0 ;
       if (Alternative_t0Error < 0.){
         Alternative_t0Error = 0. ;
       }else{
         Alternative_t0Error = sqrt(Alternative_t0Error) ;
       }
     }else if (Alternative_ThereIsaCscTimeEarly == 1 && Alternative_ThereIsaCscTimeLate == 0){
       Alternative_t0      = Alternative_LatestEarlyTime       ;
       Alternative_t0Error = fabs(Alternative_LatestEarlyTime) ;
     }else if (Alternative_ThereIsaCscTimeEarly == 0 && Alternative_ThereIsaCscTimeLate == 1){
       Alternative_t0      = Alternative_EarliestLateTime ;
       Alternative_t0Error = Alternative_EarliestLateTime ;
     }else{
       Alternative_t0      = 99999. ;
       Alternative_t0Error = 99999. ;
     }
     t0      = Alternative_t0      ;
     t0Error = Alternative_t0Error ;
   }
}
