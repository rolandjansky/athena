/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TTPVALCOSMICPLOTS_H_
#define  TTPVALCOSMICPLOTS_H_
//
// Class to make lots of plots of lots of trigger variables
// Eric Torrence 
// 
// Use the PLOTTYPE to instantiate different plot progressions
// DEFAULT: 6 plots showing L1 In, L1 Out, L2Cal Out, L2Trk Out, EF Out, Reco
// RECO_BASED: 4 plots showing Reco, L1 Out, L2 Out, EF Out
//
// See example in macros/makeCosmicPlots.cxx
//
#include "TString.h"
#include "TTree.h"

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauHistHelper.h"

#include "TrigTauPerformValidation/TrigTauCounter.h"

class TTPValCosmicPlots {
 public:

  // Selector to choose type of plots to generate
  enum PLOTTYPE {
    DEFAULT,         // Standard plots starting with L1 ROIs
    RECO_BASED       // Start with reco taus 
  };
 
  TTPValCosmicPlots(TTPValCosmicPlots::PLOTTYPE type = TTPValCosmicPlots::DEFAULT);
  virtual ~TTPValCosmicPlots();

  // Functions to define selectors
  void setDefaultVersion(TrigTauSelectionManager::MENUVERSION version);
  void setSelector(TrigTauFlags::TRIGGERMENU menu);

  void setHLTAuthorType(TrigTauFlags::HLTAUTHOR type) {_hltAuthorType = type;}

  // Return pointer to selector (useful for making changes)
  TrigTauSelectionManager* getSelector() {return _selManager;}

  // Additional selections to help with cosmic samples
  void requireMinL2EmEt(float Et); // Energy in MeV
  void requireMaxL2EmEt(float Et); // Energy in MeV

  void requireAnyL2Negative(bool val=true) {_requireAnyL2Negative = val;}
  void setL2NegativeThreshold(float threshold) {_L2NegThreshold = threshold;}

  // Require offline (m_accept & bitmask) to be non-zero
  void requireRecoMask(long bitmask) {_requireRecoMask = bitmask;}

  // Require offline seed type (seed & seedmask) to be non-zero (1-calo, 2-trk)
  void requireSeedType(long seedmask) {_requireSeedMask = seedmask;}

  void clear(); // Delete everything and recreate
  void reset(); // Sets everything to zero

  void fill(const TString& filename, double wt=1.);
  void fill(TTree* tree);

  void draw(const TString& outfile);
  void drawEff(const TString& outfile);

  void write(const TString& outfile) {if (hh != 0) hh->writeAll(outfile);}

  // Lower level routines
  void setCurrentWeight(double weight) {_currentWeight = weight;}
  void defineTH(float tlo, float thi);
  void setTHBins(unsigned long bins) {_thbins=bins;}

  //  void setTHRange(const TString& filename);

  // Allow access to be able to change plot definitions if desired
  TrigTauHistHelper* hh;

  // Set energy ranges and binning for Et plots
  void setL1EtBins(unsigned int nBins, float xLo, float xHi);
  void setL2EtBins(unsigned int nBins, float xLo, float xHi);
  void setEFEtBins(unsigned int nBins, float xLo, float xHi);
  // Reco plots use EF binning

  void setEtaBins(unsigned int nBins, float xLo, float xHi);
  void setPhiBins(unsigned int nBins, float xLo, float xHi);

 private:

  TTPValCosmicPlots::PLOTTYPE _plotType;

  double _currentWeight;
  TrigTauSelectionManager::MENUVERSION _defVersion;
  TrigTauSelectionManager* _selManager;
  TrigTauCounter _nEvent;
  TrigTauBugFix _fix;

  float _minL2EmEt;
  float _maxL2EmEt;

  bool _requireAnyL2Negative;
  float _L2NegThreshold;

  unsigned long _requireRecoMask;
  unsigned long _requireSeedMask;

  // Time History information
  unsigned long _thbins;
  float _thstart;  // Starting time (in hours)
  float _thend;    // Ending time (in hours)

  // L1 energy plots
  unsigned int _nL1E;
  float _loL1E;
  float _hiL1E;

  // L2 energy plots
  unsigned int _nL2E;
  float _loL2E;
  float _hiL2E;

  // EF energy plots
  unsigned int _nEFE;
  float _loEFE;
  float _hiEFE;

  // Eta/phi binning
  unsigned int _nEta;
  float _loEta;
  float _hiEta;

  unsigned int _nPhi;
  float _loPhi;
  float _hiPhi;

  TrigTauFlags::HLTAUTHOR _hltAuthorType;

  void fillRoiHist(const TrigTauEvent* ev,
		   const TrigTauTrigChain& ch, 
		   unsigned int type,
		   double wt);

  bool passedExtraL2(const TrigTauTrigChain& ch);

  void fillDefault(TTree* tree, TrigTauEvent* event);
  void fillReco(TTree* tree, TrigTauEvent* event);
};

#endif
