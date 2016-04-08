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
#include "QatPlotting/LinToLog.h"
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QGraphicsPolygonItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsPathItem>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cfloat>
// Constructor

class PlotHist2D::Clockwork {
public:
  
  Clockwork() : histogram(nullptr),
		myProperties(nullptr) {}

  ~Clockwork() {if (myProperties) delete myProperties;}
    
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
  Plotable(),c(new Clockwork())
{
  c->histogram=source.c->histogram;
  c->nRectangle=source.c->nRectangle;
  if (source.c->myProperties) c->myProperties = new Properties(*source.c->myProperties);
  
}

// Assignment operator:
PlotHist2D & PlotHist2D::operator=(const PlotHist2D & source)
{
  if (&source!=this) {
    c->histogram=source.c->histogram;
    c->nRectangle=source.c->nRectangle;
    delete c->myProperties;
    if (source.c->myProperties) {
      c->myProperties = new Properties(*source.c->myProperties);
    }
    else {
      c->myProperties=nullptr;
    }
    c->defaultProperties = source.c->defaultProperties;
  }
  return *this;
} 


// Destructor
PlotHist2D::~PlotHist2D(){
  delete c;
}


const QRectF & PlotHist2D::rectHint() const {
  return c->nRectangle;
}


// Get the graphic description::
void PlotHist2D::describeYourselfTo(AbsPlotter *plotter) const {
  QPen pen =properties().pen;
  QBrush brush=properties().brush;
  QMatrix m=plotter->matrix(),mInverse=m.inverted();
  
  if (plotter->isLogX()) return;
  if (plotter->isLogY()) return;


  double max = c->histogram->maxContents(); 
  for (unsigned int i=0;i<c->histogram->nBinsX();i++) {
    for (unsigned int j=0;j<c->histogram->nBinsY();j++) {
      
      double bin = c->histogram->bin(i,j);
      if (bin==0) continue;
      
      double x = c->histogram->binCenterX(i,j);
      double y = c->histogram->binCenterY(i,j);
      double wx = c->histogram->binWidthX();
      double wy = c->histogram->binWidthY();
      
      double frac = bin/(max);
      double sqfrac = sqrt(frac);
      
      double minX =  x - wx*sqfrac/2.0;
      double maxX =  x + wx*sqfrac/2.0;
      double minY =  y - wy*sqfrac/2.0;
      double maxY =  y + wy*sqfrac/2.0;
      
      if (plotter->rect()->contains(QPointF(minX, minY)) ||
	  plotter->rect()->contains(QPointF(minX, maxY)) ||
	  plotter->rect()->contains(QPointF(maxX, maxY)) ||
	  plotter->rect()->contains(QPointF(maxX, minY)) ) {
	
	minX = std::max(c->histogram->minX(), minX);
	minY = std::max(c->histogram->minY(), minY);
	maxX = std::min(c->histogram->maxX(), maxX);
	maxY = std::min(c->histogram->maxY(), maxY);

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

const PlotHist2D::Properties & PlotHist2D::properties() const { 
  return c->myProperties ? *c->myProperties : c->defaultProperties;
}

void PlotHist2D::setProperties(const Properties &  properties) { 
  if (!c->myProperties) {
    c->myProperties = new Properties(properties);
  }
  else {
    *c->myProperties=properties;
  }
}

void PlotHist2D::resetProperties() {
  delete c->myProperties;
}

