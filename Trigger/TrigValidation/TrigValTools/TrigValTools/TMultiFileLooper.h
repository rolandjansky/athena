/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVALTOOLS_TMULTIFILELOOPER_H
#define TRIGVALTOOLS_TMULTIFILELOOPER_H

/**
 * @file   TMultiFileLooper.h
 * @brief  TMultiFileLooper class
 * @author Frank Winklmeier
 *
 * $Id: TMultiFileLooper.h 702373 2015-10-22 13:55:56Z fwinkl $
 */

#include "TFileLooper.h"
#include "TString.h"
#include <vector>

/**
 * @class  TMultiFileLooper
 * @brief  Base class to loop over all the keys in several ROOT files
 * @author Frank Winklmeier
 *
 * Same as TFileLooper but can loop over multiple ROOT files.
 */

class TMultiFileLooper : public TFileLooper {
 public:
  TMultiFileLooper() {}
  virtual ~TMultiFileLooper() = default;
  
  virtual Int_t run(const char* fileListName, const char* rootDir = 0);
  Int_t run();

  void addFile(const char* filename);

 private:
  std::vector<TString> m_fileList;
};

#endif
