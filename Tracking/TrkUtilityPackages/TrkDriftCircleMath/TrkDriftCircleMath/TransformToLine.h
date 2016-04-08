/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_TRANSFORMTOLINE_H
#define DCMATH_TRANSFORMTOLINE_H

#include <cmath>

#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/DriftCircle.h"

namespace TrkDriftCircleMath {

  class TransformToLine{
  public:
    TransformToLine() : m_toLineRot(0.), m_toLocalRot(0.),m_shift(0.,0.) {}

    TransformToLine( const Line& line) 
      : m_toLineRot(line.phi()), 
      m_toLocalRot(-line.phi()),
      m_shift(line.x0(),line.y0())
      {}

    LocPos toLine( const LocPos& pos ) const { return m_toLineRot*pos - m_shift; }
    
    /** only get one coordinate -> faster */
    double toLineX( const LocPos& pos ) const { return m_toLineRot.xval(pos) - m_shift.x(); }
    double toLineY( const LocPos& pos ) const { return m_toLineRot.yval(pos) - m_shift.y(); }

    LocPos toLocal( const LocPos& pos ) const { return m_toLocalRot*(pos+m_shift); }

    void set( const Line& line ) {
      m_toLineRot.set(line.phi()); 
      m_toLocalRot.set(-line.phi());
      m_shift.setX(line.x0());
      m_shift.setY(line.y0());
    }

    DriftCircle toLine( const DriftCircle& dc ) const { 
      return DriftCircle( toLine(dc.position()),dc.r(), dc.dr(), dc.state(), dc.id(), dc.index(), dc.rot() ); }
    DriftCircle toLocal( const DriftCircle& dc ) const {
      return DriftCircle( toLocal(dc.position()),dc.r(), dc.dr(), dc.state(), dc.id(), dc.index(), dc.rot() ); }
    
    /** rotate local position into line frame */
    const RotPhi& toLineRotation() const { return m_toLineRot; }

    /** rotate position in line frame into local frame */
    const RotPhi& toLocalRotation() const { return m_toLocalRot; }
  private:
    RotPhi m_toLineRot;
    RotPhi m_toLocalRot;
    LocPos m_shift;
  };

}

#endif
