/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataAccess.cxx 726661 2016-02-28 01:18:32Z ssnyder $

// STL include(s):
#include <stdio.h> // std::strcmp

// ROOT include(s):
#include <TTree.h>
#include <TChain.h>
#include <TBranch.h>

// Local include(s):
#include "TrigRootAnalysis/DataAccess.h"

ClassImp( D3PD::Trig::DataAccess )

namespace D3PD {

   namespace Trig {

      /**
       * The constructor tries to initialize the object by connecting to all the
       * available branches. However it's possible to create the object without
       * an existing event-wise TTree as well.
       *
       * @param eventTree The event-wise D3PD TTree
       * @param prefix The prefix used for the trigger branches (usually "trig_")
       */
      DataAccess::DataAccess( TTree* eventTree, const TString& prefix )
         : TNamed( "DataAccess", "Sub-class accessing the data in the D3PD" ),
           m_tree( eventTree ), m_prefix( prefix ), m_initialized( kFALSE ),
           m_detailLevel( -1 ), m_trigCostTreeName( "trig_cost" ), m_entry( 0 ),
           m_smk( this, prefix + "DB_SMK", &m_entry ),
           m_l1psk( this, prefix + "DB_L1PSK", &m_entry ),
           m_hltpsk( this, prefix + "DB_HLTPSK", &m_entry ),
           m_l1_tav( this, prefix + "L1_TAV", &m_entry ),
           m_l2_physics( this, prefix + "L2_passedPhysics", &m_entry ),
           m_ef_physics( this, prefix + "EF_passedPhysics", &m_entry ),
           m_l1_tbp( this, prefix + "L1_TBP", &m_entry ),
           m_l1_tap( this, prefix + "L1_TAP", &m_entry ),
           m_l2_raw( this, prefix + "L2_passedRaw", &m_entry ),
           m_ef_raw( this, prefix + "EF_passedRaw", &m_entry ),
           m_l2_resurrected( this, prefix + "L2_resurrected", &m_entry ),
           m_ef_resurrected( this, prefix + "EF_resurrected", &m_entry ),
           m_l2_passedThrough( this, prefix + "L2_passedThrough", &m_entry ),
           m_ef_passedThrough( this, prefix + "EF_passedThrough", &m_entry ) {

         Reset();
         Initialize();
      }

      /**
       * The destructor deletes all the allocated objects, and releases whatever
       * resource it can.
       */
      DataAccess::~DataAccess() {

         Reset();
      }

      TTree* DataAccess::GetEventTree() const {

         return m_tree;
      }

      /**
       * This function can be used to instruct the object to start using a new event-wise
       * TTree. This is very useful when processing multiple D3PD files.
       *
       * Note that it is possible to provide a null pointer to the function. In this
       * case the object will be reset, and the previous TTree will be released without
       * connecting to a new one.
       *
       * Also note that unfortunately we can't just check if the passed TTree pointer
       * is the same as the cached one. ROOT has the habit of putting the same sort
       * of TTree into the same memory space when the second file is opened for instance.
       * I've seen some weird problems because of this.
       *
       * @param eventTree The event-wise D3PD TTree
       * @returns <code>kTRUE</code> if the operation was successful,
       *          <code>kFALSE</code> otherwise
       */
      Bool_t DataAccess::SetEventTree( TTree* eventTree ) {

         //
         // If the user gave a null-pointer, reset the object and return gracefully:
         //
         if( ! eventTree ) {
            Reset();
            return kTRUE;
         }

         //
         // Reset the object and start using a new TTree:
         //
         Reset();
         m_tree = eventTree;
         return Initialize();
      }

