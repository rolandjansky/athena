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


#ifndef _HIST2DMAKER_H_
#define _HIST2DMAKER_H_
//-------------------------------------------------------------------//
// Class which allows to make a histogram from an ntuple.            //
//-------------------------------------------------------------------//
#include <string>
#include "CLHEP/GenericFunctions/AbsFunction.hh"
class Table;
class Hist2D;

class Hist2DMaker {

 public:


  // Constructor;
  Hist2DMaker(Genfun::GENFUNCTION fx, size_t nbinsX, double minX, double max,
	      Genfun::GENFUNCTION fy, size_t nbinsY, double minY, double maxY,
	      const Genfun::AbsFunction *weight=NULL);

  // Destructor:
  ~Hist2DMaker();

  // Action:
  Hist2D operator * (const Table & t) const;

 private:

  Hist2DMaker & operator = (const Hist2DMaker & );
  Hist2DMaker(const Hist2DMaker &);

  class Clockwork;
  Clockwork *c;

};

#endif
