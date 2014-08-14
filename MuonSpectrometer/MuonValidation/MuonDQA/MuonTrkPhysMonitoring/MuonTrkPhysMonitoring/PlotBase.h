/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// Base class for Plot Collections
//
// author: Felix Socher <felix.socher@cern.ch>
// -------------------------------------------------------------
#ifndef MUONTRKPHYSMONITORING_PLOTBASE_H
#define MUONTRKPHYSMONITORING_PLOTBASE_H

#include <string>
#include <utility> 
#include <vector>
#include "TH1F.h"
#include "TH2F.h"

class PlotBase {
public:
  PlotBase(PlotBase *parent);
  virtual ~PlotBase(){}
  void setTrackCollectionName(std::string sTrackCollectionName);
  void initialize();
  void finalize();
	void RegisterSubPlot(PlotBase* pPlotBase){m_vSubNodes.push_back(pPlotBase);}
	std::vector<TH1F*> retrieveBooked1DHistograms();
	std::vector<TH2F*> retrieveBooked2DHistograms();

	void Book1D(TH1F* &hist, std::string sName, std::string sLabels, int nBinsX, float minX, float maxX);
	void Book2D(TH2F* &hist, std::string sName, std::string sLabels, int nBinsX, float minX, float maxX, int nBinsY, float minY, float maxY);

private:
  virtual void initializePlots() = 0;
  virtual void finalizePlots() = 0;

protected:
  std::vector<PlotBase*>  m_vSubNodes;
  std::vector<TH1F*> m_vBooked1DHistograms;
  std::vector<TH2F*> m_vBooked2DHistograms;
  std::string m_sTrackCollectionName;
};

#endif
