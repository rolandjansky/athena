//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


#include "QatDataAnalysis/TupleCut.h"
#include <set>
TupleCut::TupleCut (const TupleCut & right):
  Cut<Tuple>(),
  m_f(right.m_f->clone()),
  m_t(right.m_t),
  m_v1(right.m_v1),
  m_v2(right.m_v2)
{}

TupleCut::TupleCut(Genfun::GENFUNCTION f, double min, double max):
  m_f(f.clone()),
  m_t(NA),
  m_v1(min),
  m_v2(max) {}

TupleCut::TupleCut(Genfun::GENFUNCTION f, Type t, double val):
m_f(f.clone()),
m_t(t),
m_v1(val),
m_v2(0)
{
}

TupleCut::~TupleCut() {
  delete m_f;
}

TupleCut *TupleCut::clone(void) const {
  return new TupleCut(*this);
}

bool TupleCut::operator () (const Tuple & t) const {

  double fx = (*m_f)(t.asDoublePrec());
  switch (m_t) {
  case NA:
    return fx >= m_v1 && fx <= m_v2;
  case GT:
    return fx > m_v1;
  case LT:
    return fx < m_v1;
  case GE:
    return fx >= m_v1;
  case LE:
    return fx <= m_v1;
  }
  return false;
} 
