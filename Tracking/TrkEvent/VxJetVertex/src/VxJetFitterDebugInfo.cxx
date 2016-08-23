/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VxJetVertex/VxJetFitterDebugInfo.h"

namespace Trk {

  VxJetFitterDebugInfo::VxJetFitterDebugInfo():
      m_numFitIterations(0),
      m_numSignFlipFitIterations(0) {}

  MsgStream& VxJetFitterDebugInfo::dump(MsgStream& sl) const {
    sl << "VxJetFitterDebugInfo: numFitIterations " << m_numFitIterations << 
      " numSignFlipFitIterations " << m_numSignFlipFitIterations<<endreq;
    //" numSignFlip " << m_numSignFlips << endreq;
    return sl;
  }

  std::ostream& VxJetFitterDebugInfo::dump(std::ostream& sl) const {
    sl << "VxJetFitterDebugInfo: numFitIterations " << m_numFitIterations << 
      " numSignFlipFitIterations " << m_numSignFlipFitIterations << std::endl;
    return sl;
  }

}

