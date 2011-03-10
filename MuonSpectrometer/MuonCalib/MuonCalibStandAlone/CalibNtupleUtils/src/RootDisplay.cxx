/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleUtils/RootDisplay.h"

#include "TObject.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TArc.h"
#include "TBox.h"
#include "TVector3.h"
#include "TPaveText.h"
#include "TText.h"

#include <iostream>
#include <math.h>

Display::Display(const char* name,
		 double xll,double yll,
		 double xrh,double yrh,int size) 
    : m_xll(xll),m_yll(yll),
      m_xrh(xrh),m_yrh(yrh),
      m_name(name),m_size(size),m_canvas(0)
{ 
  m_textBox = new TPaveText(.7, .25, .9, .35, "blNDC");
}


Display::~Display() 
{ 
  //clear(); 
  //delete m_canvas;
}


void Display::print(std::string name){
  if(m_canvas) m_canvas->Print( (name+".eps").c_str(),"eps"); 
}


void Display::update(){
  if(m_canvas) m_canvas->Update();
}


TCanvas* Display::canvas() { return m_canvas; }


void Display::clear()
{
  m_textBox->Clear();
  while(m_lines.size()!=0){
    delete m_lines.back();
    m_lines.pop_back();
  }
  while(m_arcs.size()!=0){
    delete m_arcs.back();
    m_arcs.pop_back();
  }
  while(m_boxes.size()!=0){
    delete m_boxes.back();
    m_boxes.pop_back();
  }
  delete m_canvas;
  m_canvas = 0;
}


void  Display::setSize(int size) 
{ 
  m_size = size;
  if( m_canvas ){
    delete m_canvas;
    m_canvas = 0;
  }
}


void Display::calculateCanvasSize(int &width, int& height)
{
  double w = m_xrh-m_xll;
  double h = m_yrh-m_yll;
  double scale_factor = std::max( w, h ); 
  h = m_size*h/scale_factor;
  w = m_size*w/scale_factor;

  width  = (int)w;
  height = (int)h; 
}

void Display::setTextBox(std::string /*text*/)
{
  m_textBox->Clear();
  //  TText *t0= m_textBox->AddText( "evt/pat/seg" );
  //  TText *t1= m_textBox->AddText( text.c_str() );
 }

void Display::calculateRange()
{
  double xll(0.0);
  double yll(0.0);
  double xrh(0.0);
  double yrh(0.0);
  bool first(true);

  std::vector<TArc*>::const_iterator ait = m_arcs.begin();
  for( ; ait!=m_arcs.end();++ait){
    double r = fabs( (*ait)->GetR1() );
    //std::cout << " new arc (" << (*ait)->GetX1() << "," << (*ait)->GetY1() << "), r " << r << std::endl;
    if(first) {
      xll = (*ait)->GetX1() - r;
      yll = (*ait)->GetY1() - r;
      xrh = (*ait)->GetX1() + r;
      yrh = (*ait)->GetY1() + r;
      first = false;
    }else{
      xll = std::min( xll, (*ait)->GetX1() - r );
      yll = std::min( yll, (*ait)->GetY1() - r );
      xrh = std::max( xrh, (*ait)->GetX1() + r );
      yrh = std::max( yrh, (*ait)->GetY1() + r );
    }
    //    std::cout << " new range (" << xll << "," << yll << ")  (" << xrh << "," << yrh << ") " << std::endl;
  }
  double offset = 50.;
  m_xll = xll - offset;
  m_yll = yll - offset;
  m_xrh = xrh + offset;
  m_yrh = yrh + offset;
  std::cout << " new range (" << m_xll << "," << m_yll << ")  (" << m_xrh << "," << m_yrh << ") " << std::endl;
}


