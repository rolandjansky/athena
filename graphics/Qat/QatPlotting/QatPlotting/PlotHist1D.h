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
// Plottable adaptor for Hist1D's                                                   //
//                                                                                  //
//----------------------------------------------------------------------------------//
#ifndef _PLOTHIST1D_H_
#define _PLOTHIST1D_H_
#include "QatPlotting/Plotable.h"
#include "QatPlotting/PlotHist1DProperties.h"
#include <QtCore/QRectF>
#include <vector>
class Hist1D;
class PlotHist1D: public Plotable {

public:
  
  
  // Define properties:
  typedef PlotHist1DProperties Properties;

  // Constructor
  PlotHist1D(const Hist1D &);

  // Copy constructor:
  PlotHist1D(const PlotHist1D &);

  // Assignment operator:
  PlotHist1D & operator=(const PlotHist1D &); 

  // Destructor
  virtual ~PlotHist1D();

  // Get the "natural rectangular border"
  virtual const QRectF & rectHint() const;

  // Describe to plotter, in terms of primitives:
  virtual void describeYourselfTo(AbsPlotter *plotter) const;

  // Get the title:
  virtual std::string title() const;

  // Get the text summary:
  virtual std::string textSummary() const;

  // Get the histogram:
  const Hist1D *histogram() const;

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
