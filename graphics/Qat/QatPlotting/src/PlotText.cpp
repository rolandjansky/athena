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


#include "QatPlotting/PlotText.h"
#include "QatPlotting/LinToLog.h"
#include "QatPlotting/AbsPlotter.h"
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QPainterPath>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QTextDocument>
#include <iostream>
#include <iomanip>
#include <sstream>

class PlotText::Clockwork {

  public:

  Clockwork():point(),textDocument(nullptr),nRect(nullptr){}
  ~Clockwork() {
    if (textDocument) delete textDocument; textDocument=nullptr;
    if (nRect) delete nRect; nRect=nullptr;
  }


  QPointF                               point;
  QTextDocument                         *textDocument;
  QRectF                                *nRect;
};



PlotText::PlotText (const PlotText & right):Plotable(),c(new Clockwork()){
  c->point=right.c->point;
  c->textDocument=right.c->textDocument->clone();
  c->nRect=nullptr;
}

PlotText & PlotText::operator=(const PlotText & right) {
  if (&right!=this) {
    c->point=right.c->point;
    if (c->textDocument) delete c->textDocument;
    c->textDocument =  nullptr;
    if (right.c->textDocument) c->textDocument=right.c->textDocument->clone();
    c->nRect=nullptr;
  }
  return *this;
}

// Constructor
PlotText::PlotText(double x, double y, const QString & text)
  :Plotable(),c(new Clockwork())

{
  c->point=QPointF(x,y);
  c->textDocument=new QTextDocument(text);
  c->nRect=nullptr;
}



// Destructor
PlotText::~PlotText(){
  delete c;
}


// Get the "natural maximum R"
const QRectF & PlotText::rectHint() const {
  if (!c->nRect) {
    QGraphicsTextItem aux;
    aux.setDocument(c->textDocument);
    aux.setPos(c->point);
    aux.adjustSize();
    c->nRect = new QRectF( aux.boundingRect());
  }
  return *c->nRect;
}



// Describe to plotter, in terms of primitives:
void PlotText::describeYourselfTo(AbsPlotter *plotter) const{

  LinToLog *toLogX= plotter->isLogX() ? new LinToLog (plotter->rect()->left(),plotter->rect()->right()) : nullptr;
  LinToLog *toLogY= plotter->isLogY() ? new LinToLog (plotter->rect()->top(),plotter->rect()->bottom()) : nullptr;

  double x=c->point.x();
  double y=c->point.y();

  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  if (toLogX) x = (*toLogX)(x);
  if (toLogY) y = (*toLogY)(y);



  QGraphicsTextItem *item = new QGraphicsTextItem();
  //unused variables (sroe)
  //QPointF p=c->point;
  //QPointF p1  = m.map(c->point);
  //QPointF p2 = mInverse.map(c->point);
 

  QPointF P(plotter->rect()->left(), plotter->rect()->bottom());
  QPointF Q(c->point);

  item->setDocument(c->textDocument);
  item->setPos(Q-P);
  item->setMatrix(mInverse);
  plotter->scene()->addItem(item);
  plotter->group()->addToGroup(item);
  
  delete toLogX;
  delete toLogY;
}


void PlotText::setDocument(QTextDocument *document) {
  delete c->textDocument;
  c->textDocument=document->clone();
}


