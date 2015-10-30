/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVALTOOLS_TROOT2HTML_H
#define TRIGVALTOOLS_TROOT2HTML_H

/**
 * @file   TRoot2Html.h
 * @brief  TRoot2Html class
 * @author Frank Winklmeier
 *
 * $Id: TRoot2Html.h 702373 2015-10-22 13:55:56Z fwinkl $
 */

#include <fstream>
#include <vector>

#include "TMultiFileLooper.h"
#include "TDirectory.h"
#include "TString.h"
#include "TH1.h"

class TPRegexp;

/**
 * @class  TRoot2Html
 * @brief  Create a (static) web page from ROOT histograms
 * @author Frank Winklmeier
 *
 * Create images from histograms in root files and assemble them in
 * a web page with a tree like naviation structure.
 */
class TRoot2Html : public TMultiFileLooper {
 public:

  /// C'tor
  TRoot2Html();
  /// D'tor
  virtual ~TRoot2Html() {}
  /// Copy C'tor (needed for Reflex dictionary generation)
  TRoot2Html(const TRoot2Html& other);

  /// \name Processing hooks
  //@{
  virtual void beginJob();
  virtual void endJob();
  virtual void beforeFile();
  virtual void afterFile();
  virtual void beforeDir();
  virtual void afterDir();
  virtual void processKey(TDirectory& dir, TKey& key);
  //@}
  
  /// Output directory for HTML pages
  void setOutputDir(const char* dir) {_outDir = dir;}

  /// Set image size
  void setImageSize(Int_t width, Int_t height) {_imgWidth = width; _imgHeight = height;}

  /// Add draw options for images (selected by regular expression on name)
  void addDrawOptions(const char* regexp, const char* options);

  /// Add draw options for images (selected by regular expression on class name)
  void addClassDrawOptions(const char* regexp, const char* options);
    
  /// Show full file name in tree
  void showFullFileName(Bool_t show = kTRUE) {_showFullFileName = show;}
  
 private:
  TString _fileList;
  TString _outDir;
  Int_t _imgHeight, _imgWidth;
  std::ofstream _xml;
  Int_t _nodeId;
  
  Bool_t _showFullFileName;
  std::vector< std::pair<TPRegexp*,TString> > _drawOptions;
  std::vector< std::pair<TPRegexp*,TString> > _classDrawOptions;
    
  TString hist2Png(TDirectory& dir, const TString& name);
  TString getDrawOptions(const TH1& h);
    
  const char* xmlTreeItem(const char* text);
  const char* xmlTreeItemClose();
  const char* xmlUserData(const char* name, const char* data);

  // Not copyable due to the ofstream.
  TRoot2Html& operator= (const TRoot2Html&);
};

#endif
