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



// *-----------------------------------------------------------------*/
// * A base class for functions to be visualized in the presenter    */
// * ----------------------------------------------------------------*/
#ifndef VisFunction_h_
#define VisFunction_h_
class Plotable;
#include <QtCore/QRectF>
#include <map>
#include <string>
#include "CLHEP/GenericFunctions/Parameter.hh"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/CutBase.hh"

class VisFunction {
  
public:

  // Constructor:
  VisFunction(const std::string & name);

  // Destructor:
  virtual ~VisFunction();

  // Get the number of parameters:
  unsigned int getNumParameters() const;

  // Get the Parameter:
  Genfun::Parameter *getParameter(unsigned int i) const;

  // Add a parameter
  void addParameter(Genfun::Parameter *parameter);

  // Get the number of functions:
  unsigned int getNumFunctions() const;

  // Get the Function
  const Genfun::AbsFunction *getFunction(unsigned int i) const;

  // Get the Domain Resctriction (NULL if unrestricted)
  const Cut<double> *getDomainRestriction(unsigned int i) const;

  // Add a function.  (Clones the function in, also the cut if any).
  void addFunction(Genfun::GENFUNCTION f, const Cut<double> *domainRestriction=NULL);

  // Name
  const std::string & name() const;

  // Rect Hint
  const QRectF & rectHint() const;

  // Rect Hint
  QRectF & rectHint();

 private:
  
  // Illegal operations:
  VisFunction (const VisFunction & );
  VisFunction & operator= (const VisFunction &);

  // Internals:
  class Clockwork;
  Clockwork *c;

};
#endif
