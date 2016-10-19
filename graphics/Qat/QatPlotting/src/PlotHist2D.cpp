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


#include "QatDataAnalysis/Hist2D.h"
#include "QatPlotting/PlotHist2D.h"
#include "QatPlotting/AbsPlotter.h"
//#include "QatPlotting/LinToLog.h"
//#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QGraphicsPolygonItem>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsPathItem>
//#include <iostream>
//#include <iomanip>
//#include <sstream>
//#include <cfloat>
//added, sroe
//#include "QatPlotting/PlotHist2DProperties.h"
#include <QtCore/QRectF>

// Constructor

class PlotHist2D::Clockwork {
public:
  
  Clockwork() : histogram(nullptr), nRectangle(),
		myProperties(nullptr), defaultProperties() {}

  ~Clockwork() {delete myProperties;}
  
  //copy
  Clockwork(const Clockwork & other){
    histogram = other.histogram; //objects share the pointer
    nRectangle = other.nRectangle;
    myProperties= (other.myProperties)?(new Properties(*(other.myProperties))):nullptr;
    defaultProperties=other.defaultProperties;
  }
  
  /**
	Clockwork & operator=(const Clockwork & other){
	  if (&other !=this){
			histogram = other.histogram; //objects share the pointer
			nRectangle = other.nRectangle;
			delete myProperties;
			myProperties= (other.myProperties)?(new Properties(*(other.myProperties))):nullptr;
			defaultProperties=other.defaultProperties;
		}
		return *this;
	}
  **/
  // This is state:
  const Hist2D                          *histogram;         // The histogram
  QRectF                                 nRectangle;        // The "natural" bounding rectangle
  Properties                            *myProperties;      // My properties.  May 
  Properties                             defaultProperties;
};


PlotHist2D::PlotHist2D(const Hist2D & histogram):
  Plotable(),c(new Clockwork())
{
  c->histogram=&histogram;
  c->nRectangle.setLeft  (histogram.minX());
  c->nRectangle.setRight(histogram.maxX());
  c->nRectangle.setTop(histogram.minY());
  c->nRectangle.setBottom(histogram.maxY());
}

// Copy constructor:
PlotHist2D::PlotHist2D(const PlotHist2D & source):
  Plotable(),c(new Clockwork(*(source.c)))
{
  
}

/** Assignment operator:
PlotHist2D & PlotHist2D::operator=(const PlotHist2D & source)
{
  if (&source!=this) {
    c.reset(new Clockwork(*(source.c)));
  }
  return *this;
} 
**/

// Destructor
PlotHist2D::~PlotHist2D(){
  //delete c;
}


const QRectF  PlotHist2D::rectHint() const {
  return c->nRectangle;
}


// Get the graphic description::
void PlotHist2D::describeYourselfTo(AbsPlotter *plotter) const {
  QPen pen =properties().pen;
  QBrush brush=properties().brush;
  QMatrix m=plotter->matrix(),mInverse=m.inverted();
  
  if (plotter->isLogX()) return;
  if (plotter->isLogY()) return;

  const auto & theHistogram=*(c->histogram);
  const double max = theHistogram.maxContents(); 
  const double inverseMax=1.0/max;
  const double wx = theHistogram.binWidthX();
  const double wy = theHistogram.binWidthY();
  for (unsigned int i=0;i<theHistogram.nBinsX();i++) {
    for (unsigned int j=0;j<theHistogram.nBinsY();j++) {
      
      double bin = theHistogram.bin(i,j);
      if (bin==0) continue;
      
      double x = theHistogram.binCenterX(i,j);
      double y = theHistogram.binCenterY(i,j);

      const double frac = bin*inverseMax;
      const double sqfrac = sqrt(frac);
      const double dx = wx*sqfrac*0.5;
      const double dy = wy*sqfrac*0.5;
      double minX =  x - dx;
      double maxX =  x + dx;
      double minY =  y - dy;
      double maxY =  y + dy;
      const auto & thisRectangle = plotter->rect();
      if (thisRectangle->contains(QPointF(minX, minY)) ||
	  thisRectangle->contains(QPointF(minX, maxY)) ||
	  thisRectangle->contains(QPointF(maxX, maxY)) ||
	  thisRectangle->contains(QPointF(maxX, minY)) ) {
	
	minX = std::max(theHistogram.minX(), minX);
	minY = std::max(theHistogram.minY(), minY);
	maxX = std::min(theHistogram.maxX(), maxX);
	maxY = std::min(theHistogram.maxY(), maxY);

	QPainterPath path;
	path.moveTo(m.map(QPointF(minX,minY)));
	path.lineTo(m.map(QPointF(minX,maxY)));
	path.lineTo(m.map(QPointF(maxX,maxY)));
	path.lineTo(m.map(QPointF(maxX,minY)));
	path.lineTo(m.map(QPointF(minX,minY)));
	
	
	QAbstractGraphicsShapeItem *shape = new QGraphicsPathItem(path);
	
	shape->setPen(pen);
	shape->setBrush(brush);
	shape->setMatrix(mInverse);
	plotter->scene()->addItem(shape);
	plotter->group()->addToGroup(shape);
	
      }
    }
  }
}


// Get the histogram:
const Hist2D *PlotHist2D::histogram() const {
  return c->histogram;
}

const PlotHist2D::Properties  PlotHist2D::properties() const { 
  return c->myProperties ? *c->myProperties : c->defaultProperties;
}

void PlotHist2D::setProperties(const Properties &  properties) { 
  delete c->myProperties;
  c->myProperties = new Properties(properties);
}

void PlotHist2D::resetProperties() {
  delete c->myProperties;
  c->myProperties=nullptr;
}

