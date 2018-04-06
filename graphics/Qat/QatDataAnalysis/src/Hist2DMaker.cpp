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


#include "QatDataAnalysis/Hist2DMaker.h"
#include "QatDataAnalysis/Hist2D.h"
#include "QatDataAnalysis/Table.h"



class Hist2DMaker::Clockwork {

public:

  Genfun::AbsFunction   *fX;
  size_t                nBinsX;
  double                minX;
  double                maxX;
  Genfun::AbsFunction   *fY;
  size_t                nBinsY;
  double                minY;
  double                maxY;
  const Genfun::AbsFunction   *w;
 
};


Hist2DMaker::Hist2DMaker (Genfun::GENFUNCTION fX, size_t nBinsX, double minX, double maxX,
			  Genfun::GENFUNCTION fY, size_t nBinsY, double minY, double maxY,
			  const Genfun::AbsFunction *weight) : 
  m_c(new Clockwork()) {
  m_c->fX         = fX.clone();
  m_c->nBinsX     = nBinsX;
  m_c->minX       = minX;
  m_c->maxX       = maxX;
  m_c->fY         = fY.clone();
  m_c->nBinsY     = nBinsY;
  m_c->minY       = minY;
  m_c->maxY       = maxY;
  m_c->w         = weight ? weight->clone() : NULL;
}

Hist2DMaker::~Hist2DMaker() {
  delete m_c->fX;
  delete m_c->fY;
  delete m_c->w;
  delete m_c;
}


Hist2D Hist2DMaker::operator * (const Table & table) const {


  Hist2D h(table.name(),
	   m_c->nBinsX, m_c->minX, m_c->maxX,
	   m_c->nBinsY, m_c->minY, m_c->maxY);



  
  for  (size_t t=0; ; t++) {
    

    TupleConstLink tuple = table[t];
    if (!tuple) break;
    
    const Genfun::Argument & a  = tuple->asDoublePrec();
    if (m_c->w) {
      Genfun::GENFUNCTION W = *m_c->w;
      h.accumulate((*(m_c->fX))(a), (*(m_c->fY))(a), W(a));
    }
    else {
      h.accumulate((*(m_c->fX))(a), (*(m_c->fY))(a));
    }
  }
  return h;
}


