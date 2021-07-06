/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUGIRLLOWBETATAG_H
#define MUONCOMBINEDEVENT_MUGIRLLOWBETATAG_H


#include "MuonCombinedEvent/TagBase.h"
#include <vector>

#include "MuonCombinedEvent/CandidateSummary.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkSegment/SegmentCollection.h"

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
    */
    MuGirlLowBetaTag( const ElementLink<TrackCollection>& combLink, const std::vector<ElementLink<Trk::SegmentCollection> >& segments );

    /** Constructor taking a list of segments
    */
    MuGirlLowBetaTag( const std::vector<ElementLink<Trk::SegmentCollection> >& segments );

    /** destructor */
    ~MuGirlLowBetaTag();

    /** access combined track */
    const Trk::Track* combinedTrack() const;

    /*ElementLink to the combined track*/
    ElementLink<TrackCollection> combinedTrackLink() const {return m_combLink;}

    /** access update extrapolated track, returns zero if none are available */
    const Trk::Track* updatedExtrapolatedTrack() const;

    /** set update extrapolated track*/
    void setUpdatedExtrapolatedTrack(ElementLink<TrackCollection> meLink);

    /** access segments */
    const std::vector<ElementLink<Trk::SegmentCollection> >& segments() const ;

    /* stau Dressing */
    void setMuBeta(float muBeta);
    float getMuBeta();

    void setStauSummary(MuGirlNS::CandidateSummary* stauSummary);
    MuGirlNS::CandidateSummary* getStauSummary();
    
    void setStauExtras(MuGirlNS::StauExtras* stauExtras);
    MuGirlNS::StauExtras* getStauExtras();
    const MuGirlNS::StauExtras* getStauExtras() const;
    
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
    ElementLink<TrackCollection> m_combLink;
    ElementLink<TrackCollection> m_meLink;
    std::vector<ElementLink<Trk::SegmentCollection> > m_segments; /// list of segments

    //dressing
    float m_muBeta; 
    MuGirlNS::CandidateSummary* m_stauSummary;
    MuGirlNS::StauExtras* m_stauExtras;
    MuGirlNS::RHExtras* m_rhExtras;
  };
  inline bool operator<( const MuGirlLowBetaTag& t1,  const MuGirlLowBetaTag& t2 ){
    const Trk::FitQuality* t1FQ = (t1.combinedTrack()!=nullptr)? t1.combinedTrack()->fitQuality() : 0;
    const Trk::FitQuality* t2FQ = (t2.combinedTrack()!=nullptr)? t2.combinedTrack()->fitQuality() : 0;
    if (t1FQ!=NULL && t2FQ!=NULL)
    {
        return t1FQ->chiSquared() < t2FQ->chiSquared();
    }
    return t1.segments().size() < t2.segments().size(); 
  }

  inline const Trk::Track* MuGirlLowBetaTag::combinedTrack() const { return m_combLink.isValid() ? *m_combLink : nullptr; }

  inline const Trk::Track* MuGirlLowBetaTag::updatedExtrapolatedTrack() const { return m_meLink.isValid() ? *m_meLink : nullptr; }
  
  inline void MuGirlLowBetaTag::setUpdatedExtrapolatedTrack(ElementLink<TrackCollection> meLink) { m_meLink=meLink;}

  inline const std::vector<ElementLink<Trk::SegmentCollection> >& MuGirlLowBetaTag::segments() const { return m_segments; }

  inline const Trk::Track* MuGirlLowBetaTag::primaryTrack() const {
    return combinedTrack();
  }

//stau dressing
  inline void MuGirlLowBetaTag::setMuBeta(float muBeta) { m_muBeta = muBeta; }
  inline float MuGirlLowBetaTag::getMuBeta() { return m_muBeta; }
  
  inline void MuGirlLowBetaTag::setStauSummary(MuGirlNS::CandidateSummary* stauSummary) {m_stauSummary = stauSummary;}
  inline MuGirlNS::CandidateSummary* MuGirlLowBetaTag::getStauSummary() {return m_stauSummary;}
  
  inline void MuGirlLowBetaTag::setStauExtras(MuGirlNS::StauExtras* stauExtras) {m_stauExtras = stauExtras;}
  inline MuGirlNS::StauExtras* MuGirlLowBetaTag::getStauExtras() {return m_stauExtras;}
  inline const MuGirlNS::StauExtras* MuGirlLowBetaTag::getStauExtras() const {return m_stauExtras;}
  
  inline void MuGirlLowBetaTag::setRHExtras(MuGirlNS::RHExtras* rhExtras) {m_rhExtras = rhExtras;}
  inline MuGirlNS::RHExtras* MuGirlLowBetaTag::getRHExtras() {return m_rhExtras;}
}


#endif
