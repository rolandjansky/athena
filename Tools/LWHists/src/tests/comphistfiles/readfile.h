/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "rootincludes.h"
#include <map>
#include <string>
#include <cassert>

typedef std::map<std::string,TH1*> KeyToHistMap;

//____________________________________________________________________
void appendHistsFromTDirRecursively ( TDirectory*dir, KeyToHistMap& hists,
				      const std::string& posinfile,const std::string& histpostfix )
{
  TIter nextkey( dir->GetListOfKeys() );
  TKey *key;
  while ((key = (TKey*)nextkey())) {
    TObject *obj = key->ReadObj();
    TDirectory * subdir = dynamic_cast<TDirectory*>(obj);
    if (subdir) {
      appendHistsFromTDirRecursively(subdir,hists,posinfile+std::string(subdir->GetName())+'/',histpostfix);
      continue;
    }
    TH1 * h = dynamic_cast<TH1*>(obj);
    if (h) {
      assert(hists.find(posinfile+h->GetName())==hists.end());
      TH1* hclone = dynamic_cast<TH1*>(h->Clone((h->GetName()+histpostfix).c_str()));
      assert(hclone);
      hists[posinfile+h->GetName()] = hclone;
    }
  }
}

//____________________________________________________________________
bool getAllHistsInRootFile(const std::string& filename,KeyToHistMap&hists)
{
  //NB: Do NOT delete the file before returning. Due to the workings
  //of ROOT, this results in the read histograms to become invalid!!
  TFile *file = TFile::Open(filename.c_str());
  if (!file||!file->IsOpen()) {
    //delete file;
    return false;
  }
  std::string posinfile="/";
  //NB: filename might contain directories. Should we replace all '/'?
  appendHistsFromTDirRecursively(file,hists,posinfile,"_from_"+filename);
  //  delete file;
  return true;
}
