/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Implementation file for class CutBookkeeperContainer_v1
// Author: Karsten Koeneke, May 2014 <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

#include "xAODCutFlow/versions/CutBookkeeperContainer_v1.h"

namespace xAOD {


  ////////////////
  // Constructors
  ////////////////

  // CutBookkeeperContainer_v1::CutBookkeeperContainer_v1()
  //   : DataVector<CutBookkeeper_v1> (SG::OWN_ELEMENTS)
  // { }


  /// Constructor
  CutBookkeeperContainer_v1::CutBookkeeperContainer_v1( SG::OwnershipPolicy ownPolicy,
                                                        SG::IndexTrackingPolicy trackIndices )
    : DataVector<xAOD::CutBookkeeper_v1>(ownPolicy,trackIndices)
  { }

  /// Constructor
  CutBookkeeperContainer_v1::CutBookkeeperContainer_v1( CutBookkeeperContainer_v1::iterator first,
                                                        CutBookkeeperContainer_v1::iterator last,
                                                        SG::OwnershipPolicy ownPolicy,
                                                        SG::IndexTrackingPolicy trackIndices )
    : DataVector<xAOD::CutBookkeeper_v1>(first,last,ownPolicy,trackIndices)
  { }


  /// Destructor
  CutBookkeeperContainer_v1::~CutBookkeeperContainer_v1()
  { }



  // CutBookkeeperContainer_v1::CutBookkeeperContainer_v1( const CutBookkeeperContainer_v1& rhs ) :
  //   DataVector<CutBookkeeper_v1> (rhs)
  // { }
  //
  //
  // CutBookkeeperContainer_v1&
  // CutBookkeeperContainer_v1::operator=( const CutBookkeeperContainer_v1& rhs ) {
  //   if ( this != &rhs ) {
  //     DataVector<CutBookkeeper_v1>::operator=(rhs);
  //   }
  //   return *this;
  // }
  //
  //
  // CutBookkeeperContainer_v1::CutBookkeeperContainer_v1( const SG::OwnershipPolicy own ) :
  //   DataVector<CutBookkeeper_v1>(own)
  // { }

