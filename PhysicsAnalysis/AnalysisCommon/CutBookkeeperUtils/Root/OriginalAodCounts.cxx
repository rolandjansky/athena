/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CutBookkeeperUtils/OriginalAodCounts.h"

#include <xAODCutFlow/CutBookkeeper.h>
#include <xAODCutFlow/CutBookkeeperContainer.h>

#include "xAODRootAccess/TEvent.h"

#include "TFile.h"

AodCountsConfig::AodCountsConfig()
  // add default values here
{
}

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


OriginalAodCounts getOriginalAodCounts(xAOD::TEvent& event,
                                       const AodCountsConfig&) {

  OriginalAodCounts counts;

  // check for corruption
  const xAOD::CutBookkeeperContainer* incompleteCBC = nullptr;
  if(!event.retrieveMetaInput(incompleteCBC, "IncompleteCutBookkeepers").isSuccess()){
    throw std::runtime_error("Failed to retrieve IncompleteCutBookkeepers from MetaData!");
  }
  counts.nIncomplete = incompleteCBC->size();

  // Now, let's find the actual information
  const xAOD::CutBookkeeperContainer* completeCBC = 0;
  if(!event.retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()){
    throw std::runtime_error("Failed to retrieve CutBookkeepers from MetaData!");
  }

  // Now, let's actually find the right one that contains all the needed info...
  const xAOD::CutBookkeeper* allEventsCBK = 0;
  int maxCycle = -1;
  for (const auto *cbk: *completeCBC) {
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

