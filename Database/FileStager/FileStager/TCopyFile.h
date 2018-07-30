/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILESTAGER_TCOPYFILE_H
#define FILESTAGER_TCOPYFILE_H

#include <iostream>
#include <TROOT.h>
#include <TFile.h>
#include <string>

class TCopyFile : public TFile
{
 public:
  TCopyFile();
  TCopyFile(const char *fname, Option_t *option="", const char *ftitle="", Int_t compress=1);
  virtual ~TCopyFile();

  virtual const char* GetName() const { return m_inFile.c_str(); }

  static void SetOriginalTFile(bool originalTFile=true) { s_originalTFile=originalTFile; }
  static void SetFixPrexix(const char* in, const char* out) {
    s_prefixin = in;
    s_prefixout = out;
  }

  static void PrintInfo();

 private:
  const char* GetCopyFile(const char *fname);

  static bool s_originalTFile;
  static std::string s_prefixin;
  static std::string s_prefixout;

  std::string m_inFile;

  ClassDef(TCopyFile,1)
} ;

//bool TCopyFile::_originalTFile=true;
//std::string TCopyFile::_prefixin;
//std::string TCopyFile::_prefixout;

#endif // not FILESTAGER_TCOPYFILE_H

