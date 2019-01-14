/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  m_errorCode = 0;

  if (fileListName==0) {
    cout << "Invalid file list (0)" << endl;
    m_errorCode = 1;
    return m_errorCode;
  }
  
  ifstream ifs;
  ifs.open(fileListName);
  if (!ifs) {
    cout << "Cannot open file " << fileListName << endl;
    m_errorCode = 1;
    return m_errorCode;
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
  m_errorCode = 0;
  beginJob();
  
  vector<TString>::iterator iter;
  for (iter=m_fileList.begin(); iter!=m_fileList.end(); iter++) {
    processFile(*iter);
  }

  endJob();
  return m_errorCode;
}

void TMultiFileLooper::addFile(const char*filename)
{
  if (filename) m_fileList.push_back(TString(filename));
}
