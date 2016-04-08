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


#include "QatDataModeling/TableLikelihoodFunctional.h"
#include "QatDataAnalysis/Table.h"
#include "QatDataAnalysis/Tuple.h"
#include "CLHEP/GenericFunctions/Argument.hh"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include <cmath>
TableLikelihoodFunctional::TableLikelihoodFunctional(const Table & table):
_table(table)
{}

TableLikelihoodFunctional::~TableLikelihoodFunctional() {
}

double TableLikelihoodFunctional::operator [] (const Genfun::AbsFunction & function) const {
  double logLikelihood(0);

 for  (size_t t=0; ; t++) {

   TupleConstLink tuple = _table [t];
   if (!tuple) break;
	    
   const Genfun::Argument & a  = tuple->asDoublePrec();

    double f = function(a);
    if (f<0)
      std::cerr
	<< "Warning.. negative likelihood arg[" << t << "]=" << a 
	<< std::endl;
    logLikelihood -= log(f);

 }
  return 2.0*logLikelihood;
}

