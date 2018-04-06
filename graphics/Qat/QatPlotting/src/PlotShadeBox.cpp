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


#include "QatPlotting/PlotShadeBox.h"
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
#include <memory>
class PlotShadeBox::Clockwork {


public:

  Clockwork():myProperties(NULL) {}
  ~Clockwork() { delete myProperties;}
  
  QRectF                                nRectangle; 
  Properties                            *myProperties;
  Properties                            defaultProperties;
  std::vector<QPointF>                  points;
  std::vector<double>                   sizePlus;  
  std::vector<double>                   sizeMnus;
};


// Constructor
PlotShadeBox::PlotShadeBox():
  Plotable(),m_c(new Clockwork())
{
  m_c->nRectangle.setLeft  (+1.0E100);
  m_c->nRectangle.setRight (-1.0E100);
  m_c->nRectangle.setTop   (+1.0E100);
  m_c->nRectangle.setBottom(-1.0E100);
  
}



// Destructor
PlotShadeBox::~PlotShadeBox(){
  delete m_c;
}



const QRectF  PlotShadeBox::rectHint() const {
  return m_c->nRectangle;
}



  // Add Points:
void PlotShadeBox::addPoint(const QPointF & point, double errorPlus, double errorMinus) {
  m_c->points.push_back(point);
  m_c->sizePlus.push_back(errorPlus);
  m_c->sizeMnus.push_back(errorMinus);

  
  m_c->nRectangle.setLeft(std::min(m_c->nRectangle.left(),point.x()));
  m_c->nRectangle.setRight(std::max(m_c->nRectangle.right(),point.x()));
  m_c->nRectangle.setBottom(std::min(m_c->nRectangle.bottom(),point.y()+errorPlus));
  m_c->nRectangle.setTop(std::max(m_c->nRectangle.top(),point.y()-errorMinus));
}

void PlotShadeBox::addPoint( const QPointF & point, double size) {
  
  m_c->points.push_back(point);
  m_c->sizePlus.push_back(size);
  m_c->sizeMnus.push_back(size);

  
  m_c->nRectangle.setLeft(std::min(m_c->nRectangle.left(),point.x()));
  m_c->nRectangle.setRight(std::max(m_c->nRectangle.right(),point.x()));
  m_c->nRectangle.setBottom(std::min(m_c->nRectangle.bottom(),point.y()+size));
  m_c->nRectangle.setTop(std::max(m_c->nRectangle.top(),point.y()-size));
}


void PlotShadeBox::describeYourselfTo(AbsPlotter * plotter) const {

  QPen pen =properties().pen;
  QBrush brush=properties().brush;
  const double hSize = properties().horizontalSize;
  const double halfLength = hSize*0.5;
  //fix coverity issue 16789, 16790 (sroe)
  using LogFnPtr_t = std::unique_ptr<LinToLog> ;
  
  const auto rectangle = plotter->rect();
  const auto left = rectangle->left();
  const auto right = rectangle->right();
  const auto bottom = rectangle->bottom();
  const auto top = rectangle->top();
  const bool xAxisIsLogScale(plotter->isLogX());
  const bool yAxisIsLogScale(plotter->isLogX());
  LogFnPtr_t toLogX( xAxisIsLogScale ? new LinToLog (left,right) : nullptr);
  LogFnPtr_t toLogY( yAxisIsLogScale ? new LinToLog (top, bottom) : nullptr);

  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  for (unsigned int i=0;i<m_c->points.size();i++) {
    double  xdxp = m_c->points[i].x() + halfLength;
    double  xdxm = m_c->points[i].x() - halfLength;
    double  ydyp = m_c->points[i].y() + m_c->sizePlus[i];
    double  ydym = m_c->points[i].y() - m_c->sizeMnus[i];
    
    xdxp = xAxisIsLogScale ? (*toLogX) (xdxp): xdxp;
    xdxm = xAxisIsLogScale ? (*toLogX) (xdxm): xdxm;
    ydyp = yAxisIsLogScale ? (*toLogY) (ydyp): ydyp;
    ydym = yAxisIsLogScale ? (*toLogY) (ydym): ydym;
    
    if (ydyp<top) return;
    if (ydym>bottom) return;
    if (xdxp<left) return;
    if (xdxm>right) return;

    xdxp = std::min(xdxp,right);
    xdxm = std::max(xdxm,left);
    ydyp = std::min(ydyp,bottom);
    ydym = std::max(ydym,top);

    QRectF rect;
    rect.setTop(ydym);
    rect.setBottom(ydyp);
    rect.setLeft(xdxm);
    rect.setRight(xdxp);
    QGraphicsRectItem *shape = new QGraphicsRectItem(m.mapRect(rect));
        
    shape->setPen(pen);
    shape->setBrush(brush);
    shape->setMatrix(mInverse);
    plotter->scene()->addItem(shape);
    plotter->group()->addToGroup(shape);
  }

}

const PlotShadeBox::Properties  PlotShadeBox::properties() const { 
  return m_c->myProperties ? *m_c->myProperties : m_c->defaultProperties;
}

void PlotShadeBox::setProperties(const Properties &  properties) { 
  if (!m_c->myProperties) {
    m_c->myProperties = new Properties(properties);
  }
  else {
    *m_c->myProperties=properties;
  }
}

void PlotShadeBox::resetProperties() {
  delete m_c->myProperties;
}

