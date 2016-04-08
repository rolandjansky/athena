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


#ifndef _Selection_h_
#define _Selection_h_
#include "QatDataAnalysis/Table.h"
#include "QatDataAnalysis/Tuple.h"
#include "CLHEP/GenericFunctions/CutBase.hh"
// -----------------------------------------------------//
// This class defines selection operations on tables:   //
// -----------------------------------------------------//

class Selection {
 
 public:

  // Constructor:
  Selection( const Cut<Tuple> & cut);

  // Copy Constructor:
  Selection (const Selection & sel);

  // Destructor:
  ~Selection();

  // Assignment
  Selection & operator = (const Selection & sel);

  // Operation on Table
  Table operator * (const Table & table) const;


 private:
  
  const Cut<Tuple> * _cut;
  
};

#endif
