/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// This functions header
#include "xAODCutFlow/xAODCutFlowHelpers.h"

// STL includes
#include <string>
#include <vector>

// ROOT includes
#include <TH1.h>

// Local include(s):
#include "xAODCutFlow/CutBookkeeperContainer.h"



/// Make a histogram of the number of accepted events from a container.
::TH1* xAOD::CutFlowHelpers::nAcceptedEvents( const xAOD::CutBookkeeperContainer* cbkCont,
                                              const std::string& histName,
                                              int minCycle, int maxCycle ) {
  // Lets first make some checks
  if (!cbkCont) return 0;

  // Get the maximum cycle number that we need to worry about
  const int cbkMaxCycle = cbkCont->maxCycle();
  if (maxCycle<0) maxCycle = cbkMaxCycle;

  // Now, find all xAOD::CutBookkeepers in the container that we care about
  std::vector<const xAOD::CutBookkeeper*> cbks;
  cbks.reserve(cbkCont->size());
  for ( const xAOD::CutBookkeeper* cbk : *cbkCont ){
    const int cycle = cbk->cycle();
    if ( cycle >= minCycle && cycle <= maxCycle ){ cbks.push_back(cbk); }
  }
  const std::size_t nCBK = cbks.size();

  // Create the histogram:
  const std::string histTitle = histName + ", minCycle=" + std::to_string(minCycle)
                                         + ", maxCycle=" + std::to_string(maxCycle);
  ::TH1* resultHist = new ::TH1D( histName.c_str(), histTitle.c_str(),
                                  nCBK, 0.0, nCBK );
  // Set some properties of the histogram
  resultHist->GetXaxis()->SetTitle("CutBookkeeper Name");
  resultHist->GetYaxis()->SetTitle("nAcceptedEvents");

  // Now, set the bin content for all bins
  for (std::size_t i=0; i<nCBK; ++i){
    const xAOD::CutBookkeeper* cbk = cbks[0];
    const double nEvents = static_cast<double>(cbk->nAcceptedEvents());
    const double error = std::sqrt(nEvents);
    const std::string& name = cbk->name();
    const int binIdx = static_cast<int>(i+1);
    resultHist->SetBinContent(binIdx,nEvents);
    resultHist->SetBinError(binIdx,error);
    resultHist->GetXaxis()->SetBinLabel(binIdx,name.c_str());
  }

  // Done
  return resultHist;
}
