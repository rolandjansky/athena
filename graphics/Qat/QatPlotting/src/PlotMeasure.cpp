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


#include "QatPlotting/PlotMeasure.h"
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

class PlotMeasure::Clockwork {


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
PlotMeasure::PlotMeasure():
  Plotable(),c(new Clockwork())
{
  c->nRectangle.setLeft  (+1.0E100);
  c->nRectangle.setRight (-1.0E100);
  c->nRectangle.setTop   (+1.0E100);
  c->nRectangle.setBottom(-1.0E100);
  
}



// Destructor
PlotMeasure::~PlotMeasure(){
  delete c;
}



const QRectF & PlotMeasure::rectHint() const {
  return c->nRectangle;
}




void PlotMeasure::addPoint( const QPointF & point, double sizePlus, double sizeMnus) {
  
  c->points.push_back(point);
  c->sizePlus.push_back(sizePlus);
  c->sizeMnus.push_back(sizeMnus);

  
  c->nRectangle.setLeft(std::min(c->nRectangle.left(),point.x()-sizeMnus));
  c->nRectangle.setRight(std::max(c->nRectangle.right(),point.x()+sizePlus));
  c->nRectangle.setBottom(std::min(c->nRectangle.bottom(),point.y()));
  c->nRectangle.setTop(std::max(c->nRectangle.top(),point.y()));
}


void PlotMeasure::describeYourselfTo(AbsPlotter * plotter) const {

  QPen pen =properties().pen;
  QBrush brush=properties().brush;
  int symbolSize=properties().symbolSize;
  Properties::SymbolStyle symbolStyle=properties().symbolStyle;
  
  LinToLog *toLogX= plotter->isLogX() ? new LinToLog (plotter->rect()->left(),plotter->rect()->right()) : NULL;
  LinToLog *toLogY= plotter->isLogY() ? new LinToLog (plotter->rect()->top(),plotter->rect()->bottom()) : NULL;

  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  
  for (unsigned int i=0;i<c->points.size();i++) {
    double x = plotter->isLogX() ? (*toLogX) (c->points[i].x()) : c->points[i].x();
    
    double y = plotter->isLogY() ? (*toLogY) (c->points[i].y()) : c->points[i].y();
    double  xdxp = plotter->isLogX() ? (*toLogX)(c->points[i].x() + c->sizePlus[i]) : c->points[i].x() + c->sizePlus[i];
    double  xdxm = plotter->isLogX() ? (*toLogX)(c->points[i].x() - c->sizeMnus[i]) : c->points[i].x() - c->sizeMnus[i];
    
    QPointF loc(x, y );
    QSizeF  siz(symbolSize,symbolSize);
    QPointF ctr(siz.width()/2.0, siz.height()/2.0);
    QPointF had(siz.width()/2.0, 0);
    QPointF vad(0,siz.height()/2.0);
    //QPointF penShiftLeft(-pen.widthF()/2.0, 0.0);
    QPointF penShiftLeft(0,0);
    QPointF plus(xdxp,y);
    QPointF mnus(xdxm,y);
    
    if (plotter->rect()->contains(loc)) {
      QAbstractGraphicsShapeItem *shape=NULL;
      if (symbolStyle==Properties::CIRCLE) {
	shape = new QGraphicsEllipseItem(QRectF(m.map(loc)-ctr,siz));
      }
      else if (symbolStyle==Properties::SQUARE) {
	shape = new QGraphicsRectItem(QRectF(m.map(loc)-ctr,siz));
      }
      else if (symbolStyle==Properties::TRIANGLE_U) {
	QVector<QPointF> points;
	points.push_back(m.map(loc)-ctr+(QPointF(0,symbolSize)));
	points.push_back(m.map(loc)-ctr+(QPointF(symbolSize,symbolSize)));
	points.push_back(m.map(loc)-ctr+(QPointF(symbolSize/2,0)));
	points.push_back(m.map(loc)-ctr+(QPointF(0,symbolSize)));
	shape = new QGraphicsPolygonItem(QPolygonF(points));
      }
      else if (symbolStyle==Properties::TRIANGLE_L) {
	QVector<QPointF> points;
	points.push_back(m.map(loc)-ctr+(QPointF(0,            0)));
	points.push_back(m.map(loc)-ctr+(QPointF(symbolSize,   0)));
	points.push_back(m.map(loc)-ctr+(QPointF(symbolSize/2, symbolSize)));
	points.push_back(m.map(loc)-ctr+(QPointF(0,            0)));
	shape = new QGraphicsPolygonItem(QPolygonF(points));
      }
      else {
	throw std::runtime_error("In PlotMeasure:  unknown plot symbol");
      }
      
      shape->setPen(pen);
      shape->setBrush(brush);
      shape->setMatrix(mInverse);
      plotter->scene()->addItem(shape);
      plotter->group()->addToGroup(shape);
      
      {
	QLineF  pLine(m.map(loc)-had, m.map(plus));
	if (plotter->rect()->contains(plus)) {
	  QGraphicsLineItem *line=new QGraphicsLineItem(pLine);
	  line->setPen(pen);
	  line->setMatrix(mInverse);
	  plotter->scene()->addItem(line);
	  plotter->group()->addToGroup(line);
	  
	  QGraphicsLineItem *topLine=new QGraphicsLineItem(QLineF(m.map(plus)+vad+penShiftLeft,m.map(plus)-vad+penShiftLeft));
	  topLine->setPen(pen);
	  topLine->setMatrix(mInverse);
	  plotter->scene()->addItem(topLine);
	  plotter->group()->addToGroup(topLine);
	  
	}
	else {
	  QPointF intersection;
	  QLineF bottomLine(plotter->rect()->bottomRight(),plotter->rect()->topRight());
	  QLineF::IntersectType type=bottomLine.intersect(QLineF(loc,plus),&intersection);
	  if (type==QLineF::BoundedIntersection) {
	    QPointF plus=intersection;
	    QLineF  pLine(m.map(loc)-had, m.map(plus));
	    QGraphicsLineItem *line=new QGraphicsLineItem(pLine);
	    line->setPen(pen);
	    line->setMatrix(mInverse);
	    plotter->scene()->addItem(line);
	    plotter->group()->addToGroup(line);
	  }
	}
      }
      
      {
	QLineF  mLine(m.map(loc)+had, m.map(mnus));
	if (plotter->rect()->contains(mnus)) {
	  QGraphicsLineItem *line=new QGraphicsLineItem(mLine);
	  line->setPen(pen);
	  line->setMatrix(mInverse);
	  plotter->scene()->addItem(line);
	  plotter->group()->addToGroup(line);
	  
	  QGraphicsLineItem *bottomLine=new QGraphicsLineItem(QLineF(m.map(mnus)+vad+penShiftLeft,m.map(mnus)-vad+penShiftLeft));
	  bottomLine->setPen(pen);
	  bottomLine->setMatrix(mInverse);
	  plotter->scene()->addItem(bottomLine);
	  plotter->group()->addToGroup(bottomLine);
	  
	}
	else {
	  QPointF intersection;
	  QLineF topLine(plotter->rect()->topLeft(),plotter->rect()->bottomLeft());
	  QLineF::IntersectType type=topLine.intersect(QLineF(loc,mnus),&intersection);
	  if (type==QLineF::BoundedIntersection) {
	    QPointF mnus=intersection;
	    QLineF  mLine(m.map(loc)+had, m.map(mnus));
	    QGraphicsLineItem *line=new QGraphicsLineItem(mLine);
	    line->setPen(pen);
	    line->setMatrix(mInverse);
	    plotter->scene()->addItem(line);
	    plotter->group()->addToGroup(line);
	  }
	}
      }
    }
    else if (plotter->rect()->contains(mnus)) {
      QPointF intersection;
      QLineF bottomLine(plotter->rect()->bottomLeft(),plotter->rect()->bottomRight());
      QLineF::IntersectType type=bottomLine.intersect(QLineF(loc,mnus),&intersection);
      if (type==QLineF::BoundedIntersection) {
	QPointF loc=intersection;
	QLineF  mLine(m.map(loc), m.map(mnus));
	QGraphicsLineItem *line=new QGraphicsLineItem(mLine);
	line->setPen(pen);
	line->setMatrix(mInverse);
	plotter->scene()->addItem(line);
	plotter->group()->addToGroup(line);
	
	QGraphicsLineItem *bottomLine=new QGraphicsLineItem(QLineF(m.map(mnus)+vad-QPointF(pen.widthF(),0),m.map(mnus)-vad));
	bottomLine->setPen(pen);
	bottomLine->setMatrix(mInverse);
	plotter->scene()->addItem(bottomLine);
	plotter->group()->addToGroup(bottomLine);
	
      }
    }
    else if (plotter->rect()->contains(plus)) {
      QPointF intersection;
      QLineF topLine(plotter->rect()->topLeft(),plotter->rect()->topRight());
      QLineF::IntersectType type=topLine.intersect(QLineF(loc,plus),&intersection);
      if (type==QLineF::BoundedIntersection) {
	QPointF loc=intersection;
	QLineF  pLine(m.map(loc), m.map(plus));
	QGraphicsLineItem *line=new QGraphicsLineItem(pLine);
	line->setPen(pen);
	line->setMatrix(mInverse);
	plotter->scene()->addItem(line);
	plotter->group()->addToGroup(line);
	
	QGraphicsLineItem *topLine=new QGraphicsLineItem(QLineF(m.map(plus)+vad+penShiftLeft,m.map(plus)-vad+penShiftLeft));
	topLine->setPen(pen);
	topLine->setMatrix(mInverse);
	plotter->scene()->addItem(topLine);
	plotter->group()->addToGroup(topLine);
	
      }
    }
  }
  delete toLogX;
  delete toLogY;
}

const PlotMeasure::Properties & PlotMeasure::properties() const { 
  return c->myProperties ? *c->myProperties : c->defaultProperties;
}

void PlotMeasure::setProperties(const Properties &  properties) { 
  if (!c->myProperties) {
    c->myProperties = new Properties(properties);
  }
  else {
    *c->myProperties=properties;
  }
}

void PlotMeasure::resetProperties() {
  delete c->myProperties;
}

