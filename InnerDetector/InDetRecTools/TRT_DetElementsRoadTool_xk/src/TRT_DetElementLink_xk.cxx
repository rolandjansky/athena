/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_DetElementLink_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "TRT_DetElementsRoadTool_xk/TRT_DetElementLink_xk.h"

///////////////////////////////////////////////////////////////////
// Constructor with parameters
///////////////////////////////////////////////////////////////////

InDet::TRT_DetElementLink_xk::TRT_DetElementLink_xk
(const InDetDD::TRT_BaseElement*& el,const double* P)
{
  m_detelement = el                           ; // det elements
  m_phi        = float(P[ 2])                 ; // azimuthal angle
  m_sin        = float(P[ 5])                 ; // sin(phi)
  m_cos        = float(P[ 6])                 ; // cos(phi)
  m_centerf    = float(P[18])                 ; //
  m_used       = false                        ;
  m_way        = 0.                           ;
 } 
