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


#ifndef  ValueList_h_
#define  ValueList_h_
#include "QatDataAnalysis/Value.h"
#include <vector>

class ValueList {

 public:

  // Type Definitions:
  typedef std::vector<Value>::const_iterator ConstIterator;

  // Constructor:
  inline ValueList();

  // Destructor:
  inline ~ValueList();

  // Add an attribute to the list:
  inline void add(const Value & value);

  // Iterate over the attributes;
  inline ConstIterator begin () const;
  inline ConstIterator end() const;

  // Random access:
  inline Value & operator[] (size_t i) ;
  inline const Value & operator[] (size_t i) const;

  // Size of the attribute list;
  inline size_t size() const;
  
  inline void clear();

 private:

  std::vector<Value> _valueList;

};

#include "QatDataAnalysis/ValueList.icc"
#endif
