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


#ifndef _Value_h_
#define _Value_h_
#include <vector>
#include <algorithm>
#include <iostream>

class Value {

 public:

  Value(const void * value,  size_t sizeInBytes):
    _rep(std::vector<char>(sizeInBytes)){
      std::copy ((char *) value, (char *) value + sizeInBytes, _rep.begin());
    }

  const char * asCharStar() const { return & _rep[0];}
  
  void clear() {
    std::fill(_rep.begin(),_rep.end(),0);
  }
  
  template <typename T> void setValue (const T & t) {
    * ((T *) & _rep[0]) = t;
  }

 private:

  std::vector<char> _rep;
};


#include "QatDataAnalysis/Value.icc"
#endif
