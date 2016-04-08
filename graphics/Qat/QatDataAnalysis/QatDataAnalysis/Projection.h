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


#ifndef _Projection_h_
#define _Projection_h_
#include "QatDataAnalysis/Tuple.h"
#include "QatDataAnalysis/Table.h"
#include "CLHEP/GenericFunctions/CutBase.hh"
// -----------------------------------------------------//
// This class defines projection operations on tables:  //
// -----------------------------------------------------//

class Projection {
 
 public:

  // Constructor:
  Projection();

  // Copy Constructor:
  Projection (const Projection & P);

  // Destructor:
  ~Projection();

  // Assignment
  Projection & operator = (const Projection & sel);

  // Operation on Table
  Table operator * (const Table & table) const;

  // Add a datum
  void add(const std::string & name);
  

 private:
  
  std::set<std::string> _nameList;
  
};

#endif
