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


#include "QatPlotting/RealArg.h"
namespace RealArg {

  Eq::Eq(double value): _value(value){}
  bool Eq::operator() (const double & x) const {return x==_value;}
  Eq *Eq::clone() const { return new Eq(*this);}

  Lt::Lt(double value): _value(value){}
  bool Lt::operator() (const double & x) const {return x<_value;}
  Lt *Lt::clone() const { return new Lt(*this);}

  Gt::Gt(double value): _value(value) {}
  bool Gt::operator() (const double & x) const {return x>_value;}
  Gt *Gt::clone() const { return new Gt(*this);}

}

