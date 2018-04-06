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


#include "QatDataAnalysis/HistogramManager.h"
#include <algorithm>

HistogramManager::HistogramManager(const std::string & name): m_name(name) {
}


const std::string & HistogramManager::name() const { 
  return m_name;
}


std::string & HistogramManager::name() { 
  return m_name;
}


HistogramManager::H1Iterator HistogramManager::beginH1() {
  return m_histograms.begin();
}

HistogramManager::H1Iterator HistogramManager::endH1() {
  return m_histograms.end();
}

HistogramManager::H1ConstIterator HistogramManager::beginH1() const{
  return m_histograms.begin();
}

HistogramManager::H1ConstIterator HistogramManager::endH1() const {
  return m_histograms.end();
}

HistogramManager::H2Iterator HistogramManager::beginH2() {
  return m_scatterPlots.begin();
}

HistogramManager::H2Iterator HistogramManager::endH2() {
  return m_scatterPlots.end();
}

HistogramManager::H2ConstIterator HistogramManager::beginH2() const{
  return m_scatterPlots.begin();
}

HistogramManager::H2ConstIterator HistogramManager::endH2() const {
  return m_scatterPlots.end();
}

HistogramManager::DIterator HistogramManager::beginDir() {
  return m_managers.begin();
}

HistogramManager::DIterator HistogramManager::endDir() {
  return m_managers.end();
}

HistogramManager::DConstIterator HistogramManager::beginDir() const{
  return m_managers.begin();
}

HistogramManager::DConstIterator HistogramManager::endDir() const {
  return m_managers.end();
}

HistogramManager::TIterator HistogramManager::beginTable() {
  return m_tables.begin();
}

HistogramManager::TIterator HistogramManager::endTable() {
  return m_tables.end();
}

HistogramManager::TConstIterator HistogramManager::beginTable() const{
  return m_tables.begin();
}

HistogramManager::TConstIterator HistogramManager::endTable() const {
  return m_tables.end();
}

const Hist1D           *HistogramManager::findHist1D(const std::string & name) const{
  H1ConstIterator i = std::find_if(m_histograms.begin(),m_histograms.end(), NameEq(name));
  if (i!=m_histograms.end()) return *i;
  else return NULL;
}

const Hist2D           *HistogramManager::findHist2D(const std::string & name) const{
  H2ConstIterator i = std::find_if(m_scatterPlots.begin(),m_scatterPlots.end(), NameEq(name));
  if (i!=m_scatterPlots.end()) return *i;
  else return NULL;
}

const HistogramManager           *HistogramManager::findDir(const std::string & name) const {
  DConstIterator i = std::find_if(m_managers.begin(),m_managers.end(), NameEq(name));
  if (i!=m_managers.end()) return *i;
  else return NULL;
}

const Table           *HistogramManager::findTable(const std::string & name) const {
  TConstIterator i = std::find_if(m_tables.begin(),m_tables.end(), NameEq(name));
  if (i!=m_tables.end()) return *i;
  else return NULL;
}




Hist1D *HistogramManager::newHist1D(const std::string & name, unsigned int nBins, double min, double max) {
  purge(name);
  Hist1D * newHist = new Hist1D(name,nBins,min, max);
  m_histograms.push_back(newHist);
  m_histograms.sort(SortByName());
  return newHist;
}

Hist1D *HistogramManager::newHist1D(const Hist1D & source) {
  purge(source.name());
  Hist1D * newHist = new Hist1D(source);
  m_histograms.push_back(newHist);
  m_histograms.sort(SortByName());
  return newHist;
}

Table *HistogramManager::newTable(const Table & source) {


  purge(source.name());
  Table * newTable = new Table(source);
  m_tables.push_back(newTable);
  m_tables.sort(SortByName());
  return newTable;
}

Hist2D *HistogramManager::newHist2D(const std::string & name, unsigned int nBinsX, double minX, double maxX,
				                            unsigned int nBinsY, double minY, double maxY) {

  purge(name);
  Hist2D *newHist = new Hist2D(name,nBinsX,minX, maxX, nBinsY, minY, maxY);
  m_scatterPlots.push_back(newHist);
  m_scatterPlots.sort(SortByName());
  return newHist;
}

Hist2D *HistogramManager::newHist2D(const Hist2D & source) {
  purge(source.name());
  Hist2D * newHist = new Hist2D(source);
  m_scatterPlots.push_back(newHist);
  m_scatterPlots.sort(SortByName());
  return newHist;
}




Table           *HistogramManager::newTable(const std::string & name, TupleStoreLink store) {
  purge (name);
  Table *t = store ? new Table(name,store): new Table(name);
  m_tables.push_back(t);
  m_tables.sort(SortByName());
  return t;
}


HistogramManager *HistogramManager::newDir(const std::string & name) {
  purge (name);
  HistogramManager *t = new HistogramManager(name);
  m_managers.push_back(t);
  m_managers.sort(SortByName());
  return t;
}


void HistogramManager::nameOrder() {
  // Sort tables: Other objects already ordered.
  m_tables.sort(SortByName());
}

void HistogramManager::purge(const std::string & name) {

  H1Iterator doomedHistogram = std::find_if(m_histograms.begin(),m_histograms.end(), NameEq(name));
  if (doomedHistogram!=m_histograms.end()) {
    delete *doomedHistogram;
    m_histograms.erase(doomedHistogram);
  }
  
  H2Iterator doomedScatterPlot = std::find_if(m_scatterPlots.begin(),m_scatterPlots.end(), NameEq(name));
  if (doomedScatterPlot!=m_scatterPlots.end()) {
    delete *doomedScatterPlot;
    m_scatterPlots.erase(doomedScatterPlot);
  }
  
  TIterator doomedTable = std::find_if(m_tables.begin(),m_tables.end(), NameEq(name));
  if (doomedTable!=m_tables.end()) {
    delete *doomedTable;
    m_tables.erase(doomedTable);
  }
  
  DIterator doomedDir = std::find_if(m_managers.begin(),m_managers.end(), NameEq(name));
  if (doomedDir!=m_managers.end()) {
    delete *doomedDir;
    m_managers.erase(doomedDir);
  }
}



HistogramManager::~HistogramManager() {
  for (H1Iterator i=m_histograms.begin();   i!=m_histograms.end();  i++)   delete *i;
  for (DIterator  i=m_managers.begin();     i!=m_managers.end();    i++)   delete *i;
  for (H2Iterator i=m_scatterPlots.begin(); i!=m_scatterPlots.end();i++)   delete *i;
  for (TIterator  i=m_tables.begin();       i!=m_tables.end();      i++)   delete *i;
}
