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


#ifndef _PLOTTEXT_H_
#define _PLOTTEXT_H_
#include "QatPlotting/Plotable.h"
#include <QtCore/QRectF>
#include <QtCore/QString>

class QGraphicsTextItem;
class QTextDocument;

class PlotText: public Plotable {

public:

   // Constructors
  PlotText(double x, double y, const QString & string=QString("")); 

  // Destructor
  virtual ~PlotText();

  // Get the "natural rectangular border"
  virtual const QRectF & rectHint() const;

  // Describe to plotter, in terms of primitives:
  virtual void describeYourselfTo(AbsPlotter *plotter) const;

  // Set the document:
  void setDocument(QTextDocument *document);

  // Copy constructor:
  PlotText(const PlotText &);
  
  // Assignment operator:
  PlotText & operator=(const PlotText &); 
  
private:

  class Clockwork;
  Clockwork *c;

};
#endif
