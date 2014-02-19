/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/QuadFocuserVertical.h"

#include "FPTracker/Point.h"
#include "FPTracker/TransversePoint.h"
#include <cmath>
#include <iomanip>
#include <iostream>

namespace FPTracker{

  void QuadFocuserVertical::focus(double qk, double qkl, double qkc, const TransversePoint& dxyz, const Point& dir ) {
    //std::cout<<std::setprecision(9)<<"VFocus: qk "<<qk<<" qkl "<<qkl<<" qkc "<<qkc<<" displ "<<dxyz<<" dir "<<dir<<'\n';
    m_ye  =     std::cos(qkl)* dxyz[1] + std::sin(qkl)*dir[1]/qkc ;
    m_yae = -qk*std::sin(qkl)* dxyz[1] + std::cos(qkl)*dir[1]/dir[2]  ;
    m_xe  =    std::cosh(qkl)* dxyz[0] + std::sinh(qkl)*dir[0]/qkc ;
    m_xae = qk*std::sinh(qkl)* dxyz[0] + std::cosh(qkl)*dir[0]/dir[2] ;
  }
  
  double QuadFocuserVertical::xe()  const {return m_xe;}
  double QuadFocuserVertical::xae() const {return m_xae;}
  double QuadFocuserVertical::ye()  const {return m_ye;}
  double QuadFocuserVertical::yae() const {return m_yae;}

  const std::string QuadFocuserVertical::s_label = "VFocuser";
  std::string QuadFocuserVertical::label() const {return s_label;}
}

  
  
