/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagInfo/JetProbInfoBase.h"
#include "GaudiKernel/MsgStream.h"
#include <math.h>
#include <iostream>
#include <vector>

namespace Analysis {

  /** Default constructor */
  JetProbInfoBase::JetProbInfoBase() : BaseTagInfo(), m_ntrk(0) { 
    m_tagLikelihood.clear();
  }

  /** constructor with info type */
  JetProbInfoBase::JetProbInfoBase(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType), m_ntrk(0) { 
    m_tagLikelihood.clear();
  }

  /** Copy constructor
      not really needed now, but the object might get more complicated ...
  */
  JetProbInfoBase::JetProbInfoBase(const JetProbInfoBase& rhs) : BaseTagInfo(rhs), m_ntrk(rhs.m_ntrk) {
  }

  /** assigenment operator */
  JetProbInfoBase& JetProbInfoBase::operator= (const JetProbInfoBase& rhs)
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
  JetProbInfoBase::~JetProbInfoBase() {
  }

  /** print */
  MsgStream& operator<<( MsgStream& out, const JetProbInfoBase& info) {
    double pb = info.tagLikelihood()[0];
    out << " - Tag type " << info.infoType() 
	<< " : Prob= " << pb << " based on " << info.nbTracks() << " tracks." << endreq;
    return out;
  }

  /** print */
  std::ostream& operator<<( std::ostream& out, const JetProbInfoBase& info) {
    double pb = info.tagLikelihood()[0];
    out << " - Tag type " << info.infoType() 
	<< " : Prob= " << pb << " based on " << info.nbTracks() << " tracks." << std::endl;
    return out;
  }

}
