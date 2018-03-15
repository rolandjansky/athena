/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileStager/TCopyFile.h"
#include "FileStager/TStageManager.h"
//#include "TROOT.h"
//#include "TEnv.h"

using std::string;

ClassImp(TCopyFile)

bool TCopyFile::s_originalTFile=true;
std::string TCopyFile::s_prefixin;
std::string TCopyFile::s_prefixout;


TCopyFile::TCopyFile()
  : TFile()
{
}


TCopyFile::TCopyFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress) 
  : TFile(GetCopyFile(fname),option,ftitle,compress)
  , m_inFile(fname)
{
}


TCopyFile::~TCopyFile()
{
  //this->TFile::~TFile();
  if (s_originalTFile) return;

  TStageManager& manager(TStageManager::instance());
  manager.releaseFile(m_inFile.c_str());
}


const char* 
TCopyFile::GetCopyFile(const char* fname) 
{
  //cout << "TCopyFile::GetCopyFile fname = " << (!filename.empty()?filename:"<null>") << endl ;
  if (s_originalTFile) return fname;

  TStageManager& manager(TStageManager::instance());
  return manager.getFile(fname);
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

