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
// Plottable adaptor for Function1D's                                               //
//                                                                                  //
//----------------------------------------------------------------------------------//
#ifndef _PLOTFUNCTION1D_H_
#define _PLOTFUNCTION1D_H_
#include "QatPlotting/Plotable.h"
#include "QatPlotting/PlotFunction1DProperties.h"
#include "CLHEP/GenericFunctions/CutBase.hh"
#include <QtCore/QRectF>
#include <vector>
#include <string>
#include <limits.h>

namespace Genfun {
  class AbsFunction;
  class Parameter;
}

class PlotFunction1D: public Plotable{

public:

  // Define Properties:
  typedef PlotFunction1DProperties Properties;

   // Constructors
  PlotFunction1D(const Genfun::AbsFunction & function,
		 const Cut<double> & domainRestriction,
		 const QRectF   & rectHint = QRectF(QPointF(-10, -10), QSizeF(20, 20)));

   // Constructors
  PlotFunction1D(const Genfun::AbsFunction & function,
		 const QRectF   & rectHint = QRectF(QPointF(-10, -10), QSizeF(20, 20)));

  // Copy constructor:
  PlotFunction1D(const PlotFunction1D &);

  // Assignment operator:
  PlotFunction1D & operator=(const PlotFunction1D &); 

  // Destructor
  virtual ~PlotFunction1D();

  // Get the "natural rectangular border"
  virtual const QRectF & rectHint() const;

  // Describe to plotter, in terms of primitives:
  virtual void describeYourselfTo(AbsPlotter *plotter) const;

  // Set the properties
  void setProperties(const Properties &properties);

  // Revert to default properties:
  void resetProperties();

  // Get the properties (either default, or specific)
  const Properties &properties () const;
  
protected:

  class Clockwork;
  Clockwork *c;
  

};
#include "QatPlotting/PlotFunction1D.icc"
#endif
