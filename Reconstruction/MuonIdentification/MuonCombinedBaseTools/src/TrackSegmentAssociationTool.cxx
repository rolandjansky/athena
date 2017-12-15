/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackSegmentAssociationTool.h"
#include "TrkTrack/Track.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"

namespace Muon {

  //Constructor

  TrackSegmentAssociationTool::TrackSegmentAssociationTool(const std::string& t, 
                                                                 const std::string& n, 
                                                                 const IInterface* p):
    AthAlgTool(t,n,p),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
  {
  
    declareInterface<TrackSegmentAssociationTool>(this);
    //  declareInterface<ITrackSegmentAssociationTool>(this);

    declareProperty("MuonEDMHelperTool",m_helper);
    declareProperty("MuonEDMPrinterTool",m_printer);
    declareProperty("MuonSegmentLocation",m_segmentLocation="MuonSegments");
    declareProperty("StauSegmentLocation",m_stauSegmentLocation="StauSegments");
  }

  //Destructor

  TrackSegmentAssociationTool::~TrackSegmentAssociationTool() {}

  //Initialization

  StatusCode TrackSegmentAssociationTool::initialize() {
    ATH_CHECK(m_helper.retrieve());
    ATH_CHECK(m_printer.retrieve());
    return StatusCode::SUCCESS;
  }

  //Finalization

  StatusCode TrackSegmentAssociationTool::finalize() {
    return StatusCode::SUCCESS;
  }


  const xAOD::MuonSegmentContainer* TrackSegmentAssociationTool::retrieveSegments( std::string location ) const {
    
    if( !evtStore()->contains<xAOD::MuonSegmentContainer>(location) ) {
      ATH_MSG_DEBUG( "No segments in StoreGate at " << location );
      return 0;
    }
    const xAOD::MuonSegmentContainer* segments = 0;
    if( evtStore()->retrieve(segments, location).isFailure() || !segments ) {
      ATH_MSG_DEBUG( "Unable to retrieve segments at " << location );
      return 0;
    }
    ATH_MSG_VERBOSE("Got segments " << segments->size() << " at " << location);
    return segments;
  }
  
  /** Returns a list of segments that match with the input Muon. */
  bool TrackSegmentAssociationTool::associatedSegments(const xAOD::Muon& muon, 
                                                       std::vector< ElementLink<xAOD::MuonSegmentContainer> >& associatedSegments ) const {

    // don't do this for MuGirlLowBeta
    if( muon.author() == xAOD::Muon::MuGirlLowBeta ){
      return false;
    }
    
    // get segments 
    std::string location = (muon.author() == xAOD::Muon::MuGirlLowBeta) ? m_stauSegmentLocation : m_segmentLocation;
    const xAOD::MuonSegmentContainer* segments = retrieveSegments(location);
    if( !segments ) return false;

    // only fill if the primary track particle is not equal to the ID track particle and 
    // it has an associated track with track states 
    const xAOD::TrackParticle* tp = muon.primaryTrackParticle();        
    if( !tp || !tp->track() || !tp->track()->measurementsOnTrack() || tp == muon.trackParticle( xAOD::Muon::InnerDetectorTrackParticle) ) return false;

    const Trk::Track& track = *tp->track();
    
    struct MatchResult {
      ElementLink<xAOD::MuonSegmentContainer> link;
      MuonSegmentKey key;
      unsigned int numberOfMatchedMeasurements;
      MatchResult( ElementLink<xAOD::MuonSegmentContainer> link_, MuonSegmentKey key_, unsigned int numberOfMatchedMeasurements_ ):
        link(link_),key(key_),numberOfMatchedMeasurements(numberOfMatchedMeasurements_) {}

    };

    std::vector< MatchResult > matched_segs;

    ATH_MSG_VERBOSE("track " << m_printer->print(track) << std::endl << m_printer->printStations(track) );

    MuonSegmentKey trackKeys(tp->track()->measurementsOnTrack()->stdcont());

    CompareMuonSegmentKeys compareKeys;

    for( unsigned int i=0;i<segments->size(); ++i) {

      const xAOD::MuonSegment* curr_seg = (*segments)[i];
      if(!curr_seg) continue;

      // check element link to Muon::MuonSegment
      ElementLink< Trk::SegmentCollection > muonSegmentLink =  curr_seg->muonSegment();
      if( !muonSegmentLink.isValid() || !*muonSegmentLink ) {
        ATH_MSG_DEBUG("Segment without valid link");
        continue;
      }
      const MuonSegment* muonSegment = dynamic_cast<const MuonSegment*>(*muonSegmentLink);
      if( !muonSegment ) continue;
      MuonSegmentKey segmentKeys(*muonSegment);
      CompareMuonSegmentKeys::OverlapResult overlapResult = compareKeys(trackKeys,segmentKeys,true);
      if( overlapResult == CompareMuonSegmentKeys::NoOverlap ) continue;
      MatchResult matchResult(ElementLink<xAOD::MuonSegmentContainer>(*segments,i),segmentKeys,compareKeys.intersectionSize);
      matched_segs.push_back( matchResult );
      ATH_MSG_DEBUG ("numberOfMatchedMeasurements = " << matchResult.numberOfMatchedMeasurements );
    }
    ATH_MSG_DEBUG ("Number of matched space-time points = " << matched_segs.size());


    // refined selection
    std::vector<bool> matched_segs_isSelected;
    matched_segs_isSelected.assign(matched_segs.size(), bool(true));

    for(unsigned int i=0; i<matched_segs.size(); i++) {
      if(!matched_segs_isSelected[i]) continue;
      
      MatchResult& result_i = matched_segs[i];
      const xAOD::MuonSegment* seg_i = *result_i.link;
      int nMatched_i = result_i.numberOfMatchedMeasurements;
      float chi2PerDof_i = seg_i->chiSquared()/seg_i->numberDoF();

      for(unsigned int j=i+1; j<matched_segs.size(); j++) {
        if(!matched_segs_isSelected[j]) continue;

        MatchResult& result_j = matched_segs[j];
        const xAOD::MuonSegment* seg_j = *result_j.link;
        int nMatched_j = result_j.numberOfMatchedMeasurements;
        float chi2PerDof_j = seg_j->chiSquared()/seg_j->numberDoF();

        // In case the two segments have common hits:
        // 1) choose the one with higher number of matched hits to the track.
        // 2) if the numbers of matched hits are the same, choose the one with smaller chi2/DoF.
        CompareMuonSegmentKeys::OverlapResult overlapResult = compareKeys(result_i.key,result_j.key,true);
        if( overlapResult == CompareMuonSegmentKeys::NoOverlap ) continue;
        if(nMatched_j > nMatched_i) {
          matched_segs_isSelected[i] = false;
        } else if(nMatched_j < nMatched_i) {
          matched_segs_isSelected[j] = false;
        } else {
          if(chi2PerDof_j < chi2PerDof_i) matched_segs_isSelected[i] = false;
          else matched_segs_isSelected[j] = false;
        }

        ATH_MSG_VERBOSE ("Segments " << i << " and " << j << " have " << compareKeys.intersectionSize << "hit(s) in common.");
        if(matched_segs_isSelected[i]) ATH_MSG_VERBOSE ("Keeping segment " << i);
        else ATH_MSG_VERBOSE ("Keeping segment " << j);
      }
    }

    for(unsigned int i=0; i<matched_segs.size(); i++) {
      if(matched_segs_isSelected[i]) {
        associatedSegments.push_back(matched_segs.at(i).link);
      }
    }
    ATH_MSG_DEBUG ("Selected segments " << associatedSegments.size());

    return true;
  }


}
