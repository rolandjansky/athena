/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cassert>

// ROOT
#include "TH1.h"

// Local
#include "TrigCostBase/HistMan.h"
#include "TrigCostExec/TimeHist.h"

using namespace std;

//-----------------------------------------------------------------------------
Anp::TimeHist::TimeHist(const std::string &name)
  :fName(name),
   fDebug(false),
   fNBins(100),
   fNWait(100),
   fLowerEdge(0),
   fUpperEdge(0),
   fIgnLowerFrac(0),
   fIgnUpperFrac(0.01),
   fPadLowerFrac(0),
   fPadUpperFrac(0),
   fFixLowerEdge(false),
   fFixUpperEdge(false),
   fIsValid(false),
   fNFill(0),
   fNLower(0),
   fNUpper(0)
{
}

//-----------------------------------------------------------------------------
void Anp::TimeHist::Config(const Registry &reg)
{
  // Read configuration
  reg.Get("TimeHist", "Debug",  fDebug);
  reg.Get("TimeHist::Name",     fName);
  reg.Get("TimeHist::NBins",    fNBins);
  reg.Get("TimeHist::NWait",    fNWait);

  reg.Get("TimeHist::IgnLowerFrac", fIgnLowerFrac);
  reg.Get("TimeHist::IgnUpperFrac", fIgnUpperFrac);
  reg.Get("TimeHist::PadLowerFrac", fPadLowerFrac);
  reg.Get("TimeHist::PadUpperFrac", fPadUpperFrac);
  reg.Get("TimeHist::FixLowerEdge", fFixLowerEdge);
  reg.Get("TimeHist::FixUpperEdge", fFixUpperEdge);
  reg.Get("TimeHist::LowerEdge",    fLowerEdge);
  reg.Get("TimeHist::UpperEdge",    fUpperEdge);

  if(fNWait > 0 && fNWait < 10) {
    cerr << "TimeHist::Config - reset nwait to 10" << endl;
    fNWait = 10;
  }

  // Create histogram
  if(fNWait == 0 && fNBins > 1 && fLowerEdge < fUpperEdge) {
    fHist = Hist1d<double>(fNBins, fLowerEdge, fUpperEdge);
    fIsValid = true;
  }
}

//-----------------------------------------------------------------------------
void Anp::TimeHist::Fill(double value)
{
  //
  // Fill temporary vector or histogram
  //
  ++fNFill;

  if(fNWait > 0) {
    if(fWait.size() < fNWait) {
      fWait.push_back(value);
      return;
    }
    else {
      CreateHist();
    }
  }

  fHist.Fill(value);
  if(value < fLowerEdge) ++fNLower;
  if(value > fUpperEdge) ++fNUpper;

  return;
}

//-----------------------------------------------------------------------------
void Anp::TimeHist::Finalize()
{
  //
  // Create histogram using current data
  //
  if(fNWait > 0) {
    if(fWait.empty()) {
      fIsValid = false;
      if(fDebug) {
	cerr << "TimeHist::Finalize - bad state: nwait, wait.size=" 
	     << fNWait << ", " << fWait.size() << endl;
      }
      return;
    }
    else {
      CreateHist();
    }
  }
}

//-----------------------------------------------------------------------------
TH1* Anp::TimeHist::GetTH1() const
{
  //
  // Create TH1 histogram, using HistMan as a guide
  //
  if(fNFill < 1) return 0; 

  TH1 *hinfo = Anp::HistMan::Instance().CreateTH1(fName, "time_alg");
  TH1 *hdata = Anp::CreateTH1<double>(fHist, fName);

  Anp::SetDir(hinfo, 0);
  Anp::SetDir(hdata, 0);
  
  if(hinfo) {
    const string hname = string(hinfo->GetName()) + "_dyn";

    hdata -> SetTitle(hinfo->GetTitle());
    hdata -> SetName(hname.c_str());

    hdata -> GetXaxis() -> SetTitle(hinfo->GetXaxis()->GetTitle());
    hdata -> GetXaxis() -> CenterTitle();

    hdata -> GetYaxis() -> SetTitle(hinfo->GetYaxis()->GetTitle());
    hdata -> GetYaxis() -> CenterTitle();
    hdata -> GetYaxis() -> SetTitleOffset(1.3);
  } 

  delete hinfo;
  return hdata;
}

//-----------------------------------------------------------------------------
void Anp::TimeHist::CreateHist()
{
  //
  // Create new histogram from wait vector
  //
  if(fNWait < 1 || fNBins < 2 || fWait.empty()) {
    cerr << "TimeHist::CreateHist - logic error" << endl;
    fNWait = 0; fWait.clear(); return; 
  }

  // Sort values
  std::sort(fWait.begin(), fWait.end());
  
  // Compute lower edge
  if(!fFixLowerEdge) {
    if(fIgnLowerFrac > 0.0 && fIgnLowerFrac < 0.5 && fWait.size() >= 10) {
      for(unsigned int i = 0; i < fWait.size(); ++i) {
	if(double(i+1)/double(fWait.size()) > fIgnLowerFrac) {
	  fLowerEdge = fWait[i];
	  break;
	}
      }
    }
    else {
      fLowerEdge = fWait.front();
    }
  }

  // Compute upper edge
  if(!fFixUpperEdge) {
    if(fIgnUpperFrac > 0.0 && fIgnUpperFrac < 0.5 && fWait.size() >= 10) {
      for(int i = fWait.size() - 1; i >= 0; --i) {
	if(double(fWait.size()-i)/double(fWait.size()) > fIgnUpperFrac) {
	  fUpperEdge = fWait[i];
	  break;
	}
      }
    }
    else {
      fUpperEdge = fWait.back();
    }
  }

  if(!(fLowerEdge < fUpperEdge)) {
    fLowerEdge = -1.0;
    fUpperEdge = +1.0;
  }
  else {
    // Increase range by pad fraction on reach side
    if(fPadLowerFrac > 0.0 || fPadUpperFrac > 0.0) {
      const double hrange = fUpperEdge - fLowerEdge;
      if(fPadLowerFrac > 0.0) fLowerEdge -= hrange*fPadLowerFrac;
      if(fPadUpperFrac > 0.0) fUpperEdge += hrange*fPadUpperFrac;
    }
  }

  // Make new histogram
  fHist = Hist1d<double>(fNBins, fLowerEdge, fUpperEdge);
  
  // Fill histogram
  for(unsigned int i = 0; i < fWait.size(); ++i) {
    const double value = fWait[i];
    if(value < fLowerEdge) ++fNLower;
    if(value > fUpperEdge) ++fNUpper;
    fHist.Fill(value);
  }

  // Clean up
  fNWait = 0; fWait.clear();
  fIsValid = true;
}