  // Merge the content of another xAOD::CutBookkeeperContainer into this one
  void CutBookkeeperContainer_v1::merge( const CutBookkeeperContainer_v1* otherCont )
  {
    // Create an vector of indices of all the newly transferred CutBookkeepers
    std::vector< std::size_t > newEBKIndices;
    // Loop through otherCont.
    // If element already in this container, update event counts, otherwise create new element
    for ( std::size_t i=0; i<otherCont->size(); ++i ){
      const xAOD::CutBookkeeper_v1* otherEBK = otherCont->at(i);

      // Loop through this container and see if we find a match
      bool foundEBKToUpdate(false);
      for ( std::size_t j=0; j<this->size(); ++j ) {
        xAOD::CutBookkeeper_v1* ebkToUpdate = this->at(j);
        // Check if they are identical, if so, update; else add otherEBK
        if ( otherEBK->isEqualTo(ebkToUpdate) ) {
          ebkToUpdate->setPayload( ebkToUpdate->payload() + otherEBK->payload() );
          foundEBKToUpdate = true;
          break;
        }
      } // End: Inner loop over contToUpdate
      if (!foundEBKToUpdate) {
        xAOD::CutBookkeeper_v1* newEBK = new xAOD::CutBookkeeper_v1();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(otherEBK);
        this->push_back( newEBK );
        std::size_t ebIdx = newEBK->index();
        newEBKIndices.push_back(ebIdx);
      }
    } // End: Outer loop over contToUpdate

    // Now, we still need to fix the cross-referencing of the newly added CutBookkkeepers
    for ( std::size_t i=0; i<newEBKIndices.size(); ++i ) {
      std::size_t otherIndex = newEBKIndices.at(i);
      xAOD::CutBookkeeper_v1* ebkToModify = this->at(otherIndex);

      // Parent check
      if ( ebkToModify->hasParent() ) {
        const xAOD::CutBookkeeper_v1* oldParent = ebkToModify->parent();
        xAOD::CutBookkeeperContainer_v1::iterator matchIter =  std::find( this->begin(),
                                                                          this->end(),
                                                                          oldParent );
        // If we found it, we can modify
        if ( matchIter != this->end() ) {
          ebkToModify->setParent( *matchIter );
        }
        // If we didn't find it, we need to add it
        else {
          xAOD::CutBookkeeper_v1* newEBK = new xAOD::CutBookkeeper_v1();
          if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
          newEBK->makePrivateStore(oldParent);
          this->push_back( newEBK );
          ebkToModify->setParent( newEBK );
        }
      } // Done fixing parent

      // Children check
      for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nChildren(); ++oldIdx ) {
        const xAOD::CutBookkeeper_v1* oldEBK = ebkToModify->child(oldIdx);
        xAOD::CutBookkeeperContainer_v1::iterator matchIter =  std::find( this->begin(),
                                                                          this->end(),
                                                                          oldEBK );
        // If we found it, we can modify
        if ( matchIter != this->end() ) {
          ebkToModify->addChild( *matchIter );
        }
        // If we didn't find it, we need to add it
        else {
          xAOD::CutBookkeeper_v1* newEBK = new xAOD::CutBookkeeper_v1();
          if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
          newEBK->makePrivateStore(oldEBK);
          this->push_back( newEBK );
          ebkToModify->addChild( newEBK );
        }
      } // Done fixing children

      // Used others check
      for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nUsedOthers(); ++oldIdx ) {
        const xAOD::CutBookkeeper_v1* oldEBK = ebkToModify->usedOther(oldIdx);
        xAOD::CutBookkeeperContainer_v1::iterator matchIter =  std::find( this->begin(),
                                                                          this->end(),
                                                                          oldEBK );
        // If we found it, we can modify
        if ( matchIter != this->end() ) {
          ebkToModify->addUsedOther( *matchIter );
        }
        // If we didn't find it, we need to add it
        else {
          xAOD::CutBookkeeper_v1* newEBK = new xAOD::CutBookkeeper_v1();
          if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
          newEBK->makePrivateStore(oldEBK);
          this->push_back( newEBK );
          ebkToModify->addUsedOther( newEBK );
        }
      } // Done fixing used others

      // Siblings check
      for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nSiblings(); ++oldIdx ) {
        const xAOD::CutBookkeeper_v1* oldEBK = ebkToModify->sibling(oldIdx);
        xAOD::CutBookkeeperContainer_v1::iterator matchIter =  std::find( this->begin(),
                                                                          this->end(),
                                                                          oldEBK );
        // If we found it, we can modify
        if ( matchIter != this->end() ) {
          ebkToModify->addSibling( *matchIter );
        }
        // If we didn't find it, we need to add it
        else {
          xAOD::CutBookkeeper_v1* newEBK = new xAOD::CutBookkeeper_v1();
          if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
          newEBK->makePrivateStore(oldEBK);
          this->push_back( newEBK );
          ebkToModify->addSibling( newEBK );
        }
      } // Done fixing siblings
    } // Done fixing all cross references
    return;
  }


  // Get the minimum cycle number of any xAOD::CutBookkeepers in the container
  int CutBookkeeperContainer_v1::minCycle() const
  {
    // Now, iterate over all CutBookkeepers and search for the lowest cycle number
    int minCycle=9999999;
    for ( std::size_t i=0; i<this->size(); ++i ) {
      // Get the current old EBK
      const xAOD::CutBookkeeper_v1* cbk = this->at(i);
      int inCycle = cbk->cycle();
      if (inCycle < minCycle) minCycle = inCycle;
    }
    if (minCycle==9999999){ minCycle = 0; }
    return minCycle;
  }


  // Get the maximum cycle number of any xAOD::CutBookkeepers in the container
  int CutBookkeeperContainer_v1::maxCycle() const
  {
    // Now, iterate over all CutBookkeepers and search for the highest cycle number
    int maxCycle=0;
    for ( std::size_t i=0; i<this->size(); ++i ) {
      // Get the current old EBK
      const xAOD::CutBookkeeper_v1* cbk = this->at(i);
      int inCycle = cbk->cycle();
      if (inCycle > maxCycle) maxCycle = inCycle;
    }
    return maxCycle;
  }

} // namespace xAOD
