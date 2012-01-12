/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Utility class to keep track of a bunch
// of histograms at once.
//
#ifndef TRIGTAUHISTHELPER_H
#define TRIGTAUHISTHELPER_H

#include <list>
#include <map>
#include <vector>

#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TCanvas.h"

using namespace std;

class TrigTauHistHelper {
public:
  TrigTauHistHelper();
  TrigTauHistHelper(unsigned long nh);  // Number of histograms per family
  virtual ~TrigTauHistHelper();

  void reset(); // Set all counts to zero
  void clear(); // Delete everything

  void setNHist(unsigned long nhist) {_nHist = nhist;}

  void create(TString tag, long nbx, double xlo, double xhi, 
	      TString xAxis="");
  void create(TString tag, long nbx, double xlo, double xhi,
	      long nby, double ylo, double yhi,
	      TString xAxis="", TString yAxis="",
	      TString opt="COLZ");

  // itype is an index to the particular histogram in a family
  void fill1D(TString tag, long itype, double x, double wt=1.);
  void fill2D(TString tag, long itype, double x, double y, double wt=1.);

  // Generic versions
  void fill(TString tag, long itype, double x, double wt) {
    fill1D(tag, itype, x, wt);
  };
  void fill(TString tag, long itype, double x, double y, double wt) {
    fill2D(tag, itype, x, y, wt);
  };

  // Retrieve specific histograms
  TH1D* get1D(TString tag, long itype);
  TH2D* get2D(TString tag, long itype);

  void draw(TString itag, long itype);

  void writeAll(TString outfile);
  void drawAll(TString plotfile, unsigned long nStart, unsigned long nEnd, TCanvas* c);
  void drawAll(TString plotfile, long nPerPage, TCanvas* c);
  void drawEffAll(TString plotfile, long nPerPage, long nDenom, TCanvas* c);

  void setYAxisLabels(const vector<TString>& lab) {_yAxisLabels = lab;};
  void setLogY(TString tag) {_logY[tag] = true;};  // Draw 1D plot as logy

private:
  unsigned long _nHist;  // Total number of histograms per family
  bool _debug;

  map<TString, TH1D*> _h1D;
  map<TString, TH2D*> _h2D;
  map<TString, bool>  _logY;
  map<TString, TString> _opt;

  list<TString> _tags;

  vector<TString> _yAxisLabels;
};

#endif
