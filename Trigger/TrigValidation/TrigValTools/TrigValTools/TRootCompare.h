/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVALTOOLS_TROOTCOMPARE_H
#define TRIGVALTOOLS_TROOTCOMPARE_H

/**
 * @file   TRootCompare.h
 * @brief  TRootCompare class
 * @author Frank Winklmeier
 *
 * $Id: TRootCompare.h 702373 2015-10-22 13:55:56Z fwinkl $
 */

#include "TFileLooper.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TKey.h"

/**
 * @class  TRootCompare
 * @brief  Class to compare the histograms in two root files
 * @author Frank Winklmeier
 *
 * Compare histograms in two root files. Statistics of differences
 * is printed as well as a PS file with histograms overlayed.
 */
class TRootCompare : public TFileLooper {
 public:
  enum ALG {BIN, CHI2, AXIS};

 public:
  TRootCompare();
  virtual ~TRootCompare();
  
  virtual void processKey(TDirectory& dir, TKey& key);
  virtual void beginJob();
  virtual void endJob();
  
  Bool_t setReferenceFile(const char* filename, const char* baseDir = 0);
  Bool_t setOutputFile(const char* filename);
  Bool_t setPsFile(const char* filename);
  void setAlg(TRootCompare::ALG alg, Double_t threshold) {
    _alg = alg; _threshold = threshold;
  }
  void drawNormalized(Bool_t norm = kTRUE) { _drawNormalized = norm; }
  void drawDiff(Bool_t diff = kTRUE) { _drawDiff = diff; }
  
  Int_t matchingHist() const { return _histMatch; }
  Int_t totalHist() const { return _histTotal; }
  Int_t missingHist() const { return _histMissing; }
  
 private:
  Bool_t compareHist(TH1& h, TH1& href);
  void createDirectory(TFile* f, const char* dirpath);
  void printCanvas(const char* filename);
  
 private:
  TFile* _refFile;
  TFile* _outFile;
  TString _refRootDir;
  TString _psFile;
  TCanvas* _can;
  
  ALG _alg;
  Double_t _threshold;
  Int_t _histMatch;
  Int_t _histTotal;
  Int_t _histMissing;
  Bool_t _drawNormalized;
  Bool_t _drawDiff;
  
  std::vector<std::string> _noMatch;
 
};

#endif
