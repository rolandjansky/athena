/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/algorithm/string.hpp>

#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigNavStructure/TriggerElement.h"

#include <string>
#include <vector>

#include "TPython.h"
#include "TTree.h"
#include "TROOT.h"

#include <iostream>



namespace TDTUtilities_impl {

  // splitting a string at the ','
  std::vector<std::string>
    split(const std::string& line) {
    std::vector<std::string> res;
    std::string::size_type old_pos = 0, pos = line.find(',',0);
    while( pos != std::string::npos ) {
      res.push_back(line.substr(old_pos,pos-old_pos));
      old_pos = pos + 1;
      pos = line.find(',', old_pos);
    }
    // last entry
    if (old_pos < line.size())
      res.push_back(line.substr(old_pos,line.size()-old_pos));
    return res;
  }
}


std::vector< std::string >
Trig::convertStringToVector(const std::string& triggerNames) {
  std::string temp(triggerNames);
  std::string::size_type pos;
  // new erase all spaces
  while ((pos = temp.find_first_of(' ')) != std::string::npos)
    temp.erase(pos,1);
  return TDTUtilities_impl::split(temp);
}


std::vector< std::string >
Trig::keyWrap(const std::vector< std::string >& triggerNames) {
  std::vector< std::string > v;
  v.assign(triggerNames.begin(),triggerNames.end());
  std::sort(v.begin(),v.end());
  v.erase( std::unique(v.begin(),v.end()),v.end());
  return v;
}


void
Trig::BuildTransientTrees(const std::vector< std::string >& PoolFileList, TTree*& eventTree, TTree*& metadataTree) {

   // import necessary python libs
   TPython::Exec("import ROOT");
   TPython::Exec("import AthenaROOTAccess.transientTree");
   
   // create the persistent trees
   TPython::Exec("ct = ROOT.AthenaROOTAccess.TChainROOTAccess(\'CollectionTree\')");
   TPython::Exec("mdt = ROOT.AthenaROOTAccess.TChainROOTAccess(\'MetaData\')");
   for(std::vector<std::string>::const_iterator fIt = PoolFileList.begin(); fIt != PoolFileList.end(); fIt++ ) {
      std::string inputAODCT  = "ct.Add(\""  + *fIt + "/CollectionTree\")";
      std::string inputAODMDT = "mdt.Add(\"" + *fIt + "/MetaData\")";
      TPython::Exec(inputAODCT.c_str());
      TPython::Exec(inputAODMDT.c_str());
   }
   TPython::Exec("trct  = AthenaROOTAccess.transientTree.makeTree(ct)");
   TPython::Exec("trmdt = AthenaROOTAccess.transientTree.makeTree(mdt, persTreeName = \'MetaData\', dhTreeName = \'MetaDataHdrDataHeader\')");

   eventTree    = (TTree*)gROOT->Get("CollectionTree_trans");
   metadataTree = (TTree*)gROOT->Get("MetaData_trans");

}

void
Trig::DestroyTransientTrees() {
   TPython::Exec("del ct");
   TPython::Exec("del mdt");
   TPython::Exec("del trct");
   TPython::Exec("del trmdt");
}



std::string Trig::getTEName(const HLT::TriggerElement& te)
{ 
  std::string s; 
  return (TrigConf::HLTTriggerElement::getLabel(te.getId(), s) ? s : ""); 
}
