/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/QuadFocuserNull.h"

#include "FPTracker/Point.h"
#include "FPTracker/TransversePoint.h"
#include <cmath>

namespace FPTracker{
  void QuadFocuserNull::focus(double, double, double, const TransversePoint& dxyz, const Point& dir ) {
    //                                 ----- null focussing 
    m_xe  =     dxyz[0];
    m_xae =     dir[0];
    m_ye  =     dxyz[1];
    m_yae =     dir[1];
    

  }
  
  double QuadFocuserNull::xe()         const {return m_xe;}
  double QuadFocuserNull::xae()        const {return m_xae;}
  double QuadFocuserNull::ye()         const {return m_ye;}
  double QuadFocuserNull::yae()        const {return m_yae;}
  const std::string QuadFocuserNull::s_label = "NullFocuser";
  std::string QuadFocuserNull::label() const {return s_label;}
}

  
  
