/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/TrackCountingInfo.h"
#include "GaudiKernel/MsgStream.h"

namespace Analysis {

  TrackCountingInfo::TrackCountingInfo() : BaseTagInfo(),
					   m_ntrk(0),
					   m_d0sig_2nd(    -20.),
					   m_d0sig_abs_2nd( 0.),
					   m_d0sig_3rd(    -20.),
					   m_d0sig_abs_3rd( 0.)    
  {
    m_tagLikelihood.clear();
  }

  TrackCountingInfo::TrackCountingInfo(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType),
								     m_ntrk(0),
								     m_d0sig_2nd(    -20.),
								     m_d0sig_abs_2nd( 0.),
								     m_d0sig_3rd(    -20.),
								     m_d0sig_abs_3rd( 0.)
  { 
    m_tagLikelihood.clear();
  }

  TrackCountingInfo::TrackCountingInfo(const TrackCountingInfo& rhs) : BaseTagInfo(rhs), 
								       m_ntrk(         rhs.m_ntrk), 
 								       m_d0sig_2nd(    rhs.m_d0sig_2nd),
								       m_d0sig_abs_2nd(rhs.m_d0sig_abs_2nd),
								       m_d0sig_3rd(    rhs.m_d0sig_3rd),
								       m_d0sig_abs_3rd(rhs.m_d0sig_abs_3rd)
  {}

  TrackCountingInfo& TrackCountingInfo::operator= (const TrackCountingInfo& rhs)
  {
    if (this!=&rhs)
      {
	m_ntrk          = rhs.m_ntrk;
 	m_d0sig_2nd     = rhs.m_d0sig_2nd;
 	m_d0sig_abs_2nd = rhs.m_d0sig_abs_2nd;
 	m_d0sig_3rd     = rhs.m_d0sig_3rd;
 	m_d0sig_abs_3rd = rhs.m_d0sig_abs_3rd;
      }
    return *this;
  }

  TrackCountingInfo::~TrackCountingInfo() {
  }

  MsgStream& operator<<( MsgStream& out, const TrackCountingInfo& info) {
    out << " - Tag type " << info.infoType() 
	<< "2nd/3rd highest ip2d significance: "   << info.d0sig_2nd()     << " / "  << info.d0sig_3rd() 
	<< "2nd/3rd highest |ip2d significance|: " << info.d0sig_abs_2nd() << " / "  << info.d0sig_abs_3rd() 
	<< endreq; 
    return out;
  }

  std::ostream& operator<<( std::ostream& out, const TrackCountingInfo& info) {
    out << " - Tag type " << info.infoType() 
	<< "2nd/3rd highest ip2d significance: "   << info.d0sig_2nd()     << " / "  << info.d0sig_3rd() 
	<< "2nd/3rd highest |ip2d significance|: " << info.d0sig_abs_2nd() << " / "  << info.d0sig_abs_3rd() 
	<< std::endl; 
    return out;
  }

}
