// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//             Code produced by D3PDMakerReader
//
//  author: Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
// -------------------------------------------------------------

#include <TPRegexp.h>
#include <TList.h>
#include <TDataMember.h>

#include "../TrigCostD3PD/TrigCostD3PDObject.h"

ClassImp( D3PDReader::TrigCostD3PDObject )

namespace D3PDReader {

   /**
    * This constructor should be used when the object will be used to read
    * variables from an existing ntuple. The object will also be able to
    * output variables, but it will also need to read them from somewhere.
    *
    * @param master Reference to the variable holding the current event number
    * @param prefix Prefix of the variables in the D3PD
    */
   TrigCostD3PDObject::TrigCostD3PDObject( const ::Long64_t& master, const char* prefix )
      : UserD3PDObject( master, prefix ),
        fHandles(),
        fFromInput( kTRUE ),
        fPrefix( prefix ) {

      SetVarHandles( &master );
   }

   /**
    * This constructor can be used when the object will only have to output
    * (and temporarily store) new information into an output ntuple. For
    * instance when one wants to create a selected/modified list of information.
    *
    * @param prefix Prefix of the variables in the D3PD
    */
   TrigCostD3PDObject::TrigCostD3PDObject( const char* prefix )
      : UserD3PDObject( prefix ),
        fHandles(),
        fFromInput( kFALSE ),
        fPrefix( prefix ) {

      SetVarHandles( 0 );
   }

   /**
    * @returns The branch name prefix used by the object
    */
   const char* TrigCostD3PDObject::GetPrefix() const {

      return fPrefix;
   }

   /**
    * @param prefix The prefix that should be used for the variables
    */
   void TrigCostD3PDObject::SetPrefix( const char* prefix ) {

      // Call the base class's function:
      UserD3PDObject::SetPrefix( prefix );

      // Remember the prefix:
      fPrefix = prefix;

      // Set all the variable names:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         itr->second->SetName( ::TString( prefix ) + itr->first );
      }

