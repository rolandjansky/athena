///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class BookkeeperTool
// Authors: Jack Cranshaw <Jack.Cranshaw@cern.ch>
///////////////////////////////////////////////////////////////////

#include "EventBookkeeperTools/BookkeeperTool.h"

// STL include
#include <algorithm>

#include "AthenaKernel/MetaCont.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/GenericMetadataTool.h"
#include "AthenaKernel/ICutFlowSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthContainersInterfaces/IConstAuxStoreMeta.h"

BookkeeperTool::BookkeeperTool(const std::string& type, 
                               const std::string& name,
                               const IInterface*  parent)
  : GenericMetadataTool <xAOD::CutBookkeeperContainer, xAOD::CutBookkeeperAuxContainer>(type,name,parent)
{
}

BookkeeperTool::~BookkeeperTool()
{
}


namespace {

xAOD::CutBookkeeper*
resolveLink (const xAOD::CutBookkeeper* old,
             xAOD::CutBookkeeperContainer& contToUpdate,
             const xAOD::CutBookkeeperContainer& otherCont,
             const std::vector<size_t>& otherIndices)
{
  {
    xAOD::CutBookkeeperContainer::iterator matchIter = 
      std::find( contToUpdate.begin(),
                 contToUpdate.end(),
                 old );
    if (matchIter != contToUpdate.end())
      return *matchIter;
  }

  {
    xAOD::CutBookkeeperContainer::const_iterator matchIter = 
      std::find( otherCont.begin(),
                 otherCont.end(),
                 old );
    if (matchIter != contToUpdate.end()) {
      size_t pos = matchIter - otherCont.begin();
      return contToUpdate[otherIndices[pos]];
    }
  }

  // If we didn't find it, we need to add it
  xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
  if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
  newEBK->makePrivateStore(old);
  contToUpdate.push_back( newEBK );
  return newEBK;
}


} // anonymous namespace

//
// (Merge) method required by base clase GenericMetdataTool
//   Note that the implementation of the IMetaDataTool interface 
//   is done in GenericMetadataTool and configured by properties 
//   of that class
StatusCode
BookkeeperTool::updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                             const xAOD::CutBookkeeperContainer* otherCont ) 
{
  ATH_MSG_DEBUG("calling updateContainer(...)" );
  ATH_MSG_VERBOSE("Have container to update with size=" << contToUpdate->size()
                  << ", and other container with size=" << otherCont->size() );

  size_t origSize = contToUpdate->size();

  // Vector of indices in contToUpdate of elements in otherCont.
  std::vector< std::size_t > otherIndices (otherCont->size());
  // Loop through otherCont.
  // If element already in contToUpdate, update event counts, otherwise create new element
  for ( std::size_t i=0; i<otherCont->size(); ++i ) {
    const xAOD::CutBookkeeper* otherEBK = otherCont->at(i);
    ATH_MSG_VERBOSE("Looping through otherCont at index " << i);
    ATH_MSG_VERBOSE("Have otherEBK with: name=" << otherEBK->name()
                    << ", cycle=" << otherEBK->cycle()
                    << ", nAcceptedEvents=" << otherEBK->nAcceptedEvents()
                    << ", inputStream=" << otherEBK->inputStream() );


    // Loop through the container to be updated (contToUpdate) and see if we find a match
    bool foundEBKToUpdate(false);
    for ( std::size_t j=0; j<contToUpdate->size(); ++j ) {
      xAOD::CutBookkeeper* ebkToUpdate = contToUpdate->at(j);
      // Check if they are identical, if so, update; else add otherEBK
      if ( otherEBK->isEqualTo(ebkToUpdate) ) {
        ebkToUpdate->setPayload( ebkToUpdate->payload() + otherEBK->payload() );
        otherIndices[i] = j;
        foundEBKToUpdate = true;
        break;
      }
      else {
        ATH_MSG_INFO("otherEBK("<<otherEBK->name()<<") != ebkToUpdate("<<ebkToUpdate->name());
      }
    } // End: Inner loop over contToUpdate
    if (!foundEBKToUpdate) {
      xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
      if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
      newEBK->makePrivateStore(otherEBK);
      contToUpdate->push_back( newEBK );
      std::size_t ebIdx = newEBK->index();
      otherIndices[i] = ebIdx;
    }
  } // End: Outer loop over contToUpdate

  // Now, we still need to fix the cross-referencing of the newly added CutBookkkeepers
  for ( std::size_t i=origSize; i<contToUpdate->size(); ++i ) {
    xAOD::CutBookkeeper* ebkToModify = contToUpdate->at(i);

    // Parent check
    if ( ebkToModify->hasParent() ) {
      const xAOD::CutBookkeeper* oldParent = ebkToModify->parent();
      const xAOD::CutBookkeeper* newParent = resolveLink (oldParent,
                                                          *contToUpdate,
                                                          *otherCont,
                                                          otherIndices);
      ebkToModify->setParent (newParent);
    } // Done fixing parent

    // Children check
    std::vector< xAOD::CutBookkeeper* > newChildren;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nChildren(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->child(oldIdx);
      newChildren.push_back (resolveLink (oldEBK,
                                          *contToUpdate,
                                          *otherCont,
                                          otherIndices));
    } // Done fixing children
    ebkToModify->setChildren (newChildren);

    // Used others check
    std::vector< xAOD::CutBookkeeper* > newOthers;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nUsedOthers(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->usedOther(oldIdx);
      newOthers.push_back (resolveLink (oldEBK,
                                        *contToUpdate,
                                        *otherCont,
                                        otherIndices));
    } // Done fixing used others
    ebkToModify->setUsedOthers (newOthers);

    // Siblings check
    std::vector< xAOD::CutBookkeeper* > newSiblings;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nSiblings(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->sibling(oldIdx);
      newSiblings.push_back (resolveLink (oldEBK,
                                          *contToUpdate,
                                          *otherCont,
                                          otherIndices));
    } // Done fixing siblings
    ebkToModify->setSiblings (newSiblings);
  } // Done fixing all cross references
  return StatusCode::SUCCESS;
}

