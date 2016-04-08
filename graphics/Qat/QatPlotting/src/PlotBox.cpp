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


#include "QatPlotting/PlotBox.h"
#include "QatPlotting/AbsPlotter.h"
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItemGroup>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <array>

class PlotBox::Clockwork {

  public:

  Clockwork():rectangle(), myProperties(nullptr), defaultProperties(),x(0),y(0),dx(0),dy(0){}
  ~Clockwork() { delete myProperties;}


  
  QRectF                                rectangle;         // The "natural" bounding rectangle
  Properties                            *myProperties;
  Properties                            defaultProperties;
  double                                x,y,dx,dy;
};

// Constructor
PlotBox::PlotBox(double x, double y, double dx, double dy) 

  :Plotable(),c(new Clockwork())

{
  c->rectangle=QRectF(QPointF(x,y),QSizeF(2*dx, 2*dy));
  c->x=x;
  c->y=y;
  c->dx=dx;
  c->dy=dy;
}



// Destructor
PlotBox::~PlotBox(){
  delete c;
}


// Get the "natural maximum R"
const QRectF & PlotBox::rectHint() const {
  return c->rectangle;
}



// Describe to plotter, in terms of primitives:
void PlotBox::describeYourselfTo(AbsPlotter *plotter) const{
  if (plotter->isLogY()) return;
  if (plotter->isLogX()) return;

  QPen pen =properties().pen;
  QBrush brush=properties().brush;
  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  QPainterPath path;
  bool started=false;
  static CLHEP::HepVector U0(2); U0[0]=+c->dx; U0[1]=+c->dy;
  static CLHEP::HepVector U1(2); U1[0]=+c->dx; U1[1]=-c->dy;
  static CLHEP::HepVector U2(2); U2[0]=-c->dx; U2[1]=-c->dy;
  static CLHEP::HepVector U3(2); U3[0]=-c->dx; U3[1]=+c->dy;
  std::array<CLHEP::HepVector,5> plusU={U0, U1, U2, U3, U0};
  for (int i=0;i<5;i++) {
    CLHEP::HepVector V(2);
    V[0]=c->x;
    V[1]=c->y;
    V+=plusU[i];
    if (plotter->rect()->contains(QPointF(V[0],V[1]))){
      double x = V[0];
      double y = V[1];
      if (!started) {
	      started=true;
	      path.moveTo(m.map(QPointF(x,y)));
      } else {
	      path.lineTo(m.map(QPointF(x,y)));
      }
    }
  }

  QGraphicsPathItem *polyline=new QGraphicsPathItem(path);
  polyline->setPen(pen);
  polyline->setBrush(brush);
  polyline->setMatrix(mInverse);
  plotter->scene()->addItem(polyline);
  plotter->group()->addToGroup(polyline);

}



const PlotBox::Properties & PlotBox::properties() const { 
  return c->myProperties ? *c->myProperties : c->defaultProperties;
}

void PlotBox::setProperties(const Properties &  properties) { 
  if (!c->myProperties) {
    c->myProperties = new Properties(properties);
  }
  else {
    *c->myProperties=properties;
  }
}

void PlotBox::resetProperties() {
  delete c->myProperties;
}

