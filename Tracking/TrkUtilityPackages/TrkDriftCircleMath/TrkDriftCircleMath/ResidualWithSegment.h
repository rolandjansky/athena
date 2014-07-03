/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_RESIDUALWITHSEGMENT_H
#define DCMATH_RESIDUALWITHSEGMENT_H

#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/LocPos.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/ResidualWithLine.h"

namespace TrkDriftCircleMath {
  
  /** class to calculate residual of a hit with a segment and calculate the local 
      track errors 
  */
  class ResidualWithSegment : public ResidualWithLine {
  public:
    ResidualWithSegment(const Segment& segment) : ResidualWithLine(segment.line()), m_line(segment.line()),
      m_errorY02(segment.dy0()*segment.dy0()), m_errorPhi2(segment.dtheta()*segment.dtheta()) {}
      
    /** calculate the track error at the position of a drift circle */
    double trackError2( const DriftCircle& dc ) const {
      LocPos posInLineFrame = m_transToLine.toLine(dc.position());
      return m_errorY02 + posInLineFrame.x()*m_errorPhi2;
    }

    /** calculate the track error at the position of a cluster */
    double trackError2( const Cluster& cl ) const {
      double yDistToSegment = cl.position().y() - m_line.position().y();

      double sinPhi   = m_transToLine.toLineRotation().sinphi();
      if( sinPhi < 1e-13 ) sinPhi = 1e-13;
      double invSinPhi2 = sinPhi*sinPhi;
      invSinPhi2 = 1./invSinPhi2;

      return m_errorY02*invSinPhi2 + yDistToSegment*yDistToSegment*invSinPhi2*m_errorPhi2;
    }

  private:
    Line             m_line;
    double           m_errorY02;
    double           m_errorPhi2;
  };

}

#endif
