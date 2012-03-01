/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileStager/TCopyFile.h"
#include "FileStager/TStageManager.h"
//#include "TROOT.h"
//#include "TEnv.h"

ClassImp(TCopyFile)

bool TCopyFile::_originalTFile=true;
std::string TCopyFile::_prefixin;
std::string TCopyFile::_prefixout;


TCopyFile::TCopyFile()
  : TFile()
{
}


TCopyFile::TCopyFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress) 
  : TFile(GetCopyFile(fname),option,ftitle,compress)
  , _inFile(fname)
{
}


TCopyFile::~TCopyFile()
{
  //this->TFile::~TFile();
  if (_originalTFile) return;

  TStageManager& manager(TStageManager::instance());
  manager.releaseFile(_inFile.c_str());
}


const char* 
TCopyFile::GetCopyFile(const char* fname) 
{
  string filename(fname);

  //cout << "TCopyFile::GetCopyFile fname = " << (!filename.empty()?filename:"<null>") << endl ;
  if (_originalTFile) return filename.c_str();

  TStageManager& manager(TStageManager::instance());
  return manager.getFile(filename.c_str());
}


void
TCopyFile::PrintInfo()
{
  /* xrootd */
  //gEnv->SetValue("XNet.ReadAheadSize",0);
  //gEnv->SetValue("XNet.Debug",3);
  //gDebug = 7;

  std::cout<<"===================================================================" << std::endl;
  std::cout<<"|| TFile Information   " << std::endl;
  std::cout<<"|| Read Bytes (fgBytesRead):     " << TFile::fgBytesRead << std::endl;
  std::cout<<"|| Write Bytes (fgBytesWrite):   " << TFile::fgBytesWrite << std::endl;
  std::cout<<"|| File Counter (fgFileCounter): " << TFile::fgFileCounter << std::endl;
  std::cout<<"|| Read Calls (fgReadCalls):     " << TFile::fgReadCalls << std::endl;
  std::cout<<"===================================================================" << std::endl;
}

