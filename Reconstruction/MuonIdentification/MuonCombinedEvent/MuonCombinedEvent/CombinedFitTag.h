/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_COMBINEDFITTAG_H
#define MUONCOMBINEDEVENT_COMBINEDFITTAG_H


#include "MuidEvent/FieldIntegral.h"
#include "MuonCombinedEvent/TagBase.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkTrack/TrackCollection.h"
#include <vector>

namespace Trk {
  class Track;
}

namespace MuonCombined {
  
  class MuonCandidate;

  /** TagBase implementation for a combined fit */
  class CombinedFitTag : public TagBase {
  public:
    
    /** Constructor taking a MuonCandidate as input
	Users should ensure that the life time of the MuonCandidate 
	The class takes ownership of the track score
    */
    CombinedFitTag( xAOD::Muon::Author author, const MuonCandidate& muonCandidate, 
		    const Trk::TrackScore& score );

    /** destructor */
    ~CombinedFitTag();

    /** access combined track */
    const Trk::Track* combinedTrack() const;

    /*ElementLink to combined track*/
    ElementLink<TrackCollection> combinedTrackLink() const {return m_combLink;}

    /** set combined track ElementLink*/
    void setCombinedTrackLink(ElementLink<TrackCollection> combLink){m_combLink=combLink;}

    /** access to MuonCandidate */
    const MuonCandidate& muonCandidate() const;

    /** access update extrapolated track, returns zero if none are available */
    const Trk::Track* updatedExtrapolatedTrack() const;

    /* ElementLink to ME track*/
    ElementLink<TrackCollection> updatedExtrapolatedTrackLink() const {return m_MELink;}

    /** set update extrapolated track ElementLink */
    void setUpdatedExtrapolatedTrackLink(ElementLink<TrackCollection> MELink){m_MELink=MELink;}

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
    ElementLink<TrackCollection> m_combLink; //link to combined track
    Trk::TrackScore      m_trackScore;
    ElementLink<TrackCollection> m_MELink; //link to ME track

    Rec::FieldIntegral m_fieldIntegral;

    double m_matchChi2; 
    int    m_matchDoF; 
    double m_matchProb;
    double m_momentumBalanceSignificance;

  };

  inline bool operator<( const CombinedFitTag& t1,  const CombinedFitTag& t2 ){
    return t1.trackScore() < t2.trackScore();
  }

  inline const Trk::Track* CombinedFitTag::combinedTrack() const { return m_combLink.isValid() ? *m_combLink : nullptr; }

  inline const MuonCandidate& CombinedFitTag::muonCandidate() const { return *m_muonCandidate; }
  
  inline const Trk::Track* CombinedFitTag::updatedExtrapolatedTrack() const { return m_MELink.isValid() ? *m_MELink : nullptr; }
  
  inline void CombinedFitTag::innerMatch(double chi2, int dof, double prob) 
  {
    m_matchChi2=chi2; 
    m_matchDoF=dof; 
    m_matchProb=prob;
  }

  inline const Trk::Track* CombinedFitTag::primaryTrack() const {
    return combinedTrack();
  }

}


#endif

