/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AthDataAccess.cxx 726661 2016-02-28 01:18:32Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "AthDataAccess.h"

namespace D3PD {

   AthDataAccess::AthDataAccess() {

      setPrefix( "trig_" );
   }

   ::Int_t AthDataAccess::GetDetailLevel() {

      // Check for the detail level 0 branches:
      if( ( ! m_l1_tav.cptr() ) ||
          ( ! m_l2_physics.cptr() ) ||
          ( ! m_ef_physics.cptr() ) ) {

         REPORT_MESSAGE( MSG::ERROR )
            << "The input D3PD doesn't seem to contain trigger decision data";
         return -1;

      } else if( ( ! m_l1_tbp.cptr() ) ||
                 ( ! m_l1_tap.cptr() ) ||
                 ( ! m_l2_raw.cptr() ) ||
                 ( ! m_ef_raw.cptr() ) ) {

         // The D3PD contains branches with detail level 0:
         return 0;

      } else if( ( ! m_l2_resurrected.cptr() ) ||
                 ( ! m_ef_resurrected.cptr() ) ||
                 ( ! m_l2_passedThrough.cptr() ) ||
                 ( ! m_ef_passedThrough.cptr() ) ) {

         // The D3PD contains branches with detail level 1:
         return 1;

      }

      // This is the full Monty, detail level 2:
      return 2;
   }

   ::Int_t AthDataAccess::GetSMK() {

      return static_cast< ::Int_t >( *m_smk );
   }

   ::Int_t AthDataAccess::GetL1PSK() {

      return static_cast< ::Int_t >( *m_l1psk );
   }

   ::Int_t AthDataAccess::GetHLTPSK() {

      return static_cast< ::Int_t >( *m_hltpsk );
   }

   const std::vector< unsigned int >*
   AthDataAccess::GetL1Result( L1ResultType type ) {

      switch( type ) {

      case TBP:
         return m_l1_tbp.cptr();
         break;
      case TAP:
         return m_l1_tap.cptr();
         break;
      case TAV:
         return m_l1_tav.cptr();
         break;
      default:
         REPORT_MESSAGE( MSG::ERROR )
            << "Unknown result type (" << type << ") requested";
         return 0;
         break;
      }
   }

   const std::vector< short >*
   AthDataAccess::GetL2Result( HLTResultType type ) {

      switch( type ) {

      case Physics:
         return m_l2_physics.cptr();
         break;
      case Raw:
         return m_l2_raw.cptr();
         break;
      case Resurrected:
         return m_l2_resurrected.cptr();
         break;
      case PassedThrough:
         return m_l2_passedThrough.cptr();
         break;
      default:
         REPORT_MESSAGE( MSG::ERROR )
            << "Unknown result type (" << type << ") requested";
         return 0;
         break;
      }
   }

   const std::vector< short >*
   AthDataAccess::GetEFResult( HLTResultType type ) {

      switch( type ) {

      case Physics:
         return m_ef_physics.cptr();
         break;
      case Raw:
         return m_ef_raw.cptr();
         break;
      case Resurrected:
         return m_ef_resurrected.cptr();
         break;
      case PassedThrough:
         return m_ef_passedThrough.cptr();
         break;
      default:
         REPORT_MESSAGE( MSG::ERROR )
            << "Unknown result type (" << type << ") requested";
         return 0;
         break;
      }
   }

   void AthDataAccess::setPrefix( const std::string& prefix ) {

      m_smk    = SG::RVar< ::UInt_t >( prefix + "DB_SMK" );
      m_l1psk  = SG::RVar< ::UInt_t >( prefix + "DB_L1PSK" );
      m_hltpsk = SG::RVar< ::UInt_t >( prefix + "DB_HLTPSK" );

      m_l1_tav     = SG::RVar< std::vector< unsigned int > >( prefix +
                                                              "L1_TAV" );
      m_l2_physics = SG::RVar< std::vector< short > >( prefix +
                                                       "L2_passedPhysics" );
      m_ef_physics = SG::RVar< std::vector< short > >( prefix +
                                                       "EF_passedPhysics" );

      m_l1_tbp = SG::RVar< std::vector< unsigned int > >( prefix +
                                                          "L1_TBP" );
      m_l1_tap = SG::RVar< std::vector< unsigned int > >( prefix +
                                                          "L1_TAP" );
      m_l2_raw = SG::RVar< std::vector< short > >( prefix +
                                                   "L2_passedRaw" );
      m_ef_raw = SG::RVar< std::vector< short > >( prefix +
                                                   "EF_passedRaw" );

      m_l2_resurrected = SG::RVar< std::vector< short > >( prefix +
                                                           "L2_resurrected" );
      m_ef_resurrected = SG::RVar< std::vector< short > >( prefix +
                                                           "EF_resurrected" );

      m_l2_passedThrough = SG::RVar< std::vector< short > >( prefix +
                                                             "L2_passedThrough" );
      m_ef_passedThrough = SG::RVar< std::vector< short > >( prefix +
                                                             "EF_passedThrough" );

      return;
   }

} // namespace D3PD