      /**
       * This function can come in *very* handy when implementing an event selection
       * using a D3PD. Using this function it's very easy to copy over the trigger
       * decision information of the selected events into an output TTree.
       *
       * @param outputTree The TTree to write the trigger decision branches to
       * @param detailLevel The detail level for which the branches should be copied
       * @returns <code>kTRUE</code> if the operation was successful,
       *          <code>kFALSE</code> otherwise
       */
      ::Bool_t DataAccess::WriteToTree( ::TTree* outputTree, ::Int_t detailLevel ) {

         if( detailLevel >= 0 ) {
            if( m_smk.IsAvailable() ) {
               m_smk.SetActive();
               m_smk.WriteTo( outputTree );
            }
            if( m_l1psk.IsAvailable() ) {
               m_l1psk.SetActive();
               m_l1psk.WriteTo( outputTree );
            }
            if( m_hltpsk.IsAvailable() ) {
               m_hltpsk.SetActive();
               m_hltpsk.WriteTo( outputTree );
            }

            if( m_l1_tav.IsAvailable() ) {
               m_l1_tav.SetActive();
               m_l1_tav.WriteTo( outputTree );
            }
            if( m_l2_physics.IsAvailable() ) {
               m_l2_physics.SetActive();
               m_l2_physics.WriteTo( outputTree );
            }
            if( m_ef_physics.IsAvailable() ) {
               m_ef_physics.SetActive();
               m_ef_physics.WriteTo( outputTree );
            }
         }

         if( detailLevel >= 1 ) {
            if( m_l1_tbp.IsAvailable() ) {
               m_l1_tbp.SetActive();
               m_l1_tbp.WriteTo( outputTree );
            }
            if( m_l1_tap.IsAvailable() ) {
               m_l1_tap.SetActive();
               m_l1_tap.WriteTo( outputTree );
            }
            if( m_l2_raw.IsAvailable() ) {
               m_l2_raw.SetActive();
               m_l2_raw.WriteTo( outputTree );
            }
            if( m_ef_raw.IsAvailable() ) {
               m_ef_raw.SetActive();
               m_ef_raw.WriteTo( outputTree );
            }
         }

         if( detailLevel >= 2 ) {
            if( m_l2_resurrected.IsAvailable() ) {
               m_l2_resurrected.SetActive();
               m_l2_resurrected.WriteTo( outputTree );
            }
            if( m_ef_resurrected.IsAvailable() ) {
               m_ef_resurrected.SetActive();
               m_ef_resurrected.WriteTo( outputTree );
            }

            if( m_l2_passedThrough.IsAvailable() ) {
               m_l2_passedThrough.SetActive();
               m_l2_passedThrough.WriteTo( outputTree );
            }
            if( m_ef_passedThrough.IsAvailable() ) {
               m_ef_passedThrough.SetActive();
               m_ef_passedThrough.WriteTo( outputTree );
            }
         }

         return kTRUE;
      }

      /**
       * Unfortunately however I tried I didn't manage to figure out an elegant way
       * of automatically figuring out which event the tool should be operating on.
       * Of course if the user issues TTree::GetEntry(...) calls on the event-wise
       * TTree himself/herself, then the tool gets the correct data automatically.
       * But since this is usually a slow thing to do, the user can also explicitly
       * request the tool to read a new event using this function.
       *
       * @param entry The entry from the event-wise tree that should be loaded
       * @param getall Flag telling whether all the sub-branches should be read
       *               (Set to "1" when writing out pre-selected events!)
       * @returns The number of bytes read (not really...)
       */
      Int_t DataAccess::GetEntry( Long64_t entry, Int_t getall ) {

         // Update the internal variable:
         m_entry = entry;

         // Figure out the correct entry if we've been given a TChain:
         TChain* chain = dynamic_cast< TChain* >( m_tree );
         if( chain ) {
            m_entry = chain->LoadTree( entry );
         }

         // Load the contents of all the branches if requested. This makes sure
         // that the memory contents are always correct for the current event in
         // case the events are written out.
         if( getall ) {
            if( m_smk.IsAvailable() ) m_smk();
            if( m_l1psk.IsAvailable() ) m_l1psk();
            if( m_hltpsk.IsAvailable() ) m_hltpsk();

            if( m_l1_tav.IsAvailable() ) m_l1_tav();
            if( m_l2_physics.IsAvailable() ) m_l2_physics();
            if( m_ef_physics.IsAvailable() ) m_ef_physics();

            if( m_l1_tbp.IsAvailable() ) m_l1_tbp();
            if( m_l1_tap.IsAvailable() ) m_l1_tap();
            if( m_l2_raw.IsAvailable() ) m_l2_raw();
            if( m_ef_raw.IsAvailable() ) m_ef_raw();

            if( m_l2_resurrected.IsAvailable() ) m_l2_resurrected();
            if( m_ef_resurrected.IsAvailable() ) m_ef_resurrected();

            if( m_l2_passedThrough.IsAvailable() ) m_l2_passedThrough();
            if( m_ef_passedThrough.IsAvailable() ) m_ef_passedThrough();
         }

         return 1;
      }

      Int_t DataAccess::GetDetailLevel() {

         return m_detailLevel;
      }

      Int_t DataAccess::GetSMK() {

         return m_smk();
      }

      Int_t DataAccess::GetL1PSK() {

         return m_l1psk();
      }

      Int_t DataAccess::GetHLTPSK() {

         return m_hltpsk();
      }

      const std::vector< unsigned int >*
      DataAccess::GetL1Result( L1ResultType type ) {

         switch( type ) {

         case TBP:
            return m_l1_tbp();
            break;
         case TAP:
            return m_l1_tap();
            break;
         case TAV:
            return m_l1_tav();
            break;
         default:
            Error( "GetL1Result", "Unknown result type requested" );
            return 0;
            break;
         }
      }

      const std::vector< short >*
      DataAccess::GetL2Result( HLTResultType type ) {

         switch( type ) {

         case Physics:
            return m_l2_physics();
            break;
         case Raw:
            return m_l2_raw();
            break;
         case Resurrected:
            return m_l2_resurrected();
            break;
         case PassedThrough:
            return m_l2_passedThrough();
            break;
         default:
            Error( "GetL2Result", "Unknown result type requested" );
            return 0;
            break;
         }
      }

