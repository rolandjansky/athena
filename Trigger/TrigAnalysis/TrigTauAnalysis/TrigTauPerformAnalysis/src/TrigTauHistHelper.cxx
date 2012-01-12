/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <algorithm>
using namespace std;

#include "TFile.h"
#include "TObjArray.h"

#include "TrigTauPerformAnalysis/TrigTauHistHelper.h"

TrigTauHistHelper::TrigTauHistHelper() :
        _nHist(1), 
        _debug(false), 
        _yAxisLabels(1, "")
{}

TrigTauHistHelper::TrigTauHistHelper(unsigned long nh) : _nHist(nh), 
							 _debug(false), 
							 _yAxisLabels(nh, "")
{
  _h1D.clear();
  _h2D.clear();
  _opt.clear();
  _tags.clear();  
}

TrigTauHistHelper::~TrigTauHistHelper() {
  //  clear();
}

void
TrigTauHistHelper::clear() {
  for (map<TString, TH1D*>::iterator imap = _h1D.begin();
       imap != _h1D.end(); imap++) {
    delete imap->second;
  }

  for (map<TString, TH2D*>::iterator imap = _h2D.begin();
       imap != _h2D.end(); imap++) {
    delete imap->second;
  }
  _h1D.clear();
  _h2D.clear();
  _logY.clear();
  _tags.clear();
  _opt.clear();
  _yAxisLabels.clear();

}

void
TrigTauHistHelper::reset() {
  for (map<TString, TH1D*>::iterator imap = _h1D.begin();
       imap != _h1D.end(); imap++) {
    (imap->second)->Reset();
  }

  for (map<TString, TH2D*>::iterator imap = _h2D.begin();
       imap != _h2D.end(); imap++) {
    (imap->second)->Reset();
  }
}

void
TrigTauHistHelper::create(TString tag, long nbx, double xlo, double xhi,
		   TString xAxis) {

  if (_debug) cout << "Creating family with tag " << tag << endl;
  // Check if this is a unique tag
  if (find(_tags.begin(), _tags.end(), tag) != _tags.end()) {
    cout << "TrigTauHistHelper::create - Histogram family with name " << tag
	 << " already exists!" << endl;
    return;
  }

  _tags.push_back(tag);

  TString title;
  for (unsigned int iHist=0; iHist<_nHist; iHist++) {
    title = tag;
    title += iHist;
    if (_debug) cout << " -- " << title << " " << xAxis << " " << _yAxisLabels[iHist] << endl;

    TH1D* h = new TH1D(title, "", nbx, xlo, xhi);

    // Set axis labels
    h->GetXaxis()->SetTitle(xAxis);
    h->GetYaxis()->SetTitle(_yAxisLabels[iHist]);

    // Check if this already exists
    if (_h1D.find(title) != _h1D.end()) {
      cout << "TrigTauHistHelper::create - Histogram with name " << title
	   << " already exists!" << endl;
      return;
    }

    // Safe to insert new element
    _h1D[title] = h;
  }

  _logY[tag] = false;

}

void
TrigTauHistHelper::create(TString tag, long nbx, double xlo, double xhi,
		   long nby, double ylo, double yhi,
		   TString xAxis, TString yAxis, TString opt) {

  if (_debug) cout << "Creating family with tag " << tag << endl;
  // Check if this is a unique tag
  if (std::find(_tags.begin(), _tags.end(), tag) != _tags.end()) {
    cout << "TrigTauHistHelper::create - Histogram family with name " << tag
	 << " already exists!" << endl;
    return;
  }

  _tags.push_back(tag);

  TString title;
  for (unsigned int iHist=0; iHist<_nHist; iHist++) {
    title = tag;
    title += iHist;
    if (_debug) cout << " -- " << title << " " << xAxis << " " << yAxis << endl;
    TH2D* h = new TH2D(title, "", nbx, xlo, xhi, nby, ylo, yhi);
    h->GetXaxis()->SetTitle(xAxis);
    h->GetYaxis()->SetTitle(yAxis);

    // Check if this already exists
    if (_h2D.find(title) != _h2D.end()) {
      cout << "TrigTauHistHelper::create - Histogram with name " << title
	   << " already exists!" << endl;
      return;
    }

    // Safe to insert new element
    _h2D[title] = h;
    _opt[title] = opt;
  }

}

void
TrigTauHistHelper::fill1D(TString tag, long itype, double x, double wt) {

  TH1D* h = get1D(tag, itype);
  if (h == 0) {
    cout << "TrigTauHistHelper::fill - Histogram with name " << tag
	 << " and type " << itype
	 << " does not exist!" << endl;
    return;
  }
  
  h->Fill(x, wt);
  return;
}

void
TrigTauHistHelper::fill2D(TString tag, long itype, double x, double y, double wt) {

  TH2D* h = get2D(tag, itype);
  if (h == 0) {
    cout << "TrigTauHistHelper::fill - Histogram with name " << tag
	 << " and type " << itype
	 << " does not exist!" << endl;
    return;
  }

  h->Fill(x, y, wt);
  return;
}

TH1D*
TrigTauHistHelper::get1D(TString tag, long itype) {

  // Form the name
  TString title = tag;
  title += itype;

  // Check that this exists
  map<TString, TH1D*>::iterator ih = _h1D.find(title);
  if (ih == _h1D.end()) return 0;

  return ih->second;
}

