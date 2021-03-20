/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAuxAssociationMapCnv_v1.cxx 693797 2015-09-08 22:06:19Z khoo $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/versions/MissingETAssociationMap_v1.h"

// Local include(s):
#include "xAODMissingETAuxAssociationMapCnv_v1.h"

#include <utility>
#include <algorithm>

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::VERBOSE

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

using namespace MissingETBase;
using namespace MissingETBase::Types;

xAODMissingETAuxAssociationMapCnv_v1::xAODMissingETAuxAssociationMapCnv_v1()
{
}

bool maskSumptSort(const std::pair<bitmask_t,constvec_t>& p1, const std::pair<bitmask_t,constvec_t>& p2) {
  if(MissingETBase::Numerical::isEqual(p1.second.sumpt(),p2.second.sumpt())) {
    return p1.first>p2.first;
  }
  return p1.second.sumpt()<p2.second.sumpt();
}

void xAODMissingETAuxAssociationMapCnv_v1::
persToTrans( const xAOD::MissingETAuxAssociationMap_v1* oldObj,
             xAOD::MissingETAuxAssociationMap* newObj,
             MsgStream& log ) const {

   // Greet the user:
   ATH_MSG( "Converting xAOD::MissingETAuxAssociationMap_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   // Set up interface containers on top of them:

   //The old  uses v_
   xAOD::MissingETAssociationMap_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
     oldInt.push_back( new xAOD::MissingETAssociation_v1() );
   }
   oldInt.setStore( oldObj );
   
   xAOD::MissingETAssociationMap newInt;
   for( size_t i = 0; i < newObj->size(); ++i ) {
     newInt.push_back( new xAOD::MissingETAssociation() );
   }
   newInt.setStore( newObj );

   unsigned char calOverlap(11), trkOverlap(25);
   std::map<bitmask_t,constvec_t> calmap, trkmap;
   std::vector<std::pair<bitmask_t,constvec_t> > sortedCalPairs, sortedTrkPairs;
   sortedCalPairs.reserve(10);
   sortedTrkPairs.reserve(10);
   for( size_t iAssoc=0; iAssoc < oldInt.size(); ++iAssoc ) {
     const xAOD::MissingETAssociation* assoc = oldInt[iAssoc];
     ATH_MSG("On assoc " << iAssoc << ", size = " << assoc->size() );

     //////////////////////////////////////////////////////////////////////////////////////////
     // Set up maps
     calmap.clear();
     trkmap.clear();
     sortedCalPairs.clear();
     sortedTrkPairs.clear();

     for(size_t iObj=0; iObj<assoc->size(); ++iObj) {
       constvec_t calvec = assoc->calVec(iObj);
       constvec_t trkvec = assoc->trkVec(iObj);
       bitmask_t calmask = 1<<iObj;
       bitmask_t trkmask = 1<<iObj;
       ATH_MSG("  Obj " << iObj);
       // Build up masks identifying overlaps
       for(size_t iOverlap=0; iOverlap<assoc->overlapIndices(iObj).size(); ++iOverlap) {
	 size_t overlapIndex = assoc->overlapIndices(iObj)[iOverlap];
	 size_t overlapType = assoc->overlapTypes(iObj)[iOverlap];
	 ATH_MSG("    Overlap " << iOverlap << " (" << overlapIndex << ", " << overlapType << ")" );
	 if( (calOverlap&overlapType) && calvec.sumpt() <= assoc->calVec(overlapIndex).sumpt() ) {
	   ATH_MSG("    cal overlap " << overlapIndex);
	   calmask |= 1<<overlapIndex;
	 }
	 if( (trkOverlap&overlapType) && trkvec.sumpt() <= assoc->trkVec(overlapIndex).sumpt() ) {
	   ATH_MSG("    trk overlap " << overlapIndex);
	   trkmask |= 1<<overlapIndex;
	 }
       }
       ATH_MSG("    calmask = " << calmask << ", trkmask = " << trkmask);
       // Ensure uniqueness
       if(calmap[calmask].sumpt()==0 && calvec.sumpt()>1e-9) {
	 calmap[calmask] = calvec;
	 sortedCalPairs.emplace_back(calmask,calvec);
	 ATH_MSG("    Add unique cal mask " << calmask << ", sumpt " << calvec.sumpt());
       }
       if(trkmap[trkmask].sumpt()==0 && trkvec.sumpt()>1e-9) {
	 trkmap[trkmask] = trkvec;
	 sortedTrkPairs.emplace_back(trkmask,trkvec);
	 ATH_MSG("    Add unique trk mask " << trkmask << ", sumpt " << trkvec.sumpt());
       }
     }

     // Sort in ascending sumpT then by increasing mask value
     // Probably redundant as identical values should have been overlap-removed
     std::sort(sortedCalPairs.begin(),sortedCalPairs.end(),maskSumptSort);
     size_t iPair(0);
     for(auto& calpair : sortedCalPairs) {
       bitmask_t calmask = calpair.first;
       double sumpt = calpair.second.sumpt();
       ATH_MSG("  Do subtractions on calmask " << calmask << " with sumpt " << sumpt);

       for(size_t jPair(iPair+1); jPair<sortedCalPairs.size(); ++jPair) {
	 const auto& calpair2=sortedCalPairs[jPair];
	 ATH_MSG("    Check calmask " << calpair2.first << " with sumpt " << calpair2.second.sumpt());
	 if(calmask > calpair2.first && (calmask & calpair2.first)==calpair2.first) {
	   // subtract from the next most inclusive calmask that overlaps this one
	   calmap[calpair2.first] -= calpair.second;
	   ATH_MSG("  Cal mask " << calmask << ", sumpt " << sumpt
		   << ", subtract from " << calpair2.first
		   << " (" << calpair2.second.sumpt() << " --> " << calmap[calpair2.first].sumpt() << ")");
	   break;
	 }
       }
       ++iPair;
     }

     std::sort(sortedTrkPairs.begin(),sortedTrkPairs.end(),maskSumptSort);
     iPair = 0;
     for(auto& trkpair : sortedTrkPairs) {
       bitmask_t trkmask = trkpair.first;
       double sumpt = trkpair.second.sumpt();
       ATH_MSG("  Do subtractions on trkmask " << trkmask << " with sumpt " << sumpt);

       for(size_t jPair(iPair+1); jPair<sortedTrkPairs.size(); ++jPair) {
	 const auto& trkpair2=sortedTrkPairs[jPair];
	 ATH_MSG("    Check trkmask " << trkpair2.first << " with sumpt " << trkpair2.second.sumpt());
	 if(trkmask > trkpair2.first && (trkmask&trkpair2.first)==trkpair2.first) {
	   // subtract from the next most inclusive trkmask that overlaps this one
	   trkmap[trkpair2.first] -= trkpair.second;
	   ATH_MSG("  Trk mask " << trkmask << ", sumpt " << sumpt
		   << ", subtract from " << trkpair2.first
		   << " (" << trkpair2.second.sumpt() << " --> " << trkmap[trkpair2.first].sumpt() << ")");
	   break;
	 }
       }
       ++iPair;
     }

     xAOD::MissingETAssociation* newassoc = newInt[iAssoc];
     newassoc->clearCalVecs();
     // Convert maps to vector representation
     size_t iKey(0);
     for (const auto& calpair : calmap) {
       bitmask_t bm = calpair.first;
       constvec_t cv = calpair.second;
       if(cv.sumpt()>1e-9) {
	 ATH_MSG("  CalKey " << bm << ", constvec sumpt = " << cv.sumpt());
	 newassoc->addCalVec(bm,cv.cpx(),cv.cpy(),cv.cpz(),cv.ce(),cv.sumpt());
	 ATH_MSG("  retrieve key " << newassoc->calkey(iKey) << ", sumpt " << 
		 newassoc->calVec(iKey).sumpt() );
	 ++iKey;
       }
     }
     ATH_MSG("Final calsize = " << newassoc->sizeCal() << ", " << newassoc->cale().size());

     iKey=0;
     newassoc->clearTrkVecs();
     for (const auto& trkpair : trkmap) {
       bitmask_t bm = trkpair.first;
       constvec_t cv = trkpair.second;
       if(cv.sumpt()>1e-9) {
	 ATH_MSG("  TrkKey " << bm << ", constvec sumpt = " << cv.sumpt());
	 newassoc->addTrkVec(bm,cv.cpx(),cv.cpy(),cv.cpz(),cv.ce(),cv.sumpt());
	 ATH_MSG("  retrieve key " << newassoc->trkkey(iKey) << ", sumpt " << 
		 newassoc->trkVec(iKey).sumpt() );
	 ++iKey;
       }
     }     
     ATH_MSG("Final trksize = " << newassoc->sizeTrk() << ", " << newassoc->trke().size());

   } // Loop over associations

   // Print what happened:
   ATH_MSG( "Converting xAOD::MissingETAuxAssociationMap_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODMissingETAuxAssociationMapCnv_v1::
transToPers( const xAOD::MissingETAuxAssociationMap*,
             xAOD::MissingETAuxAssociationMap_v1*,
             MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODMissingETAuxAssociationMapCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODMissingETAuxAssociationMapCnv_v1::"
                             "transToPers" );

   return;
}
