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


#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/LinToLog.h"
#include "QatPlotting/AbsPlotter.h"
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QGraphicsPolygonItem>
#include <QtGui/QGraphicsRectItem>
#include <iostream>
#include <iomanip>
#include <sstream>

class PlotPoint::Clockwork {

  public:

  Clockwork():x(0.0),y(0.0),nRectangle(),myProperties(nullptr),defaultProperties()  {}
  ~Clockwork() { if (myProperties) delete myProperties; myProperties=nullptr;}


  double                                x,y;
  QRectF                                nRectangle;   
  Properties                            *myProperties;
  Properties                            defaultProperties;
};



PlotPoint::PlotPoint (const PlotPoint & right):Plotable(),c(new Clockwork()){
  if (right.c->myProperties) {
    c->myProperties= new Properties(*right.c->myProperties);
  }
  c->x=right.c->x;
  c->y=right.c->y;
  c->nRectangle=right.c->nRectangle;
}

PlotPoint & PlotPoint::operator=(const PlotPoint & right) {
  if (&right!=this) {
    if (c->myProperties) delete c->myProperties; c->myProperties=nullptr;
    if (right.c->myProperties) c->myProperties= new Properties(*(right.c->myProperties));
    c->x=right.c->x;
    c->y=right.c->y;
    c->nRectangle=right.c->nRectangle;
    c->defaultProperties = right.c->defaultProperties;
  }
  return *this;
}

// Constructor
PlotPoint::PlotPoint(double x, double y)
  :Plotable(),c(new Clockwork())

{
  c->x=x;
  c->y=y;
  
  c->nRectangle=QRectF(c->x-1E-10, c->y-1E-10, 2E-10, 2E-10);

}



// Destructor
PlotPoint::~PlotPoint(){
  delete c;
}


// Get the "natural maximum R"
const QRectF & PlotPoint::rectHint() const {
  return c->nRectangle;
}



// Describe to plotter, in terms of primitives:
void PlotPoint::describeYourselfTo(AbsPlotter *plotter) const{

  LinToLog *toLogX= plotter->isLogX() ? new LinToLog (plotter->rect()->left(),plotter->rect()->right()) : NULL;
  LinToLog *toLogY= plotter->isLogY() ? new LinToLog (plotter->rect()->top(),plotter->rect()->bottom()) : NULL;

  double x=c->x;
  double y=c->y;

  QPen pen =properties().pen;
  QBrush brush=properties().brush;
  int symbolSize=properties().symbolSize;
  PlotPoint::Properties::SymbolStyle symbolStyle=properties().symbolStyle;
  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  if (toLogX) x = (*toLogX)(x);
  if (toLogY) y = (*toLogY)(y);

  QPointF loc(x, y );
  QSizeF  siz(symbolSize,symbolSize);
  QPointF ctr(siz.width()/2.0, siz.height()/2.0);
  QPointF had(siz.width()/2.0, 0);
  QPointF vad(0,siz.height()/2.0);
      
  if (plotter->rect()->contains(loc)) {
    QAbstractGraphicsShapeItem *shape=NULL;
    if (symbolStyle==PlotPoint::Properties::CIRCLE) {
      shape = new QGraphicsEllipseItem(QRectF(m.map(loc)-ctr,siz));
    }
    else if (symbolStyle==PlotPoint::Properties::SQUARE) {
      shape = new QGraphicsRectItem(QRectF(m.map(loc)-ctr,siz));
    }
    else if (symbolStyle==PlotPoint::Properties::TRIANGLE_U) {
      QVector<QPointF> points;
      points.push_back(m.map(loc)-ctr+(QPointF(0,symbolSize)));
      points.push_back(m.map(loc)-ctr+(QPointF(symbolSize,symbolSize)));
      points.push_back(m.map(loc)-ctr+(QPointF(symbolSize/2,0)));
      points.push_back(m.map(loc)-ctr+(QPointF(0,symbolSize)));
      shape = new QGraphicsPolygonItem(QPolygonF(points));
    }
    else if (symbolStyle==PlotPoint::Properties::TRIANGLE_L) {
      QVector<QPointF> points;
      points.push_back(m.map(loc)-ctr+(QPointF(0,            0)));
      points.push_back(m.map(loc)-ctr+(QPointF(symbolSize,   0)));
      points.push_back(m.map(loc)-ctr+(QPointF(symbolSize/2, symbolSize)));
      points.push_back(m.map(loc)-ctr+(QPointF(0,            0)));
      shape = new QGraphicsPolygonItem(QPolygonF(points));
    }
    else {
      throw std::runtime_error("In PlotPoint:  unknown plot symbol");
    }
    
    shape->setPen(pen);
    shape->setBrush(brush);
    shape->setMatrix(mInverse);
    plotter->scene()->addItem(shape);
    plotter->group()->addToGroup(shape);
    
  }

  delete toLogX;
  delete toLogY;
}

const PlotPoint::Properties & PlotPoint::properties() const { 
  return c->myProperties ? *c->myProperties : c->defaultProperties;
}

void PlotPoint::setProperties(const Properties &  properties) { 
  if (!c->myProperties) {
    c->myProperties = new Properties(properties);
  }
  else {
    *c->myProperties=properties;
  }
}

void PlotPoint::resetProperties() {
  delete c->myProperties;
}

