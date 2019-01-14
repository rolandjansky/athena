/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    m_alg = alg; m_threshold = threshold;
  }
  void drawNormalized(Bool_t norm = kTRUE) { m_drawNormalized = norm; }
  void drawDiff(Bool_t diff = kTRUE) { m_drawDiff = diff; }
  
  Int_t matchingHist() const { return m_histMatch; }
  Int_t totalHist() const { return m_histTotal; }
  Int_t missingHist() const { return m_histMissing; }
  
 private:
  Bool_t compareHist(TH1& h, TH1& href);
  void createDirectory(TFile* f, const char* dirpath);
  void printCanvas(const char* filename);
  
 private:
  TFile* m_refFile;
  TFile* m_outFile;
  TString m_refRootDir;
  TString m_psFile;
  TCanvas* m_can;
  
  ALG m_alg;
  Double_t m_threshold;
  Int_t m_histMatch;
  Int_t m_histTotal;
  Int_t m_histMissing;
  Bool_t m_drawNormalized;
  Bool_t m_drawDiff;
  
  std::vector<std::string> m_noMatch;
 
};

#endif
