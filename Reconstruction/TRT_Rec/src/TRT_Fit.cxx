/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header:
//  Author: Jim Loken
//

#include "TRT_Rec/TRT_Fit.h"

TRT_Fit::TRT_Fit ()
    : m_aFit	(0.),
      m_bFit	(0.),
      m_c1	(0.),
      m_c2	(0.),
      m_c3	(0.),
      m_c4	(0.),
      m_c5	(0.)
{ }


TRT_Fit::~TRT_Fit ()
{ }

void
TRT_Fit::init (void)
{
    m_aFit	= 0.0;
    m_bFit	= 0.0;
    m_c1	= 0.0;
    m_c2	= 0.0;
    m_c3	= 0.0;
    m_c4	= 0.0;
    m_c5	= 0.0;
}

void
TRT_Fit::solve (void)
{
    double det = m_c1*m_c3 - m_c2*m_c2;
    if (det == 0.0) return;
    m_aFit = (m_c3*m_c4 - m_c2*m_c5)/det;
    m_bFit = (m_c1*m_c5 - m_c2*m_c4)/det;
}

void
TRT_Fit::sum (double r, double deltaPhi, double weight) 
{
    m_c1 += weight;
    m_c2 += weight*r;
    m_c3 += weight*r*r;
    m_c4 += weight*deltaPhi;
    m_c5 += weight*r*deltaPhi;
}
