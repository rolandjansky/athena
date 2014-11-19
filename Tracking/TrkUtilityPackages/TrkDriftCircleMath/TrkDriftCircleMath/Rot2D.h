/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_ROT2D_H
#define DCMATH_ROT2D_H

namespace TrkDriftCircleMath {
 
  class Rot2D {
  public:
    Rot2D( double xx, double xy, double yx, double yy )
      : m_xx(xx),m_xy(xy),m_yx(yx),m_yy(yy) {}
    
    double xx() const { return m_xx; }
    double xy() const { return m_xy; }
    double yx() const { return m_yx; }
    double yy() const { return m_yy; }
    
  private:
    double m_xx;
    double m_xy;
    double m_yx;
    double m_yy;
  };

}

#endif