void Display::draw()
{
//   int w,h;
//   calculateRange();
//   calculateCanvasSize(w,h);
//   if(!m_canvas){ 
//     m_canvas = new TCanvas(m_name.c_str(),m_name.c_str(),w,h); 
//   }else{
//     m_canvas->SetCanvasSize(w,h);
//   }

  //  std::cout << "Drawing event: " << m_lines.size() << " lines, " 
  //	    << m_arcs.size() + m_boxes.size() << " hits " << std::endl;

  //  m_canvas->Range(m_xll,m_yll,m_xrh,m_yrh);

//   std::cout << "Canvas: " << std::endl;
//   std::cout << "         " << m_yrh << std::endl;
//   std::cout << h << std::endl;
//   std::cout << "         " << m_yll << std::endl;
//   std::cout << "               " << m_xll << "                    " << m_xrh << std::endl;
//   std::cout << "                           " << w << std::endl;

  m_canvas->cd(); 
  //  m_textBox->Draw("same"); 
  m_textBox->Draw();
  std::vector<TObject*>::const_iterator oit = m_geometry.begin();
  for( ; oit!=m_geometry.end();++oit){
    (*oit)->Draw("same");
  }
  std::vector<TLine*>::const_iterator lit = m_lines.begin();
  for( ; lit!=m_lines.end();++lit){
    (*lit)->Draw("same");
  }
  std::vector<TArc*>::const_iterator ait = m_arcs.begin();
  for( ; ait!=m_arcs.end();++ait){
    //    std::cout << "    " << (*ait)->GetX1() << " | " <<  (*ait)->GetY1() << std::endl;
    (*ait)->Draw("same");
  }
  std::vector<TBox*>::const_iterator bit = m_boxes.begin();
  for( ; bit!=m_boxes.end();++bit){
    (*bit)->Draw("same");
  }
 
  m_canvas->Update();
  m_canvas->Draw();
  m_canvas->Update();
  m_canvas->Draw();
  
}

void ShapeCreator::applyProjection(const TVector3& pos, double& x1, double& x2){
  if(m_projection == 0){
    x1 = pos.x();
    x2 = pos.y();
  }else if(m_projection == 1){
    x1 = pos.x();
    x2 = pos.z();
  }else if(m_projection == 2){
    x1 = pos.z();
    x2 = pos.y();
  }else if(m_projection == 3){
    x1 = pos.y();
    x2 = pos.z();
  }else if(m_projection == 4){
    x2 = sqrt( pos.y()*pos.y() + pos.x()*pos.x() );
    x1 = pos.z();
  }
  //  std::cout << "in projection x1,x2 = " << x1 << " " << x2 << std::endl;
  
}


void ShapeCreator::setProjection(int p)
{
  if(p != 0 && p != 1 && p != 2 && p != 3 && p != 4)
    std::cout << "ERROR Wrong projection " << std::endl;
  m_projection = p;
}


TLine* ShapeCreator::createLine(const TVector3& pos, const TVector3& dir, double length, bool half)
{
  double x1,x2;
  double dx1,dx2;
  
  applyProjection(pos,x1,x2);
  applyProjection(dir,dx1,dx2);
  
  double xu1, xu2, xl1, xl2;
  if( !half ){
    xu1 = x1 + 0.5*dx1*length;
    xu2 = x2 + 0.5*dx2*length;
    xl1 = x1 - 0.5*dx1*length;
    xl2 = x2 - 0.5*dx2*length;
  } else {
    xu1 = x1 + 0.5*dx1*length;
    xu2 = x2 + 0.5*dx2*length;
    xl1 = x1 ;
    xl2 = x2 ;
  }
  
  return new TLine(xu1,xu2,xl1,xl2);
}


TArc*  ShapeCreator::createArc(const TVector3& pos, double r)
{
  double x1,x2;
  applyProjection(pos,x1,x2);

  return new TArc(x1,x2,r);
}

TArc* ShapeCreator::createTube(const TVector3& pos)
{
  double x1,x2;
  applyProjection(pos,x1,x2);
  TArc* arc = new TArc(x1, x2, 14.6);
  arc->SetLineColor(7);

  return arc;
}


TBox*  ShapeCreator::createBox(const TVector3& upperleft, const TVector3& lowerright)
{
  double x1,x2;
  double y1,y2;
  
  applyProjection(upperleft,x1,x2);
  applyProjection(lowerright,y1,y2);


  return new TBox(x1,x2,y1,y2);
}