      return;
   }

   /**
    * This function should be called every time a new TFile is opened
    * by your analysis code.
    *
    * @param tree Pointer to the TTree with the variables
    */
   void TrigCostD3PDObject::ReadFrom( TTree* tree ) {

      // Check if the object will be able to read from the TTree:
      if( ! fFromInput ) {
         Error( "ReadFrom", "The object was not created with the correct" );
         Error( "ReadFrom", "constructor to read data from a D3PD!" );
         return;
      }

      // Call the base class's function:
      UserD3PDObject::ReadFrom( tree );

      // Call ReadFrom(...) on all the variables:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         itr->second->ReadFrom( tree );
      }

      return;
   }

   /**
    * This function can be called to connect the active variables of the object
    * to an output TTree. It can be called multiple times, then the variables
    * will be written to multiple TTrees.
    *
    * @param tree Pointer to the TTree where the variables should be written
    */
   void TrigCostD3PDObject::WriteTo( TTree* tree ) {

      // Call the base class's function:
      UserD3PDObject::WriteTo( tree );

      // Call WriteTo(...) on all the variables:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         itr->second->WriteTo( tree );
      }

      return;
   }

   /**
    * This is a convenience function for turning the branches active or
    * inactive conveniently. If the parameter is set to <code>kTRUE</code>
    * then the branches available from the input which match the given
    * pattern are turned active.
    * When it's set to <code>kFALSE</code> then all the variables matching
    * the pattern are turned inactive.
    *
    * @param active Flag behaving as explained above
    * @param pattern Regular expression specifying which branches to modify
    */
   void TrigCostD3PDObject::SetActive( ::Bool_t active, const ::TString& pattern ) {

      // Call the base class's function:
      UserD3PDObject::SetActive( active, pattern );

      ::TPRegexp re( pattern );

      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         if( ! re.Match( fPrefix + itr->first ) ) continue;
         if( active ) {
            if( itr->second->IsAvailable() ) itr->second->SetActive( active );
         } else {
            itr->second->SetActive( active );
         }
      }

      return;
   }

   /**
    * This function can be used to read in all the branches from the input
    * TTree which are set active for writing out. This can simplify writing
    * event selector codes immensely. Remember to set the desired variable
    * active before calling this function.
    */
   void TrigCostD3PDObject::ReadAllActive() {

      // Check if it makes sense to call this function:
      if( ! fFromInput ) {
         static ::Bool_t wPrinted = kFALSE;
         if( ! wPrinted ) {
            Warning( "ReadAllActive", "Function only meaningful when used on objects" );
            Warning( "ReadAllActive", "which are used to read information from a D3PD" );
            wPrinted = kTRUE;
         }
      }

      // Call the base class's function:
      UserD3PDObject::ReadAllActive();

      // Read in the current entry for each active variable:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         if( ! itr->second->IsActive() ) continue;
         itr->second->ReadCurrentEntry();
      }

      return;
   }

   /**
    * This function can be used to get information about the access
    * pattern/statistics of the job. It should be called at the end of
    * an analysis job to get the information about the performance of the
    * analysis.
    *
    * @returns An object describing the D3PD access statistics
    */
   // D3PDReadStats TrigCostD3PDObject::GetStatistics() const {

   //    // The result object:
   //    D3PDReadStats result = UserD3PDObject::GetStatistics();

   //    // Add the statistics from each variable to the result:
   //    std::map< ::TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
   //    std::map< ::TString, VarHandleBase* >::const_iterator end = fHandles.end();
   //    for( ; itr != end; ++itr ) {
   //       result.AddVariable( itr->second->GetStatistics() );
   //    }

   //    return result;
   // }

   /**
    * This function can be used to copy the contents of the entire object
    * for a given event. This can be useful for instance when the user
    * wants to copy all information to an output file, and modify it a bit,
    * and only then write it out.
    *
    * @param parent The object to copy the information from
    * @returns This same object, for convenience reasons
    */
   TrigCostD3PDObject& TrigCostD3PDObject::Set( const TrigCostD3PDObject& parent ) {

      // Check if this function can be used on the object:
      if( fFromInput ) {
         Error( "Set", "Objects used for reading a D3PD can't be modified!" );
         return *this;
      }

      // Call the base class's function:
      UserD3PDObject::Set( parent );

      if( parent.appId.IsAvailable() && appId.IsActive() ) {
         appId() = parent.appId();
      } else {
         appId() = 0;
      }
      if( parent.ebWeight.IsAvailable() && ebWeight.IsActive() ) {
         ebWeight() = parent.ebWeight();
      } else {
         ebWeight() = 0;
      }
      if( parent.ebWeightBG.IsAvailable() && ebWeightBG.IsActive() ) {
         ebWeightBG() = parent.ebWeightBG();
      } else {
         ebWeightBG() = 0;
      }
      if( parent.ebUnbiased.IsAvailable() && ebUnbiased.IsActive() ) {
         ebUnbiased() = parent.ebUnbiased();
      } else {
         ebUnbiased() = 0;
      }
      if( parent.ranScaleTools.IsAvailable() && ranScaleTools.IsActive() ) {
         ranScaleTools() = parent.ranScaleTools();
      } else {
         ranScaleTools() = 0;
      }
      if( parent.bunchCrossingId.IsAvailable() && bunchCrossingId.IsActive() ) {
         bunchCrossingId() = parent.bunchCrossingId();
      } else {
         bunchCrossingId() = 0;
      }
      if( parent.chain_counter.IsAvailable() && chain_counter.IsActive() ) {
         *( chain_counter() ) = *( parent.chain_counter() );
      } else {
         chain_counter()->clear();
      }
      if( parent.chain_isExpressStream.IsAvailable() && chain_isExpressStream.IsActive() ) {
         *( chain_isExpressStream() ) = *( parent.chain_isExpressStream() );
      } else {
         chain_isExpressStream()->clear();
      }
      if( parent.chain_isPassed.IsAvailable() && chain_isPassed.IsActive() ) {
         *( chain_isPassed() ) = *( parent.chain_isPassed() );
      } else {
         chain_isPassed()->clear();
      }
      if( parent.chain_isPassedRaw.IsAvailable() && chain_isPassedRaw.IsActive() ) {
         *( chain_isPassedRaw() ) = *( parent.chain_isPassedRaw() );
      } else {
         chain_isPassedRaw()->clear();
      }
      if( parent.chain_isPassthrough.IsAvailable() && chain_isPassthrough.IsActive() ) {
         *( chain_isPassthrough() ) = *( parent.chain_isPassthrough() );
      } else {
         chain_isPassthrough()->clear();
      }
      if( parent.chain_isPrescaled.IsAvailable() && chain_isPrescaled.IsActive() ) {
         *( chain_isPrescaled() ) = *( parent.chain_isPrescaled() );
      } else {
         chain_isPrescaled()->clear();
      }
      if( parent.chain_isResurrected.IsAvailable() && chain_isResurrected.IsActive() ) {
         *( chain_isResurrected() ) = *( parent.chain_isResurrected() );
      } else {
         chain_isResurrected()->clear();
      }
      if( parent.chain_level.IsAvailable() && chain_level.IsActive() ) {
         *( chain_level() ) = *( parent.chain_level() );
      } else {
         chain_level()->clear();
      }
      if( parent.chain_n.IsAvailable() && chain_n.IsActive() ) {
         chain_n() = parent.chain_n();
      } else {
         chain_n() = 0;
      }
      if( parent.chain_timer.IsAvailable() && chain_timer.IsActive() ) {
         *( chain_timer() ) = *( parent.chain_timer() );
      } else {
         chain_timer()->clear();
      }
      if( parent.chain_wasL1AfterPrescale.IsAvailable() && chain_wasL1AfterPrescale.IsActive() ) {
         *( chain_wasL1AfterPrescale() ) = *( parent.chain_wasL1AfterPrescale() );
      } else {
         chain_wasL1AfterPrescale()->clear();
      }
      if( parent.chain_wasL1AfterVeto.IsAvailable() && chain_wasL1AfterVeto.IsActive() ) {
         *( chain_wasL1AfterVeto() ) = *( parent.chain_wasL1AfterVeto() );
      } else {
         chain_wasL1AfterVeto()->clear();
      }
      if( parent.chain_wasL1BeforePrescale.IsAvailable() && chain_wasL1BeforePrescale.IsActive() ) {
         *( chain_wasL1BeforePrescale() ) = *( parent.chain_wasL1BeforePrescale() );
      } else {
         chain_wasL1BeforePrescale()->clear();
      }
      if( parent.costEvent.IsAvailable() && costEvent.IsActive() ) {
         costEvent() = parent.costEvent();
      } else {
         costEvent() = 0;
      }
      if( parent.costRunNsec.IsAvailable() && costRunNsec.IsActive() ) {
         costRunNsec() = parent.costRunNsec();
      } else {
         costRunNsec() = 0;
      }
      if( parent.costRunSec.IsAvailable() && costRunSec.IsActive() ) {
         costRunSec() = parent.costRunSec();
      } else {
         costRunSec() = 0;
      }
      if( parent.eventNumber.IsAvailable() && eventNumber.IsActive() ) {
         eventNumber() = parent.eventNumber();
      } else {
         eventNumber() = 0;
      }
      if( parent.l1_ctpId.IsAvailable() && l1_ctpId.IsActive() ) {
         *( l1_ctpId() ) = *( parent.l1_ctpId() );
      } else {
         l1_ctpId()->clear();
      }
      if( parent.l1_isPrescaled.IsAvailable() && l1_isPrescaled.IsActive() ) {
         *( l1_isPrescaled() ) = *( parent.l1_isPrescaled() );
      } else {
         l1_isPrescaled()->clear();
      }
      if( parent.l1_isVetoed.IsAvailable() && l1_isVetoed.IsActive() ) {
         *( l1_isVetoed() ) = *( parent.l1_isVetoed() );
      } else {
         l1_isVetoed()->clear();
      }
      if( parent.l1_n.IsAvailable() && l1_n.IsActive() ) {
         l1_n() = parent.l1_n();
      } else {
         l1_n() = 0;
      }
      if( parent.l1_passed.IsAvailable() && l1_passed.IsActive() ) {
         *( l1_passed() ) = *( parent.l1_passed() );
      } else {
         l1_passed()->clear();
      }
      if( parent.l1_passedAfterPrescale.IsAvailable() && l1_passedAfterPrescale.IsActive() ) {
         *( l1_passedAfterPrescale() ) = *( parent.l1_passedAfterPrescale() );
      } else {
         l1_passedAfterPrescale()->clear();
      }
      if( parent.l1_passedAfterVeto.IsAvailable() && l1_passedAfterVeto.IsActive() ) {
         *( l1_passedAfterVeto() ) = *( parent.l1_passedAfterVeto() );
      } else {
         l1_passedAfterVeto()->clear();
      }
      if( parent.l1_passedBeforePrescale.IsAvailable() && l1_passedBeforePrescale.IsActive() ) {
         *( l1_passedBeforePrescale() ) = *( parent.l1_passedBeforePrescale() );
      } else {
         l1_passedBeforePrescale()->clear();
      }
      if( parent.lumi.IsAvailable() && lumi.IsActive() ) {
         lumi() = parent.lumi();
      } else {
         lumi() = 0;
      }
      if( parent.lumiLength.IsAvailable() && lumiLength.IsActive() ) {
         lumiLength() = parent.lumiLength();
      } else {
         lumiLength() = 0;
      }
      if( parent.nanoSeconds.IsAvailable() && nanoSeconds.IsActive() ) {
         nanoSeconds() = parent.nanoSeconds();
      } else {
         nanoSeconds() = 0;
      }
      if( parent.rob_data_id.IsAvailable() && rob_data_id.IsActive() ) {
         *( rob_data_id() ) = *( parent.rob_data_id() );
      } else {
         rob_data_id()->clear();
      }
      if( parent.rob_data_isCached.IsAvailable() && rob_data_isCached.IsActive() ) {
         *( rob_data_isCached() ) = *( parent.rob_data_isCached() );
      } else {
         rob_data_isCached()->clear();
      }
      if( parent.rob_data_isDisabled.IsAvailable() && rob_data_isDisabled.IsActive() ) {
         *( rob_data_isDisabled() ) = *( parent.rob_data_isDisabled() );
      } else {
         rob_data_isDisabled()->clear();
      }
      if( parent.rob_data_isIgnored.IsAvailable() && rob_data_isIgnored.IsActive() ) {
         *( rob_data_isIgnored() ) = *( parent.rob_data_isIgnored() );
      } else {
         rob_data_isIgnored()->clear();
      }
      if( parent.rob_data_isRetrieved.IsAvailable() && rob_data_isRetrieved.IsActive() ) {
         *( rob_data_isRetrieved() ) = *( parent.rob_data_isRetrieved() );
      } else {
         rob_data_isRetrieved()->clear();
      }
      if( parent.rob_data_isStatusOk.IsAvailable() && rob_data_isStatusOk.IsActive() ) {
         *( rob_data_isStatusOk() ) = *( parent.rob_data_isStatusOk() );
      } else {
         rob_data_isStatusOk()->clear();
      }
      if( parent.rob_data_isStatusPrefetched.IsAvailable() && rob_data_isStatusPrefetched.IsActive() ) {
         *( rob_data_isStatusPrefetched() ) = *( parent.rob_data_isStatusPrefetched() );
      } else {
         rob_data_isStatusPrefetched()->clear();
      }
      if( parent.rob_data_isUnclassified.IsAvailable() && rob_data_isUnclassified.IsActive() ) {
         *( rob_data_isUnclassified() ) = *( parent.rob_data_isUnclassified() );
      } else {
         rob_data_isUnclassified()->clear();
      }
      if( parent.rob_data_n.IsAvailable() && rob_data_n.IsActive() ) {
         *( rob_data_n() ) = *( parent.rob_data_n() );
      } else {
         rob_data_n()->clear();
      }
      if( parent.rob_data_size.IsAvailable() && rob_data_size.IsActive() ) {
         *( rob_data_size() ) = *( parent.rob_data_size() );
      } else {
         rob_data_size()->clear();
      }
      if( parent.rob_n.IsAvailable() && rob_n.IsActive() ) {
         rob_n() = parent.rob_n();
      } else {
         rob_n() = 0;
      }
      if( parent.rob_requestorId.IsAvailable() && rob_requestorId.IsActive() ) {
         *( rob_requestorId() ) = *( parent.rob_requestorId() );
      } else {
         rob_requestorId()->clear();
      }
      if( parent.rob_sum_isCached.IsAvailable() && rob_sum_isCached.IsActive() ) {
         *( rob_sum_isCached() ) = *( parent.rob_sum_isCached() );
      } else {
         rob_sum_isCached()->clear();
      }
      if( parent.rob_sum_isDisabled.IsAvailable() && rob_sum_isDisabled.IsActive() ) {
         *( rob_sum_isDisabled() ) = *( parent.rob_sum_isDisabled() );
      } else {
         rob_sum_isDisabled()->clear();
      }
      if( parent.rob_sum_isIgnored.IsAvailable() && rob_sum_isIgnored.IsActive() ) {
         *( rob_sum_isIgnored() ) = *( parent.rob_sum_isIgnored() );
      } else {
         rob_sum_isIgnored()->clear();
      }
      if( parent.rob_sum_isRetrieved.IsAvailable() && rob_sum_isRetrieved.IsActive() ) {
         *( rob_sum_isRetrieved() ) = *( parent.rob_sum_isRetrieved() );
      } else {
         rob_sum_isRetrieved()->clear();
      }
      if( parent.rob_sum_isUnclassified.IsAvailable() && rob_sum_isUnclassified.IsActive() ) {
         *( rob_sum_isUnclassified() ) = *( parent.rob_sum_isUnclassified() );
      } else {
         rob_sum_isUnclassified()->clear();
      }
      if( parent.rob_sum_n.IsAvailable() && rob_sum_n.IsActive() ) {
         *( rob_sum_n() ) = *( parent.rob_sum_n() );
      } else {
         rob_sum_n()->clear();
      }
      if( parent.rob_sum_nRob.IsAvailable() && rob_sum_nRob.IsActive() ) {
         *( rob_sum_nRob() ) = *( parent.rob_sum_nRob() );
      } else {
         rob_sum_nRob()->clear();
      }
      if( parent.rob_sum_size.IsAvailable() && rob_sum_size.IsActive() ) {
         *( rob_sum_size() ) = *( parent.rob_sum_size() );
      } else {
         rob_sum_size()->clear();
      }
      if( parent.rob_sum_subDet.IsAvailable() && rob_sum_subDet.IsActive() ) {
         *( rob_sum_subDet() ) = *( parent.rob_sum_subDet() );
      } else {
         rob_sum_subDet()->clear();
      }
      if( parent.rob_timeStartMicroSec.IsAvailable() && rob_timeStartMicroSec.IsActive() ) {
         *( rob_timeStartMicroSec() ) = *( parent.rob_timeStartMicroSec() );
      } else {
         rob_timeStartMicroSec()->clear();
      }
      if( parent.rob_timeStartSec.IsAvailable() && rob_timeStartSec.IsActive() ) {
         *( rob_timeStartSec() ) = *( parent.rob_timeStartSec() );
      } else {
         rob_timeStartSec()->clear();
      }
      if( parent.rob_timeStopMicroSec.IsAvailable() && rob_timeStopMicroSec.IsActive() ) {
         *( rob_timeStopMicroSec() ) = *( parent.rob_timeStopMicroSec() );
      } else {
         rob_timeStopMicroSec()->clear();
      }
      if( parent.rob_timeStopSec.IsAvailable() && rob_timeStopSec.IsActive() ) {
         *( rob_timeStopSec() ) = *( parent.rob_timeStopSec() );
      } else {
         rob_timeStopSec()->clear();
      }
      if( parent.rob_timer.IsAvailable() && rob_timer.IsActive() ) {
         *( rob_timer() ) = *( parent.rob_timer() );
      } else {
         rob_timer()->clear();
      }
      if( parent.roi_area.IsAvailable() && roi_area.IsActive() ) {
         *( roi_area() ) = *( parent.roi_area() );
      } else {
         roi_area()->clear();
      }
      if( parent.roi_eta.IsAvailable() && roi_eta.IsActive() ) {
         *( roi_eta() ) = *( parent.roi_eta() );
      } else {
         roi_eta()->clear();
      }
      if( parent.roi_id.IsAvailable() && roi_id.IsActive() ) {
         *( roi_id() ) = *( parent.roi_id() );
      } else {
         roi_id()->clear();
      }
      if( parent.roi_isTypeEmTau.IsAvailable() && roi_isTypeEmTau.IsActive() ) {
         *( roi_isTypeEmTau() ) = *( parent.roi_isTypeEmTau() );
      } else {
         roi_isTypeEmTau()->clear();
      }
      if( parent.roi_isTypeEnergy.IsAvailable() && roi_isTypeEnergy.IsActive() ) {
         *( roi_isTypeEnergy() ) = *( parent.roi_isTypeEnergy() );
      } else {
         roi_isTypeEnergy()->clear();
      }
      if( parent.roi_isTypeJet.IsAvailable() && roi_isTypeJet.IsActive() ) {
         *( roi_isTypeJet() ) = *( parent.roi_isTypeJet() );
      } else {
         roi_isTypeJet()->clear();
      }
      if( parent.roi_isTypeJetEt.IsAvailable() && roi_isTypeJetEt.IsActive() ) {
         *( roi_isTypeJetEt() ) = *( parent.roi_isTypeJetEt() );
      } else {
         roi_isTypeJetEt()->clear();
      }
      if( parent.roi_isTau.IsAvailable() && roi_isTau.IsActive() ) {
         *( roi_isTau() ) = *( parent.roi_isTau() );
      } else {
         roi_isTau()->clear();
      }
      if( parent.roi_isTypeMuon.IsAvailable() && roi_isTypeMuon.IsActive() ) {
         *( roi_isTypeMuon() ) = *( parent.roi_isTypeMuon() );
      }
       else {
         roi_isTypeMuon()->clear();
      }
      if( parent.roi_isTypeNone.IsAvailable() && roi_isTypeNone.IsActive() ) {
         *( roi_isTypeNone() ) = *( parent.roi_isTypeNone() );
      } else {
         roi_isTypeNone()->clear();
      }
      if( parent.roi_n.IsAvailable() && roi_n.IsActive() ) {
         roi_n() = parent.roi_n();
      } else {
         roi_n() = 0;
      }
      if( parent.roi_nL1Thresholds.IsAvailable() && roi_nL1Thresholds.IsActive() ) {
         *( roi_nL1Thresholds() ) = *( parent.roi_nL1Thresholds() );
      } else {
         roi_nL1Thresholds()->clear();
      }
      if( parent.roi_phi.IsAvailable() && roi_phi.IsActive() ) {
         *( roi_phi() ) = *( parent.roi_phi() );
      } else {
         roi_phi()->clear();
      }
      if( parent.roi_et.IsAvailable() && roi_et.IsActive() ) {
         *( roi_et() ) = *( parent.roi_et() );
      } else {
         roi_et()->clear();
      }       
      if( parent.roi_etLarge.IsAvailable() && roi_etLarge.IsActive() ) {
         *( roi_etLarge() ) = *( parent.roi_etLarge() );
      } else {
         roi_etLarge()->clear();
      }       
      if( parent.roi_muCharge.IsAvailable() && roi_muCharge.IsActive() ) {
         *( roi_muCharge() ) = *( parent.roi_muCharge() );
      } else {
         roi_muCharge()->clear();
      }       
      if( parent.roi_isoBits.IsAvailable() && roi_isoBits.IsActive() ) {
         *( roi_isoBits() ) = *( parent.roi_isoBits() );
      } else {
         roi_isoBits()->clear();
      }       
      if( parent.roi_vectorEX.IsAvailable() && roi_vectorEX.IsActive() ) {
         *( roi_vectorEX() ) = *( parent.roi_vectorEX() );
      } else {
         roi_vectorEX()->clear();
      }       
      if( parent.roi_vectorEY.IsAvailable() && roi_vectorEY.IsActive() ) {
         *( roi_vectorEY() ) = *( parent.roi_vectorEY() );
      } else {
         roi_vectorEY()->clear();
      }       
      if( parent.roi_overflowEX.IsAvailable() && roi_overflowEX.IsActive() ) {
         *( roi_overflowEX() ) = *( parent.roi_overflowEX() );
      } else {
         roi_overflowEX()->clear();
      }       
      if( parent.roi_overflowEY.IsAvailable() && roi_overflowEY.IsActive() ) {
         *( roi_overflowEY() ) = *( parent.roi_overflowEY() );
      } else {
         roi_overflowEY()->clear();
      }       
      if( parent.roi_overflowET.IsAvailable() && roi_overflowET.IsActive() ) {
         *( roi_overflowET() ) = *( parent.roi_overflowET() );
      } else {
         roi_overflowET()->clear();
      }
      if( parent.runNumber.IsAvailable() && runNumber.IsActive() ) {
         runNumber() = parent.runNumber();
      } else {
         runNumber() = 0;
      }
      if( parent.seconds.IsAvailable() && seconds.IsActive() ) {
         seconds() = parent.seconds();
      } else {
         seconds() = 0;
      }
      if( parent.seq_algTotalTime.IsAvailable() && seq_algTotalTime.IsActive() ) {
         *( seq_algTotalTime() ) = *( parent.seq_algTotalTime() );
      } else {
         seq_algTotalTime()->clear();
      }
      if( parent.seq_alg_isCached.IsAvailable() && seq_alg_isCached.IsActive() ) {
         *( seq_alg_isCached() ) = *( parent.seq_alg_isCached() );
      } else {
         seq_alg_isCached()->clear();
      }
      if( parent.seq_alg_isCalled.IsAvailable() && seq_alg_isCalled.IsActive() ) {
         *( seq_alg_isCalled() ) = *( parent.seq_alg_isCalled() );
      } else {
         seq_alg_isCalled()->clear();
      }
      if( parent.seq_alg_n.IsAvailable() && seq_alg_n.IsActive() ) {
         *( seq_alg_n() ) = *( parent.seq_alg_n() );
      } else {
         seq_alg_n()->clear();
      }
      if( parent.seq_alg_position.IsAvailable() && seq_alg_position.IsActive() ) {
         *( seq_alg_position() ) = *( parent.seq_alg_position() );
      } else {
         seq_alg_position()->clear();
      }
      if( parent.seq_alg_roi_index.IsAvailable() && seq_alg_roi_index.IsActive() ) {
         *( seq_alg_roi_index() ) = *( parent.seq_alg_roi_index() );
      } else {
         seq_alg_roi_index()->clear();
      }
      if( parent.seq_alg_roi_n.IsAvailable() && seq_alg_roi_n.IsActive() ) {
         *( seq_alg_roi_n() ) = *( parent.seq_alg_roi_n() );
      } else {
         seq_alg_roi_n()->clear();
      }
      if( parent.seq_alg_timeStartMicroSec.IsAvailable() && seq_alg_timeStartMicroSec.IsActive() ) {
         *( seq_alg_timeStartMicroSec() ) = *( parent.seq_alg_timeStartMicroSec() );
      } else {
         seq_alg_timeStartMicroSec()->clear();
      }
      if( parent.seq_alg_timeStartSec.IsAvailable() && seq_alg_timeStartSec.IsActive() ) {
         *( seq_alg_timeStartSec() ) = *( parent.seq_alg_timeStartSec() );
      } else {
         seq_alg_timeStartSec()->clear();
      }
      if( parent.seq_alg_timeStopMicroSec.IsAvailable() && seq_alg_timeStopMicroSec.IsActive() ) {
         *( seq_alg_timeStopMicroSec() ) = *( parent.seq_alg_timeStopMicroSec() );
      } else {
         seq_alg_timeStopMicroSec()->clear();
      }
      if( parent.seq_alg_timeStopSec.IsAvailable() && seq_alg_timeStopSec.IsActive() ) {
         *( seq_alg_timeStopSec() ) = *( parent.seq_alg_timeStopSec() );
      } else {
         seq_alg_timeStopSec()->clear();
      }
      if( parent.seq_alg_timer.IsAvailable() && seq_alg_timer.IsActive() ) {
         *( seq_alg_timer() ) = *( parent.seq_alg_timer() );
      } else {
         seq_alg_timer()->clear();
      }
      if( parent.seq_channelCounter.IsAvailable() && seq_channelCounter.IsActive() ) {
         *( seq_channelCounter() ) = *( parent.seq_channelCounter() );
      } else {
         seq_channelCounter()->clear();
      }
      if( parent.seq_index.IsAvailable() && seq_index.IsActive() ) {
         *( seq_index() ) = *( parent.seq_index() );
      } else {
         seq_index()->clear();
      }
      if( parent.seq_isAlreadyExecuted.IsAvailable() && seq_isAlreadyExecuted.IsActive() ) {
         *( seq_isAlreadyExecuted() ) = *( parent.seq_isAlreadyExecuted() );
      } else {
         seq_isAlreadyExecuted()->clear();
      }
      if( parent.seq_isExecuted.IsAvailable() && seq_isExecuted.IsActive() ) {
         *( seq_isExecuted() ) = *( parent.seq_isExecuted() );
      } else {
         seq_isExecuted()->clear();
      }
      if( parent.seq_isInitial.IsAvailable() && seq_isInitial.IsActive() ) {
         *( seq_isInitial() ) = *( parent.seq_isInitial() );
      } else {
         seq_isInitial()->clear();
      }
      if( parent.seq_isPrevious.IsAvailable() && seq_isPrevious.IsActive() ) {
         *( seq_isPrevious() ) = *( parent.seq_isPrevious() );
      } else {
         seq_isPrevious()->clear();
      }
      if( parent.seq_level.IsAvailable() && seq_level.IsActive() ) {
         *( seq_level() ) = *( parent.seq_level() );
      } else {
         seq_level()->clear();
      }
      if( parent.seq_n.IsAvailable() && seq_n.IsActive() ) {
         seq_n() = parent.seq_n();
      } else {
         seq_n() = 0;
      }
      if( parent.seq_roi.IsAvailable() && seq_roi.IsActive() ) {
         *( seq_roi() ) = *( parent.seq_roi() );
      } else {
         seq_roi()->clear();
      }
      if( parent.seq_timer.IsAvailable() && seq_timer.IsActive() ) {
         *( seq_timer() ) = *( parent.seq_timer() );
      } else {
         seq_timer()->clear();
      }
      if( parent.te_childIndex.IsAvailable() && te_childIndex.IsActive() ) {
         *( te_childIndex() ) = *( parent.te_childIndex() );
      } else {
         te_childIndex()->clear();
      }
      if( parent.te_clid.IsAvailable() && te_clid.IsActive() ) {
         *( te_clid() ) = *( parent.te_clid() );
      } else {
         te_clid()->clear();
      }
      if( parent.te_id.IsAvailable() && te_id.IsActive() ) {
         *( te_id() ) = *( parent.te_id() );
      } else {
         te_id()->clear();
      }
      if( parent.te_index.IsAvailable() && te_index.IsActive() ) {
         *( te_index() ) = *( parent.te_index() );
      } else {
         te_index()->clear();
      }
      if( parent.te_isActiveState.IsAvailable() && te_isActiveState.IsActive() ) {
         *( te_isActiveState() ) = *( parent.te_isActiveState() );
      } else {
         te_isActiveState()->clear();
      }
      if( parent.te_isErrorState.IsAvailable() && te_isErrorState.IsActive() ) {
         *( te_isErrorState() ) = *( parent.te_isErrorState() );
      } else {
         te_isErrorState()->clear();
      }
      if( parent.te_isInitialTe.IsAvailable() && te_isInitialTe.IsActive() ) {
         *( te_isInitialTe() ) = *( parent.te_isInitialTe() );
      } else {
         te_isInitialTe()->clear();
      }
      if( parent.te_isL1ThresholdTe.IsAvailable() && te_isL1ThresholdTe.IsActive() ) {
         *( te_isL1ThresholdTe() ) = *( parent.te_isL1ThresholdTe() );
      } else {
         te_isL1ThresholdTe()->clear();
      }
      if( parent.te_isOutputEFNode.IsAvailable() && te_isOutputEFNode.IsActive() ) {
         *( te_isOutputEFNode() ) = *( parent.te_isOutputEFNode() );
      } else {
         te_isOutputEFNode()->clear();
      }
      if( parent.te_isOutputL2Node.IsAvailable() && te_isOutputL2Node.IsActive() ) {
         *( te_isOutputL2Node() ) = *( parent.te_isOutputL2Node() );
      } else {
         te_isOutputL2Node()->clear();
      }
      if( parent.te_isRegularTe.IsAvailable() && te_isRegularTe.IsActive() ) {
         *( te_isRegularTe() ) = *( parent.te_isRegularTe() );
      } else {
         te_isRegularTe()->clear();
      }
      if( parent.te_isRoITe.IsAvailable() && te_isRoITe.IsActive() ) {
         *( te_isRoITe() ) = *( parent.te_isRoITe() );
      } else {
         te_isRoITe()->clear();
      }
      if( parent.te_isTerminalNode.IsAvailable() && te_isTerminalNode.IsActive() ) {
         *( te_isTerminalNode() ) = *( parent.te_isTerminalNode() );
      } else {
         te_isTerminalNode()->clear();
      }
      if( parent.te_isTopologicalTe.IsAvailable() && te_isTopologicalTe.IsActive() ) {
         *( te_isTopologicalTe() ) = *( parent.te_isTopologicalTe() );
      } else {
         te_isTopologicalTe()->clear();
      }
      if( parent.te_n.IsAvailable() && te_n.IsActive() ) {
         te_n() = parent.te_n();
      } else {
         te_n() = 0;
      }
      if( parent.te_parentIndex.IsAvailable() && te_parentIndex.IsActive() ) {
         *( te_parentIndex() ) = *( parent.te_parentIndex() );
      } else {
         te_parentIndex()->clear();
      }
      if( parent.te_roiId.IsAvailable() && te_roiId.IsActive() ) {
         *( te_roiId() ) = *( parent.te_roiId() );
      } else {
         te_roiId()->clear();
      }
      if( parent.timer.IsAvailable() && timer.IsActive() ) {
         timer() = parent.timer();
      } else {
         timer() = 0;
      }
      if( parent.timerChainProcess.IsAvailable() && timerChainProcess.IsActive() ) {
         timerChainProcess() = parent.timerChainProcess();
      } else {
         timerChainProcess() = 0;
      }
      if( parent.timerEndSteer.IsAvailable() && timerEndSteer.IsActive() ) {
         timerEndSteer() = parent.timerEndSteer();
      } else {
         timerEndSteer() = 0;
      }
      if( parent.timerMon.IsAvailable() && timerMon.IsActive() ) {
         timerMon() = parent.timerMon();
      } else {
         timerMon() = 0;
      }
      if( parent.timerResultBuilder.IsAvailable() && timerResultBuilder.IsActive() ) {
         timerResultBuilder() = parent.timerResultBuilder();
      } else {
         timerResultBuilder() = 0;
      }
      if( parent.timerTrigCost.IsAvailable() && timerTrigCost.IsActive() ) {
         timerTrigCost() = parent.timerTrigCost();
      } else {
         timerTrigCost() = 0;
      }

      return *this;
   }

   /**
    * This function is used internally to access VarHandle members
    * by name. This is necessary to push some setup from compile time
    * to run time. It may sound weird, but it makes a lot of sense for large
    * classes.
    *
    * @param name The name of the C++ variable (not of the branch)
    * @returns A pointer to the VarHandle object
    */
   VarHandleBase* TrigCostD3PDObject::GetVarHandle( const char* name ) {

      if( ! ::strcmp( name, "appId" ) ) {
         return &appId;
      }
      else if( ! ::strcmp( name, "ebWeight" ) ) {
         return &ebWeight;
      }
      else if( ! ::strcmp( name, "ebWeightBG" ) ) {
         return &ebWeightBG;
      }
      else if( ! ::strcmp( name, "ebUnbiased" ) ) {
         return &ebUnbiased;
      }
      else if( ! ::strcmp( name, "ranScaleTools" ) ) {
         return &ranScaleTools;
      }
      else if( ! ::strcmp( name, "bunchCrossingId" ) ) {
         return &bunchCrossingId;
      }
      else if( ! ::strcmp( name, "chain_counter" ) ) {
         return &chain_counter;
      }
      else if( ! ::strcmp( name, "chain_isExpressStream" ) ) {
         return &chain_isExpressStream;
      }
      else if( ! ::strcmp( name, "chain_isPassed" ) ) {
         return &chain_isPassed;
      }
      else if( ! ::strcmp( name, "chain_isPassedRaw" ) ) {
         return &chain_isPassedRaw;
      }
      else if( ! ::strcmp( name, "chain_isPassthrough" ) ) {
         return &chain_isPassthrough;
      }
      else if( ! ::strcmp( name, "chain_isPrescaled" ) ) {
         return &chain_isPrescaled;
      }
      else if( ! ::strcmp( name, "chain_isResurrected" ) ) {
         return &chain_isResurrected;
      }
      else if( ! ::strcmp( name, "chain_level" ) ) {
         return &chain_level;
      }
      else if( ! ::strcmp( name, "chain_n" ) ) {
         return &chain_n;
      }
      else if( ! ::strcmp( name, "chain_timer" ) ) {
         return &chain_timer;
      }
      else if( ! ::strcmp( name, "chain_wasL1AfterPrescale" ) ) {
         return &chain_wasL1AfterPrescale;
      }
      else if( ! ::strcmp( name, "chain_wasL1AfterVeto" ) ) {
         return &chain_wasL1AfterVeto;
      }
      else if( ! ::strcmp( name, "chain_wasL1BeforePrescale" ) ) {
         return &chain_wasL1BeforePrescale;
      }
      else if( ! ::strcmp( name, "costEvent" ) ) {
         return &costEvent;
      }
      else if( ! ::strcmp( name, "costRunNsec" ) ) {
         return &costRunNsec;
      }
      else if( ! ::strcmp( name, "costRunSec" ) ) {
         return &costRunSec;
      }
      else if( ! ::strcmp( name, "eventNumber" ) ) {
         return &eventNumber;
      }
      else if( ! ::strcmp( name, "l1_ctpId" ) ) {
         return &l1_ctpId;
      }
      else if( ! ::strcmp( name, "l1_isPrescaled" ) ) {
         return &l1_isPrescaled;
      }
      else if( ! ::strcmp( name, "l1_isVetoed" ) ) {
         return &l1_isVetoed;
      }
      else if( ! ::strcmp( name, "l1_n" ) ) {
         return &l1_n;
      }
      else if( ! ::strcmp( name, "l1_passed" ) ) {
         return &l1_passed;
      }
      else if( ! ::strcmp( name, "l1_passedAfterPrescale" ) ) {
         return &l1_passedAfterPrescale;
      }
      else if( ! ::strcmp( name, "l1_passedAfterVeto" ) ) {
         return &l1_passedAfterVeto;
      }
      else if( ! ::strcmp( name, "l1_passedBeforePrescale" ) ) {
         return &l1_passedBeforePrescale;
      }
      else if( ! ::strcmp( name, "lumi" ) ) {
         return &lumi;
      }
      else if( ! ::strcmp( name, "lumiLength" ) ) {
         return &lumiLength;
      }
      else if( ! ::strcmp( name, "nanoSeconds" ) ) {
         return &nanoSeconds;
      }
      else if( ! ::strcmp( name, "rob_data_id" ) ) {
         return &rob_data_id;
      }
      else if( ! ::strcmp( name, "rob_data_isCached" ) ) {
         return &rob_data_isCached;
      }
      else if( ! ::strcmp( name, "rob_data_isDisabled" ) ) {
         return &rob_data_isDisabled;
      }
      else if( ! ::strcmp( name, "rob_data_isIgnored" ) ) {
         return &rob_data_isIgnored;
      }
      else if( ! ::strcmp( name, "rob_data_isRetrieved" ) ) {
         return &rob_data_isRetrieved;
      }
      else if( ! ::strcmp( name, "rob_data_isStatusOk" ) ) {
         return &rob_data_isStatusOk;
      }
      else if( ! ::strcmp( name, "rob_data_isStatusPrefetched" ) ) {
         return &rob_data_isStatusPrefetched;
      }
      else if( ! ::strcmp( name, "rob_data_isUnclassified" ) ) {
         return &rob_data_isUnclassified;
      }
      else if( ! ::strcmp( name, "rob_data_n" ) ) {
         return &rob_data_n;
      }
      else if( ! ::strcmp( name, "rob_data_size" ) ) {
         return &rob_data_size;
      }
      else if( ! ::strcmp( name, "rob_n" ) ) {
         return &rob_n;
      }
      else if( ! ::strcmp( name, "rob_requestorId" ) ) {
         return &rob_requestorId;
      }
      else if( ! ::strcmp( name, "rob_sum_isCached" ) ) {
         return &rob_sum_isCached;
      }
      else if( ! ::strcmp( name, "rob_sum_isDisabled" ) ) {
         return &rob_sum_isDisabled;
      }
      else if( ! ::strcmp( name, "rob_sum_isIgnored" ) ) {
         return &rob_sum_isIgnored;
      }
      else if( ! ::strcmp( name, "rob_sum_isRetrieved" ) ) {
         return &rob_sum_isRetrieved;
      }
      else if( ! ::strcmp( name, "rob_sum_isUnclassified" ) ) {
         return &rob_sum_isUnclassified;
      }
      else if( ! ::strcmp( name, "rob_sum_n" ) ) {
         return &rob_sum_n;
      }
      else if( ! ::strcmp( name, "rob_sum_nRob" ) ) {
         return &rob_sum_nRob;
      }
      else if( ! ::strcmp( name, "rob_sum_size" ) ) {
         return &rob_sum_size;
      }
      else if( ! ::strcmp( name, "rob_sum_subDet" ) ) {
         return &rob_sum_subDet;
      }
      else if( ! ::strcmp( name, "rob_timeStartMicroSec" ) ) {
         return &rob_timeStartMicroSec;
      }
      else if( ! ::strcmp( name, "rob_timeStartSec" ) ) {
         return &rob_timeStartSec;
      }
      else if( ! ::strcmp( name, "rob_timeStopMicroSec" ) ) {
         return &rob_timeStopMicroSec;
      }
      else if( ! ::strcmp( name, "rob_timeStopSec" ) ) {
         return &rob_timeStopSec;
      }
      else if( ! ::strcmp( name, "rob_timer" ) ) {
         return &rob_timer;
      }
      else if( ! ::strcmp( name, "roi_area" ) ) {
         return &roi_area;
      }
      else if( ! ::strcmp( name, "roi_eta" ) ) {
         return &roi_eta;
      }
      else if( ! ::strcmp( name, "roi_id" ) ) {
         return &roi_id;
      }
      else if( ! ::strcmp( name, "roi_isTypeEmTau" ) ) {
         return &roi_isTypeEmTau;
      }
      else if( ! ::strcmp( name, "roi_isTau" ) ) {
         return &roi_isTau;
      }
      else if( ! ::strcmp( name, "roi_isTypeEnergy" ) ) {
         return &roi_isTypeEnergy;
      }
      else if( ! ::strcmp( name, "roi_isTypeJet" ) ) {
         return &roi_isTypeJet;
      }
      else if( ! ::strcmp( name, "roi_isTypeJetEt" ) ) {
         return &roi_isTypeJetEt;
      }
      else if( ! ::strcmp( name, "roi_isTypeMuon" ) ) {
         return &roi_isTypeMuon;
      }
      else if( ! ::strcmp( name, "roi_isTypeNone" ) ) {
         return &roi_isTypeNone;
      }
      else if( ! ::strcmp( name, "roi_n" ) ) {
         return &roi_n;
      }
      else if( ! ::strcmp( name, "roi_nL1Thresholds" ) ) {
         return &roi_nL1Thresholds;
      }
       else if( ! ::strcmp( name, "roi_phi" ) ) {
         return &roi_phi;
      }
      else if( ! ::strcmp( name, "roi_et" ) ) {
         return &roi_et;
      }
      else if( ! ::strcmp( name, "roi_etLarge" ) ) {
         return &roi_etLarge;
      }
      else if( ! ::strcmp( name, "roi_muCharge" ) ) {
         return &roi_muCharge;
      }
      else if( ! ::strcmp( name, "roi_isoBits" ) ) {
         return &roi_isoBits;
      }
      else if( ! ::strcmp( name, "roi_vectorEX" ) ) {
         return &roi_vectorEX;
      }
      else if( ! ::strcmp( name, "roi_vectorEY" ) ) {
         return &roi_vectorEY;
      }
      else if( ! ::strcmp( name, "roi_overflowEX" ) ) {
         return &roi_overflowEX;
      }
      else if( ! ::strcmp( name, "roi_overflowEY" ) ) {
         return &roi_overflowEY;
      }
      else if( ! ::strcmp( name, "roi_overflowET" ) ) {
         return &roi_overflowET;
      }
      else if( ! ::strcmp( name, "runNumber" ) ) {
         return &runNumber;
      }
      else if( ! ::strcmp( name, "seconds" ) ) {
         return &seconds;
      }
      else if( ! ::strcmp( name, "seq_algTotalTime" ) ) {
         return &seq_algTotalTime;
      }
      else if( ! ::strcmp( name, "seq_alg_isCached" ) ) {
         return &seq_alg_isCached;
      }
      else if( ! ::strcmp( name, "seq_alg_isCalled" ) ) {
         return &seq_alg_isCalled;
      }
      else if( ! ::strcmp( name, "seq_alg_n" ) ) {
         return &seq_alg_n;
      }
      else if( ! ::strcmp( name, "seq_alg_position" ) ) {
         return &seq_alg_position;
      }
      else if( ! ::strcmp( name, "seq_alg_roi_index" ) ) {
         return &seq_alg_roi_index;
      }
      else if( ! ::strcmp( name, "seq_alg_roi_n" ) ) {
         return &seq_alg_roi_n;
      }
      else if( ! ::strcmp( name, "seq_alg_timeStartMicroSec" ) ) {
         return &seq_alg_timeStartMicroSec;
      }
      else if( ! ::strcmp( name, "seq_alg_timeStartSec" ) ) {
         return &seq_alg_timeStartSec;
      }
      else if( ! ::strcmp( name, "seq_alg_timeStopMicroSec" ) ) {
         return &seq_alg_timeStopMicroSec;
      }
      else if( ! ::strcmp( name, "seq_alg_timeStopSec" ) ) {
         return &seq_alg_timeStopSec;
      }
      else if( ! ::strcmp( name, "seq_alg_timer" ) ) {
         return &seq_alg_timer;
      }
      else if( ! ::strcmp( name, "seq_channelCounter" ) ) {
         return &seq_channelCounter;
      }
      else if( ! ::strcmp( name, "seq_index" ) ) {
         return &seq_index;
      }
      else if( ! ::strcmp( name, "seq_isAlreadyExecuted" ) ) {
         return &seq_isAlreadyExecuted;
      }
      else if( ! ::strcmp( name, "seq_isExecuted" ) ) {
         return &seq_isExecuted;
      }
      else if( ! ::strcmp( name, "seq_isInitial" ) ) {
         return &seq_isInitial;
      }
      else if( ! ::strcmp( name, "seq_isPrevious" ) ) {
         return &seq_isPrevious;
      }
      else if( ! ::strcmp( name, "seq_level" ) ) {
         return &seq_level;
      }
      else if( ! ::strcmp( name, "seq_n" ) ) {
         return &seq_n;
      }
      else if( ! ::strcmp( name, "seq_roi" ) ) {
         return &seq_roi;
      }
      else if( ! ::strcmp( name, "seq_timer" ) ) {
         return &seq_timer;
      }
      else if( ! ::strcmp( name, "te_childIndex" ) ) {
         return &te_childIndex;
      }
      else if( ! ::strcmp( name, "te_clid" ) ) {
         return &te_clid;
      }
      else if( ! ::strcmp( name, "te_id" ) ) {
         return &te_id;
      }
      else if( ! ::strcmp( name, "te_index" ) ) {
         return &te_index;
      }
      else if( ! ::strcmp( name, "te_isActiveState" ) ) {
         return &te_isActiveState;
      }
      else if( ! ::strcmp( name, "te_isErrorState" ) ) {
         return &te_isErrorState;
      }
      else if( ! ::strcmp( name, "te_isInitialTe" ) ) {
         return &te_isInitialTe;
      }
      else if( ! ::strcmp( name, "te_isL1ThresholdTe" ) ) {
         return &te_isL1ThresholdTe;
      }
      else if( ! ::strcmp( name, "te_isOutputEFNode" ) ) {
         return &te_isOutputEFNode;
      }
      else if( ! ::strcmp( name, "te_isOutputL2Node" ) ) {
         return &te_isOutputL2Node;
      }
      else if( ! ::strcmp( name, "te_isRegularTe" ) ) {
         return &te_isRegularTe;
      }
      else if( ! ::strcmp( name, "te_isRoITe" ) ) {
         return &te_isRoITe;
      }
      else if( ! ::strcmp( name, "te_isTerminalNode" ) ) {
         return &te_isTerminalNode;
      }
      else if( ! ::strcmp( name, "te_isTopologicalTe" ) ) {
         return &te_isTopologicalTe;
      }
      else if( ! ::strcmp( name, "te_n" ) ) {
         return &te_n;
      }
      else if( ! ::strcmp( name, "te_parentIndex" ) ) {
         return &te_parentIndex;
      }
      else if( ! ::strcmp( name, "te_roiId" ) ) {
         return &te_roiId;
      }
      else if( ! ::strcmp( name, "timer" ) ) {
         return &timer;
      }
      else if( ! ::strcmp( name, "timerChainProcess" ) ) {
         return &timerChainProcess;
      }
      else if( ! ::strcmp( name, "timerEndSteer" ) ) {
         return &timerEndSteer;
      }
      else if( ! ::strcmp( name, "timerMon" ) ) {
         return &timerMon;
      }
      else if( ! ::strcmp( name, "timerResultBuilder" ) ) {
         return &timerResultBuilder;
      }
      else if( ! ::strcmp( name, "timerTrigCost" ) ) {
         return &timerTrigCost;
      }

      Error( "GetVarHandle", "Variable \"%s\" unknown", name );
      return 0;
   }

   /**
    * This function is used internally to set up all the VarHandle members
    * of the class. It speeds up compilation *a lot* to do this at run-time
    * like this, instead of putting a lot of lines of code operating on
    * the std::map member.
    *
    * @param master Pointer to the master index, or a null pointer
    */
   void TrigCostD3PDObject::SetVarHandles( const ::Long64_t* master ) {

      // Create a list of variable-branch name pairs:
      static const Int_t NVARNAMES = 129;
      static const char* VARNAMES[ NVARNAMES ][ 2 ] = {
         { "appId", "appId" },
         { "ebWeight", "ebWeight" },
         { "ebWeightBG", "ebWeightBG" },
         { "ebUnbiased", "ebUnbiased" },         
         { "ranScaleTools", "ranScaleTools" },         
         { "bunchCrossingId", "bunchCrossingId" },
         { "chain_counter", "chain_counter" },
         { "chain_isExpressStream", "chain_isExpressStream" },
         { "chain_isPassed", "chain_isPassed" },
         { "chain_isPassedRaw", "chain_isPassedRaw" },
         { "chain_isPassthrough", "chain_isPassthrough" },
         { "chain_isPrescaled", "chain_isPrescaled" },
         { "chain_isResurrected", "chain_isResurrected" },
         { "chain_level", "chain_level" },
         { "chain_n", "chain_n" },
         { "chain_timer", "chain_timer" },
         { "chain_wasL1AfterPrescale", "chain_wasL1AfterPrescale" },
         { "chain_wasL1AfterVeto", "chain_wasL1AfterVeto" },
         { "chain_wasL1BeforePrescale", "chain_wasL1BeforePrescale" },
         { "costEvent", "costEvent" },
         { "costRunNsec", "costRunNsec" },
         { "costRunSec", "costRunSec" },
         { "eventNumber", "eventNumber" },
         { "l1_ctpId", "l1_ctpId" },
         { "l1_isPrescaled", "l1_isPrescaled" },
         { "l1_isVetoed", "l1_isVetoed" },
         { "l1_n", "l1_n" },
         { "l1_passed", "l1_passed" },
         { "l1_passedAfterPrescale", "l1_passedAfterPrescale" },
         { "l1_passedAfterVeto", "l1_passedAfterVeto" },
         { "l1_passedBeforePrescale", "l1_passedBeforePrescale" },
         { "lumi", "lumi" },
         { "lumiLength", "lumiLength" },
         { "nanoSeconds", "nanoSeconds" },
         { "rob_data_id", "rob_data_id" },
         { "rob_data_isCached", "rob_data_isCached" },
         { "rob_data_isDisabled", "rob_data_isDisabled" },
         { "rob_data_isIgnored", "rob_data_isIgnored" },
         { "rob_data_isRetrieved", "rob_data_isRetrieved" },
         { "rob_data_isStatusOk", "rob_data_isStatusOk" },
         { "rob_data_isStatusPrefetched", "rob_data_isStatusPrefetched" },
         { "rob_data_isUnclassified", "rob_data_isUnclassified" },
         { "rob_data_n", "rob_data_n" },
         { "rob_data_size", "rob_data_size" },
         { "rob_n", "rob_n" },
         { "rob_requestorId", "rob_requestorId" },
         { "rob_sum_isCached", "rob_sum_isCached" },
         { "rob_sum_isDisabled", "rob_sum_isDisabled" },
         { "rob_sum_isIgnored", "rob_sum_isIgnored" },
         { "rob_sum_isRetrieved", "rob_sum_isRetrieved" },
         { "rob_sum_isUnclassified", "rob_sum_isUnclassified" },
         { "rob_sum_n", "rob_sum_n" },
         { "rob_sum_nRob", "rob_sum_nRob" },
         { "rob_sum_size", "rob_sum_size" },
         { "rob_sum_subDet", "rob_sum_subDet" },
         { "rob_timeStartMicroSec", "rob_timeStartMicroSec" },
         { "rob_timeStartSec", "rob_timeStartSec" },
         { "rob_timeStopMicroSec", "rob_timeStopMicroSec" },
         { "rob_timeStopSec", "rob_timeStopSec" },
         { "rob_timer", "rob_timer" },
         { "roi_area", "roi_area" },
         { "roi_eta", "roi_eta" },
         { "roi_id", "roi_id" },
         { "roi_isTypeEmTau", "roi_isTypeEmTau" },
         { "roi_isTau", "roi_isTau" },         
         { "roi_isTypeEnergy", "roi_isTypeEnergy" },
         { "roi_isTypeJet", "roi_isTypeJet" },
         { "roi_isTypeJetEt", "roi_isTypeJetEt" },
         { "roi_isTypeMuon", "roi_isTypeMuon" },
         { "roi_isTypeNone", "roi_isTypeNone" },
         { "roi_n", "roi_n" },
         { "roi_nL1Thresholds", "roi_nL1Thresholds" },
         { "roi_phi", "roi_phi" },
         { "roi_et", "roi_et" },
         { "roi_etLarge", "roi_etLarge" },
         { "roi_muCharge", "roi_muCharge" },
         { "roi_isoBits", "roi_isoBits" },
         { "roi_vectorEX", "roi_vectorEX" },
         { "roi_vectorEY", "roi_vectorEY" },
         { "roi_overflowEX", "roi_overflowEX" },
         { "roi_overflowEY", "roi_overflowEY" },
         { "roi_overflowET", "roi_overflowET" },
         { "runNumber", "runNumber" },
         { "seconds", "seconds" },
         { "seq_algTotalTime", "seq_algTotalTime" },
         { "seq_alg_isCached", "seq_alg_isCached" },
         { "seq_alg_isCalled", "seq_alg_isCalled" },
         { "seq_alg_n", "seq_alg_n" },
         { "seq_alg_position", "seq_alg_position" },
         { "seq_alg_roi_index", "seq_alg_roi_index" },
         { "seq_alg_roi_n", "seq_alg_roi_n" },
         { "seq_alg_timeStartMicroSec", "seq_alg_timeStartMicroSec" },
         { "seq_alg_timeStartSec", "seq_alg_timeStartSec" },
         { "seq_alg_timeStopMicroSec", "seq_alg_timeStopMicroSec" },
         { "seq_alg_timeStopSec", "seq_alg_timeStopSec" },
         { "seq_alg_timer", "seq_alg_timer" },
         { "seq_channelCounter", "seq_channelCounter" },
         { "seq_index", "seq_index" },
         { "seq_isAlreadyExecuted", "seq_isAlreadyExecuted" },
         { "seq_isExecuted", "seq_isExecuted" },
         { "seq_isInitial", "seq_isInitial" },
         { "seq_isPrevious", "seq_isPrevious" },
         { "seq_level", "seq_level" },
         { "seq_n", "seq_n" },
         { "seq_roi", "seq_roi" },
         { "seq_timer", "seq_timer" },
         { "te_childIndex", "te_childIndex" },
         { "te_clid", "te_clid" },
         { "te_id", "te_id" },
         { "te_index", "te_index" },
         { "te_isActiveState", "te_isActiveState" },
         { "te_isErrorState", "te_isErrorState" },
         { "te_isInitialTe", "te_isInitialTe" },
         { "te_isL1ThresholdTe", "te_isL1ThresholdTe" },
         { "te_isOutputEFNode", "te_isOutputEFNode" },
         { "te_isOutputL2Node", "te_isOutputL2Node" },
         { "te_isRegularTe", "te_isRegularTe" },
         { "te_isRoITe", "te_isRoITe" },
         { "te_isTerminalNode", "te_isTerminalNode" },
         { "te_isTopologicalTe", "te_isTopologicalTe" },
         { "te_n", "te_n" },
         { "te_parentIndex", "te_parentIndex" },
         { "te_roiId", "te_roiId" },
         { "timer", "timer" },
         { "timerChainProcess", "timerChainProcess" },
         { "timerEndSteer", "timerEndSteer" },
         { "timerMon", "timerMon" },
         { "timerResultBuilder", "timerResultBuilder" },
         { "timerTrigCost", "timerTrigCost" }
      };

      // Set up the fHandles map using this list:
      for( Int_t i = 0; i < NVARNAMES; ++i ) {
         VarHandleBase* vh = GetVarHandle( VARNAMES[ i ][ 0 ] );
         vh->SetName( fPrefix + VARNAMES[ i ][ 1 ] );
         vh->SetMaster( master );
         fHandles[ VARNAMES[ i ][ 0 ] ] = vh;
      }

      return;
   }

} // namespace D3PDReader
