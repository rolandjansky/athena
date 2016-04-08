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


//----------------------------*FitFunction*-----------------------//
//                                                                //
//   Joe Boudreau December 2K                                     //
//   Abstract base class for fit functions.  These "functions"    //
//   are actually multiple functions, controlled by the same      //
//   parameters, or at least having some of the same parameters.  //
//   This allows us to handle "constrained fits"                  //
//                                                                //
//----------------------------------------------------------------//
#ifndef FitFunction_h_
#define FitFunction_h_

class Hist1D;
namespace Genfun {
  class AbsFunction;
  class Parameter;
}


class FitFunction {
  
public:

  // Constructor:
  FitFunction();

  // Destructor:
  virtual ~FitFunction();

  // Get number of parameters:
  virtual int  getNumParameters() const = 0;

  // Get number of functions:
  virtual int getNumFunctions() const = 0;

  // Get i^th parameter
  Genfun::Parameter & getParameter(int i); 
  virtual const Genfun::Parameter & getParameter(int i) const = 0; 

  // Get i^th function:
  virtual const Genfun::AbsFunction & getFunction(int i) const = 0;

  // Is the i^th function normalized?
  virtual bool isIntegrable(int i=0) = 0;

  // Give a hint to the i^th function, where it should start from:
  // Do nothing (by default);
  virtual void hint (const Hist1D & /*histogram*/, int /*i*/) {;}
 
 

};
#endif
