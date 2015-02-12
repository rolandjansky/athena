/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     SoftMuonInfo.cxx
PACKAGE:  offline/PhysicsAnalysis/BTaggingID/BTaggingUtils
PURPOSE:  EDM for b-tagging based on soft muon identification
********************************************************************/

#include "JetTagInfo/SoftMuonInfo.h"

namespace Analysis
{

/** Default constructor */
SoftMuonInfo::SoftMuonInfo() : BaseTagInfo(), m_trackinfo()
{
}

/** constructor with info type */
SoftMuonInfo::SoftMuonInfo(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType), m_trackinfo()
{
}

/** Copy constructor
  not really needed now, but the object might get more complicated ...
*/
SoftMuonInfo::SoftMuonInfo(const SoftMuonInfo& rhs) : BaseTagInfo(rhs), m_trackinfo(rhs.m_trackinfo) 
{}

/** assigenment operator */
SoftMuonInfo& SoftMuonInfo::operator= (const SoftMuonInfo& rhs)
{
    if (this!=&rhs)
    {
        m_trackinfo=rhs.m_trackinfo;
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
    }
    return *this;
}

/** Default destructor */
SoftMuonInfo::~SoftMuonInfo()
{}

  MsgStream& operator<<( MsgStream& out, const SoftMuonInfo& info) {
    int ntrk = info.numTrackInfo();
    out << " - Tag type " << info.infoType() 
	<< " based on " << ntrk << " muon tracks:" << endreq;
    for(int i=0;i<ntrk;i++) {
      out << " -> " << i << info.getTrackInfo(i) << endreq;
    }
    return out;
  }

  std::ostream& operator<<( std::ostream& out, const SoftMuonInfo& info) {
    int ntrk = info.numTrackInfo();
    out << " - Tag type " << info.infoType() 
	<< " based on " << ntrk << " muon tracks:" << std::endl;
    for(int i=0;i<ntrk;i++) {
      out << " -> " << info.getTrackInfo(i) << std::endl;
    }
    return out;
  }


}

