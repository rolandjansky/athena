/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CutBookKeeperUtils/OriginalAodCounts.h"

#include "xAODCutflow/CutBookkeeper.h"
#include "xAODCutflow/CutBookkeeperContainer.h"

#include "xAODRootAccess/TEvent.h"

#include "TFile.h"
#include "TTree.h"

OriginalAodCounts::OriginalAodCounts():
  nEventsProcessed(0),
  sumOfWeights(0),
  sumOfWeightsSquared(0),
  nIncomplete(0)
{
}

OriginalAodCounts& OriginalAodCounts::operator+=(const OriginalAodCounts& a) {
  nEventsProcessed += a.nEventsProcessed;
  sumOfWeights += a.sumOfWeights;
  sumOfWeightsSquared += a.sumOfWeightsSquared;
  nIncomplete += a.nIncomplete;
  return *this;
}


OriginalAodCounts operator+(const OriginalAodCounts& a, const OriginalAodCounts& b) {
  OriginalAodCounts ret = a;
  ret += b;
  return ret;
}

// this is copied from
// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisMetadata#Analysis_Metadata_Root_or_athena


OriginalAodCounts getOriginalAodCounts(TFile& file, xAOD::TEvent& event) {
  // get the MetaData tree once a new file is opened, with
  TTree *MetaData = dynamic_cast<TTree*>(file.Get("MetaData"));
  if (!MetaData) {
    throw std::logic_error("MetaData not found! Exiting.");
  }
  MetaData->LoadTree(0);

  //check if file is from a DxAOD
  bool isDerivation = !MetaData->GetBranch("StreamAOD");

  if(!isDerivation) throw std::logic_error("this isn't a DAOD");

  OriginalAodCounts counts;

  // check for corruption
  const xAOD::CutBookkeeperContainer* incompleteCBC = nullptr;
  if(!event.retrieveMetaInput(incompleteCBC, "IncompleteCutBookkeepers").isSuccess()){
    throw std::logic_error("Failed to retrieve IncompleteCutBookkeepers from MetaData!");
  }
  counts.nIncomplete = incompleteCBC->size();

  // Now, let's find the actual information
  const xAOD::CutBookkeeperContainer* completeCBC = 0;
  if(!event.retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()){
    throw std::logic_error("Failed to retrieve CutBookkeepers from MetaData!");
  }

  // Now, let's actually find the right one that contains all the needed info...
  const xAOD::CutBookkeeper* allEventsCBK = 0;
  int maxCycle = -1;
  for (const auto& cbk: *completeCBC) {
    if (cbk->cycle() > maxCycle &&
        cbk->name() == "AllExecutedEvents" &&
        cbk->inputStream() == "StreamAOD") {
      allEventsCBK = cbk;
      maxCycle = cbk->cycle();
    }
  }
  counts.nEventsProcessed  = allEventsCBK->nAcceptedEvents();
  counts.sumOfWeights        = allEventsCBK->sumOfEventWeights();
  counts.sumOfWeightsSquared = allEventsCBK->sumOfEventWeightsSquared();
  return counts;
}
