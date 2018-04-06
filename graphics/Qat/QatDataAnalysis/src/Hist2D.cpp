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
#include <stdexcept>
Hist2D::Hist2D(const std::string & name, size_t nBinsX, double minX, double maxX,
	                                  size_t nBinsY, double minY, double maxY):
  m_c(new Clockwork)
{
  m_c->name=name;
  m_c->nBinsX=nBinsX;
  m_c->nBinsY=nBinsY;
  m_c->minX=minX;
  m_c->maxX=maxX;
  m_c->minY=minY;
  m_c->maxY=maxY;
  m_c->binWidthX=(maxX-minX)/nBinsX;
  m_c->binWidthY=(maxY-minY)/nBinsY;
  m_c->overUnderFlow[0][0]=m_c->overUnderFlow[0][1]=m_c->overUnderFlow[0][2]=0;
  m_c->overUnderFlow[1][0]=m_c->overUnderFlow[1][1]=m_c->overUnderFlow[1][2]=0;
  m_c->overUnderFlow[2][0]=m_c->overUnderFlow[2][1]=m_c->overUnderFlow[2][2]=0;
  m_c->overflow=0;
  m_c->xW=0;
  m_c->xxW=0; 
  m_c->yW=0;
  m_c->yyW=0;
  m_c->xyW=0;
  m_c->sumWeight=0;
  m_c->minContents=0;
  m_c->maxContents=0;
  m_c->contents.resize(nBinsX*nBinsY);
  m_c->errors.resize(nBinsX*nBinsY);
}

void Hist2D::clear() {
  m_c->overUnderFlow[0][0]=m_c->overUnderFlow[0][1]=m_c->overUnderFlow[0][2]=0;
  m_c->overUnderFlow[1][0]=m_c->overUnderFlow[1][1]=m_c->overUnderFlow[1][2]=0;
  m_c->overUnderFlow[2][0]=m_c->overUnderFlow[2][1]=m_c->overUnderFlow[2][2]=0;
  m_c->overflow=0;
  m_c->xW=0;
  m_c->xxW=0; 
  m_c->yW=0;
  m_c->yyW=0;
  m_c->xyW=0;
  m_c->sumWeight=0;
  m_c->minContents=0;
  m_c->maxContents=0;
  std::fill(m_c->contents.begin(),m_c->contents.end(), 0);
  std::fill(m_c->errors.begin(),  m_c->errors.end(),   0);}


Hist2D::~Hist2D() {
  delete m_c;
}

void Hist2D::accumulate(double x, double y, double weight) {

  int    nx = m_c->nBinsX, ny=m_c->nBinsY;

  OVF a,b;
  int binX = int((x-m_c->minX)/m_c->binWidthX);
  int binY = int((y-m_c->minY)/m_c->binWidthY);
  if (x<m_c->minX) {
    a=Underflow;
  }
  else if (binX<nx) {
    a=InRange;
  }
  else {
    a=Overflow;
  }
  if (y<m_c->minY) {
    b=Underflow;
  }
  else if (binY<ny) {
    b=InRange;
  }
  else {
    b=Overflow;
  }
  m_c->overUnderFlow[a][b]++;

  if (a==InRange && b==InRange) {
    
    m_c->xW        +=  x*weight;
    m_c->xxW       += (x*x*weight);
    m_c->yW        += (y*weight);
    m_c->yyW       += (y*y*weight);
    m_c->xyW       += (x*y*weight);
    m_c->sumWeight += weight;
    m_c->minContents = 0;
    m_c->maxContents = 0;
    m_c->contents[ii(binX,binY)]+= weight;
    m_c->errors[ii(binX,binY)]  += weight*weight;
  }
}

Hist2D & Hist2D::operator += (const Hist2D & source) {
  if ((m_c->nBinsX!=source.m_c->nBinsX) ||
      (m_c->minX  !=source.m_c->minX)   ||
      (m_c->maxX  !=source.m_c->maxX)   ||
      (m_c->nBinsY!=source.m_c->nBinsY) ||
      (m_c->minY  !=source.m_c->minY)   ||
      (m_c->maxY  !=source.m_c->maxY)) 
    {
      throw (std::runtime_error("Incompatible histograms are added"));
    }
  else 
    {
    m_c->overflow  += source.m_c->overflow;
    for (int i=0;i<3;i++) {
      for (int j=0;j<3;j++) {
	m_c->overUnderFlow[i][j]+=source.m_c->overUnderFlow[i][j];
      }
    }

    m_c->xW        += source.m_c->xW;
    m_c->yW        += source.m_c->yW;
    m_c->xxW       += source.m_c->xxW;
    m_c->yyW       += source.m_c->yyW;
    m_c->xyW       += source.m_c->xyW;
    m_c->sumWeight += source.m_c->sumWeight;
    m_c->minContents = 0;
    m_c->maxContents = 0;
    for (size_t i=0;i<m_c->nBinsX;i++) {
      for (size_t j=0; j<m_c->nBinsY;j++) {
	size_t k = ii(i,j);
	m_c->contents[k] += source.m_c->contents[k];
	m_c->errors[k]   += source.m_c->errors[k];
      }
    }
  }
  return *this;
}


// Get the internals:
const Hist2D::Clockwork *Hist2D::clockwork() const {
  return m_c;
}
  
// Remake this from the interals:
Hist2D::Hist2D(const Clockwork * cw) {
  m_c  = new Clockwork(*cw);
}

Hist2D::Hist2D( const Hist2D & source) {
  m_c = new Clockwork(*source.m_c);
}

Hist2D & Hist2D::operator = (const Hist2D & source ) {
  if (this!=&source) {
    delete m_c;
    m_c = new Clockwork(*source.m_c);
  }
  return *this;
}
