/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDECISION_TRIGDECISIONDICT_H
#define TRIGDECISION_TRIGDECISIONDICT_H

#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/ChainGroupFunctions.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/ConfigurationAccess.h"
#include "TrigDecisionTool/DecisionAccess.h"
#include "TrigDecisionTool/ExpertMethods.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/TrigDecisionToolCore.h"
#include "TrigDecisionTool/TDTUtilities.h"


#include "TPython.h"
#include "TROOT.h"
class TTree;


namespace Trig {


void
BuildTransientTrees(const std::vector< std::string >& PoolFileList, TTree*& eventTree, TTree*& metadataTree) {

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
DestroyTransientTrees() {
   TPython::Exec("del ct");
   TPython::Exec("del mdt");
   TPython::Exec("del trct");
   TPython::Exec("del trmdt");
}

} // namespace Trig


namespace TDTStruct {
   ToolHandle<Trig::TrigDecisionTool> tdt;

}

#endif
