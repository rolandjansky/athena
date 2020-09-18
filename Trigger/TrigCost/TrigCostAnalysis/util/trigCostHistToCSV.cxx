/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AsgTools/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <string.h>
#include <set>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include "TrigCostAnalysis/TableConstructors/AlgorithmTableConstructor.h"
#include "TrigCostAnalysis/TableConstructors/GlobalTableConstructor.h"
#include "TrigCostAnalysis/TableConstructors/ThreadOccupancyTableConstructor.h"

#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include "TKey.h"

/**
 * @file trigCostHistToCSV
 * @brief Standalone ROOT application to read the output histogram file of 
 * TrigCostAnalysis and spit out CSV table representation of the data for presentation.
 */


MsgStream msgStream(nullptr, "trigCostHistToCSV");


MsgStream& msg() {
  return msgStream;
} 


TString constructCSVName(const std::vector<TString>& dir) {
  if (dir.size() != 2) {
    msg() << MSG::ERROR << "Expecting exactly 2 directories";
    return "";
  }
  TString csv = "Table_";
  csv += dir.at(1); // e.g. "Algorithm_HLT"
  csv += "_";
  csv += dir.at(0); // e.g. "All"
  csv += ".csv";
  return csv;
}


void exportDirectory(TFile* file, const std::vector<TString>& dir, float walltime) {
  TString exportName = constructCSVName(dir);
  msg() << MSG::DEBUG << "  Do Histo for " << exportName << endmsg;

  std::string table;
  const TString rootDir = dir.at(0) + "/" + dir.at(1) + "/";
  if (dir.at(1) == "Algorithm_HLT") {
    AlgorithmTableConstructor atc("Algorithm_HLT");
    table = atc.getTable(file, rootDir, walltime);
  } else if (dir.at(1) == "Algorithm_Class_HLT") {
    AlgorithmTableConstructor atc("Algorithm_Class_HLT");
    table = atc.getTable(file, rootDir, walltime);
  } else if (dir.at(1) == "Global_HLT") {
    GlobalTableConstructor gtc("Global_HLT");
    table = gtc.getTable(file, rootDir, walltime);
  } else if (dir.at(1) == "Thread_Occupancy_HLT") {
    ThreadOccupancyTableConstructor totc("Thread_Occupancy_HLT");
    table = totc.getTable(file, rootDir, walltime);
  }

  std::ofstream fstream(exportName.Data());
  fstream << table;
  msg() << MSG::INFO << "    Saved " << exportName << endmsg;

}


float getWalltime(TString name) {
  TIter nextKey( gDirectory->GetListOfKeys() );
  TKey* key = nullptr;
  while ( (key = (TKey*)nextKey()) ) {
    TObject* obj = key->ReadObj();
    if ( obj->GetName() == name + "_walltime") {
      TH1* hist = dynamic_cast<TH1*>(obj);
      return hist->GetBinContent(1);
    }
  }
  msg() << MSG::ERROR << "Unable to locate walltime";
  return 0;
}


void exploreTree(TFile* file, const size_t level = 0, std::vector<TString> dir = std::vector<TString>()) {
  float walltime = 0;
  if (level == 1) {
    walltime = getWalltime(dir.at(0));
  }
  TIter nextKey( gDirectory->GetListOfKeys() );
  TKey* key = nullptr;
  while ( (key = (TKey*)nextKey()) ) {
    TObject* obj = key->ReadObj();
    const TString name = obj->GetName();
    if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
      dir.push_back( name );
      TString fullDir;
      for (const TString& s : dir) {
        fullDir += s;
        fullDir += "/";
      }
      msg() << MSG::DEBUG << "Level " << level << ":" << fullDir << endmsg;
      file->cd(fullDir);
      if (level < 1) {
        exploreTree(file, level + 1, dir);
      } else {
        exportDirectory(file, dir, walltime);
        dir.pop_back();
      }
    } else if (name.First("_walltime") != kNPOS) {
      // This is fine
    } else {
      msg() << MSG::ERROR << "Only expecting TDirectories at level " << level << ", got " << name << endmsg;
    }
  }
}


int main() {

  msg().setLevel(MSG::DEBUG);
  msg() << MSG::INFO << "Executing trigCostHistToCSV" << endmsg;

  std::unique_ptr<TFile> file( TFile::Open("TrigCostRoot_Results.root") );
  if (file == nullptr) {
    return 1;
  }
  exploreTree(file.get());

  return 0;
}
