/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagInfo/IPInfoBase.h"
#include "GaudiKernel/MsgStream.h"
#include <math.h>
#include <iostream>
#include <vector>

namespace Analysis {

  /** Default constructor */
  IPInfoBase::IPInfoBase() : BaseTagInfo(), m_ntrk(0) { 
    m_tagLikelihood.clear();
  }

  /** constructor with info type */
  IPInfoBase::IPInfoBase(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType), m_ntrk(0) { 
    m_tagLikelihood.clear();
  }

  /** Copy constructor
      not really needed now, but the object might get more complicated ...
  */
  IPInfoBase::IPInfoBase(const IPInfoBase& rhs) : BaseTagInfo(rhs), m_ntrk(rhs.m_ntrk) {
  }

  /** assigenment operator */
  IPInfoBase& IPInfoBase::operator= (const IPInfoBase& rhs)
  {
    if (this!=&rhs)
      {
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
	m_ntrk=rhs.m_ntrk;
      }
    return *this;
  }

  /** Default destructor */
  IPInfoBase::~IPInfoBase() {
  }

  /** print */
  MsgStream& operator<<( MsgStream& out, const IPInfoBase& info) {
    double pb = info.tagLikelihood()[0];
    double pu = info.tagLikelihood()[1];
    double ww = log(pb/pu);
    out << " - Tag type " << info.infoType() 
	<< " : P(b)= " << pb << " P(u)= " << pu << " W= " << ww 
	<< " based on " << info.nbTracks() << " tracks." << endreq;
    return out;
  }

  /** print */
  std::ostream& operator<<( std::ostream& out, const IPInfoBase& info) {
    double pb = info.tagLikelihood()[0];
    double pu = info.tagLikelihood()[1];
    double ww = log(pb/pu);
    out << " - Tag type " << info.infoType() 
	<< " : P(b)= " << pb << " P(u)= " << pu << " W= " << ww 
	<< " based on " << info.nbTracks() << " tracks." << std::endl;
    return out;
  }

}
