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


//-------------------------------------------------------------//
//                                                             //
// This functional returns the Likelihood of a functoin        //
// given some data                                             //
//                                                             //
//-------------------------------------------------------------//
#ifndef _TableLikelihoodFunctional_h_
#define _TableLikelihoodFunctional_h_
#include "CLHEP/GenericFunctions/AbsFunctional.hh"
#include "CLHEP/GenericFunctions/ArgumentList.hh"
class Table;
class TableLikelihoodFunctional:public Genfun::AbsFunctional {
  
 public:
  
  // Constructor:
  TableLikelihoodFunctional(const Table & table);
  
  // Destructor:
  ~TableLikelihoodFunctional();
  
  // Evaluate ChiSquared of a function w.r.t the data.
  virtual double operator [] (const Genfun::AbsFunction & function) const;
  
 private:
  
  const Table &_table;
  
};

#endif
