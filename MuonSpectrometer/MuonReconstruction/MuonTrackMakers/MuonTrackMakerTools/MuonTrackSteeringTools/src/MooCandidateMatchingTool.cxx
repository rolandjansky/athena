/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MooCandidateMatchingTool.h"
#include "MuPatCandidateTool.h"
#include "MuonTrackFindingEvent/MuPatTrack.h"
#include "MuonTrackFindingEvent/MuPatCandidateBase.h"
#include "MuonTrackFindingEvent/MuPatSegment.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMatchingTool.h"
#include "MuonRecToolInterfaces/IMuonTrackSegmentMatchingTool.h"

#include "MuonSegment/MuonSegment.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/JacobianPhiThetaLocalAngles.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <sstream>

namespace { // local funcs
#if 0
inline double robust_acos(double x) {
  if ( x >=  1.0 ) return 0.0;
  if ( x <= -1.0 ) return M_PI;
  return std::acos(x);
}
#endif

// limit angle difference to -pi/2 < x <= pi/2
inline double theta_diff(double x) {
  while ( x <= -M_PI/2.0 ) x += M_PI;
  while ( x >  +M_PI/2.0 ) x -= M_PI;
  return x;
}

// limit angle difference to -pi < x <= pi
inline double phi_diff(double x) {
  while ( x <= -M_PI ) x += 2.0 * M_PI;
  while ( x >  +M_PI ) x -= 2.0 * M_PI;
  return x;
}

  // print covariance and correlation on one line
  std::string printCovCorr( Amg::MatrixX& cov, const std::string& title, unsigned int prec = 3 ) {
    std::ostringstream oss;
    oss << title << "   " << Amg::toString(cov,prec) << std::endl;
    return oss.str();
  }

  
} // local namespace

namespace Muon {
  
  MooCandidateMatchingTool::MooTrackSegmentMatchResult::MooTrackSegmentMatchResult() {
    clear();
  }

  MooCandidateMatchingTool::MooTrackSegmentMatchResult::~MooTrackSegmentMatchResult() {
  }

  void MooCandidateMatchingTool::MooTrackSegmentMatchResult::clear() {
    TrackSegmentMatchResult::clear();
    MCTBTrack = 0;
    MCTBSegment = 0;
  }
  
  
  MooCandidateMatchingTool::MooCandidateMatchingTool(const std::string& t, const std::string& n, const IInterface* p)    
    : AthAlgTool(t,n,p),
      m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_slExtrapolator("Trk::Extrapolator/MuonStraightLineExtrapolator"),
      m_atlasExtrapolator("Trk::Extrapolator/AtlasExtrapolator"), 
      m_segmentMatchingTool("Muon::MuonSegmentMatchingTool/MuonSegmentMatchingTool"),
      m_segmentMatchingToolTight("Muon::MuonSegmentMatchingTool/MuonSegmentMatchingToolTight"),
      m_magFieldSvc("AtlasFieldSvc",n),
      m_candidateTool("Muon::MuPatCandidateTool/MuPatCandidateTool"),
      m_goodSegmentMatches(0),
      m_goodSegmentMatchesTight(0),
      m_segmentMatches(0),
      m_segmentMatchesTight(0),
      m_goodSegmentTrackMatches(0),
      m_goodSegmentTrackMatchesTight(0),
      m_sameSideOfPerigee(0),
      m_otherSideOfPerigee(0),
      m_sameSideOfPerigeeTrk(0),
      m_otherSideOfPerigeeTrk(0),
      m_segmentTrackMatches(0),
      m_segmentTrackMatchesTight(0),
      m_reasonsForMatchOk(TrackSegmentMatchResult::NumberOfReasons),
      m_reasonsForMatchNotOk(TrackSegmentMatchResult::NumberOfReasons)
  {
    for ( int i = 0; i < int(TrackSegmentMatchResult::NumberOfReasons); ++i ) {
      m_reasonsForMatchOk[i]    = 0;
      m_reasonsForMatchNotOk[i] = 0;
    }


    declareInterface<MooCandidateMatchingTool>(this);
    declareInterface<IMuonTrackSegmentMatchingTool>(this);
    declareProperty("SLExtrapolator",           m_slExtrapolator );
    declareProperty("Extrapolator",             m_atlasExtrapolator );
    declareProperty("MagFieldSvc",    m_magFieldSvc );
    declareProperty("RequireSameSide",          m_requireSameSide = false, "require entries to be on the same side of the Perigee or Calorimeter" );
    declareProperty("MinimumRadiusSideMatch",   m_minimumSideMatchRadius = 4000., "All intersects outside the radius will be accepted");
    declareProperty("CaloMatchZ",               m_caloMatchZ = 6500., "limit in Z to determine Calo crossing");
    declareProperty("MatchAngleXCut",           m_matchAngXCut = 0.1, "Local X (phi) angle difference cut for track-segment matching. Disabled if 0");
    declareProperty("MatchAngleYCut",           m_matchAngYCut = 0.01, "Local Y (theta) angle difference cut for track-segment matching. Disabled if 0");
    declareProperty("MatchAngleXPullCut",       m_matchAngXPullCut = 10.0, "Local X (phi) angle difference pull cut for track-segment matching. Disabled if 0");
    declareProperty("MatchAngleYPullCut",       m_matchAngYPullCut = 25.0, "Local Y (theta) angle difference pull cut for track-segment matching. Disabled if 0");
    declareProperty("MatchPosXCut",             m_matchPosXCut =  100.0, "Local X (2nd coord) position difference cut (mm) for track-segment matching. Disabled if 0");
    declareProperty("MatchPosYCut",             m_matchPosYCut =  30.0, "Local Y (precision) position difference cut (mm) for track-segment matching. Disabled if 0");
    declareProperty("MatchPosPullCut",          m_matchPosPullCut = 10.0, "Local position pull cut for track-segment matching. Disabled if 0");
    declareProperty("MatchChiSquaredCut",       m_matchChiSquaredCut = 100.0, "Cut on the track-segment matching chi-squared");
    declareProperty("MatchChiSquaredCutTight",  m_matchChiSquaredCutTight = 25.0, "Cut on the track-segment matching chi-squared in case a tight cut is requested");
    declareProperty("AlignmentErrorPosX",       m_alignErrorPosX = 0.0, "Alignment precision in local X direction");
    declareProperty("AlignmentErrorPosY",       m_alignErrorPosY = 0.0, "Alignment precision in local Y direction");
    declareProperty("AlignmentErrorAngleX",     m_alignErrorAngleX = 0.0, "Alignment precision in local X angle");
    declareProperty("AlignmentErrorAngleY",     m_alignErrorAngleY = 0.0, "Alignment precision in local Y angle");
    declareProperty("SegmentMatchingTool",      m_segmentMatchingTool);
    declareProperty("SegmentMatchingToolTight", m_segmentMatchingToolTight);
    declareProperty("DoTrackSegmentMatching",   m_doTrackSegmentMatching = false, "Apply dedicated track-segment matching");
    declareProperty("TrackSegmentPreMatching",  m_trackSegmentPreMatchingStrategy = 0, "0=no segments match,1=any segment match,2=all segment match");
  }

  MooCandidateMatchingTool::~MooCandidateMatchingTool() { }
    
