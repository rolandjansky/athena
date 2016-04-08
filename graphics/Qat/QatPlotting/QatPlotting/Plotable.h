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


//--------------------------------------------------------------------------//
// Class Plotable                                                           //
//                                                                          //
// Joe Boudreau June 2K                                                     //
//                                                                          //
// This class is a base class for all plotable objects.  It describes       //
// the plottable thing in terms of individual points and connected points   //
// (polylines).  The plotters can then blindly iterate through their        //
// plottables, obtaining their points, scaling them, and plotting them.     //
//                                                                          //
// The plottables also hold the plot styles                                 //
//                                                                          //
//--------------------------------------------------------------------------//
#ifndef _PLOTABLE_H_
#define _PLOTABLE_H_
#include <string>
class QRectF;
class AbsPlotter;
class Plotable {
  
public:

  // Constructor
  Plotable();

  // Destructor
  virtual ~Plotable();

  // Get the "natural rectangular border"
  virtual const QRectF & rectHint() const=0;

  // Describe to plotter, in terms of primitives:
  virtual void describeYourselfTo (AbsPlotter *plotter) const =0;

};
#endif

