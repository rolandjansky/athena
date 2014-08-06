/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuCTPI_RDOFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigConfL1Data/TriggerThreshold.h"

// Local include(s):
#include "MuCTPI_RDOFillerTool.h"

namespace D3PD {

   MuCTPI_RDOFillerTool::MuCTPI_RDOFillerTool( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
      : BlockFillerTool< MuCTPI_RDO >( type, name, parent ),
        m_rpcRoiSvc( "LVL1RPC::RPCRecRoiSvc", name ),
        m_tgcRoiSvc( "LVL1TGC::TGCRecRoiSvc", name )
   {
      // Avoid coverity warnings
      m_writeRawInfo = true;
      m_writeReconstructedInfo = true;
      book().ignore();

      declareProperty( "WriteRawInfo", m_writeRawInfo = false );
      declareProperty( "WriteReconstructedInfo", m_writeReconstructedInfo = false );

      declareProperty( "RPCRecRoiSvc", m_rpcRoiSvc );
      declareProperty( "TGCRecRoiSvc", m_tgcRoiSvc );
   }

   StatusCode MuCTPI_RDOFillerTool::initialize() {

      // Call the initialization of the base class:
      CHECK( BlockFillerTool< MuCTPI_RDO >::initialize() );

      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      if( m_writeReconstructedInfo ) {
         CHECK( m_rpcRoiSvc.retrieve() );
         CHECK( m_tgcRoiSvc.retrieve() );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode MuCTPI_RDOFillerTool::book() {

      if( m_writeRawInfo ) {
         CHECK( addVariable( "candMultiplicity", m_candidateMultiplicities,
                             "Muon candidate multiplicity for the trigger bunch crossing" ) );
         CHECK( addVariable( "nDataWords", m_nDataWords,
                             "Number of muon candidate data words" ) );
         CHECK( addVariable( "dataWords", m_dataWords,
                             "Muon candidate data words" ) );
      }
      if( m_writeReconstructedInfo ) {
         CHECK( addVariable( "dw_eta", m_dw_eta,
                             "Eta direction of the muon candidate" ) );
         CHECK( addVariable( "dw_phi", m_dw_phi,
                             "Phi direction of the muon candidate" ) );
         CHECK( addVariable( "dw_source", m_dw_source,
                             "Muon candidate system ID (Barrel=0, Endcap=1, Forward=2)" ) );
         CHECK( addVariable( "dw_hemisphere", m_dw_hemisphere,
                             "Muon candidate hemisphere (-z=0, +z=1)" ) );
         CHECK( addVariable( "dw_bcid", m_dw_bcid,
                             "Bunch crossing ID of the muon candidate" ) );
         CHECK( addVariable( "dw_sectorID", m_dw_sectorID,
                             "Sector number of the muon candidate" ) );
         CHECK( addVariable( "dw_thrNumber", m_dw_thrNumber,
                             "Threshold number of the muon candidate" ) );
         CHECK( addVariable( "dw_RoINumber", m_dw_roi,
                             "RoI (pad) number of the muon candidate" ) );
         CHECK( addVariable( "dw_overlapFlags", m_dw_overlapFlags,
                             "Overlap flags set by the sector logic" ) );
         CHECK( addVariable( "dw_firstCandidate", m_dw_firstCandidate,
                             "True if this was the highest pT candidate in its sector" ) );
         CHECK( addVariable( "dw_moreCandInRoI", m_dw_moreCandInRoI,
                             "True if there were >1 candidates in its pad" ) );
         CHECK( addVariable( "dw_moreCandInSector", m_dw_moreCandInSector,
                             "True if there were >2 candidates in its sector" ) );
         CHECK( addVariable( "dw_charge", m_dw_charge,
                             "Candidate sign (0=negative,1=positive,100=undefined)" ) );
         CHECK( addVariable( "dw_vetoed", m_dw_vetoed,
                             "Candidate vetoed in multiplicity sum" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode MuCTPI_RDOFillerTool::fill( const MuCTPI_RDO& rdo ) {

      if( m_writeRawInfo ) {
         *m_candidateMultiplicities = rdo.getAllCandidateMultiplicities();
         *m_nDataWords = rdo.dataWord().size();
         *m_dataWords = rdo.dataWord();
      }
      if( m_writeReconstructedInfo ) {

         // Clear all the vectors by hand, as the framework is not taking
         // care about it in this case...
         m_dw_eta->clear(); m_dw_phi->clear(); m_dw_source->clear();
         m_dw_hemisphere->clear(); m_dw_bcid->clear(); m_dw_sectorID->clear();
         m_dw_thrNumber->clear(); m_dw_roi->clear(); m_dw_overlapFlags->clear();
         m_dw_firstCandidate->clear(); m_dw_moreCandInRoI->clear();
         m_dw_moreCandInSector->clear(); m_dw_charge->clear();
         m_dw_vetoed->clear();

         // Create some dummy LVL1 muon thresholds:
         std::vector< TrigConf::TriggerThreshold* > dummy_thresholds;

         // Loop over the MuCTPI data words, and "reconstruct" them:
         std::vector< uint32_t >::const_iterator dw_itr = rdo.dataWord().begin();
         std::vector< uint32_t >::const_iterator dw_end = rdo.dataWord().end();
         for( ; dw_itr != dw_end; ++dw_itr ) {

            // Use the same class that is used by the LVL2 steering to decode
            // the muon RoIs:
            LVL1::RecMuonRoI roi( convertToRoIWord( *dw_itr ),
                                  &( *m_rpcRoiSvc ), &( *m_tgcRoiSvc ),
                                  &dummy_thresholds );

            // Extract all the information about the muon candidate:
            m_dw_eta->push_back( roi.eta() );
            m_dw_phi->push_back( roi.phi() );
            m_dw_source->push_back( roi.sysID() );
            m_dw_hemisphere->push_back( roi.subsysID() );
            m_dw_bcid->push_back( getBCID( *dw_itr ) );
            m_dw_sectorID->push_back( roi.sectorID() );
            m_dw_thrNumber->push_back( roi.getThresholdNumber() );
            m_dw_roi->push_back( roi.getRoINumber() );
            m_dw_overlapFlags->push_back( roi.getOverlap() );
            m_dw_firstCandidate->push_back( roi.firstCandidate() );
            m_dw_moreCandInRoI->push_back( roi.padOverflow() );
            m_dw_moreCandInSector->push_back( roi.sectorOverflow() );
            m_dw_charge->push_back( static_cast< short >( roi.candidateCharge() ) );
            m_dw_vetoed->push_back( roi.candidateVetoed() );

         }

      }

      return StatusCode::SUCCESS;
   }

   /**
    * For more information about the format of the MuCTPI data words and the
    * RoI words, see the document: https://edms.cern.ch/file/248757/1/mirod.pdf
    *
    * See also: https://edms.cern.ch/file/1142589/1/MIROD-Event-Format.pdf
    */
   uint32_t MuCTPI_RDOFillerTool::convertToRoIWord( uint32_t dataWord ) {

      return ( ( dataWord & 0x18000000 ) | ( ( dataWord & 0x3fe0000 ) >> 3 ) |
               ( dataWord & 0x3fff ) );
   }

   /**
    * For more information about the format of the MuCTPI data words and the
    * RoI words, see the document: https://edms.cern.ch/file/248757/1/mirod.pdf
    */
   short MuCTPI_RDOFillerTool::getBCID( uint32_t dataWord ) {

      return ( ( dataWord >> 14 ) & 0x7 );
   }

} // namespace D3PD
