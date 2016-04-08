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


#ifndef  AttributeList_h_
#define  AttributeList_h_
#include "QatDataAnalysis/Attribute.h"
#include "CLHEP/GenericFunctions/RCBase.hh"
#include <vector>

class AttributeList: public Genfun::RCBase {

 public:

  // Type Definitions:
  typedef std::vector<Attribute>::const_iterator ConstIterator;

  // Constructor:
  AttributeList();

  // Add an attribute to the list:
  void add( const std::string & name, const std::type_info & type);

  // Lock the attribute list:
  void lock();

  // Iterate over the attributes;
  inline ConstIterator begin () const;
  inline ConstIterator end() const;

  // Random access:
  inline Attribute & operator[] (size_t i) ;
  inline const Attribute & operator[] (size_t i) const;

  // Size of the attribute list;
  inline size_t size() const;
  

 private:

  // Destructor:
  ~AttributeList();

  std::vector<Attribute> _attrList;
  bool _locked;

};

#include "QatDataAnalysis/AttributeList.icc"
#endif
