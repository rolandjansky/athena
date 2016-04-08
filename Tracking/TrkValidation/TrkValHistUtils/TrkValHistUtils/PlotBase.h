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
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TTree.h"

typedef std::pair<TH1*, std::string> HistData;
typedef std::pair<TTree*, std::string> TreeData;

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
  ///Retrieve all booked trees
  std::vector<TreeData> retrieveBookedTrees();

  ///@name Methods to book monitoring histograms
  ///@{

  /// Book a TH1D histogram
  TH1F* Book1D(const std::string & name, const std::string & labels, int nBins, float start, float end, bool prependDir = true);
  /// Book a TH1D histogram using refHist as reference for number of bins and axis range
  TH1F* Book1D(const std::string & name, TH1* refHist, const std::string & labels, bool prependDir = true);

  /// Book a TH2D histogram
  TH2F* Book2D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, bool prependDir = true);
  /// Book a TH2D histogram using refHist as reference for number of bins and axis range
  TH2F* Book2D(const std::string & name, TH2* refHist, const std::string & labels, bool prependDir = true);
  /// Book a TH2D histogram with variable x axis binning
  TH2F* Book2D(const std::string & name, const std::string & labels, int nBinsX, Double_t* binsX, int nBinsY, Double_t startY, Double_t endY, bool prependDir = true);

  /// Book a TH3D histogram
  TH3F* Book3D(const std::string & name, const std::string & labels, int nBinsX, float startX, float endX, int nBinsY, float startY, float endY, int nBinsZ, float startZ, float endZ, bool prependDir = true);
  /// Book a TH3D histogram using refHist as reference for number of bins and axis range
  TH3F* Book3D(const std::string & name, TH3* refHist, const std::string & labels, bool prependDir = true);

  /// Book a TProfile histogram
  TProfile* BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float startX, float endX, float startY=-1, float endY=-1, bool prependDir = true);
  /// Book a TProfile histogram with variable binning in x-axis
  TProfile* BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float* binsX, bool prependDir = true);
  /// Book a TProfile histogram with variable binning in x-axis and limits in y-values
  TProfile* BookTProfileRangeY(const std::string &name, const std::string & labels, int nBinsX, double* binsX, double startY, double endY, bool prependDir = true); //cannot overload, conflicts with previous definitions
  /// Book a TProfile 2D histogram with variable binning in x-axis and limits in y-values
  TProfile2D * BookTProfile2D(const std::string &name, const std::string & labels, const int nBinsX, 
    const double xlo, const double xhi, const int nBinsY, const double ylo, const double yhi, bool prependDir=true);
  ///@}
  /// Book a TTree
  TTree* BookTree(const std::string & name, bool prependDir = true);
  
  std::string getDirectory(){return m_sDirectory;}

private:
  virtual void initializePlots(){;}
  virtual void finalizePlots(){;}
  std::string constructPrefix(std::string dir, bool prependDir);

protected:
  std::vector<PlotBase*> m_vSubNodes;
  std::vector<HistData>  m_vBookedHistograms;
  std::vector<TreeData>  m_vBookedTrees;
  std::string m_sDirectory;
  int m_iDetailLevel;
};

#endif
