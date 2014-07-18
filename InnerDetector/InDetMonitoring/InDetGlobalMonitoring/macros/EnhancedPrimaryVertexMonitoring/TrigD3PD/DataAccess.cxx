/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataAccess.cxx 339267 2011-01-07 16:30:10Z krasznaa $

// ROOT include(s):
#include <TTree.h>
#include <TChain.h>
#include <TBranch.h>

// Local include(s):
#include "DataAccess.h"

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
           m_detailLevel( -1 ),
           m_l1_tav( 0 ), m_l2_physics( 0 ), m_ef_physics( 0 ),
           m_l1_tbp( 0 ), m_l1_tap( 0 ), m_l2_raw( 0 ), m_ef_raw( 0 ),
           m_l2_resurrected( 0 ), m_ef_resurrected( 0 ),
           m_l2_passedThrough( 0 ), m_ef_passedThrough( 0 ) {

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
       * Unfortunately however I tried I didn't manage to figure out an elegant way
       * of automatically figuring out which event the tool should be operating on.
       * Of course if the user issues TTree::GetEntry(...) calls on the event-wise
       * TTree himself/herself, then the tool gets the correct data automatically.
       * But since this is usually a slow thing to do, the user can also explicitly
       * request the tool to read a new event using this function.
       *
       * @param entry The entry from the event-wise tree that should be loaded
       * @param getall Flag telling whether all the sub-branches should be read
       * @returns The number of bytes read
       */
      Int_t DataAccess::GetEntry( Long64_t entry, Int_t getall ) {

         // The number of bytes read:
         Int_t bytes = 0;

         // Figure out the correct entry if we've been given a TChain:
         Long64_t centry = entry;
         TChain* chain = dynamic_cast< TChain* >( m_tree );
         if( chain ) {
            centry = chain->LoadTree( entry );
         }

         if( m_b_smk )    bytes += m_b_smk->GetEntry( centry, getall );
         if( m_b_l1psk )  bytes += m_b_l1psk->GetEntry( centry, getall );
         if( m_b_hltpsk ) bytes += m_b_hltpsk->GetEntry( centry, getall );

         if( m_b_l1_tav )     bytes += m_b_l1_tav->GetEntry( centry, getall );
         if( m_b_l2_physics ) bytes += m_b_l2_physics->GetEntry( centry, getall );
         if( m_b_ef_physics ) bytes += m_b_ef_physics->GetEntry( centry, getall );

         if( m_b_l1_tbp ) bytes += m_b_l1_tbp->GetEntry( centry, getall );
         if( m_b_l1_tap ) bytes += m_b_l1_tap->GetEntry( centry, getall );
         if( m_b_l2_raw ) bytes += m_b_l2_raw->GetEntry( centry, getall );
         if( m_b_ef_raw ) bytes += m_b_ef_raw->GetEntry( centry, getall );

         if( m_b_l2_resurrected )   bytes += m_b_l2_resurrected->GetEntry( centry, getall );
         if( m_b_ef_resurrected )   bytes += m_b_ef_resurrected->GetEntry( centry, getall );
         if( m_b_l2_passedThrough ) bytes += m_b_l2_passedThrough->GetEntry( centry, getall );
         if( m_b_ef_passedThrough ) bytes += m_b_ef_passedThrough->GetEntry( centry, getall );

         return bytes;
      }

      Int_t DataAccess::GetDetailLevel() const {

         return m_detailLevel;
      }

      Int_t DataAccess::GetSMK() const {

         return m_smk;
      }

      Int_t DataAccess::GetL1PSK() const {

         return m_l1psk;
      }

      Int_t DataAccess::GetHLTPSK() const {

         return m_hltpsk;
      }

      const std::vector< unsigned int >* DataAccess::GetL1Result( L1ResultType type ) const {

         switch( type ) {

         case TBP:
            return m_l1_tbp;
            break;
         case TAP:
            return m_l1_tap;
            break;
         case TAV:
            return m_l1_tav;
            break;
         default:
            Error( "GetL1Result", "Unknown result type requested" );
            return 0;
            break;
         }
      }

      const std::vector< short >* DataAccess::GetL2Result( HLTResultType type ) const {

         switch( type ) {

         case Physics:
            return m_l2_physics;
            break;
         case Raw:
            return m_l2_raw;
            break;
         case Resurrected:
            return m_l2_resurrected;
            break;
         case PassedThrough:
            return m_l2_passedThrough;
            break;
         default:
            Error( "GetL2Result", "Unknown result type requested" );
            return 0;
            break;
         }
      }

      const std::vector< short >* DataAccess::GetEFResult( HLTResultType type ) const {

         switch( type ) {

         case Physics:
            return m_ef_physics;
            break;
         case Raw:
            return m_ef_raw;
            break;
         case Resurrected:
            return m_ef_resurrected;
            break;
         case PassedThrough:
            return m_ef_passedThrough;
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
         if( ( ! ( m_b_smk    = m_tree->GetBranch( m_prefix + "DB_SMK" ) ) ) ||
             ( ! ( m_b_l1psk  = m_tree->GetBranch( m_prefix + "DB_L1PSK" ) ) ) ||
             ( ! ( m_b_hltpsk = m_tree->GetBranch( m_prefix + "DB_HLTPSK" ) ) ) ) {
            Error( "Initialize", "The supplied D3PD doesn't seem to hold trigger configuration "
                   "metadata" );
            return kFALSE;
         }

         m_tree->SetBranchAddress( m_prefix + "DB_SMK",    &m_smk,    &m_b_smk );
         m_tree->SetBranchAddress( m_prefix + "DB_L1PSK",  &m_l1psk,  &m_b_l1psk );
         m_tree->SetBranchAddress( m_prefix + "DB_HLTPSK", &m_hltpsk, &m_b_hltpsk );

         //
         // Try to initialize the variables coming from detail level 0:
         //
         if( ( ! ( m_b_l1_tav     = m_tree->GetBranch( m_prefix + "L1_TAV" ) ) ) ||
             ( ! ( m_b_l2_physics = m_tree->GetBranch( m_prefix + "L2_passedPhysics" ) ) ) ||
             ( ! ( m_b_ef_physics = m_tree->GetBranch( m_prefix + "EF_passedPhysics" ) ) ) ) {
            Error( "Initialize", "The supplied TTree doesn't seem to hold trigger decision "
                   "information" );
            return kFALSE;
         }

         m_tree->SetBranchAddress( m_prefix + "L1_TAV",           &m_l1_tav,
                                   &m_b_l1_tav );
         m_tree->SetBranchAddress( m_prefix + "L2_passedPhysics", &m_l2_physics,
                                   &m_b_l2_physics );
         m_tree->SetBranchAddress( m_prefix + "EF_passedPhysics", &m_ef_physics,
                                   &m_b_ef_physics );

         m_detailLevel = 0;

         //
         // Try to initialize the variables coming from detail level 1:
         //
         if( ( ! ( m_b_l1_tbp = m_tree->GetBranch( m_prefix + "L1_TBP" ) ) ) ||
             ( ! ( m_b_l1_tap = m_tree->GetBranch( m_prefix + "L1_TAP" ) ) ) ||
             ( ! ( m_b_l2_raw = m_tree->GetBranch( m_prefix + "L2_passedRaw" ) ) ) ||
             ( ! ( m_b_ef_raw = m_tree->GetBranch( m_prefix + "EF_passedRaw" ) ) ) ) {
            // Apparently the D3PD was created with detail level 0...
            return kTRUE;
         }

         m_tree->SetBranchAddress( m_prefix + "L1_TBP",       &m_l1_tbp, &m_b_l1_tbp );
         m_tree->SetBranchAddress( m_prefix + "L1_TAP",       &m_l1_tap, &m_b_l1_tap );
         m_tree->SetBranchAddress( m_prefix + "L2_passedRaw", &m_l2_raw, &m_b_l2_raw );
         m_tree->SetBranchAddress( m_prefix + "EF_passedRaw", &m_ef_raw, &m_b_ef_raw );

         m_detailLevel = 1;

         //
         // Try to initialize the variables coming from detail level 2:
         //
         if( ( ! ( m_b_l2_resurrected   = m_tree->GetBranch( m_prefix + "L2_resurrected" ) ) ) ||
             ( ! ( m_b_ef_resurrected   = m_tree->GetBranch( m_prefix + "EF_resurrected" ) ) ) ||
             ( ! ( m_b_l2_passedThrough = m_tree->GetBranch( m_prefix + "L2_passedThrough" ) ) ) ||
             ( ! ( m_b_ef_passedThrough = m_tree->GetBranch( m_prefix + "EF_passedThrough" ) ) ) ) {
            // Apparently the D3PD was created with detail level 1...
            return kTRUE;
         }

         m_tree->SetBranchAddress( m_prefix + "L2_resurrected",   &m_l2_resurrected,
                                   &m_b_l2_resurrected );
         m_tree->SetBranchAddress( m_prefix + "EF_resurrected",   &m_ef_resurrected,
                                   &m_b_ef_resurrected );
         m_tree->SetBranchAddress( m_prefix + "L2_passedThrough", &m_l2_passedThrough,
                                   &m_b_l2_passedThrough );
         m_tree->SetBranchAddress( m_prefix + "EF_passedThrough", &m_ef_passedThrough,
                                   &m_b_ef_passedThrough );

         m_detailLevel = 2;

         return kTRUE;
      }

      void DataAccess::Reset() {

         m_initialized = kFALSE;
         m_detailLevel = -1;

         m_b_smk = 0;
         m_b_l1psk = 0;
         m_b_hltpsk = 0;

         if( m_l1_tav )     delete m_l1_tav; m_l1_tav = 0;
         if( m_l2_physics ) delete m_l2_physics; m_l2_physics = 0;
         if( m_ef_physics ) delete m_ef_physics; m_ef_physics = 0;

         m_b_l1_tav = 0;
         m_b_l2_physics = 0;
         m_b_ef_physics = 0;

         if( m_l1_tbp ) delete m_l1_tbp; m_l1_tbp = 0;
         if( m_l1_tap ) delete m_l1_tap; m_l1_tap = 0;
         if( m_l2_raw ) delete m_l2_raw; m_l2_raw = 0;
         if( m_ef_raw ) delete m_ef_raw; m_ef_raw = 0;

         m_b_l1_tbp = 0;
         m_b_l1_tap = 0;
         m_b_l2_raw = 0;
         m_b_ef_raw = 0;

         if( m_l2_resurrected ) delete m_l2_resurrected; m_l2_resurrected = 0;
         if( m_ef_resurrected ) delete m_ef_resurrected; m_ef_resurrected = 0;

         m_b_l2_resurrected = 0;
         m_b_ef_resurrected = 0;

         if( m_l2_passedThrough ) delete m_l2_passedThrough; m_l2_passedThrough = 0;
         if( m_ef_passedThrough ) delete m_ef_passedThrough; m_ef_passedThrough = 0;

         m_b_l2_passedThrough = 0;
         m_b_ef_passedThrough = 0;

         return;
      }

   } // namespace Trig

} // namespace D3PD
