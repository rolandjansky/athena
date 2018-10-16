/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// File     : showCalibration.C
// Author   : Frank Filthaut
// Purpose  : inspect the container objects in the calibration ROOT file

// Either load the library before running ".x showCalibration.C+" (i.e. compiled),
// then you'll need the include. Or run ".x showCalibration.C" and call
// gSystem->Load("libCalibrationDataInterface.so"); in here - but then the #include
// is in the way: CINT *replaces* elements from the dictionary when loading
// the #include :-(

// This macro has been developed within the RootCore build environment:
// root -l $ROOTCOREDIR/scripts/load_packages.C+
//   .L showCalibration.C+
//   showCalibration()
// NB see below for the arguments to the call

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"

using std::cout;
using std::endl;
using std::setw;
using std::vector;
using std::string;

#ifndef __CINT__
#  include "CalibrationDataInterface/CalibrationDataContainer.h"
#endif

using Analysis::CalibrationDataContainer;
using Analysis::CalibrationDataFunctionContainer;
using Analysis::CalibrationDataHistogramContainer;

void showCalibration(TString fileName,
		     TString pathName,
		     TString uncertainty = "list",
		     bool plot = false) {

#ifdef __CINT__
  gSystem->Load("libCalibrationDataInterface.so");
#endif

  if (uncertainty == "help") {
    cout << "usage: showCalibration(fileName, pathName, uncertainty)" << endl;
    cout << "where" << endl;
    cout << "     fileName:    name of the CDI ROOT file" << endl;
    cout << "     pathName:    name of the calibration object's path name within the ROOT file (see e.g. output from checkCalibrationFile())" << endl;
    cout << "     uncertainty: uncertainty to be shown in addition to statistical uncertainty (use \"list\" to obtain a list; use \"total\" to sum stat and syst uncertainties)" << endl;
    return;
  }

  TFile* f = TFile::Open(fileName.Data(), "READ");
  f->cd();

  CalibrationDataHistogramContainer* c;
  f->GetObject(pathName.Data(), c);
  if (c) {
    // Object found; now go about showing its properties
    // Special case: uncertainty == "list": show a list of (meaningful) uncertainties
    if (uncertainty == "list") {
      vector<string> uncs = c->listUncertainties();
      for (unsigned int i = 0; i < uncs.size(); ++i)
	if (uncs[i] != "comment" && uncs[i] != "statistics" && uncs[i] != "MCreference")
	  cout << uncs[i] << endl;
    } else if (uncertainty == "comment") {
      cout << c->getComment() << endl;
    } else if (uncertainty == "MChadronisation") {
      cout << c->getHadronisation() << endl;
    } else if (uncertainty == "excluded_set") {
      cout << c->getExcludedUncertainties() << endl;
    } else {
      bool doTotal = uncertainty == "total";
      bool doExtrap = uncertainty == "extrapolation";
      if (doTotal) cout << "NB: printing total uncertainties" << endl;
      TH1* hunc = dynamic_cast<TH1*>(c->GetValue(doTotal ? "systematics" : uncertainty.Data()));
      if (hunc) {
	TH1* href = (uncertainty == "result") ? hunc : dynamic_cast<TH1*>(c->GetValue("result"));
	href->SetStats(kFALSE);
	Int_t ndim = href->GetDimension();
	if (ndim == 1) {
	  if (plot) {
	    href->SetLineWidth(2);
	    href->SetLineColor(kBlack); href->SetMarkerColor(kBlack);
	    href->Draw("PE");
	  }
	  // printout to screen
	  cout << setw(15) << href->GetXaxis()->GetTitle() << setw(11) << "value" << setw(10) << "stat unc";
	  if (hunc != href) cout << "  " << uncertainty.Data();
	  cout << "\n-----------------------------------------------------------------" << endl;
	  Int_t firstbin = hunc->GetXaxis()->FindFixBin(href->GetXaxis()->GetBinCenter(1));
	  for (Int_t bin = 0; bin < hunc->GetNbinsX(); ++bin) {
	    // determine the bin number in the reference ("result") histogram
	    Int_t binref = bin - (firstbin-1);
	    if (binref < 0) binref = 0;
	    else if (binref >= href->GetNbinsX()) binref = href->GetNbinsX()-1;
	    double uncstat = href->GetBinError(binref+1);
	    cout << " [" << setw(6) << hunc->GetBinLowEdge(bin+1) << ","  << setw(6) << hunc->GetBinLowEdge(bin+2) << "]"
		 << "  " << setw(8) << href->GetBinContent(binref+1) << "  " << setw(8) << uncstat;
	    if (hunc != href) {
	      double unc = hunc->GetBinContent(bin+1);
	      if (doTotal) unc = std::sqrt(unc*unc + uncstat*uncstat);
	      cout << "  " << setw(8) << unc;
	    }
	    cout << endl;
	  }
	  if (plot && hunc != href) {
	    TH1* hclone = dynamic_cast<TH1*>(href->Clone((uncertainty+"_clone").Data()));
	    for (Int_t bin = 1; bin < hclone->GetNbinsX(); ++bin)
	      hclone->SetBinError(bin, hunc->GetBinContent(bin));
	    hclone->SetFillColor(kGreen+3);
	    hclone->Draw("E3 SAME");
	    // then redraw the original histogram
	    href->Draw("PE SAME");
	  }
	} else if (ndim == 2) {
	  // this is the more likely case... many histograms are stored as 2D ones, but with only one bin in the Y dimension.
	  // In these cases, project onto the X axis so as to be able to draw properly.
	  if (href->GetNbinsX() == 1) {
	    if (hunc->GetNbinsX() != 1) {
	      cout << "inconsistent X binning used between results and " << uncertainty << " histograms!" << endl;
	      f->Close();
	      return;
	    }
	    if (((! doExtrap) && href->GetNbinsY() != hunc->GetNbinsY()) ||
		(doExtrap && href->GetNbinsY() > hunc->GetNbinsY())) {
	      cout << "inconsistent Y binning used between results and " << uncertainty << " histograms!" << endl;
	      f->Close();
	      return;
	    }
	    TH1D* hpr = dynamic_cast<TH1D*>(dynamic_cast<TH2*>(hunc)->ProjectionY((uncertainty+"_projY").Data()));
	    // printout to screen
	    cout << setw(15) << href->GetYaxis()->GetTitle() << setw(11) << "value" << setw(10) << "stat unc";
	    if (hunc != href) cout << "  " << uncertainty.Data();
	    cout << "\n-----------------------------------------------------------------" << endl;
	    Int_t firstbin = hunc->GetYaxis()->FindFixBin(href->GetYaxis()->GetBinCenter(1));
	    for (Int_t bin = 0; bin < hpr->GetNbinsX(); ++bin) {
	      // determine the bin number in the reference ("result") histogram
	      Int_t binref = bin - (firstbin-1);
	      if (binref < 0) binref = 0;
	      else if (binref >= href->GetNbinsY()) binref = href->GetNbinsY()-1;
	      double uncstat = href->GetBinError(binref+1);
	      cout << " [" << setw(6) << hpr->GetBinLowEdge(bin+1) << ","  << setw(6) << hpr->GetBinLowEdge(bin+2) << "]"
		   << "  " << setw(8) << href->GetBinContent(1, binref+1) << "  " << setw(8) << uncstat;
	      if (hunc != href) {
		double unc = hunc->GetBinContent(1, bin+1);
		if (doTotal) unc = std::sqrt(unc*unc + uncstat*uncstat);
		cout << "  " << setw(8) << unc;
	      }
	      cout << endl;
	    }
	    // if (plot) {
	    //   hpr->SetLineWidth(2);
	    //   hpr->SetLineColor(kBlack); hpr->SetMarkerColor(kBlack);
	    //   hpr->Draw("EP");
	    //   if (hunc != href) {
	    // 	TH1* hclone = dynamic_cast<TH1*>(hpr->Clone((uncertainty+"_clone").Data()));
	    // 	for (Int_t bin = 1; bin < hclone->GetNbinsX(); ++bin)
	    // 	  hclone->SetBinError(bin, hunc->GetBinContent(1, bin));
	    // 	hclone->SetFillColor(kGreen+3);
	    // 	hclone->Draw("E3 SAME");
	    // 	// then redraw the original histogram
	    // 	hpr->Draw("PE SAME");
	    //   }
	    // }
	  } else {
	    cout << setw(15) << href->GetXaxis()->GetTitle() << setw(16) << href->GetYaxis()->GetTitle() << setw(11) << "value" << setw(10) << "stat unc";
	    if (hunc != href) cout << "  " << uncertainty.Data();
	    cout << "\n---------------------------------------------------------------------------------" << endl;
	    Int_t firstbinx = hunc->GetXaxis()->FindFixBin(href->GetXaxis()->GetBinCenter(1));
	    Int_t firstbiny = hunc->GetYaxis()->FindFixBin(href->GetYaxis()->GetBinCenter(1));
	    for (Int_t binx = 0; binx < hunc->GetNbinsX(); ++binx) {
	      // determine the bin number in the reference ("result") histogram
	      Int_t binrefx = binx - (firstbinx-1);
	      if (binrefx < 0) binrefx = 0;
	      else if (binrefx >= href->GetNbinsX()) binrefx = href->GetNbinsX()-1;
	      for (Int_t biny = 0; biny < hunc->GetNbinsY(); ++biny) {
		// determine the bin number in the reference ("result") histogram
		Int_t binrefy = biny - (firstbiny-1);
		if (binrefy < 0) binrefy = 0;
		else if (binrefy >= href->GetNbinsY()) binrefy = href->GetNbinsY()-1;
		Int_t bin = hunc->GetBin(binx+1, biny+1), binref = href->GetBin(binrefx+1, binrefy+1);
		double uncstat = href->GetBinError(binref);
		cout <<  " [" << setw(6) << hunc->GetXaxis()->GetBinLowEdge(binx+1) << ","  << setw(6) << hunc->GetXaxis()->GetBinLowEdge(binx+2)
		     << "],[" << setw(6) << hunc->GetYaxis()->GetBinLowEdge(biny+1) << ","  << setw(6) << hunc->GetYaxis()->GetBinLowEdge(biny+2) << "],"
		     << "  " << setw(8) << href->GetBinContent(binref) << "  " << setw(8) << uncstat;
		if (hunc != href) {
		  double unc = hunc->GetBinContent(bin);
		  if (doTotal) unc = std::sqrt(unc*unc + uncstat*uncstat);
		  cout << "  " << setw(8) << unc;
		}
		cout << endl;
	      }
	    }
	    if (plot) href->Draw("LEGO");
	  }
	} else {
	  // this is the case for continuous tagging
	  cout << setw(15) << href->GetXaxis()->GetTitle() << setw(16) << href->GetYaxis()->GetTitle() << setw(16) << href->GetZaxis()->GetTitle()
	       << setw(11) << "value" << setw(10) << "stat unc";
	    if (hunc != href) cout << "  " << uncertainty.Data();
	    cout << "\n---------------------------------------------------------------------------------" << endl;
	    for (Int_t binx = 0; binx < href->GetNbinsX(); ++binx)
	      for (Int_t biny = 0; biny < href->GetNbinsY(); ++biny)
		for (Int_t binz = 0; binz < href->GetNbinsZ(); ++binz) {
		  Int_t bin = href->GetBin(binx+1, biny+1, binz+1);
		  double uncstat = href->GetBinError(bin);
		  cout <<  " [" << setw(6) << href->GetXaxis()->GetBinLowEdge(binx+1) << ","  << setw(6) << href->GetXaxis()->GetBinLowEdge(binx+2)
		       << "],[" << setw(6) << href->GetYaxis()->GetBinLowEdge(biny+1) << ","  << setw(6) << href->GetYaxis()->GetBinLowEdge(biny+2)
		       << "],[" << setw(6) << href->GetZaxis()->GetBinLowEdge(binz+1) << ","  << setw(6) << href->GetZaxis()->GetBinLowEdge(binz+2)
		       << "]  " << setw(8) << href->GetBinContent(bin) << "  " << setw(8) << uncstat;
		  if (hunc != href) {
		    double unc = hunc->GetBinContent(bin);
		    if (doTotal) unc = std::sqrt(unc*unc + uncstat*uncstat);
		    cout << "  " << setw(8) << unc;
		  }
		  cout << endl;
		}
	    // if (plot) href->Draw("LEGO");
	}
      } else
	cout << "error: specified uncertainty (" << uncertainty << ") cannot be found" << endl;
    }
  } else
    cout << "error: calibration object (" << pathName.Data() << ") cannot be found or is not a histogram container" << endl;
  f->Close();
}
