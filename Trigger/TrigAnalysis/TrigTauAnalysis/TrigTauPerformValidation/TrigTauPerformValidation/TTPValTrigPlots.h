/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TTPVALTRIGPLOTS_H_
#define  TTPVALTRIGPLOTS_H_
//
// Class to make lots of plots of lots of trigger variables
//
#include "TString.h"
#include "TTree.h"

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauHistHelper.h"

#include "TrigTauPerformValidation/TrigTauCounter.h"

class TTPValTrigPlots {
 public:
  TTPValTrigPlots();
  virtual ~TTPValTrigPlots();

  // Functions to define selectors
  void setDefaultVersion(TrigTauSelectionManager::MENUVERSION version);
  void setSelector(TrigTauFlags::TRIGGERMENU menu);

  void setHLTAuthorType(TrigTauFlags::HLTAUTHOR type) {_hltAuthorType = type;}

  // Return pointer to selector (useful for making changes)
  TrigTauSelectionManager* getSelector() {return _selManager;}

  void clear(); // Delete everything and recreate
  void reset(); // Sets everything to zero

  void fill(const TString& filename, double wt=1.);
  void fill(TTree* tree);

  void draw(const TString& outfile);

  void write(const TString& outfile) {hh.writeAll(outfile);}

  // Lower level routines
  void setCurrentWeight(double weight) {_currentWeight = weight;}
  void defineTH(unsigned long tlo, unsigned long thi);
  void setTHBins(unsigned long bins) {_thbins=bins;}

  //  void setTHRange(const TString& filename);

  // Allow access to be able to change plot definitions if desired
  TrigTauHistHelper hh;

 private:

  double _currentWeight;
  TrigTauSelectionManager::MENUVERSION _defVersion;
  TrigTauSelectionManager* _selManager;
  TrigTauCounter _nEvent;
  TrigTauBugFix _fix;

  // Time History information
  unsigned long _thbins;
  unsigned long _thstart;  // Starting time 
  unsigned long _thend;    // Ending time

  TrigTauFlags::HLTAUTHOR _hltAuthorType;

  void fillRoiHist(const TrigTauEvent* ev,
		   const TrigTauTrigChain& ch, 
		   unsigned int type,
		   double wt);

};

#endif
