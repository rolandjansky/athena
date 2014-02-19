/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/QuadFocuserHorizontal.h"

#include "FPTracker/Point.h"
#include "FPTracker/TransversePoint.h"
#include <cmath>
#include <iomanip>
#include <iostream>
namespace FPTracker{

  void QuadFocuserHorizontal::focus(double qk, double qkl, double qkc, const TransversePoint& dxyz, const Point& dir ) {
    //                                 ----- horizontally focussing 
    // std::cout<<std::setprecision(9)<<"HFocus: qk "<<qk<<" qkl "<<qkl<<" qkc "<<qkc<<" displ "<<dxyz<<" dir "<<dir<<'\n';
    m_xe  =     cos(qkl)* dxyz[0] + sin(qkl)*dir[0]/qkc ; 
    m_xae = -qk*sin(qkl)* dxyz[0] + cos(qkl)*dir[0]/dir[2] ; 
    m_ye  =    std::cosh(qkl)* dxyz[1] + std::sinh(qkl)*dir[1]/qkc  ;
    m_yae = qk*std::sinh(qkl)* dxyz[1] + std::cosh(qkl)*dir[1]/dir[2] ; 

  }
  
  double QuadFocuserHorizontal::xe()         const {return m_xe;}
  double QuadFocuserHorizontal::xae()        const {return m_xae;}
  double QuadFocuserHorizontal::ye()         const {return m_ye;}
  double QuadFocuserHorizontal::yae()        const {return m_yae;}
  
  const std::string QuadFocuserHorizontal::s_label = "HFocuser";
  std::string QuadFocuserHorizontal::label() const {return s_label;}
}

  
  
