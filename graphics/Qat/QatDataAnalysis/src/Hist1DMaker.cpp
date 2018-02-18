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


#include "QatDataAnalysis/Hist1DMaker.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/Attribute.h"
#include "QatDataAnalysis/Table.h"
#include "CLHEP/GenericFunctions/Argument.hh"
class Hist1DMaker::Clockwork {

public:

  Genfun::AbsFunction   *f;
  size_t                nBins;
  double                min;
  double                max;
  Genfun::AbsFunction   *w;
  
};


Hist1DMaker::Hist1DMaker (Genfun::GENFUNCTION f, size_t nbinsx, double min, double max,
			  const Genfun::AbsFunction *weight) : m_c(new Clockwork()) {
  m_c->f         = f.clone();
  m_c->nBins     = nbinsx;
  m_c->min       = min;
  m_c->max       = max;
  m_c->w         = weight ? weight->clone() : NULL;
}

Hist1DMaker::~Hist1DMaker() {
  delete m_c->f;
  delete m_c->w;
  delete m_c;
}


Hist1D Hist1DMaker::operator * (const Table & table) const {

  Hist1D h(table.name(),m_c->nBins, m_c->min, m_c->max);

  
  for  (size_t t=0; ; t++) {
    
    TupleConstLink tuple = table[t];
    if (!tuple) break;

    const Genfun::Argument & a  = tuple->asDoublePrec();
    
    if (m_c->w) {
      Genfun::GENFUNCTION W=*m_c->w;
      h.accumulate((*(m_c->f))(a), W(a));
    }
    else {
      h.accumulate((*(m_c->f))(a));
    }    
  }
  return h;
}
