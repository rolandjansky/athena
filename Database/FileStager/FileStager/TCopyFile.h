/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TGRIDCOPYFILE_HH__
#define __TGRIDCOPYFILE_HH__

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

  virtual const char* GetName() const { return _inFile.c_str(); }

  static void SetOriginalTFile(bool originalTFile=true) { _originalTFile=originalTFile; }
  static void SetFixPrexix(const char* in, const char* out) {
    _prefixin = in;
    _prefixout = out;
  }

  static void PrintInfo();

 private:
  const char* GetCopyFile(const char *fname);

  static bool _originalTFile;
  static std::string _prefixin;
  static std::string _prefixout;

  std::string _inFile;

  ClassDef(TCopyFile,1)
} ;

//bool TCopyFile::_originalTFile=true;
//std::string TCopyFile::_prefixin;
//std::string TCopyFile::_prefixout;

#endif

