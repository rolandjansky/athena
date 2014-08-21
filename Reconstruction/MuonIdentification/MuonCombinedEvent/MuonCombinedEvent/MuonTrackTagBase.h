/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUONTRACKTAGBASE_H
#define MUONCOMBINEDEVENT_MUONTRACKTAGBASE_H


namespace Trk {
  class Track;
}

namespace MuonCombined {
  
  class MuonTrackTagBase : public TagBase {

  public:
    
    MuonTrackTagBase(TagBase::Author author, const Trk::Track* msTrack) : TagBase(author,TagBase::Type::MuonStandAlone), 
									  m_msTrack(msTrack), m_msExtrapolatedTrack(0) {}

    const Trk::Track* getMSTrack() const { return m_msTrack; }
    const Trk::Track* getMSExtrapolatedTrack() const { return m_msExtrapolatedTrack; }
    void setMSExtrapolatedTrack(const Trk::Track *msextraptrack) {m_msExtrapolatedTrack=msextraptrack;}
  private:
    
    const Trk::Track* m_msTrack;
    const Trk::Track* m_msExtrapolatedTrack;
    
  };

}


#endif

