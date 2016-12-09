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
#include <utility> //for std::pair
#include <vector>
//should be possible to fwd-declare TH1F, TH2F, TH3F, TProfile, TProfile2D TTree .. why isn't this done?
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TTree.h"
#include "TEfficiency.h"


//fwd-declares
class TEfficiency;

typedef std::pair<TH1*, std::string> HistData;
typedef std::pair<TTree*, std::string> TreeData;
typedef std::pair<TEfficiency*, std::string> EfficiencyData;

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
  ///Retrieve all booked efficiency objects
  std::vector<EfficiencyData> retrieveBookedEfficiencies();
  ///@name Methods to book monitoring histograms
  ///Note: methods starting with capitals should be deprecated in favour of camel-cased methods
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
  TProfile* BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float startX, float endX, float startY=-1, float endY=-1, bool prependDir = true, bool useRMS=false);
  /// Book a TProfile histogram with variable binning in x-axis
  TProfile* BookTProfile(const std::string &name, const std::string & labels, int nBinsX, float* binsX, bool prependDir = true);
  /// Book a TProfile histogram with variable binning in x-axis and limits in y-values
  TProfile* BookTProfileRangeY(const std::string &name, const std::string & labels, int nBinsX, double* binsX, double startY, double endY, bool prependDir = true); //cannot overload, conflicts with previous definitions
  /// Book a TProfile 2D histogram with variable binning in x-axis and limits in y-values
  TProfile2D * BookTProfile2D(const std::string &name, const std::string & labels, const int nBinsX, const double xlo, const double xhi, const int nBinsY, const double ylo, const double yhi, bool prependDir=true, bool useRMS=false);
  /// Book a TProfile 2D histogram with variable binning in x-axis and limits in y-values
  TProfile2D * BookTProfile2D(const std::string &name, const std::string &labels, const int nBinsX, double* binsX, const int nBinsY, double* binsY, bool prependDir=true, bool useRMS=false);
  /// Book a (1-D) TEfficiency histogram
  TEfficiency * BookTEfficiency(const std::string &name, const std::string & labels, const int nBinsX, const float xlo, const float xhi, const bool prependDir = true);
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
  std::vector<EfficiencyData>  m_vBookedEfficiencies;
  std::string m_sDirectory;
  int m_iDetailLevel;
};

#endif
