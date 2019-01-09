/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  void setOutputDir(const char* dir) {m_outDir = dir;}

  /// Set image size
  void setImageSize(Int_t width, Int_t height) {m_imgWidth = width; m_imgHeight = height;}

  /// Add draw options for images (selected by regular expression on name)
  void addDrawOptions(const char* regexp, const char* options);

  /// Add draw options for images (selected by regular expression on class name)
  void addClassDrawOptions(const char* regexp, const char* options);
    
  /// Show full file name in tree
  void showFullFileName(Bool_t show = kTRUE) {m_showFullFileName = show;}
  
 private:
  TString m_fileList;
  TString m_outDir;
  Int_t m_imgHeight, m_imgWidth;
  std::ofstream m_xml;
  Int_t m_nodeId;
  
  Bool_t m_showFullFileName;
  std::vector< std::pair<TPRegexp*,TString> > m_drawOptions;
  std::vector< std::pair<TPRegexp*,TString> > m_classDrawOptions;
    
  TString hist2Png(TDirectory& dir, const TString& name);
  TString getDrawOptions(const TH1& h);
    
  TString xmlTreeItem(const char* text);
  TString xmlTreeItemClose();
  TString xmlUserData(const char* name, const char* data);

  // Not copyable due to the ofstream.
  TRoot2Html& operator= (const TRoot2Html&);
};

#endif
