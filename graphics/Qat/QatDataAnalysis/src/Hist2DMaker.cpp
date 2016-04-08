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
  c(new Clockwork()) {
  c->fX         = fX.clone();
  c->nBinsX     = nBinsX;
  c->minX       = minX;
  c->maxX       = maxX;
  c->fY         = fY.clone();
  c->nBinsY     = nBinsY;
  c->minY       = minY;
  c->maxY       = maxY;
  c->w         = weight ? weight->clone() : NULL;
}

Hist2DMaker::~Hist2DMaker() {
  delete c->fX;
  delete c->fY;
  delete c->w;
  delete c;
}


Hist2D Hist2DMaker::operator * (const Table & table) const {


  Hist2D h(table.name(),
	   c->nBinsX, c->minX, c->maxX,
	   c->nBinsY, c->minY, c->maxY);



  
  for  (size_t t=0; ; t++) {
    

    TupleConstLink tuple = table[t];
    if (!tuple) break;
    
    const Genfun::Argument & a  = tuple->asDoublePrec();
    if (c->w) {
      Genfun::GENFUNCTION W = *c->w;
      h.accumulate((*(c->fX))(a), (*(c->fY))(a), W(a));
    }
    else {
      h.accumulate((*(c->fX))(a), (*(c->fY))(a));
    }
  }
  return h;
}


