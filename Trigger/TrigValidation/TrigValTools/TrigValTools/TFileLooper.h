/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVALTOOLS_TFILELOOPER_H
#define TRIGVALTOOLS_TFILELOOPER_H

/**
 * @file   TFileLooper.h
 * @brief  TFileLooper class
 * @author Frank Winklmeier
 *
 */

#include <fstream>
#include <vector>

#include "TDirectory.h"
#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TPRegexp.h"


/**
 * @class  TFileLooper
 * @brief  Base class to loop over all the keys in a ROOT file
 * @author Frank Winklmeier
 *
 * Loop over all the keys in a given ROOT file. Apply regular expressions
 * to the keys to relevant keys. The action for each key needs to be
 * implemented in a derived class.
 */

class TFileLooper {
 public:
  TFileLooper() = default;
  virtual ~TFileLooper() = default;

  TFileLooper (const TFileLooper& other) = delete;
  TFileLooper& operator= (const TFileLooper& other) = delete;

  /// Start processing
  virtual Int_t run(const char* filename, const char* rootDir = 0);

  /**
   * Method called for every key. Only dummy implementation.
   * Needs to be implemented in derived classes.
   */
  virtual void processKey(TDirectory& dir, TKey& key);

  /// \name Processing hooks
  //@{
  virtual void beginJob() {}
  virtual void endJob() {}
  virtual void beforeFile() {}
  virtual void afterFile() {}
  virtual void beforeDir() {}
  virtual void afterDir() {}
  virtual void beforeObj() {}
  virtual void afterObj() {}
  //@}

  /// Skip this comma separated list of directories
  void setDirsToSkip(const char* skipDirs) {if (skipDirs) m_skipDirs = skipDirs;}

  /// Skip keys that match this regexp
  void addFailRegexp(const char* regexp);

  /// Never skip keys that match this regexp
  void addPassRegexp(const char* regexp);

  /// Reverse pass/fail logic. First check on pass then on fail
  void passBeforeFailRegexp(Bool_t passBeforeFail = kTRUE) { m_passBeforeFail = passBeforeFail; }

  /// Set verbose mode
  void setVerbose(Bool_t verbose = kTRUE) {m_verbose = verbose;}
  
  /// Query verbose mode
  Bool_t verbose() const {return m_verbose;}

  /// Current ROOT file
  const TFile* file() const { return m_file; }

  /// Current directory
  TString rootDir() const { return m_rootDir; }

protected:
  TFile* m_file{nullptr};
  TString m_rootDir;
  TString m_skipDirs;
  Bool_t m_verbose{kFALSE};
  Bool_t m_passBeforeFail{kFALSE};
  Int_t m_errorCode{0};

  std::vector<TPRegexp> m_failRE;
  std::vector<TPRegexp> m_passRE;

  std::vector<std::string> m_skippedObjects;
  
  TString getPathFromDir(const TDirectory& dir);
  TString getKeyPath(const TDirectory& dir, const TKey& key);

  void processDir(TDirectory& dir);
  void processFile(const char* filename, const char* rootDir = 0);
  Bool_t skipDir(const TDirectory& dir);  
  Bool_t skipObject(const char* name);
  TString getDrawOptions(const TH1& h);    
};

#endif
