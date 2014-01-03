/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoObjectDecisionFillerTool.cxx 448344 2011-07-12 15:20:20Z kanno $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TrigObjectMatching/TrigMuonEFInfoDistanceFunctor.h"

// Local include(s):
#include "TrigMuonEFInfoObjectDecisionFillerTool.h"

namespace D3PD {

   TrigMuonEFInfoObjectDecisionFillerTool::TrigMuonEFInfoObjectDecisionFillerTool(
      const std::string& type,
      const std::string& name,
      const IInterface* parent )
      : TriggerObjectDecisionFillerTool< TrigMuonEFInfo >( type, name, parent,
                                                           new TrigMuonEFInfoDistanceFunctorBoth() ) {

   }

   /**
    * We have to re-implement this function, because TrigMatchTool (which is used
    * by the base class) has to be instructed to work in a specific way for this tool
    * to give meaningful results.
    *
    * By default TrigMatchTool would apply a sort of "dR" matching between
    * TrigMuonEFInfo objects to decide which one of them fired which chain.
    * But this would then tag objects reconstructed by TrigMuonEF with the MuGirl
    * chains and vice versa, because these objects are usually very close to each
    * other.
    * So we have to instruct TrigMatchTool to use a special matching for TrigMuonEFInfo
    * objects, which basically just involves comparing the pointers pointing to the
    * objects.
    *
    * Use of TrigMatch::useAnyTrack failes, since we fave FullScan chain which can
    * be flagged true by any single mun chain and also contains another muons, which
    * leads to mis-calculation of efficiency in Tag&Probe study.
    *
    * @param muon The TrigMuonEFInfo object for which we want to fill the decision
    * @returns A StatusCode describing the outcome of this function
    */
   StatusCode TrigMuonEFInfoObjectDecisionFillerTool::fill( const TrigMuonEFInfo& muon ) {

      // Remember what kind of track was used for the matching so far:
      int oldTrackType = TrigMatch::TrigMuonEFInfoHelper::trackToUse();

      // Since an issue was found for using TrigMatch::useAnyTrack as track type,
      // it is reverted to use TrigMatch::useObjectPointer.
      TrigMatch::TrigMuonEFInfoHelper::setTrackToUse( TrigMatch::useObjectPointer );

      // Fill the variables using the base class:
      CHECK( TriggerObjectDecisionFillerTool< TrigMuonEFInfo >::fill( muon ) );

      // Reset the matching tool:
      TrigMatch::TrigMuonEFInfoHelper::setTrackToUse( oldTrackType );

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
