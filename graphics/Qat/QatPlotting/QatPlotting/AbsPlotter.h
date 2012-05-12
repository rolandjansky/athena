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


//----------------------------------------------------------------------------------------//
//                                                                                        //
//  Abstract base class for plotters                                                      //
//                                                                                        //
//----------------------------------------------------------------------------------------//

#ifndef _ABSPLOTTER_H_
#define _ABSPLOTTER_H_

class Graphic;
class QRectF;
class Plotable;
class QMatrix;
class QGraphicsItemGroup;
class QGraphicsScene;
class AbsPlotter {

public:
 
  // Default constructor:
  AbsPlotter();

  // Virtual destructor:
  virtual ~AbsPlotter();

  // Add a plotable:
  virtual void add(Plotable *)=0;

  // Get the scene
  virtual QGraphicsScene *scene()=0;

  // Get the contents group:
  virtual QGraphicsItemGroup *group()=0;

  // Get the transformation matrix.
  virtual QMatrix matrix()=0;

  // Set the Plot Rectangle
  virtual void setRect(const QRectF & ) = 0;

  // Get the Plot Rectangle:
  virtual QRectF * rect()  = 0; 
  virtual const QRectF * rect() const = 0; 

  // Clear the Plots, and the rectangle as well:
  virtual void clear() = 0;

  // Is this log scale X?
  virtual bool isLogX() const=0;

  // Is this log scale Y?
  virtual bool isLogY() const=0;

  // Start Animating
  virtual void startAnimating(int frameRateMillisec)=0;

  // Stop Animating 
  virtual void stopAnimating()=0;
};

#endif

