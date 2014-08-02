#include "TROOT.h"
#include "TString.h"

#include <string>
#include <fstream>
#include <iostream>

#include "MakeGifs.C"

void MakeMyGifs(const TString TypePath = "type.txt", const TString RefFilesPath = "/afs/cern.ch/user/d/dferreir/public/references/MTPT") {
  std::string type;

  std::ifstream typeStr(TypePath.Data());
  typeStr >> type;
  typeStr.close();

  TString MyRef = RefFilesPath;
  MyRef += "/";
  MyRef += type.c_str();
  MyRef += "/";

  TString MyRefMT = TString(MyRef);
  MyRefMT += "expert-monitoring-MT.root";

  TString MyRefPT = TString(MyRef);
  MyRefPT += "expert-monitoring-PT.root";

  MakeGifs("expert-monitoring-MT.root", MyRefMT);
  MakeGifs("expert-monitoring-PT.root", MyRefPT);

}

