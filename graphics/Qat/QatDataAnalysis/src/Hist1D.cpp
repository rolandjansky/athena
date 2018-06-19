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


#include "QatDataAnalysis/Hist1D.h"
#include <stdexcept>
Hist1D::Hist1D(const std::string & name, size_t nBins, double min, double max):
  m_c(new Clockwork()) 
{
  m_c->name=name;
  m_c->nBins=nBins;
  m_c->min=min;
  m_c->max=max;
  m_c->binWidth=(max-min)/nBins;
  m_c->overflow=0;
  m_c->underflow=0;
  m_c->entries=0;
  m_c->xW=0;
  m_c->xxW=0;
  m_c->sumWeight=0;
  m_c->minContents=0;
  m_c->maxContents=0;
  m_c->contents.resize(nBins);
  m_c->errors.resize(nBins);
}

Hist1D::~Hist1D() {
  delete m_c;
}

void Hist1D::accumulate(double x, double weight) {
  int bin = int((x-m_c->min)/m_c->binWidth);
  if (x<m_c->min) {
    m_c->underflow++;
  }
  else if (bin>=int(m_c->nBins)) {
    m_c->overflow++;
  }
  else {
    m_c->entries++;
    m_c->xW        +=  x*weight;
    m_c->xxW       += (x*x*weight);
    m_c->sumWeight += weight;
    m_c->minContents = 0;
    m_c->maxContents = 0;
    m_c->contents[bin]+= weight;
    m_c->errors[bin]  += weight*weight;
  }
}

Hist1D & Hist1D::operator += (const Hist1D & source) {
  if ((m_c->nBins!=source.m_c->nBins) ||
      (m_c->min  !=source.m_c->min)   ||
      (m_c->max  !=source.m_c->max)   ) {
    throw (std::runtime_error("Incompatible histograms are added"));
  }
  else {
    m_c->overflow  += source.m_c->overflow;
    m_c->underflow += source.m_c->underflow;
    m_c->entries   += source.m_c->entries;
    m_c->xW        += source.m_c->xW;
    m_c->xxW       += source.m_c->xxW;
    m_c->sumWeight += source.m_c->sumWeight;
    m_c->minContents = 0;
    m_c->maxContents = 0;
    for (unsigned int i=0;i<m_c->nBins;i++) {
      m_c->contents[i] += source.m_c->contents[i];
      m_c->errors[i]   += source.m_c->errors[i];
    }
  }

  return *this;
}

Hist1D & Hist1D::operator -= (const Hist1D & source) {
  if ((m_c->nBins!=source.m_c->nBins) ||
      (m_c->min  !=source.m_c->min)   ||
      (m_c->max  !=source.m_c->max)   ) {
    throw (std::runtime_error("Incompatible histograms are added"));
  }
  else {
    m_c->overflow  += source.m_c->overflow;
    m_c->underflow += source.m_c->underflow;
    m_c->entries   += source.m_c->entries;
    m_c->xW        -= source.m_c->xW;
    m_c->xxW       -= source.m_c->xxW;
    m_c->sumWeight -= source.m_c->sumWeight;
    m_c->minContents = 0;
    m_c->maxContents = 0;
    for (unsigned int i=0;i<m_c->nBins;i++) {
      m_c->contents[i] -= source.m_c->contents[i];
      m_c->errors[i]   += source.m_c->errors[i];
    }
  }
  return *this;
}

Hist1D & Hist1D::operator *= (double scale) {
  m_c->xW        *= scale;
  m_c->xxW       *= scale;
  m_c->sumWeight *= scale;
  m_c->minContents = 0;
  m_c->maxContents = 0;
  for (unsigned int i=0;i<m_c->nBins;i++) {
    m_c->contents[i] *= scale;
    m_c->errors[i]   *= (scale*scale);
  }
  return *this;
}

void Hist1D::clear() {

  m_c->overflow    = 0;
  m_c->underflow   = 0;
  m_c->entries     = 0;
  m_c->xW          = 0;
  m_c->xxW         = 0;
  m_c->sumWeight   = 0;
  m_c->minContents = 0;
  m_c->maxContents = 0;
  std::fill(m_c->contents.begin(),m_c->contents.end(), 0);
  std::fill(m_c->errors.begin(),  m_c->errors.end(),   0);
  
}

// Get the internals:
const Hist1D::Clockwork *Hist1D::clockwork() const {
  return m_c;
}
  
// Remake this from the interals:
Hist1D::Hist1D(const Clockwork * cw) {
  m_c  = new Clockwork(*cw);
}

Hist1D::Hist1D( const Hist1D & source) {
  m_c = new Clockwork(*source.m_c);
}

Hist1D & Hist1D::operator = (const Hist1D & source ) {
  if (this!=&source) {
    delete m_c;
    m_c = new Clockwork(*source.m_c);
  }
  return *this;
}
