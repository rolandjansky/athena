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
    delete textDocument; textDocument=nullptr;
    delete nRect; nRect=nullptr;
  }


  QPointF                               point;
  QTextDocument                         *textDocument;
  QRectF                                *nRect;
};



PlotText::PlotText (const PlotText & right):Plotable(),m_c(new Clockwork()){
  m_c->point=right.m_c->point;
  m_c->textDocument=right.m_c->textDocument->clone();
  delete m_c->nRect;
  m_c->nRect=new QRectF(*(right.m_c->nRect));
}

PlotText & PlotText::operator=(const PlotText & right) {
  if (&right!=this) {
    Plotable::operator=(right);
    delete m_c;
    m_c=new Clockwork();
    m_c->point=right.m_c->point;
    if (right.m_c->textDocument) m_c->textDocument=right.m_c->textDocument->clone();
    m_c->nRect=new QRectF(*(right.m_c->nRect));
  }
  return *this;
}

// Constructor
PlotText::PlotText(double x, double y, const QString & text)
  :Plotable(),m_c(new Clockwork())

{
  m_c->point=QPointF(x,y);
  m_c->textDocument=new QTextDocument(text);
  m_c->nRect=nullptr;
}



// Destructor
PlotText::~PlotText(){
  delete m_c;
}


// Get the "natural maximum R"
const QRectF  PlotText::rectHint() const {
  if (!m_c->nRect) {
    QGraphicsTextItem aux;
    aux.setDocument(m_c->textDocument);
    aux.setPos(m_c->point);
    aux.adjustSize();
    m_c->nRect = new QRectF( aux.boundingRect());
  }
  return *m_c->nRect;
}



// Describe to plotter, in terms of primitives:
void PlotText::describeYourselfTo(AbsPlotter *plotter) const{

  LinToLog *toLogX= plotter->isLogX() ? new LinToLog (plotter->rect()->left(),plotter->rect()->right()) : nullptr;
  LinToLog *toLogY= plotter->isLogY() ? new LinToLog (plotter->rect()->top(),plotter->rect()->bottom()) : nullptr;

  double x=m_c->point.x();
  double y=m_c->point.y();

  QMatrix m=plotter->matrix(),mInverse=m.inverted();

  if (toLogX) x = (*toLogX)(x);
  if (toLogY) y = (*toLogY)(y);



  QGraphicsTextItem *item = new QGraphicsTextItem();
  //unused variables (sroe)
  //QPointF p=m_c->point;
  //QPointF p1  = m.map(m_c->point);
  //QPointF p2 = mInverse.map(m_c->point);
 

  QPointF P(plotter->rect()->left(), plotter->rect()->bottom());
  QPointF Q(m_c->point);

  item->setDocument(m_c->textDocument);
  item->setPos(Q-P);
  item->setMatrix(mInverse);
  plotter->scene()->addItem(item);
  plotter->group()->addToGroup(item);
  
  delete toLogX;
  delete toLogY;
}


void PlotText::setDocument(QTextDocument *document) {
  delete m_c->textDocument;
  m_c->textDocument=document->clone();
}


