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


#include "QatDataAnalysis/AttributeList.h"
#include <algorithm>
#include <stdexcept>
AttributeList::AttributeList():_locked(false) {
}

AttributeList::~AttributeList() {
}

// Add an attribute:
void AttributeList::add (const std::string & name, const std::type_info & type) {
  
  if (_locked)  throw std::runtime_error ("Error: adding attribute to a locked attribute list");
  
  Attribute a(name,type);
  if (std::find(_attrList.begin(),_attrList.end(),a)==_attrList.end()) {
    _attrList.push_back(a);
  }
  else {
    throw std::runtime_error ("Error: duplicate attribute name");
  }
}

// Lock the list against additional adding of attributes.  Also, now fill some cache 
// relating to data sizes and positions:
void AttributeList::lock() {
  if (!_locked) {
    std::sort(_attrList.begin(),_attrList.end());
    for (size_t a=0;a<_attrList.size();a++) {
      _attrList[a].attrId()=a;
    }
    _locked=true;
  }
}
