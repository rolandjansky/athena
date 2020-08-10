/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CutBookkeeperUtils.h"

namespace xAOD::CutBookkeeperUtils
{

xAOD::CutBookkeeper *resolveLink(const xAOD::CutBookkeeper *old,
                                 xAOD::CutBookkeeperContainer &contToUpdate,
                                 const xAOD::CutBookkeeperContainer &otherCont,
                                 const std::vector<size_t> &otherIndexMap)
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

StatusCode updateContainer(xAOD::CutBookkeeperContainer *contToUpdate,
                           const xAOD::CutBookkeeperContainer *otherCont) 
{
  size_t origSize = contToUpdate->size();

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
  return StatusCode::SUCCESS;
}

} // xAOD::CutBookkeeperUtils