  StatusCode MooCandidateMatchingTool::initialize(){

    ATH_CHECK( m_slExtrapolator.retrieve() );
    ATH_CHECK( m_atlasExtrapolator.retrieve() );
    ATH_CHECK( m_idHelperTool.retrieve() );
    ATH_CHECK( m_helperTool.retrieve() );
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_magFieldSvc.retrieve() );
    ATH_CHECK( m_segmentMatchingTool.retrieve() );
    ATH_CHECK( m_segmentMatchingToolTight.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode MooCandidateMatchingTool::finalize() {
    
    int segmentMatchesLoose     = m_segmentMatches - m_segmentMatchesTight;
    int goodSegmentMatchesLoose = m_goodSegmentMatches - m_goodSegmentMatchesTight;
    double sameScale  = m_sameSideOfPerigee   != 0   ? m_segmentMatches / m_sameSideOfPerigee : 1.;
    double scale      = m_segmentMatches      != 0   ? 1./((double)m_segmentMatches)      : 1.;
    double scaleTight = m_segmentMatchesTight != 0   ? 1./((double)m_segmentMatchesTight) : 1.;
    double scaleLoose =   segmentMatchesLoose != 0.0 ? 1./((double)segmentMatchesLoose)   : 1.;
    msg(MSG::INFO) << std::fixed << std::setprecision(3);
    msg(MSG::INFO) << "Segment/segment matches: total " << std::setw(7) << m_segmentMatches
                   << "  tight " << std::setw(7) << m_segmentMatchesTight << endmsg
                   << " same side  " << std::setw(7) << m_sameSideOfPerigee       << " fraction " << m_sameSideOfPerigee*scale << endmsg
                   << " other side " << std::setw(7) << m_otherSideOfPerigee      << " fraction " << m_otherSideOfPerigee*scale << endmsg
                   << " good total " << std::setw(7) << m_goodSegmentMatches      << " fraction " << m_goodSegmentMatches*scale*sameScale << endmsg
                   << " good loose " << std::setw(7) <<   goodSegmentMatchesLoose << " fraction " <<   goodSegmentMatchesLoose*scaleLoose*sameScale << endmsg
                   << " good tight " << std::setw(7) << m_goodSegmentMatchesTight << " fraction " << m_goodSegmentMatchesTight*scaleTight*sameScale << endmsg;

    int segmentTrackMatchesLoose     = m_segmentTrackMatches - m_segmentTrackMatchesTight;
    int goodSegmentTrackMatchesLoose = m_goodSegmentTrackMatches - m_goodSegmentTrackMatchesTight;
    double sameScaleTrack  = m_sameSideOfPerigeeTrk     != 0   ? m_segmentTrackMatches / m_sameSideOfPerigeeTrk : 1.;
    double scaleTrack      = m_segmentTrackMatches      != 0   ? 1./((double)m_segmentTrackMatches)      : 1.;
    double scaleTrackTight = m_segmentTrackMatchesTight != 0   ? 1./((double)m_segmentTrackMatchesTight) : 1.;
    double scaleTrackLoose =   segmentTrackMatchesLoose != 0.0 ? 1./((double)segmentTrackMatchesLoose)   : 1.;
    msg(MSG::INFO) << "Track/segment matches:   total " << std::setw(7) << m_segmentTrackMatches
          << "  tight " << std::setw(7) << m_segmentTrackMatchesTight << endmsg
          << " same side  " << std::setw(7) << m_sameSideOfPerigeeTrk         << " fraction " << m_sameSideOfPerigeeTrk*scaleTrack << endmsg
          << " other side " << std::setw(7) << m_otherSideOfPerigeeTrk        << " fraction " << m_otherSideOfPerigeeTrk*scaleTrack << endmsg
          << " good total " << std::setw(7) << m_goodSegmentTrackMatches      << " fraction " << m_goodSegmentTrackMatches*scaleTrack*sameScaleTrack << endmsg
          << " good loose " << std::setw(7) <<   goodSegmentTrackMatchesLoose << " fraction " <<   goodSegmentTrackMatchesLoose*scaleTrackLoose*sameScaleTrack << endmsg
          << " good tight " << std::setw(7) << m_goodSegmentTrackMatchesTight << " fraction " << m_goodSegmentTrackMatchesTight*scaleTrackTight*sameScaleTrack << endmsg;
    // get printing width
    unsigned int nReasons = (int)TrackSegmentMatchResult::NumberOfReasons;
    unsigned int width = 0;
    for ( unsigned int i = 0; i < nReasons ; ++i ) {
      unsigned int w = TrackSegmentMatchResult::reasonString(TrackSegmentMatchResult::Reason(i)).length();
      if ( w > width ) width = w;
    }
    // print it
    msg(MSG::INFO) << " Reasons for match failures:" << endmsg;
    for ( unsigned int i = 0; i < nReasons ; ++i ) {
      int cnt = m_reasonsForMatchNotOk[i];
      TrackSegmentMatchResult::Reason reason = TrackSegmentMatchResult::Reason(i);
      if ( cnt > 0 ) msg(MSG::INFO) << "  " << std::left << std::setw(width) << TrackSegmentMatchResult::reasonString(reason) << std::right << ": " << cnt << endmsg;
    }
    msg(MSG::INFO) << " Reasons for match successes:" << endmsg;
    for ( unsigned int i = 0; i < nReasons ; ++i ) {
      int cnt = m_reasonsForMatchOk[i];
      TrackSegmentMatchResult::Reason reason = TrackSegmentMatchResult::Reason(i);
      if ( cnt > 0 ) msg(MSG::INFO) << "  " << std::left << std::setw(width) << TrackSegmentMatchResult::reasonString(reason) << std::right << ": " << cnt << endmsg;
    }


    return StatusCode::SUCCESS;
  }



  

  bool MooCandidateMatchingTool::match( const MuPatSegment& entry1, const MuPatSegment& entry2, bool useTightCuts ) const {
    ++m_segmentMatches;

    // same segments should never be matched!
    if ( &entry1 == &entry2 ) {
      ATH_MSG_DEBUG("Matching segment with itself: " << entry1.name << ". Should not happen!. Returning false.");
      return false;
    }

    // check whether SL overlap => same station index, just one station
    bool isSLOverlap = false;
    if( entry1.stations().size() == 1 && entry2.stations().size() == 1 && entry1.stations().count(*entry2.stations().begin()) ) isSLOverlap = true;
    if( isSLOverlap ){
      ATH_MSG_DEBUG("Small/large overlap");
      // don't combined CSC and MDT segments
      if ( entry1.isMdt != entry2.isMdt ) {
        ATH_MSG_DEBUG(" mdt/csc mix ignored" );
        return false;
      }
      
      if ( !areInNeighbouringChambers( entry1, entry2 ) ) {
        ATH_MSG_DEBUG(" not in neighbouring chambers" );
        return false;
      }
      
      if( !checkSegmentDistance( entry1, entry2 ) ){
        ATH_MSG_DEBUG(" distance between segments too large" );
        return false;
      }
      
      //check whether there are hits in the same chamber layer
      std::vector<MuonStationIndex::ChIndex> intersection;
      std::set_intersection(entry1.chambers().begin(),entry1.chambers().end(),
                            entry2.chambers().begin(),entry2.chambers().end(),
                            std::back_inserter(intersection));
      
      if( !intersection.empty() ){
        ATH_MSG_DEBUG(" Segments are in the same chamber " );
        return false;
      }

      if( m_idHelperTool->issTgc(*entry1.chamberIds().begin()) || m_idHelperTool->isMM(*entry1.chamberIds().begin()) ){
	return true;
      }
    }

    ATH_MSG_VERBOSE("Matching segments: " << entry1.name << " and " << entry2.name); 

    if ( useTightCuts ) ++m_segmentMatchesTight;

    if( m_requireSameSide && !sameSide(entry1,entry2,useTightCuts) ) {
      ++m_otherSideOfPerigee;
      return false;
    }else{
      ++m_sameSideOfPerigee;
    }

    // call segment matching tool
    bool match = true;
    if (useTightCuts) {
      match = m_segmentMatchingToolTight->match(*entry1.segment,*entry2.segment);
    } else {
      match = m_segmentMatchingTool->match(*entry1.segment,*entry2.segment);
    }
    if( match ) {
      ++m_goodSegmentMatches;
      if ( useTightCuts ) ++m_goodSegmentMatchesTight;
    }

    return match;
  }

  bool MooCandidateMatchingTool::match( const Trk::Track& track, const MuonSegment& segment, bool useTightCuts ) const {

    ATH_MSG_DEBUG("Match track/segment: useTightCuts " << useTightCuts);
    // convert segment and track
    MuPatTrack* candidate = m_candidateTool->createCandidate(&track);
    if( !candidate ) { 
      ATH_MSG_VERBOSE("Failed to create track candidate");
     return false;
    }

    MuPatSegment* segInfo = m_candidateTool->createSegInfo(segment);
    if( !segInfo ) {
      ATH_MSG_VERBOSE("Failed to create segment candidate");
      candidate->releaseTrack();
      delete candidate;
      return false;
    }

    // call match
    bool ok = match(*candidate,*segInfo,useTightCuts);
    ATH_MSG_DEBUG("Match track/segment: result " << ok);
    candidate->releaseTrack();
    delete candidate;
    delete segInfo;
    
    // return result
    return ok;
  }


  bool MooCandidateMatchingTool::match( const MuPatTrack& entry1, const MuPatSegment& entry2, bool useTightCuts ) const {
    ++m_segmentTrackMatches;
    if ( useTightCuts ) ++m_segmentTrackMatchesTight;

    ATH_MSG_VERBOSE("Matching track: " << entry1.segmentNames() << " with segment: " << entry2.name);

    if( m_requireSameSide && !sameSide(entry1,entry2,useTightCuts) ) {
      ++m_otherSideOfPerigeeTrk;
      return false;
    }else{
      ++m_sameSideOfPerigeeTrk;
    }


    // pre-matching using segment-segment matching
    // 0=no segments match,1=any segment match,2=all segment match

    // First pass filtering: call segment matching tool for all segment on candidate
    bool haveMatch = true;
    if ( m_trackSegmentPreMatchingStrategy > 0 ) {
      bool haveAllMatch = true;
      bool haveAnyMatch = false;
      const std::vector<MuPatSegment*>& segments = entry1.segments();
      std::vector<MuPatSegment*>::const_iterator it = segments.begin(), it_end = segments.end();
      for ( ; it != it_end; ++it ) {
        if ( !match( **it, entry2 ) ) {
          haveAllMatch = false;
        } else {
          haveAnyMatch = true;
        }
      }
      if ( m_trackSegmentPreMatchingStrategy == 1 ) {
        haveMatch = haveAnyMatch;
      } else if ( m_trackSegmentPreMatchingStrategy == 2 ) {
        haveMatch = haveAllMatch;
      }

      if ( !haveMatch ) {
        ATH_MSG_VERBOSE("track-segment match: -> Failed in comparing segments on track");
        
        
        ++m_reasonsForMatchNotOk[TrackSegmentMatchResult::SegmentMatch];
        return false;
      }
    }

    if ( m_doTrackSegmentMatching ) {
      MooTrackSegmentMatchResult info;
      calculateTrackSegmentMatchResult( entry1, entry2, info );
      TrackSegmentMatchCuts cuts = getMatchingCuts( entry1, entry2, useTightCuts );
      haveMatch = applyTrackSegmentCuts( info, cuts );
      // update counters
      if (haveMatch) {
        ++m_reasonsForMatchOk[info.reason];
      } else {
        ++m_reasonsForMatchNotOk[info.reason];
      }
    }

    if( haveMatch ) {
      ++m_goodSegmentTrackMatches;
      if ( useTightCuts ) ++m_goodSegmentTrackMatchesTight;
    }

    return haveMatch;
  }


  TrackSegmentMatchCuts MooCandidateMatchingTool::getMatchingCuts( const MuPatTrack& /*entry1*/,
                                                                const MuPatSegment& entry2,
                                                                bool useTightCuts ) const {
    TrackSegmentMatchCuts cuts;
    cuts.useTightCuts  = useTightCuts;
    cuts.posXCut       = m_matchPosXCut;
    cuts.posYCut       = m_matchPosYCut;
    cuts.posXPullCut   = m_matchPosPullCut;
    cuts.posYPullCut   = m_matchPosPullCut;
    cuts.angleXCut     = m_matchAngXCut;
    cuts.angleYCut     = m_matchAngYCut;
    cuts.angleXPullCut = m_matchAngXPullCut;
    cuts.angleYPullCut = m_matchAngYPullCut;
    if ( useTightCuts ) {
      cuts.matchChiSquaredCut = m_matchChiSquaredCutTight;
    } else {
      cuts.matchChiSquaredCut = m_matchChiSquaredCut;
    }
    
    cuts.cutOnMatchChiSquared = true;
    if ( entry2.containsChamber(MuonStationIndex::CSS) || entry2.containsChamber(MuonStationIndex::CSL) ) {
      // CSC
      cuts.cutOnPosX = true;
      cuts.cutOnPosY = true;
      cuts.cutOnPosXPull = true;
      cuts.cutOnPosYPull = true;
      cuts.cutOnAngleX = true;
      cuts.cutOnAngleY = true;
      cuts.cutOnAngleXPull = true;
      cuts.cutOnAngleYPull = true;
    } else { // ! containsChamber(CSS || CSL)
      // MDT
      cuts.cutOnPosX = false;
      cuts.cutOnPosY = true;
      cuts.cutOnPosXPull = false;
      cuts.cutOnPosYPull = true;
      cuts.cutOnAngleX = false;
      cuts.cutOnAngleY = true;
      cuts.cutOnAngleXPull = false;
      cuts.cutOnAngleYPull = true;
    }
    
    if ( cuts.posXCut   == 0.0 ) cuts.cutOnPosX = false;
    if ( cuts.posYCut   == 0.0 ) cuts.cutOnPosY = false;
    if ( cuts.angleXCut == 0.0 ) cuts.cutOnAngleX = false;
    if ( cuts.angleYCut == 0.0 ) cuts.cutOnAngleY = false;
    if ( cuts.posXPullCut   == 0.0 ) cuts.cutOnPosXPull = false;
    if ( cuts.posYPullCut   == 0.0 ) cuts.cutOnPosYPull = false;
    if ( cuts.angleXPullCut == 0.0 ) cuts.cutOnAngleXPull = false;
    if ( cuts.angleYPullCut == 0.0 ) cuts.cutOnAngleYPull = false;
    if ( cuts.matchChiSquaredCut == 0.0 ) cuts.cutOnMatchChiSquared = false;
    
    return cuts;

  }

  bool MooCandidateMatchingTool::applyTrackSegmentCuts( MooTrackSegmentMatchResult& info, const TrackSegmentMatchCuts& cuts ) const {
    
    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << MSG::DEBUG << "track segment match:";


    if ( info.reason == TrackSegmentMatchResult::NoMomentumWithMagField ) {
      // for tracks that have no momentum, but need a curved match
      // fall-back to segment matching with the closest segment
      MuPatSegment* closestSegment = 0;
      double closestSegmentDist = 1E9;
      std::vector<MuPatSegment*>::const_iterator itS = info.MCTBTrack->segments().begin(), itS_end = info.MCTBTrack->segments().end();
      for ( ; itS != itS_end; ++itS ) {
        // calculate distance along the direction of the segment on the track
        double dist = (info.MCTBSegment->entryPars().position() - (*itS)->entryPars().position()) .dot((*itS)->entryPars().momentum().unit());
        // store closest distance
        if ( std::abs(dist) < std::abs(closestSegmentDist) ) {
          closestSegmentDist = dist;
          closestSegment = *itS;
        }
      }
      
      if ( !closestSegment ) {
        info.reason = TrackSegmentMatchResult::NoClosestSegment;
        info.matchOK = true;
      } else { // closestSegment
        // call segment matching tool
        if (cuts.useTightCuts) {
          info.matchOK = m_segmentMatchingToolTight->match( *(closestSegment->segment), *(info.segment) );
        } else {
          info.matchOK = m_segmentMatchingTool->match( *(closestSegment->segment), *(info.segment) );
        }
        if ( msgLvl(MSG::DEBUG) ) {
          if (info.matchOK) {
            msg(MSG::DEBUG) << MSG::DEBUG << " -> Passed";
          } else {
            msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed";
          }
          msg(MSG::DEBUG) << " segment match between segment-on-track " << MuonStationIndex::chName(m_idHelperTool->chamberIndex(info.trackChamberId)) 
                 << " and segment " <<  MuonStationIndex::chName(m_idHelperTool->chamberIndex(info.segmentChamberId)) << endmsg;
        }
        info.reason = TrackSegmentMatchResult::SegmentMatch;
        info.trackChamberId = closestSegment->chid;
      } // else closestSegment

      return info.matchOK;
    } // if ( info.reason == TrackSegmentMatchResult::NoMomentumWithMagField )


    if ( msgLvl(MSG::VERBOSE) ) {
      msg(MSG::DEBUG) << MSG::VERBOSE << std::fixed;
      if (info.havePosXError) {
        double diff = info.localPosXDiff;
        double totErr = std::sqrt(info.posXTotalErr2);
        double pull = totErr != 0.0 ? diff / totErr : 0.0;
        msg(MSG::DEBUG) << std::endl << std::setprecision(3) << "deltaPosX  =" << std::setw(8) << diff
               << " +- " << std::left << std::setw(8) << totErr << std::right
               << std::setprecision(2) << " pull=" << std::setw(5) << pull
               << std::setprecision(3) << " (align=" << m_alignErrorPosX << " meas=" << std::sqrt(info.posXMeasErr2)
               << " pred=" << std::sqrt(info.posXPredErr2) << ")";
      }
      if (info.havePosYError) {
        double diff = info.localPosYDiff;
        double totErr = std::sqrt(info.posYTotalErr2);
        double pull = totErr != 0.0 ? diff / totErr : 0.0;
        msg(MSG::DEBUG) << std::endl << std::setprecision(3) << "deltaPosY  =" << std::setw(8) << diff
               << " +- " << std::left << std::setw(8) << totErr << std::right
               << std::setprecision(2) << " pull=" << std::setw(5) << pull
               << " (align=" << m_alignErrorPosY << " meas=" << std::sqrt(info.posYMeasErr2)
               << " pred=" << std::sqrt(info.posYPredErr2) << ")";
      }
      if (info.haveAngleXError) {
        double diff = info.localAngleXDiff;
        double totErr = std::sqrt(info.angleXTotalErr2);
        double pull = totErr != 0.0 ? diff / totErr : 0.0;
        msg(MSG::DEBUG) << std::endl << std::setprecision(5) << "deltaAngleX=" << std::setw(8) << diff
               << " +- " << std::left << std::setw(8) << totErr << std::right
               << std::setprecision(2) << " pull=" << std::setw(5) << pull
               << std::setprecision(5) << " (align=" << m_alignErrorAngleX << " meas=" << std::sqrt(info.angleXMeasErr2)
               << " pred=" << std::sqrt(info.angleXPredErr2) << ")";
      }
      if (info.haveAngleYError) {
        double diff = info.localAngleYDiff;
        double totErr = std::sqrt(info.angleYTotalErr2);
        double pull = totErr != 0.0 ? diff / totErr : 0.0;
        msg(MSG::DEBUG) << std::endl << std::setprecision(5) << "deltaAngleY=" << std::setw(8) << diff
               << " +- " << std::left << std::setw(8) << totErr << std::right
               << std::setprecision(2) << " pull=" << std::setw(5) << pull
               << std::setprecision(5) << " (align=" << m_alignErrorAngleY << " meas=" << std::sqrt(info.angleYMeasErr2)
               << " pred=" << std::sqrt(info.angleYPredErr2) << ")";
      }
      
      msg(MSG::DEBUG) << std::endl << std::setprecision(5) << "Difference vector:" << info.diffVector;
      msg(MSG::DEBUG) << printCovCorr( info.measuredCovariance, "Measured" ) 
             << printCovCorr( info.predictionCovariance, "Predicted" )
             << printCovCorr( info.totalCovariance, "Total" )
             << "Match chi-squared: " << info.matchChiSquared << " sqrt=" << std::sqrt(info.matchChiSquared);

      // finish on new line
      if ( info.havePosXError || info.havePosYError || info.haveAngleXError || info.haveAngleYError ) {
        msg(MSG::DEBUG) << std::endl;
      }
    }

    //
    // Start checking all cuts
    //
    info.resetCuts();
    //
    // apply cut on positions
    //
    double scaledPosXErr2 = cuts.posXPullCut * cuts.posXPullCut * info.posXTotalErr2;
    
    // cut on absolute X position
    if ( cuts.cutOnPosX && info.havePosX ) {
      double posXCut2 = cuts.posXCut * cuts.posXCut + scaledPosXErr2;
      if ( info.localPosXDiff * info.localPosXDiff > posXCut2 ) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName(m_idHelperTool->chamberIndex(info.segmentChamberId)) <<  " X position cut: "
                 << info.localPosXDiff << " > " << std::sqrt(posXCut2) << endmsg;
        }
        info.setCutFailed( TrackSegmentMatchResult::PosXCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::PosXCut );
      }
    }

