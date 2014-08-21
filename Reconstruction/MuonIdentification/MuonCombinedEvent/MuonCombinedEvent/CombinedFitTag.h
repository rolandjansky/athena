/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_COMBINEDFITTAG_H
#define MUONCOMBINEDEVENT_COMBINEDFITTAG_H


#include "MuidEvent/FieldIntegral.h"
#include "MuonCombinedEvent/TagBase.h"
#include "TrkEventPrimitives/TrackScore.h"
#include <vector>

namespace Trk {
  class Track;
}

namespace MuonCombined {
  
  class MuonCandidate;

  /** TagBase implementation for a combined fit */
  class CombinedFitTag : public TagBase {
  public:
    
    /** Constructor taking a MuonCandidate and a combined track as input
	Users should ensure that the life time of the MuonCandidate 
	The class takes ownership of the combined track and the track score
    */
    CombinedFitTag( xAOD::Muon::Author author, const MuonCandidate& muonCandidate, 
		    const Trk::Track& combinedTrack, const Trk::TrackScore& score );

    /** destructor */
    ~CombinedFitTag();

    /** access combined track */
    const Trk::Track& combinedTrack() const;

    /** set combined track */
    void setCombinedTrack(const Trk::Track& combinedTrack, const Trk::TrackScore& score );

    /** release combined track, user gets ownership */
    const Trk::Track* releaseCombinedTrack();

    /** access to MuonCandidate */
    const MuonCandidate& muonCandidate() const;

    /** access update extrapolated track, returns zero if none are available */
    const Trk::Track* updatedExtrapolatedTrack() const;

    /** set update extrapolated track, takes ownership */
    void setUpdatedExtrapolatedTrack(const Trk::Track* track);

    /** release combined track, user gets ownership */
    const Trk::Track* releaseUpdatedExtrapolatedTrack();

    /** store inner match quality info */
    void innerMatch(double chi2, int dof, double prob);
    
    /** access the inner matching chi2*/
    double matchChi2() const {return m_matchChi2;}

    /** access the inner matching DoF*/
    int matchDoF() const {return m_matchDoF;}

    /** access the inner matching probability*/
    double matchProb() const {return m_matchProb;}

    /** set field integral measurements*/
    void fieldIntegral(Rec::FieldIntegral fieldI) {m_fieldIntegral=fieldI;}

    /** get field integral measurements*/
    Rec::FieldIntegral fieldIntegral() const {return m_fieldIntegral;}

    /** set momentum balance significance*/
    void momentumBalanceSignificance(double m) {m_momentumBalanceSignificance=m;}

    /** get momentum balance significance*/
    double momentumBalanceSignificance() const {return m_momentumBalanceSignificance;}

    /** get track score */
    const Trk::TrackScore& trackScore() const { return m_trackScore; }

    /** name string */
    std::string name() const { return "CombinedFitTag"; }

    /** print content to string */
    std::string toString() const;

    /** access to primary muon system track, zero if non available */
    const Trk::Track* primaryTrack() const;

  private:
    /** block copy and assignment */
    CombinedFitTag(const CombinedFitTag&) = delete;
    CombinedFitTag& operator=(const CombinedFitTag&) = delete;

    /** data content */
    const MuonCandidate* m_muonCandidate;  /// MuonCandidate 
    const Trk::Track*    m_combinedTrack;  /// combined track 
    Trk::TrackScore      m_trackScore;
    const Trk::Track*    m_updatedExtrapolatedTrack; /// updated extrapolated track

    Rec::FieldIntegral m_fieldIntegral;

    double m_matchChi2; 
    int    m_matchDoF; 
    double m_matchProb;
    double m_momentumBalanceSignificance;

  };

  inline bool operator<( const CombinedFitTag& t1,  const CombinedFitTag& t2 ){
    return t1.trackScore() < t2.trackScore();
  }

  inline const Trk::Track& CombinedFitTag::combinedTrack() const { return *m_combinedTrack; }

  inline void CombinedFitTag::setCombinedTrack(const Trk::Track& combinedTrack, const Trk::TrackScore& score) {
    delete m_combinedTrack;
    m_combinedTrack = &combinedTrack;
    m_trackScore = score;
  }
    
  inline const Trk::Track* CombinedFitTag::releaseCombinedTrack() { 
    const Trk::Track* tmp = m_combinedTrack;   
    m_combinedTrack=0;
    return tmp;
  }
  
  inline const MuonCandidate& CombinedFitTag::muonCandidate() const { return *m_muonCandidate; }
  
  inline const Trk::Track* CombinedFitTag::updatedExtrapolatedTrack() const { return m_updatedExtrapolatedTrack; }
  
  inline void CombinedFitTag::setUpdatedExtrapolatedTrack(const Trk::Track* track) {
    delete m_updatedExtrapolatedTrack;
    m_updatedExtrapolatedTrack = track; 
  }

  inline const Trk::Track* CombinedFitTag::releaseUpdatedExtrapolatedTrack() { 
    const Trk::Track* tmp = m_updatedExtrapolatedTrack;
    m_updatedExtrapolatedTrack = 0;
    return tmp;
  }

  inline void CombinedFitTag::innerMatch(double chi2, int dof, double prob) 
  {
    m_matchChi2=chi2; 
    m_matchDoF=dof; 
    m_matchProb=prob;
  }

  inline const Trk::Track* CombinedFitTag::primaryTrack() const {
    return &combinedTrack();
  }

}


#endif

