/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TMultiFileLooper.cxx
 * @brief  TMultiFileLooper implementation
 * @author Frank Winklmeier
 *
 * $Id: TMultiFileLooper.cxx,v 1.1 2008-07-28 11:02:00 fwinkl Exp $
 */

#include "TrigValTools/TMultiFileLooper.h"
#include "TString.h"

#include <iostream>

using namespace std;

Int_t TMultiFileLooper::run(const char* fileListName, const char* /*rootDir*/)
{
  _errorCode = 0;

  if (fileListName==0) {
    cout << "Invalid file list (0)" << endl;
    _errorCode = 1;
    return _errorCode;
  }
  
  ifstream ifs;
  ifs.open(fileListName);
  if (!ifs) {
    cout << "Cannot open file " << fileListName << endl;
    _errorCode = 1;
    return _errorCode;
  }
  
  while (!ifs.eof()) {
    string filename;
    ifs >> filename;
    if (filename!="") addFile(filename.c_str());
  }

  return run();
}


Int_t TMultiFileLooper::run()
{
  _errorCode = 0;
  beginJob();
  
  vector<TString>::iterator iter;
  for (iter=_fileList.begin(); iter!=_fileList.end(); iter++) {
    processFile(*iter);
  }

  endJob();
  return _errorCode;
}

void TMultiFileLooper::addFile(const char*filename)
{
  if (filename) _fileList.push_back(TString(filename));
}
