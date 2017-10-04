/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Converter.cxx 707664 2015-11-13 05:36:00Z ssnyder $

// STL include(s):
#include <map>
#include <iomanip>
#include <sstream>

// TrigT1 include(s):
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"

// Local include(s):
#include "Converter.h"
#include "BitOp.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Declaration of the constants:
   const unsigned int Converter::RoIandOverlapMask     = 0x000007fc;
   const unsigned int Converter::PtMask                = 0x00003800;
   const unsigned int Converter::RoI_SectorAddressMask = 0x003fc000;
   const unsigned int Converter::RDO_SectorAddressMask = 0x01fe0000;
   const unsigned int Converter::RoI_FirstCandMask     = 0x00400000;
   const unsigned int Converter::RDO_FirstCandMask     = 0x02000000;
   const unsigned int Converter::MoreCandInRoI         = 0x00000002;
   const unsigned int Converter::MoreCandInSec         = 0x00000001;
   const unsigned int Converter::Cand1InverseMask      = 0xfdc7fc00;
   const unsigned int Converter::Cand2InverseMask      = 0xfa3803fe;

   StatusCode Converter::convertRoIs( const std::vector< unsigned int >& roi_vector,
                                      LVL1MUONIF::Lvl1MuCTPIInput* muctpi_input ) {

      MsgLogger logger( "Converter::convertRoIs()" );

      // Maps needed for the translation:
      std::map< unsigned int, HelperRoIBarrelSector >  barrel_map;
      std::map< unsigned int, HelperRoIEndcapSector >  endcap_map;
      std::map< unsigned int, HelperRoIForwardSector > forward_map;

      ////////////////////////////////////////////////////////////////////////////////
      //                                                                            //
      //  Iterate through all the RoI words, and fill the maps with data extracted  //
      //  from them.                                                                //
      //                                                                            //
      ////////////////////////////////////////////////////////////////////////////////

      std::vector< unsigned int >::const_iterator it_roi_vector  = roi_vector.begin();
      std::vector< unsigned int >::const_iterator end_roi_vector = roi_vector.end();
      for( ; it_roi_vector != end_roi_vector; ++it_roi_vector ) {

         SectorType type = getSectorType( getRoISectorAddress( *it_roi_vector ) );

         if( type == Barrel ) {
            REPORT_VERBOSE_MSG2( "Barrel sector candidate found : 0x" << std::hex
                                 << std::setw( 8 ) << std::setfill( '0' )
                                 << *it_roi_vector,
                                 logger);

            if( isRoIFirstCand( *it_roi_vector ) ) {
               barrel_map[ getRoISectorAddress( *it_roi_vector ) ].addFirst( *it_roi_vector );
            } else {
               barrel_map[ getRoISectorAddress( *it_roi_vector ) ].addSecond( *it_roi_vector );
            }

         } else if( type == Endcap ) {

            REPORT_VERBOSE_MSG2( "Endcap sector candidate found : 0x" << std::hex
                                 << std::setw( 8 ) << std::setfill( '0' )
                                 << *it_roi_vector,
                                 logger);

            if( isRoIFirstCand( *it_roi_vector ) ) {
               endcap_map[ getRoISectorAddress( *it_roi_vector ) ].addFirst( *it_roi_vector );
            } else {
               endcap_map[ getRoISectorAddress( *it_roi_vector ) ].addSecond( *it_roi_vector );
            }

         } else if( type == Forward ) {

            REPORT_VERBOSE_MSG2( "Forward sector candidate found : 0x" << std::hex
                                 << std::setw( 8 ) << std::setfill( '0' )
                                 << *it_roi_vector,
                                 logger);

            if( isRoIFirstCand( *it_roi_vector ) ) {
               forward_map[ getRoISectorAddress( *it_roi_vector ) ].addFirst( *it_roi_vector );
            } else {
               forward_map[ getRoISectorAddress( *it_roi_vector ) ].addSecond( *it_roi_vector );
            }

         } else {

            REPORT_FATAL_MSG2( "Coding error found. System of candidate not recognized.",
                               logger);
            return StatusCode::FAILURE;
         }
      }

      ////////////////////////////////////////////////////////////////////////////////
      //                                                                            //
      //     Now iterate through the maps and fill the MuCTPI input object.         //
      //                                                                            //
      ////////////////////////////////////////////////////////////////////////////////

      std::map< unsigned int, HelperRoIBarrelSector >::const_iterator it_barrel_map =
         barrel_map.begin();
      std::map< unsigned int, HelperRoIBarrelSector >::const_iterator end_barrel_map =
         barrel_map.end();
      for( ; it_barrel_map != end_barrel_map; ++it_barrel_map ) {

          REPORT_MSG2(DEBUG, "Adding sector logic word:"  << std::endl << it_barrel_map->second.toString(), logger);

         if( it_barrel_map->second.subSystem() == HelperRoISector::A_side ) {

            muctpi_input->setSectorLogicData( it_barrel_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idBarrelSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideA(),
                                              it_barrel_map->second.sectorID() );

         } else if( it_barrel_map->second.subSystem() == HelperRoISector::C_side ) {

            muctpi_input->setSectorLogicData( it_barrel_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idBarrelSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideC(),
                                              it_barrel_map->second.sectorID() );

         } else {

            REPORT_FATAL_MSG2( "Code error detected in Converter. --> Aborting conversion!", logger );
            return StatusCode::FAILURE;
         }
      }

      std::map< unsigned int, HelperRoIEndcapSector >::const_iterator it_endcap_map =
         endcap_map.begin();
      std::map< unsigned int, HelperRoIEndcapSector >::const_iterator end_endcap_map =
         endcap_map.end();
      for( ; it_endcap_map != end_endcap_map; ++it_endcap_map ) {

          REPORT_MSG2(DEBUG, "Adding sector logic word:" << std::endl << it_endcap_map->second.toString(), logger);

         if( it_endcap_map->second.subSystem() == HelperRoISector::A_side ) {

            muctpi_input->setSectorLogicData( it_endcap_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideA(),
                                              it_endcap_map->second.sectorID() );

         } else if( it_endcap_map->second.subSystem() == HelperRoISector::C_side ) {

            muctpi_input->setSectorLogicData( it_endcap_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideC(),
                                              it_endcap_map->second.sectorID() );

         } else {

            REPORT_FATAL_MSG2( "Code error detected in Converter. --> Aborting conversion!", logger );
            return StatusCode::FAILURE;
         }
      }

      std::map< unsigned int, HelperRoIForwardSector >::const_iterator it_forward_map =
         forward_map.begin();
      std::map< unsigned int, HelperRoIForwardSector >::const_iterator end_forward_map =
         forward_map.end();
      for( ; it_forward_map != end_forward_map; ++it_forward_map ) {

          REPORT_MSG2(DEBUG, "Adding sector logic word:" << std::endl << it_forward_map->second.toString(), logger);

         if( it_forward_map->second.subSystem() == HelperRoISector::A_side ) {

            muctpi_input->setSectorLogicData( it_forward_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idForwardSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideA(),
                                              it_forward_map->second.sectorID() );

         } else if( it_forward_map->second.subSystem() == HelperRoISector::C_side ) {

            muctpi_input->setSectorLogicData( it_forward_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idForwardSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideC(),
                                              it_forward_map->second.sectorID() );

         } else {

            REPORT_FATAL_MSG2( "Code error detected in Converter. --> Aborting conversion!", logger );
            return StatusCode::FAILURE;
         }
      }

      return StatusCode::SUCCESS;
   }

   StatusCode Converter::convertRDO( const std::vector< unsigned int >& rdo_vector,
                                     unsigned int bcid,
                                     LVL1MUONIF::Lvl1MuCTPIInput* muctpi_input ) {

      MsgLogger logger( "Converter::convertRDO()" );

      // Maps needed for the translation:
      std::map< unsigned int, HelperRDOBarrelSector >  barrel_map;
      std::map< unsigned int, HelperRDOEndcapSector >  endcap_map;
      std::map< unsigned int, HelperRDOForwardSector > forward_map;

      ////////////////////////////////////////////////////////////////////////////////
      //                                                                            //
      //  Iterate through all the RDO words, and fill the maps with data extracted  //
      //  from them.                                                                //
      //                                                                            //
      ////////////////////////////////////////////////////////////////////////////////

      std::vector< unsigned int >::const_iterator it_rdo_vector  = rdo_vector.begin();
      std::vector< unsigned int >::const_iterator end_rdo_vector = rdo_vector.end();
      for( ; it_rdo_vector != end_rdo_vector; ++it_rdo_vector ) {

         // Only consider the candidates with the correct BCID:
         if( getRDOBCID( *it_rdo_vector ) != bcid ) {
            REPORT_VERBOSE_MSG2( "Discarding candidate with wrong BCID", logger );
            continue;
         }

         SectorType type = getSectorType( getRDOSectorAddress( *it_rdo_vector ) );

         if( type == Barrel ) {

            REPORT_VERBOSE_MSG2( "Barrel sector candidate found : 0x" << std::hex
                                 << std::setw( 8 ) << std::setfill( '0' )
                                 << *it_rdo_vector,
                                 logger);

            if( isRDOFirstCand( *it_rdo_vector ) ) {
               barrel_map[ getRDOSectorAddress( *it_rdo_vector ) ].addFirst( *it_rdo_vector );
            } else {
               barrel_map[ getRDOSectorAddress( *it_rdo_vector ) ].addSecond( *it_rdo_vector );
            }

         } else if( type == Endcap ) {

            REPORT_VERBOSE_MSG2( "Endcap sector candidate found : 0x" << std::hex
                                 << std::setw( 8 ) << std::setfill( '0' )
                                 << *it_rdo_vector,
                                 logger);

            if( isRDOFirstCand( *it_rdo_vector ) ) {
               endcap_map[ getRDOSectorAddress( *it_rdo_vector ) ].addFirst( *it_rdo_vector );
            } else {
               endcap_map[ getRDOSectorAddress( *it_rdo_vector ) ].addSecond( *it_rdo_vector );
            }

         } else if( type == Forward ) {

            REPORT_VERBOSE_MSG2( "Forward sector candidate found : 0x" << std::hex
                                 << std::setw( 8 ) << std::setfill( '0' )
                                 << *it_rdo_vector,
                                 logger);

            if( isRDOFirstCand( *it_rdo_vector ) ) {
               forward_map[ getRDOSectorAddress( *it_rdo_vector ) ].addFirst( *it_rdo_vector );
            } else {
               forward_map[ getRDOSectorAddress( *it_rdo_vector ) ].addSecond( *it_rdo_vector );
            }

         } else {

            REPORT_FATAL_MSG2( "Coding error found. System of candidate not recognized.", logger);
            return StatusCode::FAILURE;
         }
      }

      ////////////////////////////////////////////////////////////////////////////////
      //                                                                            //
      //     Now iterate through the maps and fill the MuCTPI input object.         //
      //                                                                            //
      ////////////////////////////////////////////////////////////////////////////////

      std::map< unsigned int, HelperRDOBarrelSector >::const_iterator it_barrel_map =
         barrel_map.begin();
      std::map< unsigned int, HelperRDOBarrelSector >::const_iterator end_barrel_map =
         barrel_map.end();
      for( ; it_barrel_map != end_barrel_map; ++it_barrel_map ) {

         REPORT_MSG2(DEBUG, "Adding sector logic word:" << std::endl << it_barrel_map->second.toString(), logger);

         if( it_barrel_map->second.subSystem() == HelperRDOSector::A_side ) {

            muctpi_input->setSectorLogicData( it_barrel_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idBarrelSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideA(),
                                              it_barrel_map->second.sectorID() );

         } else if( it_barrel_map->second.subSystem() == HelperRDOSector::C_side ) {

            muctpi_input->setSectorLogicData( it_barrel_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idBarrelSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideC(),
                                              it_barrel_map->second.sectorID() );

         } else {

            REPORT_FATAL_MSG2( "Code error detected in Converter. --> Aborting conversion!", logger );
            return StatusCode::FAILURE;
         }
      }

      std::map< unsigned int, HelperRDOEndcapSector >::const_iterator it_endcap_map =
         endcap_map.begin();
      std::map< unsigned int, HelperRDOEndcapSector >::const_iterator end_endcap_map =
         endcap_map.end();
      for( ; it_endcap_map != end_endcap_map; ++it_endcap_map ) {

         REPORT_MSG2(DEBUG, "Adding sector logic word:" << std::endl << it_endcap_map->second.toString(), logger);

         if( it_endcap_map->second.subSystem() == HelperRDOSector::A_side ) {

            muctpi_input->setSectorLogicData( it_endcap_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideA(),
                                              it_endcap_map->second.sectorID() );

         } else if( it_endcap_map->second.subSystem() == HelperRDOSector::C_side ) {

            muctpi_input->setSectorLogicData( it_endcap_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideC(),
                                              it_endcap_map->second.sectorID() );

         } else {

            REPORT_FATAL_MSG2( "Code error detected in Converter. --> Aborting conversion!", logger );
            return StatusCode::FAILURE;
         }
      }

      std::map< unsigned int, HelperRDOForwardSector >::const_iterator it_forward_map =
         forward_map.begin();
      std::map< unsigned int, HelperRDOForwardSector >::const_iterator end_forward_map =
         forward_map.end();
      for( ; it_forward_map != end_forward_map; ++it_forward_map ) {

         REPORT_MSG2(DEBUG, "Adding sector logic word:" << std::endl << it_forward_map->second.toString(), logger);

         if( it_forward_map->second.subSystem() == HelperRDOSector::A_side ) {

            muctpi_input->setSectorLogicData( it_forward_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idForwardSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideA(),
                                              it_forward_map->second.sectorID() );

         } else if( it_forward_map->second.subSystem() == HelperRDOSector::C_side ) {

            muctpi_input->setSectorLogicData( it_forward_map->second.sectorWord(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idForwardSystem(),
                                              LVL1MUONIF::Lvl1MuCTPIInput::idSideC(),
                                              it_forward_map->second.sectorID() );

         } else {

            REPORT_FATAL_MSG2( "Code error detected in Converter. --> Aborting conversion!", logger );
            return StatusCode::FAILURE;
         }
      }

      return StatusCode::SUCCESS;
   }

   Converter::SectorType Converter::getSectorType( unsigned int sector_address ) {

      if( sector_address & 0x80 ) {
         return Endcap;
      } else if( sector_address & 0x40 ) {
         return Forward;
      } else {
         return Barrel;
      }
   }

   unsigned int Converter::getRoISectorAddress( unsigned int roi_word ) {

      return BitOp::getValue( &roi_word, RoI_SectorAddressMask );
   }

   bool Converter::isRoIFirstCand( unsigned int roi_word ) {

      return ( roi_word & RoI_FirstCandMask );
   }

   unsigned int Converter::getRDOSectorAddress( unsigned int rdo_word ) {

      return BitOp::getValue( &rdo_word, RDO_SectorAddressMask );
   }

   unsigned int Converter::getRDOBCID( unsigned int rdo_word ) {

      return ( ( rdo_word >> 14 ) & 0x7 );
   }

   bool Converter::isRDOFirstCand( unsigned int rdo_word ) {

      return ( rdo_word & RDO_FirstCandMask );
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperSector class                           //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   Converter::HelperSector&
   Converter::HelperSector::operator= ( const Converter::HelperSector& right ) {

     if (this != &right) {

       this->m_word   = right.m_word;
       this->m_sector = right.m_sector;
     }
     return ( *this );
   }

   Converter::HelperSector::MuonSubSystem Converter::HelperSector::subSystem() const {

      if( BitOp::isSet( &m_sector, 0 ) ) return A_side;
      else return C_side;
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperRoISector class                        //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   void Converter::HelperRoISector::addFirst( unsigned int roi_word ) {

      m_word &= Cand1InverseMask; // Zero out all quantities of the first candidate

      BitOp::sImposeNBits( &m_word, 1, BitOp::getValue( &roi_word, Converter::RoIandOverlapMask ) ); // RoI and overlap bits
      BitOp::sImposeNBits( &m_word, 19, BitOp::getValue( &roi_word, Converter::PtMask ) );           // Pt
      if( roi_word & Converter::MoreCandInSec ) BitOp::setBit( &m_word, 0 );                         // More than two cand. in sec.
      if( roi_word & Converter::MoreCandInRoI ) BitOp::setBit( &m_word, 25 );                        // More than two cand. in RoI

      BitOp::sImposeNBits( &m_sector, 0, BitOp::getValue( &roi_word, Converter::RoI_SectorAddressMask ) );

      return;
   }

   void Converter::HelperRoISector::addSecond( unsigned int roi_word ) {

      m_word &= Cand2InverseMask; // Zero out all quantities of the second candidate

      BitOp::sImposeNBits( &m_word, 10, BitOp::getValue( &roi_word, Converter::RoIandOverlapMask ) ); // RoI and overlap bits
      BitOp::sImposeNBits( &m_word, 22, BitOp::getValue( &roi_word, Converter::PtMask ) );            // Pt
      if( roi_word & Converter::MoreCandInSec ) BitOp::setBit( &m_word, 0 );                          // More than two cand. in sec.
      if( roi_word & Converter::MoreCandInRoI ) BitOp::setBit( &m_word, 26 );                         // More than two cand. in RoI

      BitOp::sImposeNBits( &m_sector, 0, BitOp::getValue( &roi_word, Converter::RoI_SectorAddressMask ) );

      return;
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperRoIBarrelSector class                  //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   unsigned int Converter::HelperRoIBarrelSector::sectorID() const {

      return BitOp::getValue( &m_sector, 0x3e );
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperRoIEndcapSector class                  //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   unsigned int Converter::HelperRoIEndcapSector::sectorID() const {

      return BitOp::getValue( &m_sector, 0x7e );
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperRoIForwardSector class                 //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   unsigned int Converter::HelperRoIForwardSector::sectorID() const {

      return BitOp::getValue( &m_sector, 0x3e );
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperRDOSector class                        //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   void Converter::HelperRDOSector::addFirst( unsigned int rdo_word ) {

      m_word &= Cand1InverseMask; // Zero out all quantities of the first candidate

      BitOp::sImposeNBits( &m_word, 1, BitOp::getValue( &rdo_word, Converter::RoIandOverlapMask ) ); // RoI and overlap bits
      BitOp::sImposeNBits( &m_word, 19, BitOp::getValue( &rdo_word, Converter::PtMask ) );           // Pt
      if( rdo_word & Converter::MoreCandInSec ) BitOp::setBit( &m_word, 0 );                         // More than two cand. in sec.
      if( rdo_word & Converter::MoreCandInRoI ) BitOp::setBit( &m_word, 25 );                        // More than two cand. in RoI

      BitOp::sImposeNBits( &m_sector, 0, BitOp::getValue( &rdo_word, Converter::RDO_SectorAddressMask ) );

      return;
   }

   void Converter::HelperRDOSector::addSecond( unsigned int rdo_word ) {

      m_word &= Cand2InverseMask; // Zero out all quantities of the second candidate

      BitOp::sImposeNBits( &m_word, 10, BitOp::getValue( &rdo_word, Converter::RoIandOverlapMask ) ); // RoI and overlap bits
      BitOp::sImposeNBits( &m_word, 22, BitOp::getValue( &rdo_word, Converter::PtMask ) );            // Pt
      if( rdo_word & Converter::MoreCandInSec ) BitOp::setBit( &m_word, 0 );                          // More than two cand. in sec.
      if( rdo_word & Converter::MoreCandInRoI ) BitOp::setBit( &m_word, 26 );                         // More than two cand. in RoI

      BitOp::sImposeNBits( &m_sector, 0, BitOp::getValue( &rdo_word, Converter::RDO_SectorAddressMask ) );

      return;
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperRDOBarrelSector class                  //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   unsigned int Converter::HelperRDOBarrelSector::sectorID() const {

      return BitOp::getValue( &m_sector, 0x3e );
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperRDOEndcapSector class                  //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   unsigned int Converter::HelperRDOEndcapSector::sectorID() const {

      return BitOp::getValue( &m_sector, 0x7e );
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Functions of the Converter::HelperRDOForwardSector class                 //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

   unsigned int Converter::HelperRDOForwardSector::sectorID() const {

      return BitOp::getValue( &m_sector, 0x3e );
   }

   ////////////////////////////////////////////////////////////////////////////////
   //                                                                            //
   //   Output operator for helper classes                                       //
   //                                                                            //
   ////////////////////////////////////////////////////////////////////////////////

std::string
Converter::HelperSector::toString() const {

      std::ostringstream output;
      output << " ------ sector word ------" << std::endl;
      output << "  system     = " << system() << std::endl;
      output << "  m_word     = 0x" << std::hex << std::setw(8) << std::setfill('0')
             << m_word << std::endl;
      output << "  m_sector   = 0x" << std::hex << std::setw(2) << std::setfill('0')
             << m_sector << std::endl;
      output << "  sectorID   = " << std::dec << sectorID() << std::endl;
      output << "  sub-system = ";
      if( subSystem() == Converter::HelperRoISector::A_side ) {
         output << "A side" << std::endl;
      } else if( subSystem() == Converter::HelperRoISector::C_side ) {
         output << "C side" << std::endl;
      }
      output << " -------------------------";

      return output.str();
   }

} // namespace LVL1MUCTPI
