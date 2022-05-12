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
    std::unique_ptr<const Trk::Perigee> releaseCombinedParameters();

    /** access to MuonCandidate */
    const MuonCandidate& muonCandidate() const;

    /** access to match chi2 */
    double matchChi2() const;

    /** name string */
    std::string name() const override;

    /** print content to string */
    std::string toString() const override;

    /** access to primary muon system track, zero if non available */
    const Trk::Track* primaryTrack() const override;

    /** Returns whether the muon belongs to the commissioning chain **/
    bool isCommissioning() const override;

       
  private:
    /** block copy and assignment */
    StacoTag(const StacoTag&) = delete;
    StacoTag& operator=(const StacoTag&) = delete;

    /** data content */
    const MuonCandidate* m_muonCandidate{nullptr};  /// MuonCandidate 
    std::unique_ptr<const Trk::Perigee>  m_combinedParameters{nullptr};  /// combined parameters 
    double m_chi2{0.};
    
  };  

  bool operator<( const StacoTag& t1,  const StacoTag& t2 );
    
}


#endif

