/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// Base class for Plot Collections
//
// author: Felix Socher <felix.socher@cern.ch>
// -------------------------------------------------------------
#ifndef TRKVALHISTUTILS_PLOTBASE_H
#define TRKVALHISTUTILS_PLOTBASE_H

#include <string>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

typedef std::pair<TH1*, std::string> HistData;

class PlotBase {
public:
  PlotBase(PlotBase *parent, const std::string & sDir);
  virtual ~PlotBase(){}
  void initialize();
  void finalize();
  void setDetailLevel(int iDetailLevel);
  void RegisterSubPlot(PlotBase* pPlotBase){m_vSubNodes.push_back(pPlotBase);}

  ///Retrieve all booked histograms
  std::vector<HistData> retrieveBookedHistograms();

  ///@name Methods to book monitoring histograms
  ///@{

  /// Book a TH1D histogram
  TH1* Book1D(const std::string & name, const std::string & labels, int nBins, float start, float end, bool prependDir = true);
  /// Book a TH1D histogram using refHist as reference for number of bins and axis range
  TH1* Book1D(const std::string & name, TH1* refHist, const std::string & labels, bool prependDir = true);

  /// Book a TH2D histogram
  TH2* Book2D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, bool prependDir = true);
  /// Book a TH2D histogram using refHist as reference for number of bins and axis range
  TH2* Book2D(const std::string & name, TH2* refHist, const std::string & labels, bool prependDir = true);

  /// Book a TProfile histogram
  TProfile* BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float startX, float endX, float startY=-1, float endY=-1, bool prependDir = true);
  TProfile* BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float* binsX, bool prependDir = true);

  ///@}
  
  std::string getDirectory(){return m_sDirectory;}

private:
  virtual void initializePlots(){;}
  virtual void finalizePlots(){;}
  std::string ConstructPrefix(std::string dir, bool prependDir);

protected:
  std::vector<PlotBase*> m_vSubNodes;
  std::vector<HistData>  m_vBookedHistograms;
  std::string m_sDirectory;
  int m_iDetailLevel;
};

#endif