TH2D*
TrigTauHistHelper::get2D(TString tag, long itype) {

  // Form the name
  TString title = tag;
  title += itype;

  // Check that this exists
  map<TString, TH2D*>::iterator ih = _h2D.find(title);
  if (ih == _h2D.end()) return 0;

  return ih->second;
}

void
TrigTauHistHelper::writeAll(TString outfile) {

  TObjArray hlist(0);  // New array to hold histos to write

  // Add all to this list
  for (map<TString, TH1D*>::iterator imap = _h1D.begin();
       imap != _h1D.end(); imap++) {
    hlist.Add(imap->second);
  }

  for (map<TString, TH2D*>::iterator imap = _h2D.begin();
       imap != _h2D.end(); imap++) {
    hlist.Add(imap->second);
  }

  TFile f(outfile, "recreate");
  hlist.Write();
  f.Close();
}

void
TrigTauHistHelper::draw(TString itag, long itype) {

  TString title = itag;
  title += itype;

  if (_h1D.find(title) != _h1D.end()) {
    TH1D* h = _h1D[title];
    h->DrawCopy();
    if (_debug) cout << "logY for " << *itag << " is " << _logY[*itag] << endl;
  } else if (_h2D.find(title) != _h2D.end()) {
    TH2D* h = _h2D[title];
    h->DrawCopy(_opt[title]);
  }

}

void
TrigTauHistHelper::drawAll(TString plotfile, unsigned long nStart, unsigned long nEnd, TCanvas* c) {

  // Open the file
  TString dummy;
  dummy = plotfile + "[";
  c->Print(dummy);

  TString title;

  for (list<TString>::iterator itag = _tags.begin();
       itag != _tags.end(); itag++) {

    if (_debug) cout << "Drawing family " << *itag << endl;

    long padCounter = 0;
    for(unsigned int i=nStart; i<=nEnd; i++) {
      if (i >= _nHist) continue;

      TVirtualPad* tp = c->cd(++padCounter);
      
      title = *itag;
      title += i;

      if (_h1D.find(title) != _h1D.end()) {
	TH1D* h = _h1D[title];
	h->DrawCopy();
	if (_debug) cout << "logY for " << *itag << " is " << _logY[*itag] << endl;
	if (_logY[*itag]) tp->SetLogy();
	else tp->SetLogy(0);
      } else if (_h2D.find(title) != _h2D.end()) {
	TH2D* h = _h2D[title];
	tp->SetLogy(0);
	h->DrawCopy(_opt[title]);
      }

    }
    c->Print(plotfile);

  }

  // Close the file
  dummy = plotfile + "]";
  c->Print(dummy);
}

void
TrigTauHistHelper::drawAll(TString plotfile, long nPerPage, TCanvas* c) {

  // Open the file
  TString dummy;
  dummy = plotfile + "[";
  c->Print(dummy);

  TString title;

  for (list<TString>::iterator itag = _tags.begin();
       itag != _tags.end(); itag++) {

    if (_debug) cout << "Drawing family " << *itag << endl;

    long padCounter = 0;
    for(unsigned int i=0; i<_nHist; i++) {
      TVirtualPad* tp = c->cd(padCounter+1);
      
      title = *itag;
      title += i;

      if (_debug) cout << "Drawing hist " << title ;
      if (_h1D.find(title) != _h1D.end()) {
	TH1D* h = _h1D[title];
	if (_debug) cout << " found in 1D list with pointer " << h << endl;
	h->DrawCopy();
	if (_debug) cout << "logY for " << *itag << " is " << _logY[*itag] << endl;
	if (_logY[*itag]) tp->SetLogy();
	else tp->SetLogy(0);
      } else if (_h2D.find(title) != _h2D.end()) {
	TH2D* h = _h2D[title];
	if (_debug) cout << " found in 2D list with pointer " << h << endl;
	tp->SetLogy(0);
	h->DrawCopy(_opt[title]);
      } else {
	if (_debug) cout << " - wasn't found!" << endl;
      }

      if (++padCounter == nPerPage) {
	padCounter = 0;
	if (_debug) cout << "Writing page" << endl;
	c->Print(plotfile);
      }
    }
  }

  // Close the file
  dummy = plotfile + "]";
  c->Print(dummy);
}

void
TrigTauHistHelper::drawEffAll(TString plotfile, long nPerPage, long nDenom, TCanvas* c) {

  TString title;

  // Open the file
  TString dummy;
  dummy = plotfile + "[";
  c->Print(dummy);

  for (list<TString>::iterator itag = _tags.begin();
       itag != _tags.end(); itag++) {

    title = *itag;

    long padCounter = nDenom;
    const TH1D* hden = get1D(title, nDenom);
    if (hden == 0) continue;

    for(unsigned int i=nDenom; i<_nHist; i++) {
      TVirtualPad* tp = c->cd(padCounter+1);

      const TH1D* hnum = get1D(title, i);
      if (hnum == 0) continue;

      if (long(i) == nDenom) {
        hnum->DrawCopy("HIST");
      } else {
        TH1D* heff = new TH1D(*hnum);
	heff->Sumw2();
        heff->Divide(hnum, hden, 1, 1, "B");
        heff->DrawCopy();
	
        delete heff;
      }
      tp->SetLogy(0);

      if (++padCounter == nPerPage) {
        padCounter = nDenom;
        c->Print(plotfile);
      }
    }
  }

  // Close the file
  dummy = plotfile + "]";
  c->Print(dummy);

}