      const std::vector< short >*
      DataAccess::GetEFResult( HLTResultType type ) {

         switch( type ) {

         case Physics:
            return m_ef_physics();
            break;
         case Raw:
            return m_ef_raw();
            break;
         case Resurrected:
            return m_ef_resurrected();
            break;
         case PassedThrough:
            return m_ef_passedThrough();
            break;
         default:
            Error( "GetEFResult", "Unknown result type requested" );
            return 0;
            break;
         }
      }

      Bool_t DataAccess::Initialize() {

         // Return right away if there's nothing to do:
         if( m_initialized ) return kTRUE;

         //
         // Just a basic check:
         //
         if( ! m_tree ) {
            Info( "Initialize", "Null pointer provided as event tree -> "
                  "Delaying initialization" );
            return kFALSE;
         }

         //
         // Check if we're handling a TChain:
         //
         TChain* chain = dynamic_cast< TChain* >( m_tree );
         if( chain ) {
            // Make sure that a TTree is already loaded:
            if( ! chain->GetTree() ) {
               chain->LoadTree( 0 );
            }
         }

         //
         // Try to initialize the DB key variables:
         //
         m_smk.ReadFrom( m_tree );
         m_l1psk.ReadFrom( m_tree );
         m_hltpsk.ReadFrom( m_tree );
         if( ( ! m_smk.IsAvailable() ) ||
             ( ! m_l1psk.IsAvailable() ) ||
             ( ! m_hltpsk.IsAvailable() ) ) {
            Error( "Initialize", "The supplied D3PD doesn't seem to hold trigger configuration "
                   "metadata" );
            return kFALSE;
         }

         //
         // Perform a name-check if this is a TrigCost ntuple [TrigMonConfig].
         //
         if( strcmp( m_tree->GetName(), m_trigCostTreeName.Data() ) == 0 ) {
            // This is a [TrigMonConfig] file, trigger decision information is *NOT* present in the format this
            // TrigRootAnalysis package usually uses, it is however available and parsed by the TrigCostRootAnalysis
            // package (which depends on this package).
            //
            // As a result, we don't need to display any errors and can silently return here.

            // Note that m_detailLevel remains == -1
            return kTRUE;
         }

         //
         // Try to initialize the variables coming from detail level 0:
         //
         m_l1_tav.ReadFrom( m_tree );
         m_l2_physics.ReadFrom( m_tree );
         m_ef_physics.ReadFrom( m_tree );
         if( ( ! m_l1_tav.IsAvailable() ) ||
             ( ! m_l2_physics.IsAvailable() ) ||
             ( ! m_ef_physics.IsAvailable() ) ) {
            Error( "Initialize", "The supplied TTree doesn't seem to hold trigger decision "
                   "information" );
            return kFALSE;
         }
         m_detailLevel = 0;

         //
         // Try to initialize the variables coming from detail level 1:
         //
         m_l1_tbp.ReadFrom( m_tree );
         m_l1_tap.ReadFrom( m_tree );
         m_l2_raw.ReadFrom( m_tree );
         m_ef_raw.ReadFrom( m_tree );
         if( ( ! m_l1_tbp.IsAvailable() ) ||
             ( ! m_l1_tap.IsAvailable() ) ||
             ( ! m_l2_raw.IsAvailable() ) ||
             ( ! m_ef_raw.IsAvailable() ) ) {
            // Apparently the D3PD was created with detail level 0...
            return kTRUE;
         }
         m_detailLevel = 1;

         //
         // Try to initialize the variables coming from detail level 2:
         //
         m_l2_resurrected.ReadFrom( m_tree );
         m_ef_resurrected.ReadFrom( m_tree );
         m_l2_passedThrough.ReadFrom( m_tree );
         m_ef_passedThrough.ReadFrom( m_tree );
         if( ( ! m_l2_resurrected.IsAvailable() ) ||
             ( ! m_ef_resurrected.IsAvailable() ) ||
             ( ! m_l2_passedThrough.IsAvailable() ) ||
             ( ! m_ef_passedThrough.IsAvailable() ) ) {
            // Apparently the D3PD was created with detail level 1...
            return kTRUE;
         }
         m_detailLevel = 2;

         return kTRUE;
      }

      void DataAccess::Reset() {

         m_initialized = kFALSE;
         m_detailLevel = -1;

         m_smk.ReadFrom( 0 );
         m_l1psk.ReadFrom( 0 );
         m_hltpsk.ReadFrom( 0 );

         m_l1_tav.ReadFrom( 0 );
         m_l2_physics.ReadFrom( 0 );
         m_ef_physics.ReadFrom( 0 );

         m_l1_tbp.ReadFrom( 0 );
         m_l1_tap.ReadFrom( 0 );
         m_l2_raw.ReadFrom( 0 );
         m_ef_raw.ReadFrom( 0 );

         m_l2_resurrected.ReadFrom( 0 );
         m_ef_resurrected.ReadFrom( 0 );

         m_l2_passedThrough.ReadFrom( 0 );
         m_ef_passedThrough.ReadFrom( 0 );

         return;
      }

   } // namespace Trig

} // namespace D3PD
