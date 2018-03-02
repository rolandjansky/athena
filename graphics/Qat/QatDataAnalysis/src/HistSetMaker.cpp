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


#include "QatDataAnalysis/HistSetMaker.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/Hist2D.h"
#include "QatDataAnalysis/HistogramManager.h"
#include "QatDataAnalysis/Attribute.h"
#include "QatDataAnalysis/Table.h"
#include "CLHEP/GenericFunctions/Argument.hh"
class HistSetMaker::Clockwork {

public:

  struct Hist1DDescriptor {
    Hist1DDescriptor() :name{},f(NULL),nBins(0),min(0.0),max(0.0),hist(NULL),weight(NULL) {}
    std::string           name;
    Genfun::AbsFunction   *f;
    unsigned int          nBins;
    double                min;
    double                max;
    Hist1D                *hist;
    const Genfun::AbsFunction   *weight;
  };


  struct Hist2DDescriptor {
    Hist2DDescriptor() :name{},fX(NULL),nBinsX(0),minX(0.0),maxX(0.0),
      fY(NULL),nBinsY(0),minY(0.0),maxY(0.0),hist(NULL),weight(NULL) {}
    std::string           name;
    Genfun::AbsFunction   *fX;
    unsigned int          nBinsX;
    double                minX;
    double                maxX;
    Genfun::AbsFunction   *fY;
    unsigned int          nBinsY;
    double                minY;
    double                maxY;
    Hist2D                *hist;
    const Genfun::AbsFunction   *weight;
  };

  std::vector<Hist1DDescriptor>  hist1DList;
  std::vector<Hist2DDescriptor>  hist2DList;
};

HistSetMaker::HistSetMaker():m_c(new Clockwork()){
}

void HistSetMaker::scheduleHist1D(const std::string & name, Genfun::GENFUNCTION f, unsigned int nbins, double min, double max, const Genfun::AbsFunction *w) {
  Clockwork::Hist1DDescriptor descriptor;
  m_c->hist1DList.push_back(descriptor);
  m_c->hist1DList.back().name=name;
  m_c->hist1DList.back().f= f.clone();
  m_c->hist1DList.back().nBins= nbins;
  m_c->hist1DList.back().min  = min;
  m_c->hist1DList.back().max  = max;
  m_c->hist1DList.back().weight = w ? w->clone(): NULL;
}

void HistSetMaker::scheduleHist2D(const std::string & name, 
				  Genfun::GENFUNCTION fX, unsigned int nbinsX, double minX, double maxX,
				  Genfun::GENFUNCTION fY, unsigned int nbinsY, double minY, double maxY,
				  const Genfun::AbsFunction *w) {
  Clockwork::Hist2DDescriptor descriptor;
  m_c->hist2DList.push_back(descriptor);
  m_c->hist2DList.back().name=name;
  m_c->hist2DList.back().fX= fX.clone();
  m_c->hist2DList.back().nBinsX= nbinsX;
  m_c->hist2DList.back().minX  = minX;
  m_c->hist2DList.back().maxX  = maxX;
  m_c->hist2DList.back().fY= fY.clone();
  m_c->hist2DList.back().nBinsY= nbinsY;
  m_c->hist2DList.back().minY  = minY;
  m_c->hist2DList.back().maxY  = maxY;
  m_c->hist2DList.back().weight = w ? w->clone(): NULL;
}

HistSetMaker::~HistSetMaker() {
  for (unsigned int i=0;i<m_c->hist1DList.size();i++) {
    delete m_c->hist1DList[i].f;
    delete m_c->hist1DList[i].weight;
  }
  for (unsigned int i=0;i<m_c->hist2DList.size();i++) {
    delete m_c->hist2DList[i].fX;
    delete m_c->hist2DList[i].fY;
    delete m_c->hist2DList[i].weight;
  }
  delete m_c;
}


void HistSetMaker::exec (const Table & table, HistogramManager *m) const {

  // Make some histograms;
  for (unsigned int i=0;i<m_c->hist1DList.size();i++) {
    m_c->hist1DList[i].hist = m->newHist1D(m_c->hist1DList[i].name,
					 m_c->hist1DList[i].nBins,
					 m_c->hist1DList[i].min,
					 m_c->hist1DList[i].max);
  }
  for (unsigned int i=0;i<m_c->hist2DList.size();i++) {
    m_c->hist2DList[i].hist = m->newHist2D(m_c->hist2DList[i].name,
					 m_c->hist2DList[i].nBinsX,
					 m_c->hist2DList[i].minX,
					 m_c->hist2DList[i].maxX,
					 m_c->hist2DList[i].nBinsY,
					 m_c->hist2DList[i].minY,
					 m_c->hist2DList[i].maxY);
  }
  
  // Do not worry about cleaning up that memory.  Histograms are
  // owned by the manager!
  for  (unsigned int t=0; ; t++) {
    
    TupleConstLink tuple = table[t];
    if (!tuple) break;

    const Genfun::Argument & a  = tuple->asDoublePrec();
    
    for (unsigned int i=0;i<m_c->hist1DList.size();i++) {
      if (m_c->hist1DList[i].weight) {
	Genfun::GENFUNCTION W=*m_c->hist1DList[i].weight;
	m_c->hist1DList[i].hist->accumulate((*(m_c->hist1DList[i].f))(a),W(a));
      }
      else {
	m_c->hist1DList[i].hist->accumulate((*(m_c->hist1DList[i].f))(a));
      }
    }
    
    for (unsigned int i=0;i<m_c->hist2DList.size();i++) {
      if (m_c->hist2DList[i].weight) {
	Genfun::GENFUNCTION W=*m_c->hist2DList[i].weight;
	m_c->hist2DList[i].hist->accumulate((*(m_c->hist2DList[i].fX))(a), (*(m_c->hist2DList[i].fY))(a) ,W(a) );
      }
      else {
	m_c->hist2DList[i].hist->accumulate((*(m_c->hist2DList[i].fX))(a), (*(m_c->hist2DList[i].fY))(a)  );
      }

    }
    
  }
  

}


// Number of Histograms:
unsigned int HistSetMaker::numH1D() const {
  return m_c->hist1DList.size();
}
unsigned int HistSetMaker::numH2D() const{
  return m_c->hist2DList.size();
}

// Name of Histograms
const std::string & HistSetMaker::nameH1D(unsigned int i) const {
  return m_c->hist1DList[i].name;
}
const std::string & HistSetMaker::nameH2D(unsigned int i) const {
  return m_c->hist2DList[i].name;
}
