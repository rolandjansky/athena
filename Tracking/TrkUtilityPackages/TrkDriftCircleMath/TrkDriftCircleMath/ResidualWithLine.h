/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_RESIDUALWITHLINE_H
#define DCMATH_RESIDUALWITHLINE_H

#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/LocPos.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/TransformToLine.h"

namespace TrkDriftCircleMath {

  class ResidualWithLine{
  public:
    ResidualWithLine() {}
    ResidualWithLine(const Line& line) : m_transToLine(line) {}
      

    double residual( const LocPos& pos ) const
    { 
      return m_transToLine.toLineY( pos );
    }

    double residual( const DriftCircle& dc ) const
    { 
      return std::abs( residual( dc.position()) ) - std::abs( dc.r() );
    }

    double residual( const Cluster& cl ) const
    { 
      // the residual of a cluster is calculated from the point of closest approach and 
      // the angle of the line with the plane 
      double driftRes = residual( cl.position() );
      double sinPhi   = m_transToLine.toLineRotation().sinphi();
      if( sinPhi < 1e-13 ) sinPhi = 1e-13;
      return driftRes/sinPhi;
    }

    void set( const Line& line ){
      m_transToLine.set(line);
    }
  protected:
    TransformToLine m_transToLine;
  };

}

#endif
