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


//----------------------------------------------------------------------------------//
//                                                                                  //
// Plottable adaptor for Residual1D's                                                   //
//                                                                                  //
//----------------------------------------------------------------------------------//
#ifndef _PLOTRESIDUAL1D_H_
#define _PLOTRESIDUAL1D_H_
#include "QatPlotting/Plotable.h"
#include "PlotResidual1DProperties.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include <QtCore/QRectF>
#include <vector>
class Hist1D;

class PlotResidual1D: public Plotable {

public:
  
  
  // Define properties:
  typedef PlotResidual1DProperties Properties;

  // Constructor
  PlotResidual1D(const Hist1D &, Genfun::GENFUNCTION);

  // Copy constructor:
  PlotResidual1D(const PlotResidual1D &);

  // Assignment operator:
  PlotResidual1D & operator=(const PlotResidual1D &); 

  // Destructor
  virtual ~PlotResidual1D();

  // Get the "natural rectangular border"
  virtual const QRectF & rectHint() const;

  // Describe to plotter, in terms of primitives:
  virtual void describeYourselfTo(AbsPlotter *plotter) const;

  // Get the title:
  virtual std::string title() const;

  // Get the histogram:
  const Hist1D *histogram() const;

  // Get the function:
  const Genfun::AbsFunction *function () const; 

  // Set the properties
  void setProperties(const Properties &properties);

  // Revert to default properties:
  void resetProperties();

  // Get the properties (either default, or specific)
  const Properties &properties () const;

private:

  class Clockwork;
  Clockwork *c;
  
};
#endif
