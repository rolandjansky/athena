/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/SVInfoBase.h"
#include "GaudiKernel/MsgStream.h"
#include <math.h>
#include <iostream>

namespace Analysis {

  /** Default constructor */
  SVInfoBase::SVInfoBase() : BaseTagInfo() { 
    m_tagLikelihood.clear();
  }

  /** constructor with info type */
  SVInfoBase::SVInfoBase(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType) { 
    m_tagLikelihood.clear();
  }

  /** Copy constructor
      not really needed now, but the object might get more complicated ...
  */
  SVInfoBase::SVInfoBase(const SVInfoBase& rhs) : BaseTagInfo(rhs) {
  }

  /** assigenment operator */
  SVInfoBase& SVInfoBase::operator= (const SVInfoBase& rhs)
  {
    if (this!=&rhs)
      {
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
      }
    return *this;
  }

  /** Default destructor */
  SVInfoBase::~SVInfoBase() {
  }

  /** print */
  MsgStream& operator<<( MsgStream& out, const SVInfoBase& info) {
    double pb = info.tagLikelihood()[0];
    double pu = info.tagLikelihood()[1];
    double ww = log(pb/pu);
    out << " - Tag type " << info.infoType() 
	<< " : P(b)= " << pb << " P(u)= " << pu << " W= " << ww << endreq;
    return out;
  }

  /** print */
  std::ostream& operator<<( std::ostream& out, const SVInfoBase& info) {
    double pb = info.tagLikelihood()[0];
    double pu = info.tagLikelihood()[1];
    double ww = log(pb/pu);
    out << " - Tag type " << info.infoType() 
	<< " : P(b)= " << pb << " P(u)= " << pu << " W= " << ww << std::endl;
    return out;
  }

}
