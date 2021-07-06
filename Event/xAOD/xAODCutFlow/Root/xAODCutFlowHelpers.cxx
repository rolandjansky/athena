/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  if (!cbkCont) return nullptr;

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


/// Resolve links between containers
namespace
{

xAOD::CutBookkeeper *resolveLink( const xAOD::CutBookkeeper *old,
                                  xAOD::CutBookkeeperContainer &contToUpdate,
                                  const xAOD::CutBookkeeperContainer &otherCont,
                                  const std::vector<size_t> &otherIndexMap )
{
  {
    xAOD::CutBookkeeperContainer::iterator matchIter = 
      std::find(contToUpdate.begin(), contToUpdate.end(), old);
    if (matchIter != contToUpdate.end()) {
      return *matchIter;
    }
  }

  {
    xAOD::CutBookkeeperContainer::const_iterator matchIter = 
      std::find(otherCont.begin(), otherCont.end(), old);
    if (matchIter != contToUpdate.end()) {
      std::size_t pos = matchIter - otherCont.begin();
      return contToUpdate[otherIndexMap[pos]];
    }
  }

  // If we didn't find it, we need to add it
  xAOD::CutBookkeeper *newEBK = new xAOD::CutBookkeeper(); // will be owned by the container
  if (newEBK->usingPrivateStore()) { newEBK->releasePrivateStore(); }
  newEBK->makePrivateStore(old);
  contToUpdate.push_back(newEBK);
  return newEBK;
}

} // unnamed namespace


/// Helper function to update a container with information from another one
void xAOD::CutFlowHelpers::updateContainer( xAOD::CutBookkeeperContainer *contToUpdate,
                                            const xAOD::CutBookkeeperContainer *otherCont ) 
{
  std::size_t origSize = contToUpdate->size();

  // Vector of indices in contToUpdate of elements in otherCont.
  std::vector<std::size_t> otherIndexMap(otherCont->size());
  // Loop through otherCont.
  // If element already in contToUpdate, update event counts, otherwise create new element
  for (std::size_t i = 0; i < otherCont->size(); ++i) {
    const xAOD::CutBookkeeper *otherEBK = otherCont->at(i);

    // Loop through the container to be updated (contToUpdate) and see if we find a match
    bool foundEBKToUpdate{false};
    for (std::size_t j = 0; j < contToUpdate->size(); ++j) {
      xAOD::CutBookkeeper *ebkToUpdate = contToUpdate->at(j);
      // Check if they are identical, if so, update; else add otherEBK
      if (otherEBK->isEqualTo(ebkToUpdate)) {
        ebkToUpdate->setPayload(ebkToUpdate->payload() + otherEBK->payload());
        otherIndexMap[i] = j;
        foundEBKToUpdate = true;
        break;
      }
    } // End: Inner loop over contToUpdate
    if (!foundEBKToUpdate) {
      xAOD::CutBookkeeper *newEBK = new xAOD::CutBookkeeper(); // will be owned by the container
      if (newEBK->usingPrivateStore()) { newEBK->releasePrivateStore(); }
      newEBK->makePrivateStore(otherEBK);
      contToUpdate->push_back(newEBK);
      std::size_t ebIdx = newEBK->index();
      otherIndexMap[i] = ebIdx;
    }
  } // End: Outer loop over contToUpdate

  // Now, we still need to fix the cross-referencing of the newly added CutBookkkeepers
  for (std::size_t i = origSize; i < contToUpdate->size(); ++i) {
    xAOD::CutBookkeeper *ebkToModify = contToUpdate->at(i);

    // Parent check
    if (ebkToModify->hasParent()) {
      const xAOD::CutBookkeeper *oldParent = ebkToModify->parent();
      const xAOD::CutBookkeeper *newParent = resolveLink (oldParent,
                                                          *contToUpdate,
                                                          *otherCont,
                                                          otherIndexMap);
      ebkToModify->setParent (newParent);
    } // Done fixing parent

    // Children check
    std::vector<xAOD::CutBookkeeper *> newChildren;
    for ( std::size_t oldIdx = 0; oldIdx < ebkToModify->nChildren(); ++oldIdx) {
      const xAOD::CutBookkeeper *oldEBK = ebkToModify->child(oldIdx);
      newChildren.push_back(resolveLink(oldEBK,
                                        *contToUpdate,
                                        *otherCont,
                                        otherIndexMap));
    } // Done fixing children
    ebkToModify->setChildren(newChildren);

    // Used others check
    std::vector<xAOD::CutBookkeeper *> newOthers;
    for (std::size_t oldIdx = 0; oldIdx < ebkToModify->nUsedOthers(); ++oldIdx) {
      const xAOD::CutBookkeeper *oldEBK = ebkToModify->usedOther(oldIdx);
      newOthers.push_back(resolveLink(oldEBK,
                                      *contToUpdate,
                                      *otherCont,
                                      otherIndexMap));
    } // Done fixing used others
    ebkToModify->setUsedOthers(newOthers);

    // Siblings check
    std::vector<xAOD::CutBookkeeper*> newSiblings;
    for (std::size_t oldIdx = 0; oldIdx < ebkToModify->nSiblings(); ++oldIdx) {
      const xAOD::CutBookkeeper *oldEBK = ebkToModify->sibling(oldIdx);
      newSiblings.push_back(resolveLink(oldEBK,
                                        *contToUpdate,
                                        *otherCont,
                                        otherIndexMap));
    } // Done fixing siblings
    ebkToModify->setSiblings(newSiblings);
  } // Done fixing all cross references
}