    double scaledPosYErr2 = cuts.posYPullCut * cuts.posYPullCut * info.posYTotalErr2;

    // cut on absolute Y position
    if ( cuts.cutOnPosY && info.havePosY ) {
      double posYCut2 = cuts.posYCut * cuts.posYCut + scaledPosYErr2;
      if ( info.localPosYDiff * info.localPosYDiff > posYCut2 ) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName((m_idHelperTool->chamberIndex(info.segmentChamberId))) << " Y position cut: "
                 << info.localPosYDiff << " > " << std::sqrt(posYCut2) << endmsg;
        } 
        info.setCutFailed( TrackSegmentMatchResult::PosYCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::PosYCut );
      }
    }

    // cut on X position pull
    if ( cuts.cutOnPosXPull && info.havePosX && info.posXTotalErr2 > 0.0 ) {
      if ( info.localPosXDiff * info.localPosXDiff > scaledPosXErr2 ) {
        if (msgLvl(MSG::DEBUG)) {
          double pull = info.localPosXDiff / std::sqrt(info.posXTotalErr2);
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName((m_idHelperTool->chamberIndex(info.segmentChamberId))) << " X position pull cut: |"
                 << pull << "| > " << cuts.posXPullCut << endmsg;
        }
        info.setCutFailed( TrackSegmentMatchResult::PosXPullCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::PosXPullCut );
      }
    }

    // cut on Y position pull
    if ( cuts.cutOnPosYPull && info.havePosY && info.posYTotalErr2 > 0.0 ) {
      if ( info.localPosYDiff * info.localPosYDiff > scaledPosYErr2 ) {
        if (msgLvl(MSG::DEBUG)) {
          double pull = info.localPosYDiff / std::sqrt(info.posYTotalErr2);
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName((m_idHelperTool->chamberIndex(info.segmentChamberId))) << " Y position pull cut: |"
                 << pull << "| > " << cuts.posYPullCut << endmsg;
        }
        info.setCutFailed( TrackSegmentMatchResult::PosYPullCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::PosYPullCut );
      }
    }

    //
    // apply cut on angles
    //
    double scaledAngleXErr2 = cuts.angleXPullCut * cuts.angleXPullCut * info.angleXTotalErr2;
    
    // cut on absolute X angle
    if ( cuts.cutOnAngleX && info.haveAngleX ) {
      double angleXCut2 = cuts.angleXCut * cuts.angleXCut + scaledAngleXErr2;
      if ( info.localAngleXDiff * info.localAngleXDiff > angleXCut2 ) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName(m_idHelperTool->chamberIndex(info.segmentChamberId)) <<  " X angle cut: "
                 << info.localAngleXDiff << " > " << std::sqrt(angleXCut2) << endmsg;
        }
        info.setCutFailed( TrackSegmentMatchResult::AngXCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::AngXCut );
      }
    }

    double scaledAngleYErr2 = cuts.angleYPullCut * cuts.angleYPullCut * info.angleYTotalErr2;

    // cut on absolute Y angle
    if ( cuts.cutOnAngleY && info.haveAngleY ) {
      double angleYCut2 = cuts.angleYCut * cuts.angleYCut + scaledAngleYErr2;
      if ( info.localAngleYDiff * info.localAngleYDiff > angleYCut2 ) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName((m_idHelperTool->chamberIndex(info.segmentChamberId))) << " Y angle cut: "
                 << info.localAngleYDiff << " > " << std::sqrt(angleYCut2) << endmsg;
        }
        info.setCutFailed( TrackSegmentMatchResult::AngYCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::AngYCut );
      }
    }

    // cut on X angle pull
    if ( cuts.cutOnAngleXPull && info.haveAngleX && info.angleXTotalErr2 > 0.0 ) {
      if ( info.localAngleXDiff * info.localAngleXDiff > scaledAngleXErr2 ) {
        if (msgLvl(MSG::DEBUG)) {
          double pull = info.localAngleXDiff / std::sqrt(info.angleXTotalErr2);
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName((m_idHelperTool->chamberIndex(info.segmentChamberId))) << " X angle pull cut: |"
                 << pull << "| > " << cuts.angleXPullCut << endmsg;
        }
        info.setCutFailed( TrackSegmentMatchResult::AngXPullCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::AngXPullCut );
      }
    }

    if ( cuts.cutOnAngleYPull && info.haveAngleY && info.angleYTotalErr2 > 0.0 ) {
      // cut on Y angle pull
      if ( info.localAngleYDiff * info.localAngleYDiff > scaledAngleYErr2 ) {
        if (msgLvl(MSG::DEBUG)) {
          double pull = info.localAngleYDiff / std::sqrt(info.angleYTotalErr2);
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName((m_idHelperTool->chamberIndex(info.segmentChamberId))) << " Y angle pull cut: |"
                 << pull << "| > " << cuts.angleYPullCut << endmsg;
        }
        info.setCutFailed( TrackSegmentMatchResult::AngYPullCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::AngYPullCut );
      }
    }

    //
    // Cut on total chiSquared
    //
    if ( cuts.cutOnMatchChiSquared && info.haveMatchChiSquared ) {
      // cut on matching chi-squared
      if ( info.matchChiSquared > cuts.matchChiSquaredCut ) {
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed " << MuonStationIndex::chName((m_idHelperTool->chamberIndex(info.segmentChamberId))) << " match chi-squared cut: "
                 << info.matchChiSquared << " > " << cuts.matchChiSquaredCut << endmsg;
        }
        info.setCutFailed( TrackSegmentMatchResult::MatchChiSquaredCut );
      } else {
        info.setCutPassed( TrackSegmentMatchResult::MatchChiSquaredCut );
      }
    }
    
    //
    // Based on all cuts, decide to accept or reject
    //
    
    // match is OR of several matches
    info.matchOK = false;
    // if any cut was applied
    if ( info.appliedAnyCut() ) {
      if ( info.passedAllCuts() ) {
        info.matchOK = true;
        info.reason = TrackSegmentMatchResult::PassedAllCuts;
        if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << MSG::DEBUG << " -> Passed all cuts";
      } else if ( info.passedCut( TrackSegmentMatchResult::MatchChiSquaredCut ) ) {
        info.matchOK = true;
        info.reason = TrackSegmentMatchResult::PassedMatchChiSquaredCut;
        if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << MSG::DEBUG << " -> Passed match chi-quared cut"; 
      } else if ( 
		  ( info.passedCut(TrackSegmentMatchResult::PosXCut) || info.passedCut(TrackSegmentMatchResult::PosXPullCut) ) &&
                  ( info.passedCut(TrackSegmentMatchResult::PosYCut) || info.passedCut(TrackSegmentMatchResult::PosYPullCut) ) &&
                  ( info.passedCut(TrackSegmentMatchResult::AngXCut) || info.passedCut(TrackSegmentMatchResult::AngXPullCut) ) &&
		  ( info.passedCut(TrackSegmentMatchResult::AngYCut) || info.passedCut(TrackSegmentMatchResult::AngYPullCut) ) ) {
        info.matchOK = true;
        info.reason = TrackSegmentMatchResult::PassedPosAngleCuts;
        if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << MSG::DEBUG << " -> Passed cuts " << info.passedCutsString();
        if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed cuts " << info.failedCutsString();
      } else {
        info.reason = TrackSegmentMatchResult::FailedCuts;
        if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << MSG::DEBUG << " -> Failed cuts " << info.failedCutsString();
      }
    } else { // !info.appliedCuts
      if ( info.reason == TrackSegmentMatchResult::Unknown ) {
        info.reason = TrackSegmentMatchResult::NoCutsApplied;
        if ( msgLvl(MSG::DEBUG) ) {
          msg(MSG::DEBUG) << MSG::DEBUG << ". No cuts applied";
        } // if msgLvl(MSG::DEBUG)
      } // if reason == Unknown
    } // !appliedCuts

    if ( msgLvl(MSG::DEBUG) ) {
      if ( info.matchOK ) {
        msg(MSG::DEBUG) << MSG::DEBUG << " -> Accepted because " << info.reasonString() << endmsg;
      } else {
        msg(MSG::DEBUG) << MSG::DEBUG << " -> Rejected because " << info.reasonString() << endmsg;
      }
    }

    return info.matchOK;
  } // applyTrackSegmentCuts()


  void MooCandidateMatchingTool::calculateTrackSegmentMatchResult( const MuPatTrack& entry1, const MuPatSegment& entry2, MooTrackSegmentMatchResult& info ) const {
    info.clear();
    info.MCTBTrack = &entry1;
    info.MCTBSegment = &entry2;
    info.track = &entry1.track();
    info.segment = entry2.segment;
    info.segmentChamberId = entry2.chid;
    if ( !entry2.segment ) {
      ATH_MSG_WARNING( __FUNCTION__ << " entry2 does not have segment pointer.");
      info.reason = TrackSegmentMatchResult::NoSegmentPointer;
      return;
    }

    int sector2 = m_idHelperTool->sector(info.segmentChamberId);
    bool hasStereoAngle = false;
    
    // find closest track parameters
    const Trk::TrackParameters* closestPars = 0;
    double closestParsDist = 1E9;
    const Trk::TrackParameters* closestMeasPars = 0;
    double closestMeasParsDist = 1E9;
    //const Trk::TrackStateOnSurface* closestTSOS = 0;
    Identifier closestId;
    double closestIdDist = 1E9;
    bool trackHasPhi = true;
    
    // loop over TSOS
    const DataVector<const Trk::TrackStateOnSurface>* tsoses = entry1.track().trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tit = tsoses->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tit_end = tsoses->end();
    for( ;tit!=tit_end;++tit ){

      const Trk::MeasurementBase* meas = (*tit)->measurementOnTrack();

      // do not want to start from a pseudo-measurement
      const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas);
      if( pseudo ) {
        trackHasPhi = false;
        continue;
      }

      // do not want to start from non-MS measurements
      Identifier id;
      if (meas) {
        id = m_helperTool->getIdentifier(*meas);
        if ( id.is_valid() ) {
          if ( !m_idHelperTool->isMuon(id) ) continue;
          if ( m_idHelperTool->isMdt(id) && m_idHelperTool->sector(id) != sector2 ) hasStereoAngle=true;
        }
      }

      const Trk::TrackParameters* pars = (*tit)->trackParameters();
      if( !pars ){
        continue;
      }

      // calculate distance along the direction of the track
      double dist = (entry2.entryPars().position() - pars->position()).dot(entry1.entryPars().momentum().unit());
      // store closest distance
      if ( std::abs(dist) < std::abs(closestParsDist) ) {
        //closestTSOS = *tit;
        closestParsDist = dist;
        closestPars = pars;
        if (pars->covariance()) {
          closestMeasParsDist = dist;
          closestMeasPars = pars;
        }
      }
      // store closest MDT or CSC ID (for printout and performance analysis)
      if ( std::abs(dist) < std::abs(closestIdDist) ) {
        if ( meas && id.is_valid() && m_idHelperTool->isMuon(id) && (m_idHelperTool->isMdt(id) || m_idHelperTool->isCsc(id)) ) {
          closestId = id;
          closestIdDist = dist;
        }
      }
      
    }
    
    // update trackHasPhi in the cases where we do have enough phi constraints
    if( entry1.phiHits().size() > 1 ){
      // calculate difference between hits
      const Amg::Vector3D& gposFirstPhi = entry1.phiHits().front()->globalPosition();
      const Amg::Vector3D& gposLastPhi  = entry1.phiHits().back()->globalPosition();
      Amg::Vector3D globalDistance = gposLastPhi - gposFirstPhi;

      // calculate 'projective' distance
      double distance = entry1.hasEndcap() ? fabs(globalDistance.z()) : globalDistance.perp();
      
      if( distance > 500 ) trackHasPhi = true;
    }

    if ( !closestPars ) {
      ATH_MSG_DEBUG("track-segment match: No closest track parameters found.");
      info.reason = TrackSegmentMatchResult::NoClosestPars;
      return;
    }

    // set the identification of the first entry (the track - taking the closest point)
    if ( closestPars && closestId.is_valid() ) {
      info.trackChamberId = m_idHelperTool->chamberId( closestId );
    }

    if ( msgLvl(MSG::VERBOSE) ) {
      msg(MSG::DEBUG) << MSG::VERBOSE << "match Closest chamber: " << m_idHelperTool->toStringChamber(info.trackChamberId)
             << " Segment: " << m_idHelperTool->toStringChamber(info.segmentChamberId);
      const Trk::TrackParameters* tmpPars = 0;
      if (closestMeasPars) tmpPars = closestMeasPars->covariance() ? closestMeasPars : 0;
      if (tmpPars) {
        msg(MSG::DEBUG) << std::endl << "Closest measured track parameters: " << m_printer->print(*tmpPars) << std::endl << Amg::toString(*tmpPars->covariance(),10)
               << "  distance=" << closestMeasParsDist;
      }
      if (!tmpPars || tmpPars != closestPars) {
        msg(MSG::DEBUG) << std::endl << "Closest track parameters         : " << m_printer->print(*closestPars)
               << "  distance=" << closestParsDist;
      }
      msg(MSG::DEBUG) << endmsg;
    }
    
    bool straightLineMatch = !m_magFieldSvc->toroidOn();
    if ( hasStereoAngle && !trackHasPhi && (straightLineMatch || entry1.hasMomentum() ) ) {
      // can not do any extrapolation (not reliable)
      info.reason = TrackSegmentMatchResult::StereoAngleWithoutPhi;
      return;
    }
        


    //if extrapolating within EM-EO, can use straight-line extrapolator even without momentum
    if ( !straightLineMatch && !entry1.hasMomentum() && info.trackChamberId.is_valid() ) {
      MuonStationIndex::StIndex trackStationIndex = m_idHelperTool->stationIndex(info.trackChamberId);
      MuonStationIndex::StIndex segmentStationIndex = entry2.stIndex;
      if ( ((trackStationIndex == MuonStationIndex::EM && segmentStationIndex == MuonStationIndex::EO) ||
            (trackStationIndex == MuonStationIndex::EO && segmentStationIndex == MuonStationIndex::EM)    ) &&
           closestPars->position().z() * entry2.entryPars().position().z() > 0.0 ) {
        straightLineMatch = true;
	ATH_MSG_DEBUG("track in " << m_idHelperTool->toStringStation(info.trackChamberId) 
		      << " and segment in " << m_idHelperTool->toStringStation(info.segmentChamberId)
		      << " => doing straight line extrapolation match");
        }
      }
    

    //
    // for tracks that have a momentum or have straight line match
    //
    if ( straightLineMatch || entry1.hasMomentum() ) {
      
      // extrapolate to segment surface
      const Trk::IExtrapolator* extrapolator = 0;
      const Trk::TrackParameters* exPars = 0;
      const Trk::TrackParameters* exMeasPars = 0;
      if (closestMeasPars) {
        const Trk::TrackParameters* tmpPars = closestMeasPars->covariance() ? closestMeasPars : 0;
	
        if (tmpPars) {
	  
	  //Check sector+
	  if (!m_idHelperTool->isMuon(info.trackChamberId))
	    {
	      info.reason = TrackSegmentMatchResult::SegmentMatching;
	      return;
	    }
	  int trackSector   = m_idHelperTool->sector(info.trackChamberId);
	  int segmentSector = m_idHelperTool->sector(info.segmentChamberId);
	  int sectorDiff = std::abs( trackSector - segmentSector );
	  if ( sectorDiff > 1 && sectorDiff != 15 ) {
	    if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "track sector   =" << trackSector << " segment sector =" << segmentSector << " => not in neighbouring sectors " << endmsg;
	    
	    info.reason = TrackSegmentMatchResult::SegmentMatching;
	    return;
	  }
	  //Check sector-
	  
          if ( straightLineMatch && !entry1.hasMomentum() ) {
            extrapolator = &*(m_slExtrapolator);
            exPars = extrapolator->extrapolateDirectly(*tmpPars,entry2.segment->associatedSurface(),Trk::anyDirection,false,Trk::muon);
          } else {
            extrapolator = &(*m_atlasExtrapolator);
	    ATH_MSG_VERBOSE(" Extrapolating to other segment " << m_printer->print(*tmpPars) << std::endl << Amg::toString(*tmpPars->covariance(),10) );
            exPars = extrapolator->extrapolate(*tmpPars,entry2.segment->associatedSurface(),Trk::anyDirection,false,Trk::muon);
	  }
        }
        if ( !exPars ) {
         ATH_MSG_DEBUG("track-segment match: Failed to extrapolate measured track parameters\n"
			  << m_printer->print(*closestPars)
			  << "\nfrom " << m_idHelperTool->toStringChamber( info.trackChamberId ) 
			  << " to segment surface " << m_idHelperTool->toStringChamber( info.segmentChamberId )
		       << " using " << extrapolator->name() );
          info.matchOK = false;
          info.reason = TrackSegmentMatchResult::ExtrapolFailed;
          return;
        }
	
        exMeasPars = exPars->covariance() ? exPars : 0;
        if ( !exMeasPars ) {
          const Trk::IExtrapolator* extrapolator = straightLineMatch ? &(*m_slExtrapolator) : &(*m_atlasExtrapolator);
          ATH_MSG_DEBUG( "track-segment match: Did not get measured track parameters from extrapolation\n"
			 << "\nfrom " << m_idHelperTool->toStringChamber( info.trackChamberId ) 
			 << " to segment surface " << m_idHelperTool->toStringChamber( info.segmentChamberId )
			 << " using " << extrapolator->name() );
          delete exPars;
          info.reason = TrackSegmentMatchResult::ExtrapolNoErrors;
          return;
        }
	ATH_MSG_VERBOSE(" ExMeasParameters " << exMeasPars << " " <<  m_printer->print(*exMeasPars) << std::endl << Amg::toString(*exMeasPars->covariance(),10) );

      } else { //!closestMeasPars
        // no closest measured parameters, take closest parameters
	
	if ( straightLineMatch && !entry1.hasMomentum() ) {
          extrapolator = &*(m_slExtrapolator);
          exPars = extrapolator->extrapolateDirectly(*closestPars,entry2.segment->associatedSurface(),Trk::anyDirection,false,Trk::muon);
        } else {
          extrapolator = &(*m_atlasExtrapolator);
	  exPars = extrapolator->extrapolate(*closestPars,entry2.segment->associatedSurface(),Trk::anyDirection,false,Trk::muon);
	}
        if( !exPars ) {
         ATH_MSG_DEBUG("track-segment match: Failed to extrapolate track parameters without errors\n"
			  << m_printer->print(*closestPars)
			  << "\nfrom " << m_idHelperTool->toStringChamber( info.trackChamberId ) 
			  << " to segment surface " << m_idHelperTool->toStringChamber( info.segmentChamberId )
		       << " using " << extrapolator->name());
          info.matchOK = false;
          info.reason = TrackSegmentMatchResult::ExtrapolFailed;
          return;
        }
      }
      
      
      // get the available measured information by looking at the errors
      // check measurement errors
      const Amg::MatrixX& measErrors = entry2.segment->localCovariance();

      double posXMeasErr2 = measErrors(Trk::locX,Trk::locX);
      if ( posXMeasErr2 <= 999.0 ) {
        info.havePosX = true;
        info.havePosXError = true;
        info.posXMeasErr2   = posXMeasErr2;
        info.posXTotalErr2 += posXMeasErr2;
      }

      double posYMeasErr2 = measErrors(Trk::locY,Trk::locY);
      if ( info.posYMeasErr2 <= 999.0 ) {
        info.havePosY = true;
        info.havePosYError = true;
        info.posYMeasErr2   = posYMeasErr2;
        info.posYTotalErr2 += posYMeasErr2;
      }
      double phiMeasErr2   = measErrors(Trk::phi,Trk::phi);
      double thetaMeasErr2 = measErrors(Trk::theta,Trk::theta);
      if (phiMeasErr2   <= 999.0 ) {
        info.haveAngleX = true;
        info.haveAngleXError = true;
        info.angleXMeasErr2   = phiMeasErr2;
        info.angleXTotalErr2 += phiMeasErr2;
      }
      if (thetaMeasErr2 <= 999.0 ) {
        info.haveAngleY = true;
        info.haveAngleYError = true;
        info.angleYMeasErr2   = thetaMeasErr2;
        info.angleYTotalErr2 += thetaMeasErr2;
      }

      // require extrapolation errors to enable use of errors
      if ( !exMeasPars ) {
        info.haveAngleXError = false;
        info.haveAngleYError = false;
        info.havePosXError = false;
        info.havePosYError = false;
      }

      // calculate position difference
      info.localPosXDiff = exPars->parameters()[Trk::locX] - entry2.segment->localParameters()[Trk::locX];
      info.localPosYDiff = exPars->parameters()[Trk::locY] - entry2.segment->localParameters()[Trk::locY];
      
      // calculate angle difference
      Trk::LocalDirection locDirEx;
      entry2.segment->associatedSurface().globalToLocalDirection( exPars->momentum(),locDirEx );

      const Trk::LocalDirection& locDirSeg =  entry2.segment->localDirection();

      // angular residuals
      info.localAngleXDiff =   phi_diff(locDirEx.angleXZ() - locDirSeg.angleXZ());
      info.localAngleYDiff = theta_diff(locDirEx.angleYZ() - locDirSeg.angleYZ());
	
      AmgSymMatrix(5) localPredCovar;
      localPredCovar.setIdentity();
      // only calculate if needed
      if ( info.haveAngleXError || info.haveAngleYError || info.havePosXError || info.havePosYError ) {
        // Predicted angle errors are on global phi and theta. Need to convert to errors on local angles
        Trk::JacobianPhiThetaLocalAngles globalToLocalPredAnglesJacobian( exMeasPars->parameters()[Trk::phi],
                                                                          exMeasPars->parameters()[Trk::theta],
                                                                          exMeasPars->associatedSurface().transform().rotation().inverse() );

        // make the Jacobian to convert all in one go from global to local
        // so that the correlations are calculated correctly
	AmgSymMatrix(5) globalToLocalPredJacobian;
	globalToLocalPredJacobian.setIdentity();
        globalToLocalPredJacobian(Trk::locX,Trk::locX) = 1.0;
        globalToLocalPredJacobian(Trk::locY,Trk::locY) = 1.0;
        globalToLocalPredJacobian(Trk::phi,Trk::phi) = globalToLocalPredAnglesJacobian(0,0);
        globalToLocalPredJacobian(Trk::theta,Trk::theta) = globalToLocalPredAnglesJacobian(1,1);
        globalToLocalPredJacobian(Trk::theta,Trk::phi) = globalToLocalPredAnglesJacobian(0,1); // also fills (Trk::phi,Trk::theta)
        globalToLocalPredJacobian(Trk::phi,Trk::theta) = globalToLocalPredJacobian(Trk::theta,Trk::phi);
        globalToLocalPredJacobian(Trk::qOverP,Trk::qOverP) = 1.0;
	const AmgSymMatrix(5)& globalPredCovar = *exMeasPars->covariance();
        localPredCovar = globalPredCovar.similarity(globalToLocalPredJacobian);
      }
       
      if ( info.haveAngleXError && info.haveAngleYError && info.havePosXError && info.havePosYError ) {
        // Full 4D info available: CSC segments and MDT small-large overlap
        info.measuredCovariance   = measErrors.block<4,4>(0,0);//sub(1,4); hope this is ok ....
        info.predictionCovariance = localPredCovar.block<4,4>(0,0);//sub(1,4); hope this is ok ....
        info.totalCovariance = info.measuredCovariance + info.predictionCovariance;

        // add alignment errors
        info.totalCovariance(Trk::locX,Trk::locX)   += m_alignErrorPosX * m_alignErrorPosX;
        info.totalCovariance(Trk::locY,Trk::locY)   += m_alignErrorPosY * m_alignErrorPosY;
        info.totalCovariance(Trk::phi,Trk::phi)     += m_alignErrorAngleX * m_alignErrorAngleX;
        info.totalCovariance(Trk::theta,Trk::theta) += m_alignErrorAngleY * m_alignErrorAngleY;

        // fill difference vector
        Amg::VectorX diffVec(4);
        diffVec[Trk::locX]  = exPars->parameters()[Trk::locX]  - entry2.segment->localParameters()[Trk::locX];
        diffVec[Trk::locY]  = exPars->parameters()[Trk::locY]  - entry2.segment->localParameters()[Trk::locY];
        diffVec[Trk::phi]   = exPars->parameters()[Trk::phi]   - entry2.segment->localParameters()[Trk::phi];
        diffVec[Trk::theta] = exPars->parameters()[Trk::theta] - entry2.segment->localParameters()[Trk::theta];
        info.diffVector = diffVec;

        // finally calculate the match chi-squared
	Amg::MatrixX weightMatrix = info.totalCovariance.inverse();
        info.matchChiSquared = info.diffVector.transpose()*weightMatrix*info.diffVector;
        info.haveMatchChiSquared = true;

        // update separate angles (redunant with covar)
        info.posXPredErr2   = localPredCovar(Trk::locX,Trk::locX);
        info.posYPredErr2   = localPredCovar(Trk::locY,Trk::locY);
        info.angleXPredErr2 = localPredCovar(Trk::phi,Trk::phi);
        info.angleYPredErr2 = localPredCovar(Trk::theta,Trk::theta);
        // update total errors
        info.posXTotalErr2   += info.posXPredErr2;
        info.posYTotalErr2   += info.posYPredErr2;
        info.angleXTotalErr2 += info.angleXPredErr2;
        info.angleYTotalErr2 += info.angleYPredErr2;
        info.posXTotalErr2   += m_alignErrorPosX * m_alignErrorPosX;
        info.posYTotalErr2   += m_alignErrorPosY * m_alignErrorPosY;
        info.angleXTotalErr2 += m_alignErrorAngleX * m_alignErrorAngleX;
        info.angleYTotalErr2 += m_alignErrorAngleY * m_alignErrorAngleY;

      } else if ( info.haveAngleYError && info.havePosYError ) {
        // 2D Y info available:
        // normal (non-overlap) MDT segments have no X nor phi(angleXZ) measurement, and give error on local angleYZ

        // Y,angleYZ 2x2 sub-set of predicted covariance
	Amg::MatrixX predCovar(2,2);
	predCovar.setIdentity();
        predCovar(0,0) = localPredCovar(Trk::locY,Trk::locY); // Y
        predCovar(1,1) = localPredCovar(Trk::theta,Trk::theta); // angleYZ
        predCovar(1,0) = localPredCovar(Trk::theta,Trk::locY); // also sets (0,1)
	predCovar(0,1) = predCovar(1,0);
        info.predictionCovariance = predCovar;
        
        // Y,angleYZ 2x2 sub-set of measured covariance
	Amg::MatrixX measCovar(2,2);
	measCovar.setIdentity();
        measCovar(0,0) = measErrors(Trk::locY,Trk::locY);
        measCovar(1,1) = measErrors(Trk::theta,Trk::theta);
        measCovar(1,0) = measErrors(Trk::theta,Trk::locY);
	measCovar(0,1) = measCovar(1,0);

        info.measuredCovariance = measCovar;
        info.totalCovariance = info.predictionCovariance + info.measuredCovariance;
        // add alignment errors
        info.totalCovariance(0,0) += m_alignErrorPosY * m_alignErrorPosY;
        info.totalCovariance(1,1) += m_alignErrorAngleY * m_alignErrorAngleY;

        // now fill the difference vector
        Amg::VectorX diffVec(2);
        diffVec[0] = info.localPosYDiff;
        diffVec[1] = info.localAngleYDiff;
        info.diffVector = diffVec;
        
        // finally calculate the match chi-squared
	Amg::MatrixX weightMatrix = info.totalCovariance.inverse();
        info.matchChiSquared = info.diffVector.transpose()*weightMatrix*info.diffVector;
        info.haveMatchChiSquared = true;

        // fill all available information (redundant with covar)
        info.posXPredErr2   = localPredCovar(Trk::locX,Trk::locX);
        info.posYPredErr2   = localPredCovar(Trk::locY,Trk::locY);
        info.angleXPredErr2 = localPredCovar(Trk::phi,Trk::phi);
        info.angleYPredErr2 = localPredCovar(Trk::theta,Trk::theta);
        // only use Y/angleY
        info.posYTotalErr2   += info.posYPredErr2;
        info.angleYTotalErr2 += info.angleYPredErr2;
        info.posYTotalErr2   += m_alignErrorPosY * m_alignErrorPosY;
        info.angleYTotalErr2 += m_alignErrorAngleY * m_alignErrorAngleY;
       
      } else {
        info.reason = TrackSegmentMatchResult::NoMeasErrors;
      }

      delete exPars;
      exPars = 0;
      exMeasPars = 0; // points to the same object as exPars
    
    } else { //!straighLineMatch && !entry1.hasMomemtum()
      info.reason = TrackSegmentMatchResult::NoMomentumWithMagField;
      // 
      // Alternative method:
      // EM/EO -> EI
      // extrapolate (straight line) from EM/EO track and line to IP -> matching region

    } // else !straighLineMatch && !entry1.hasMomemtum()
	
    return;
  }


  bool MooCandidateMatchingTool::sameSide( const MuPatSegment& entry1, const MuPatSegment& entry2, bool sameSideOfPerigee ) const {
    ATH_MSG_VERBOSE("sameSide:  MuPatSegment, MuPatSegment");
    return sameSide(entry1.entryPars().position(), entry2.entryPars().position(), sameSideOfPerigee);
  }

  bool MooCandidateMatchingTool::sameSide( const MuPatTrack& entry1, const MuPatSegment& entry2, bool sameSideOfPerigee ) const {
    ATH_MSG_VERBOSE("sameSide:  MuPatTrack, MuPatSegment");
    return sameSide(entry1.entryPars().momentum().unit(), entry1.entryPars().position(), entry2.entryPars().position(), sameSideOfPerigee);
  }

  bool MooCandidateMatchingTool::sameSide( const MuPatTrack& entry1, const MuPatTrack& entry2, bool sameSideOfPerigee ) const {
    ATH_MSG_VERBOSE("sameSide:  MuPatTrack, MuPatTrack");
    return sameSide(entry1.entryPars().momentum().unit(), entry1.entryPars().position(), entry2.entryPars().position(), sameSideOfPerigee);
  }

  bool MooCandidateMatchingTool::sameSide( const Amg::Vector3D& pos1, const Amg::Vector3D& pos2, bool sameSideOfPerigee ) const {
    ATH_MSG_VERBOSE("sameSide:  position, position");
    Amg::Vector3D diffPos = (pos2-pos1).unit();
    return sameSide(diffPos, pos1, pos2, sameSideOfPerigee);
  }

  bool MooCandidateMatchingTool::sameSide( const Amg::Vector3D& dir, const Amg::Vector3D& pos1, const Amg::Vector3D& pos2, 
                                        bool requireSameSideOfPerigee ) const {
    // check if segments are on same side of the Perigee in the xy plane
    Amg::Vector3D dirxy = dir.unit();
    //dirxy.setZ(0);
    double distFromPerigee1 = pos1.dot(dirxy);
    double distFromPerigee2 = pos2.dot(dirxy);
    bool sameSideOfPerigee = distFromPerigee1*distFromPerigee2 > 0. ? true : false;
    bool same = sameSideOfPerigee;

    // if one segment in each endcap, then consider on different side
    bool inOppositeEndcap = false;
    double etaEndcap = 1.1;
    double eta1 = pos1.eta();
    double eta2 = pos2.eta();
    if ( (eta1 > +etaEndcap && eta2 < -etaEndcap) || (eta1 < -etaEndcap && eta2 > +etaEndcap) ) {
      inOppositeEndcap = true;
      same = false;
    }

    if( msgLvl(MSG::VERBOSE) ) {
      msg(MSG::DEBUG) << MSG::VERBOSE << "sameSide: Require same side of Perigee: " << requireSameSideOfPerigee;
      double sign1 = pos1.y() < 0 ? -1. : 1.;
      double sign2 = pos2.y() < 0 ? -1. : 1.;
      msg(MSG::DEBUG) << " Direction " << dir << std::fixed << std::setprecision(0)
             << std::endl << " pos1: dist " << std::setw(6) << distFromPerigee1 
             << " r " << std::setw(6) << pos1.perp()*sign1
             << " (x,y,z)=(" << std::setw(5) << pos1.x() << "," << std::setw(5) << pos1.y() << "," << std::setw(5) << pos1.z() << ")"
             << std::endl << " pos2: dist " << std::setw(6) << distFromPerigee2
             << " r " << std::setw(6) << pos2.perp()*sign2
             << " (x,y,z)=(" << std::setw(5) << pos2.x() << "," << std::setw(5) << pos2.y() << "," << std::setw(5) << pos2.z() << ")"
             << std::setprecision(6); // back to default

      if( sameSideOfPerigee ) {
        msg(MSG::DEBUG) << std::endl << " Same side of perigee.";
      } else {
        msg(MSG::DEBUG) << std::endl << " Other side of perigee.";
      }
      if ( inOppositeEndcap ) {
        msg(MSG::DEBUG) << " In opposite end-cap.";
      }
    }

    // if segments are on the same side of the Perigee, they are also on the same side of Calo, so can return
    if ( same ) {
      if ( msgLvl(MSG::VERBOSE) ) msg(MSG::DEBUG) << " => Accepted" << endmsg;
      return true;
    }
    
    // if requiring sameSideOfPerigee (stronger condition), then we are done here.
    if ( requireSameSideOfPerigee ) {
      if ( msgLvl(MSG::VERBOSE) ) msg(MSG::DEBUG) << " => Rejected" << endmsg;
      return false;
    }


    // Test on weaker requirement: accept if segments do not cross the calorimeter
    Amg::Vector3D intersect = pos1 - distFromPerigee1*dir;
    // Check on radial distance
    bool crossingCalo = intersect.perp() < m_minimumSideMatchRadius ? true : false;
    bool inSameEndcap = false;
    
    if( !crossingCalo ) return true;
    return false;

    // Not used code leave in for the moment coverity 14808

    // Check on Z position
    bool crossesCaloDiskA = false;
    bool crossesCaloDiskC = false;
    bool haveCaloDiskA = false;
    bool haveCaloDiskC = false;
    Amg::Vector3D caloDiskA;
    Amg::Vector3D caloDiskC;
    if ( crossingCalo ) {
      // crossing in R, now check if crossing in Z
      if ( (pos1.z() > +m_caloMatchZ && pos2.z() > +m_caloMatchZ) ||
           (pos1.z() < -m_caloMatchZ && pos2.z() < -m_caloMatchZ)    ) {
        // both in same end-cap, so not crossing calo
        inSameEndcap = true;
        crossingCalo = false;
      } else if ( (std::abs(pos1.z()) > m_caloMatchZ || std::abs(pos2.z()) > m_caloMatchZ) ) {
        // one segment inside, one segment outside, or one segment in each endcap
        // check crossing with calo endcap A surface (z > 0)
        double dz1 = pos1.z() - m_caloMatchZ;
        double dz2 = pos2.z() - m_caloMatchZ;
        if ( dz1 * dz2 <= 0 ) {
          // crossing Calo A side endcap
          crossesCaloDiskA = true;
          if ( std::abs(dir.z()) < 0.0001 ) { // protect against division by zero
            if ( dz1 > 0.0 ) crossingCalo = false; // vertical track starting outside calo
          } else {
            haveCaloDiskA = true;
            caloDiskA = pos1 - (dz1/dir.z()) * dir;
            // cross-check
            if ( std::abs(caloDiskA.z() - m_caloMatchZ) > 0.001 ) {
              if ( msgLvl(MSG::VERBOSE) ) msg(MSG::DEBUG) << endmsg; // finish printout before ERROR message
              msg(MSG::DEBUG) << MSG::ERROR << "Wrong calculation of crossing calo disk: expected z=" << m_caloMatchZ << " found z=" << caloDiskA.z() << endmsg;
            }
            if (caloDiskA.perp() > m_minimumSideMatchRadius) crossesCaloDiskA = false;
          }
        }
        // check crossing with calo endcap C surface (z < 0)
        dz1 = pos1.z() + m_caloMatchZ;
        dz2 = pos2.z() + m_caloMatchZ;
        if ( dz1 * dz2 <= 0 ) {
          // crossing Calo C side endcap
          crossesCaloDiskC = true;
          if ( std::abs(dir.z()) < 0.0001 ) { // protect against division by zero
            if ( dz1 < 0.0 ) crossingCalo = false; // vertical track starting outside calo
          } else {
            haveCaloDiskC = true;
            caloDiskC = pos1 - (dz1/dir.z()) * dir;
            // cross-check
            if ( std::abs(caloDiskC.z() + m_caloMatchZ) > 0.001 ) {
              if ( msgLvl(MSG::VERBOSE) ) msg(MSG::DEBUG) << endmsg; // finish printout before ERROR message
              msg(MSG::DEBUG) << MSG::ERROR << "Wrong calculation of crossing calo disk: expected z=" << -m_caloMatchZ << " found z=" << caloDiskC.z() << endmsg;
            }
            if (caloDiskC.perp() > m_minimumSideMatchRadius) crossesCaloDiskC = false;
          }
        }

        if ( !crossesCaloDiskA && !crossesCaloDiskC ) crossingCalo = false;

      } // if (one segment inside, one segment outside, or one segment in each endcap)
    }

    // accept if on same side of IP or out side the calo volume
    bool accepted = !crossingCalo;

    // some printout
    if( msgLvl(MSG::VERBOSE) ){
      msg(MSG::DEBUG) << MSG::VERBOSE << " Checking if crossing Calo:" << std::fixed << std::setprecision(0)
             << std::endl << " intersect: r " << std::setw(5) << intersect.perp() 
             << " (x,y,z)=(" << std::setw(5) << intersect.x() << "," << std::setw(5) << intersect.y() << "," << std::setw(5) << intersect.z() << ")"
             << " r_calo=" << m_minimumSideMatchRadius << " z_calo=" << m_caloMatchZ;
      if ( inSameEndcap ) {
        msg(MSG::DEBUG) << std::endl << " In same endcap";
      }
      if ( haveCaloDiskA ) {
        msg(MSG::DEBUG) << std::endl << " crossing disk A at" << " r " << std::setw(5) << caloDiskA.perp()
               << " (x,y,z)=(" << std::setw(5) << caloDiskA.x() << "," << std::setw(5) << caloDiskA.y() << "," << std::setw(5) << caloDiskA.z() << ")"
               << " outside=" << !crossesCaloDiskA;      
      }
      if ( haveCaloDiskC ) {
        msg(MSG::DEBUG) << std::endl << " crossing disk C at" << " r " << std::setw(5) << caloDiskC.perp()
               << " (x,y,z)=(" << std::setw(5) << caloDiskC.x() << "," << std::setw(5) << caloDiskC.y() << "," << std::setw(5) << caloDiskC.z() << ")"
               << " outside=" << !crossesCaloDiskC;
      }
      if( crossingCalo ) {
        msg(MSG::DEBUG) << std::endl << " crossing calo:";
      } else {
        msg(MSG::DEBUG) << std::endl << " not crossing calo:";  
      }

      if( accepted ) {
        msg(MSG::DEBUG) << " => Accepted";
      } else {
        msg(MSG::DEBUG) << " => Rejected";
      }
      msg(MSG::DEBUG) << std::setprecision(6)  // back to default precision
             << endmsg; // final flush
    } // if (msgLvl(MSG::VERBOSE))

    return accepted;
  }

  bool MooCandidateMatchingTool::match( const MuPatCandidateBase& entry1, const MuPatSegment& entry2, bool useTightCuts ) const {
    const MuPatSegment* seg = dynamic_cast<const MuPatSegment*>(&entry1);
    if( seg ) return match(*seg,entry2,useTightCuts);

    const MuPatTrack* track = dynamic_cast<const MuPatTrack*>(&entry1);
    if( track ) return match(*track,entry2,useTightCuts);

    return false;
  }


  void MooCandidateMatchingTool::getIdentifierSet( const std::vector<const Trk::MeasurementBase*>& measurements, std::set<Identifier>& ids ) const {

    // loop over measurements and extract all identifiers
    std::vector<const Trk::MeasurementBase*>::const_iterator it = measurements.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator it_end = measurements.end();
    for( ;it!=it_end;++it ){
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*it);
      if( rot ) {
        ids.insert(rot->identify());
      }else{
        const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*it);
        if( crot ) {
          const std::vector<const MuonClusterOnTrack*>& rots = crot->containedROTs();
          std::vector<const MuonClusterOnTrack*>::const_iterator rit = rots.begin();
          std::vector<const MuonClusterOnTrack*>::const_iterator rit_end = rots.end();
          for( ;rit!=rit_end;++rit ) ids.insert((*rit)->identify());
        }
      }
    }
  }

  bool MooCandidateMatchingTool::checkPhiHitConsistency( const MuPatCandidateBase& entry1, const MuPatCandidateBase& entry2 ) const {
    
    // check whether one of the two entries has no phi hits in which case return true
    if( entry1.phiHits().empty() || entry2.phiHits().empty() ) return true;

    std::set<Identifier> ids1;
    getIdentifierSet(entry1.phiHits(),ids1);

    std::set<Identifier> ids2;
    getIdentifierSet(entry2.phiHits(),ids2);
    
    std::vector<Identifier> intersection;
    std::set_intersection(ids1.begin(),ids1.end(),
                          ids2.begin(),ids2.end(),
                          std::back_inserter(intersection));

    unsigned int intersectionSize = intersection.size();
    // require a full overlap between the two entries or that one of the two is a subset of the other
    if( intersectionSize == ids1.size() || intersectionSize == ids2.size() ) return true;
    
    // also accept if there is no overlap 
    if( intersectionSize == 0 ) return true;

    return false;
  }

  bool MooCandidateMatchingTool::checkSegmentDistance( const MuPatSegment& entry1, const MuPatSegment& entry2 ) const {
    
    DistanceToPars distToPars(&entry1.entryPars());
    double distance = distToPars(entry2.entryPars().position());
    if( fabs(distance) > 5000. ) {
      ATH_MSG_VERBOSE("  large distance between segments, not using segment " << distance );
      return false;
    }
    return true;
  }

  bool MooCandidateMatchingTool::areInNeighbouringChambers( const MuPatSegment& seg1, const MuPatSegment& seg2 ) const {
    Identifier chid1 = seg1.chid;
    Identifier chid2 = seg2.chid;
    if ( msgLvl(MSG::VERBOSE) ) {
      msg(MSG::VERBOSE) << "check if chambers are neighbours: " <<  m_idHelperTool->toStringChamber( chid1 )
                        << " and " << m_idHelperTool->toStringChamber( chid2 ) << ": ";
    }
    // if in same chamber, then OK
    if ( chid1 == chid2 ) {
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "in same chamber" << endmsg;
      return true;
    }
    // check in r
    if ( seg1.stIndex != seg2.stIndex ) {
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "not in same station" << endmsg;
      return false;
    }

    // check in phi
    int secDiff = std::abs( m_idHelperTool->sector(chid1) - m_idHelperTool->sector(chid2) );
    if ( secDiff > 1 && secDiff != 15 ) {
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "sec1=" << m_idHelperTool->sector(chid1)
                             << "sec2=" << m_idHelperTool->sector(chid2) << " => not in neighbouring phi " << endmsg;
      return false;
    }

    // check in eta
    if ( seg1.isEndcap && seg2.isEndcap ) {
      // both in endcap
      // endcap: can compare eta indices
      if ( std::abs( m_idHelperTool->stationEta(chid1) - m_idHelperTool->stationEta(chid2) ) > 1 ) {
        if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "not in neighbouring eta" << endmsg;
        return false;
      }
    } else if ( !seg1.isEndcap && !seg2.isEndcap ) {
      // both in barrel
      const std::string& stationName1 = m_idHelperTool->mdtIdHelper().stationNameString( m_idHelperTool->mdtIdHelper().stationName( chid1 ) );
      const std::string& stationName2 = m_idHelperTool->mdtIdHelper().stationNameString( m_idHelperTool->mdtIdHelper().stationName( chid2 ) );
      std::string exceptions("MRFG");
      if ( exceptions.find(stationName1[2]) == std::string::npos && exceptions.find(stationName2[2]) == std::string::npos ) {
        // the normal case
        if ( std::abs( m_idHelperTool->stationEta(chid1) - m_idHelperTool->stationEta(chid2) ) > 1 ) {
          if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "not in neighbouring eta " << endmsg;
          return false;
        }
      } else {
        // the exceptions: use geometrical information
        const std::set<const MuonGM::MuonReadoutElement*>& roEls1 = m_candidateTool->readoutElements(seg1);
        const std::set<const MuonGM::MuonReadoutElement*>& roEls2 = m_candidateTool->readoutElements(seg2);
        std::set<const MuonGM::MuonReadoutElement*>::const_iterator it1 = roEls1.begin();
        std::set<const MuonGM::MuonReadoutElement*>::const_iterator it1_end = roEls1.end();
        std::set<const MuonGM::MuonReadoutElement*>::const_iterator it2 = roEls2.begin();
        std::set<const MuonGM::MuonReadoutElement*>::const_iterator it2_end = roEls2.end();
        for ( ; it1 != it1_end; ++it1 ) {
          for ( ; it2 != it2_end; ++it2 ) {
            double distZ = std::abs( (*it1)->center().z() - (*it2)->center().z() );
            // subtract sizes
            distZ -= 0.5 * ((*it1)->getZsize() + (*it2)->getZsize());
            if( msgLvl(MSG::VERBOSE) ) {
              msg(MSG::VERBOSE)
                     << std::endl << (*it1)->getStationType() << ": z=" << (*it1)->center().z() << "+-" << 0.5 * (*it1)->getZsize() << "  "
                     << (*it2)->getStationType() << ": z=" << (*it2)->center().z() << "+-" << 0.5 * (*it2)->getZsize() << "  "
                     << "distZ=" << distZ;
            }
            // allow some distance
            if ( distZ > 100.0 ) {
              if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " z-position too far apart" << endmsg;
              return false;
            }
          }
        }
      }
    } else {
    // don't mix barrel/endcap
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "endcap/barrel mix" << endmsg;
      return false;
    }

    if ( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " Yes!" << endmsg;
    return true;
  }
  

}
