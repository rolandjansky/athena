/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagInfo/IPInfoPlus.h"
#include "JetTagInfo/IPTrackInfo.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <vector>

namespace Analysis {

  IPInfoPlus::IPInfoPlus() : 
    BaseTagInfo(),
    m_trackinfo() {
      m_tagLikelihood.clear();
  }

  IPInfoPlus::IPInfoPlus(TagInfoType tagJetInfoType) : 
    BaseTagInfo(tagJetInfoType),
    m_trackinfo() {
      m_tagLikelihood.clear();
  }

  IPInfoPlus::IPInfoPlus(const IPInfoPlus& rhs) : BaseTagInfo(rhs),
				      m_trackinfo(rhs.m_trackinfo) {
  }

  IPInfoPlus& IPInfoPlus::operator= (const IPInfoPlus& rhs) {
    if (this!=&rhs) {
        m_trackinfo=rhs.m_trackinfo;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
      }
    return *this;
  }

  IPInfoPlus::~IPInfoPlus() {
  }

  void IPInfoPlus::updateTrackWeight(const Rec::TrackParticle* trk, std::string view, double w) {
    if(!trk) return;
    for(int i=0;i<this->numTrackInfo();i++) {
      if( trk==m_trackinfo.at(i).track() ) {
	if(view=="2D") m_trackinfo.at(i).setTrackWeight2D(w);
	if(view=="3D") m_trackinfo.at(i).setTrackWeight3D(w);
	if(view=="JP") m_trackinfo.at(i).setTrackProbJP(w);
	if(view=="JPneg") m_trackinfo.at(i).setTrackProbJPneg(w);
	break;
      }
    }
  }

  MsgStream& operator<<( MsgStream& out, const IPInfoPlus& info) {
    int ntrk = info.numTrackInfo();
    out << " - Tag type " << info.infoType() 
	<< " based on " << ntrk << " tracks:" << endreq;
    for(int i=0;i<ntrk;i++) {
      out << " -> " << i << info.getTrackInfo(i) << endreq;
    }
    return out;
  }

  std::ostream& operator<<( std::ostream& out, const IPInfoPlus& info) {
    int ntrk = info.numTrackInfo();
    out << " - Tag type " << info.infoType() 
	<< " based on " << ntrk << " tracks:" << std::endl;
    for(int i=0;i<ntrk;i++) {
      out << " -> " << info.getTrackInfo(i) << std::endl;
    }
    return out;
  }

}
