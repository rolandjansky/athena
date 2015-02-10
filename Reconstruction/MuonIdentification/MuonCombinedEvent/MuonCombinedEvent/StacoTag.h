/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_STACOTAG_H
#define MUONCOMBINEDEVENT_STACOTAG_H


#include "MuonCombinedEvent/TagBase.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
  class Track;
 
}

namespace MuonCombined {
  
  class MuonCandidate;

  /** TagBase implementation for a combined fit */
  class StacoTag : public TagBase {
  public:
    
    /** Constructor taking a MuonCandidate, a combined perigee and match chi2 as input
	Users should ensure that the life time of the MuonCandidate 
	The class takes ownership of the combined perigee 
    */
    StacoTag( const MuonCandidate& muonCandidate, std::unique_ptr<const Trk::Perigee> &perigee, double chi2 );

    /** destructor */
    ~StacoTag();

    /** access combined parameters */
    const Trk::Perigee& combinedParameters() const;

    /** release combined parameters, user gets ownership */
    const Trk::Perigee* releaseCombinedParameters();

    /** access to MuonCandidate */
    const MuonCandidate& muonCandidate() const;

    /** access to match chi2 */
    double matchChi2() const;

    /** name string */
    std::string name() const { return "StacoTag"; }

    /** print content to string */
    std::string toString() const;

    /** access to primary muon system track, zero if non available */
    const Trk::Track* primaryTrack() const;
    
  private:
    /** block copy and assignment */
    StacoTag(const StacoTag&) = delete;
    StacoTag& operator=(const StacoTag&) = delete;

    /** data content */
    const MuonCandidate* m_muonCandidate;  /// MuonCandidate 
    std::unique_ptr<const Trk::Perigee>  m_combinedParameters;  /// combined parameters 
    double m_chi2;
    
  };

  inline bool operator<( const StacoTag& t1,  const StacoTag& t2 ){
    return t1.matchChi2() < t2.matchChi2();
  }


  inline const Trk::Perigee& StacoTag::combinedParameters() const { return *m_combinedParameters.get(); }

  inline const Trk::Perigee* StacoTag::releaseCombinedParameters() { return m_combinedParameters.release(); }

  inline const MuonCandidate& StacoTag::muonCandidate() const { return *m_muonCandidate; }

  inline double StacoTag::matchChi2() const { return m_chi2; }

  inline const Trk::Track* StacoTag::primaryTrack() const {
    if( muonCandidate().extrapolatedTrack() ) return muonCandidate().extrapolatedTrack();
    return &muonCandidate().muonSpectrometerTrack();
  }
    
}


#endif

