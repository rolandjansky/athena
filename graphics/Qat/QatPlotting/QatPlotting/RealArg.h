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


// ----------------------------------------------------------//
//                                                           //
// Set of domain restrictions for 1-D functions:             //
//                                                           //
//                                                           //
// RealArg::Eq(value)                                        //
// RealArg::Gt(value)                                        //
// RealArg::Lt(value)                                        //
//                                                           //
// These are useful for specifying where a function is valid //
// i.e. the domain of the function.                          //
//                                                           //
// ----------------------------------------------------------//

#ifndef _REALARG_HH_
#define _REALARG_HH_
#include "CLHEP/GenericFunctions/CutBase.hh"
namespace RealArg {
  
  class Eq:         public Cut<double> {
  public:
    Eq(double value);
    virtual bool operator() (const double & x) const;
    virtual Eq *clone() const;
  private:
    double _value;  // value;
  };
  
  class Gt:         public Cut<double> {
  public:
    Gt(double value);
    virtual bool operator() (const double & x) const;
    virtual Gt *clone() const;
  private:
    double _value;  // value;
  };
  
  class Lt:         public Cut<double> {
  public:
    Lt(double value);
    virtual bool operator() (const double & x) const;
    virtual Lt *clone() const;
  private:
    double _value;  // value;
  };
  
}

#endif
