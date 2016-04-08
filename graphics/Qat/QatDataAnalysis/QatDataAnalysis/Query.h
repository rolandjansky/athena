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


// -------------------------------------------------------------------------//
//                                                                          //
// Query class.  This class can be used to return the result of a query     //
// that can fail.  Objects of this class can convert themselves to the      //
// result type T.  Client code can check for a valid result; if it doesn't, //
// and the result is valid, no problem.   But if the result is invalid you  //
// will print an error message and terminate.                               //
//                                                                          //
// Joe Boudreau                                                             //
//  based on Barton & Nackman's "Scientific and Engineering C++"            //
//                                                                          //
// -------------------------------------------------------------------------//
#ifndef _QUERY_H_
#define _QUERY_H_
template <class T> 
class Query {
  
public:

  // Constructor:
  inline Query(const T &);

  // Default constructor:
  inline Query();

  // Convert to "T" 
  inline operator T() const;

  // Test Validity
  inline bool isValid() const;

private:
  
  bool _failed;
  T    _instance;

};
#include "QatDataAnalysis/Query.icc"
#endif
 
