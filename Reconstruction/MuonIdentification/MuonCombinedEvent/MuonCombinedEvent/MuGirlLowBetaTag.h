/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUGIRLLOWBETATAG_H
#define MUONCOMBINEDEVENT_MUGIRLLOWBETATAG_H


#include "MuonCombinedEvent/TagBase.h"
#include <vector>

#include "MuonCombinedEvent/CandidateSummary.h"

namespace Trk {
  class Track; 
}

namespace Muon {
  class MuonSegment;
}

namespace MuonCombined {
  
  /** TagBase implementation for a combined fit */
  class MuGirlLowBetaTag : public TagBase {
  public:
    
    /** Constructor a combined track and a list of segments as input
	The class takes ownership of the combined track and the segments
    */
    MuGirlLowBetaTag( const Trk::Track* combinedTrack, const std::vector<const Muon::MuonSegment*>& segments );

    /** Constructor taking a list of 
	Users should ensure that the life time of the MuonCandidate 
	The class takes ownership of the combined track 
    */
    MuGirlLowBetaTag( const std::vector<const Muon::MuonSegment*>& segments );

    /** destructor */
    ~MuGirlLowBetaTag();

    /** access combined track */
    const Trk::Track* getCombinedTrack() const;
    const Trk::Track& combinedTrack() const;

    /** release combined track, user gets ownership */
    const Trk::Track* releaseCombinedTrack();

    /** access update extrapolated track, returns zero if none are available */
    const Trk::Track* updatedExtrapolatedTrack() const;

    /** set update extrapolated track, takes ownership */
    void setUpdatedExtrapolatedTrack(const Trk::Track* track);

    /** release combined track, user gets ownership */
    const Trk::Track* releaseUpdatedExtrapolatedTrack();

    /** access segments */
    const std::vector<const Muon::MuonSegment*>& segments() const ;

    /** release segments, user gets ownership */
    void releaseSegments( std::vector<const Muon::MuonSegment*>& segments );

    void releaseSegments();

    /* stau Dressing */
    void setMuBeta(float muBeta);
    float getMuBeta();

    void setStauSummary(MuGirlNS::CandidateSummary* stauSummary);
    MuGirlNS::CandidateSummary* getStauSummary();
    
    void setStauExtras(MuGirlNS::StauExtras* stauExtras);
    MuGirlNS::StauExtras* getStauExtras();
    
    void setRHExtras(MuGirlNS::RHExtras* rhExtras);
    MuGirlNS::RHExtras* getRHExtras();

    /** name string */
    std::string name() const { return "MuGirlLowBetaTag"; }

    /** print content to string */
    std::string toString() const { return name(); }

    /** access to primary muon system track, zero if non available */
    const Trk::Track* primaryTrack() const;

  private:
    /** block copy and assignment */
    MuGirlLowBetaTag(const MuGirlLowBetaTag&) = delete;
    MuGirlLowBetaTag& operator=(const MuGirlLowBetaTag&) = delete;

    /** data content */
    const Trk::Track*    m_combinedTrack;  /// combined track 
    const Trk::Track*    m_updatedExtrapolatedTrack; /// updated extrapolated track
    std::vector<const Muon::MuonSegment*> m_segments; /// list of segments

    //dressing
    float m_muBeta; 
    MuGirlNS::CandidateSummary* m_stauSummary;
    MuGirlNS::StauExtras* m_stauExtras;
    MuGirlNS::RHExtras* m_rhExtras;
  };
  inline bool operator<( const MuGirlLowBetaTag& t1,  const MuGirlLowBetaTag& t2 ){
    const Trk::FitQuality* t1FQ = (t1.getCombinedTrack()!=NULL)? t1.getCombinedTrack()->fitQuality() : 0;
    const Trk::FitQuality* t2FQ = (t2.getCombinedTrack()!=NULL)? t2.getCombinedTrack()->fitQuality() : 0;
    if (t1FQ!=NULL && t2FQ!=NULL)
    {
        return t1FQ->chiSquared() < t2FQ->chiSquared();
    }
    return t1.segments().size() < t2.segments().size(); 
  }

  inline const Trk::Track* MuGirlLowBetaTag::getCombinedTrack() const { return m_combinedTrack; }

  inline const Trk::Track& MuGirlLowBetaTag::combinedTrack() const { return *m_combinedTrack; }

  inline const Trk::Track* MuGirlLowBetaTag::releaseCombinedTrack() { 
    const Trk::Track* tmp = m_combinedTrack;   
    m_combinedTrack=0;
    return tmp;
  }
  
  inline const Trk::Track* MuGirlLowBetaTag::updatedExtrapolatedTrack() const { return m_updatedExtrapolatedTrack; }
  
  inline void MuGirlLowBetaTag::setUpdatedExtrapolatedTrack(const Trk::Track* track) { m_updatedExtrapolatedTrack = track; }

  inline const Trk::Track* MuGirlLowBetaTag::releaseUpdatedExtrapolatedTrack() { 
    const Trk::Track* tmp = m_updatedExtrapolatedTrack;
    m_updatedExtrapolatedTrack = 0;
    return tmp;
  }

  inline void MuGirlLowBetaTag::releaseSegments() {
    m_segments.clear();
  }


  inline const std::vector<const Muon::MuonSegment*>& MuGirlLowBetaTag::segments() const { return m_segments; }

  inline void MuGirlLowBetaTag::releaseSegments( std::vector<const Muon::MuonSegment*>& segments ) {
    std::swap(m_segments,segments);
  }

  inline const Trk::Track* MuGirlLowBetaTag::primaryTrack() const {
    return &combinedTrack();
  }

//stau dressing
  inline void MuGirlLowBetaTag::setMuBeta(float muBeta) { m_muBeta = muBeta; }
  inline float MuGirlLowBetaTag::getMuBeta() { return m_muBeta; }
  
  inline void MuGirlLowBetaTag::setStauSummary(MuGirlNS::CandidateSummary* stauSummary) {m_stauSummary = stauSummary;}
  inline MuGirlNS::CandidateSummary* MuGirlLowBetaTag::getStauSummary() {return m_stauSummary;}
  
  inline void MuGirlLowBetaTag::setStauExtras(MuGirlNS::StauExtras* stauExtras) {m_stauExtras = stauExtras;}
  inline MuGirlNS::StauExtras* MuGirlLowBetaTag::getStauExtras() {return m_stauExtras;}
  
  inline void MuGirlLowBetaTag::setRHExtras(MuGirlNS::RHExtras* rhExtras) {m_rhExtras = rhExtras;}
  inline MuGirlNS::RHExtras* MuGirlLowBetaTag::getRHExtras() {return m_rhExtras;}
}


#endif
