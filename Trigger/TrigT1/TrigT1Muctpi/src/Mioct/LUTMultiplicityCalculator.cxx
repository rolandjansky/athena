/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LUTMultiplicityCalculator.cxx 650693 2015-03-01 16:53:48Z masato $

// STL include(s):
#include <sstream>

// XercesC include(s):
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "LUTMultiplicityCalculator.h"
#include "MioctSectorSet.h"
#include "MultiplicityLogic.h"

namespace LVL1MUCTPI {

   LUTMultiplicityCalculator::LUTMultiplicityCalculator( const MultiplicityLogic* logic )
      : m_logger( "LUTMultiplicityCalculator" ), m_multiplicityLogic( logic ),
        m_flagMode( false ) {

   }

   LUTMultiplicityCalculator::LUTMultiplicityCalculator( const LUTMultiplicityCalculator& calc )
      : m_logger( calc.m_logger ),
        m_multiplicityLogic( calc.m_multiplicityLogic ), m_flagMode( calc.m_flagMode ),
        m_bb_calc_1( calc.m_bb_calc_1 ),
        m_bb_calc_2( calc.m_bb_calc_2 ), m_ee_calc_1( calc.m_ee_calc_1 ),
        m_ee_calc_2( calc.m_ee_calc_2 ), m_ee_calc_3( calc.m_ee_calc_3 ),
        m_ee_calc_4( calc.m_ee_calc_4 ), m_ee_calc_5( calc.m_ee_calc_5 ),
        m_ff_calc_1( calc.m_ff_calc_1 ), m_ff_calc_2( calc.m_ff_calc_2 ),
        m_be_calc_11( calc.m_be_calc_11 ), m_be_calc_12( calc.m_be_calc_12 ),
        m_be_calc_22( calc.m_be_calc_22 ), m_be_calc_23( calc.m_be_calc_23 ),
        m_be_calc_34( calc.m_be_calc_34 ), m_be_calc_35( calc.m_be_calc_35 ),
        m_be_calc_45( calc.m_be_calc_45 ), m_be_calc_46( calc.m_be_calc_46 ),
        m_ef_calc_21( calc.m_ef_calc_21 ), m_ef_calc_31( calc.m_ef_calc_31 ),
        m_ef_calc_42( calc.m_ef_calc_42 ), m_ef_calc_52( calc.m_ef_calc_52 ),
        m_ef_calc_63( calc.m_ef_calc_63 ) {

   }

   StatusCode LUTMultiplicityCalculator::initialize( const xercesc::DOMNode* node, bool flagMode,
                                                     bool dumpLut,const std::string& runPeriod ) {

      //
      // Remember which mode was requested:
      //
      m_flagMode = flagMode;

      //
      // Check if the received node "makes sense":
      //
      if( node->getNodeType() != xercesc::DOMNode::ELEMENT_NODE ) {
         REPORT_ERROR_MSG( "Received node is not a DOMElement" );
         return StatusCode::RECOVERABLE;
      }

      //
      // Loop over all the nodes in this XML node:
      //
      const xercesc::DOMNodeList* lut_node_list = node->getChildNodes();
      for( XMLSize_t i = 0; i < lut_node_list->getLength(); ++i ) {

         const xercesc::DOMNode* lut_node = lut_node_list->item( i );
         char* cname = xercesc::XMLString::transcode( lut_node->getNodeName() );
         std::string sname( cname );
         xercesc::XMLString::release( &cname );

         if     ( sname == "BB1LUT" )  CHECK( m_bb_calc_1.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BB2LUT" )  CHECK( m_bb_calc_2.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EE1LUT" )  CHECK( m_ee_calc_1.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EE2LUT" )  CHECK( m_ee_calc_2.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EE3LUT" )  CHECK( m_ee_calc_3.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EE4LUT" )  CHECK( m_ee_calc_4.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EE5LUT" )  CHECK( m_ee_calc_5.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "FF1LUT" )  CHECK( m_ff_calc_1.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "FF2LUT" )  CHECK( m_ff_calc_2.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BE11LUT" ) CHECK( m_be_calc_11.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BE12LUT" ) CHECK( m_be_calc_12.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BE22LUT" ) CHECK( m_be_calc_22.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BE23LUT" ) CHECK( m_be_calc_23.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BE34LUT" ) CHECK( m_be_calc_34.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BE35LUT" ) CHECK( m_be_calc_35.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BE45LUT" ) CHECK( m_be_calc_45.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "BE46LUT" ) CHECK( m_be_calc_46.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EF21LUT" ) CHECK( m_ef_calc_21.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EF31LUT" ) CHECK( m_ef_calc_31.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EF42LUT" ) CHECK( m_ef_calc_42.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EF52LUT" ) CHECK( m_ef_calc_52.initialize( lut_node, dumpLut, runPeriod ) );
         else if( sname == "EF63LUT" ) CHECK( m_ef_calc_63.initialize( lut_node, dumpLut, runPeriod ) );

      }

      return StatusCode::SUCCESS;
   }

   PtMultiplicitySet
   LUTMultiplicityCalculator::calculateMultiplicity( const MioctSectorSet& data ) const {

      //
      // Let's make sure that all the suppression flags are off as a start:
      //
      data.Barrel1.setCand1Supressed( false ); data.Barrel1.setCand2Supressed( false );
      data.Barrel2.setCand1Supressed( false ); data.Barrel2.setCand2Supressed( false );
      data.Barrel3.setCand1Supressed( false ); data.Barrel3.setCand2Supressed( false );
      data.Barrel4.setCand1Supressed( false ); data.Barrel4.setCand2Supressed( false );
      data.Endcap1.setCand1Supressed( false ); data.Endcap1.setCand2Supressed( false );
      data.Endcap2.setCand1Supressed( false ); data.Endcap2.setCand2Supressed( false );
      data.Endcap3.setCand1Supressed( false ); data.Endcap3.setCand2Supressed( false );
      data.Endcap4.setCand1Supressed( false ); data.Endcap4.setCand2Supressed( false );
      data.Endcap5.setCand1Supressed( false ); data.Endcap5.setCand2Supressed( false );
      data.Endcap6.setCand1Supressed( false ); data.Endcap6.setCand2Supressed( false );
      data.Forward1.setCand1Supressed( false ); data.Forward1.setCand2Supressed( false );
      data.Forward2.setCand1Supressed( false ); data.Forward2.setCand2Supressed( false );
      data.Forward3.setCand1Supressed( false ); data.Forward3.setCand2Supressed( false );

      PtMultiplicitySet result;

      //////////////////////////////////////////////////////////////////////////////////////////
      //                                                                                      //
      //       Flag the candidates which should be suppressed in the multiplicity count       //
      //                                                                                      //
      //////////////////////////////////////////////////////////////////////////////////////////

      //
      // Flag barrel-barrel overlaps:
      //
      m_bb_calc_1.calculate( data.Barrel1, data.Barrel2 );
      m_bb_calc_2.calculate( data.Barrel3, data.Barrel4 );

      //
      // Flag endcap-endcap overlaps:
      //
      m_ee_calc_1.calculate( data.Endcap1, data.Endcap2 );
      m_ee_calc_2.calculate( data.Endcap2, data.Endcap3 );
      m_ee_calc_3.calculate( data.Endcap3, data.Endcap4 );
      m_ee_calc_4.calculate( data.Endcap4, data.Endcap5 );
      m_ee_calc_5.calculate( data.Endcap5, data.Endcap6 );

      //
      // Flag forward-forward overlaps:
      //
      m_ff_calc_1.calculate( data.Forward1, data.Forward2 );
      m_ff_calc_2.calculate( data.Forward2, data.Forward3 );

      //
      // Flag barrel-endcap overlaps:
      //
      m_be_calc_11.calculate( data.Barrel1, data.Endcap1 );
      m_be_calc_12.calculate( data.Barrel1, data.Endcap2 );
      m_be_calc_22.calculate( data.Barrel2, data.Endcap2 );
      m_be_calc_23.calculate( data.Barrel2, data.Endcap3 );
      m_be_calc_34.calculate( data.Barrel3, data.Endcap4 );
      m_be_calc_35.calculate( data.Barrel3, data.Endcap5 );
      m_be_calc_45.calculate( data.Barrel4, data.Endcap5 );
      m_be_calc_46.calculate( data.Barrel4, data.Endcap6 );

      //
      // Flag endcap-forward overlaps:
      //
      m_ef_calc_21.calculate( data.Endcap2, data.Forward1 );
      m_ef_calc_31.calculate( data.Endcap3, data.Forward1 );
      m_ef_calc_42.calculate( data.Endcap4, data.Forward2 );
      m_ef_calc_52.calculate( data.Endcap5, data.Forward2 );
      m_ef_calc_63.calculate( data.Endcap6, data.Forward3 );

      //////////////////////////////////////////////////////////////////////////////////////////
      //                                                                                      //
      //               Calculate candidate multiplicity after overlap removal                 //
      //                                                                                      //
      //////////////////////////////////////////////////////////////////////////////////////////

      //
      // Add the unsupressed barrel candidates:
      //
      if( ( ! data.Barrel1.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Barrel1.getPtCand1(),
                                                               data.Barrel1.getSectorID(),
                                                               true );
      if( ( ! data.Barrel1.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Barrel1.getPtCand2(),
                                                               data.Barrel1.getSectorID(),
                                                               false );
      if( ( ! data.Barrel2.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Barrel2.getPtCand1(),
                                                               data.Barrel2.getSectorID(),
                                                               true );
      if( ( ! data.Barrel2.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Barrel2.getPtCand2(),
                                                               data.Barrel2.getSectorID(),
                                                               false );
      if( ( ! data.Barrel3.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Barrel3.getPtCand1(),
                                                               data.Barrel3.getSectorID(),
                                                               true );
      if( ( ! data.Barrel3.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Barrel3.getPtCand2(),
                                                               data.Barrel3.getSectorID(),
                                                               false );
      if( ( ! data.Barrel4.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Barrel4.getPtCand1(),
                                                               data.Barrel4.getSectorID(),
                                                               true );
      if( ( ! data.Barrel4.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Barrel4.getPtCand2(),
                                                               data.Barrel4.getSectorID(),
                                                               false );

      //
      // Add the unsupressed endcap candidates:
      //
      if( ( ! data.Endcap1.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap1.getPtCand1(),
                                                               data.Endcap1.getSectorID(),
                                                               true );
      if( ( ! data.Endcap1.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap1.getPtCand2(),
                                                               data.Endcap1.getSectorID(),
                                                               false );
      if( ( ! data.Endcap2.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap2.getPtCand1(),
                                                               data.Endcap2.getSectorID(),
                                                               true );
      if( ( ! data.Endcap2.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap2.getPtCand2(),
                                                               data.Endcap2.getSectorID(),
                                                               false );
      if( ( ! data.Endcap3.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap3.getPtCand1(),
                                                               data.Endcap3.getSectorID(),
                                                               true );
      if( ( ! data.Endcap3.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap3.getPtCand2(),
                                                               data.Endcap3.getSectorID(),
                                                               false );
      if( ( ! data.Endcap4.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap4.getPtCand1(),
                                                               data.Endcap4.getSectorID(),
                                                               true );
      if( ( ! data.Endcap4.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap4.getPtCand2(),
                                                               data.Endcap4.getSectorID(),
                                                               false );
      if( ( ! data.Endcap5.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap5.getPtCand1(),
                                                               data.Endcap5.getSectorID(),
                                                               true );
      if( ( ! data.Endcap5.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap5.getPtCand2(),
                                                               data.Endcap5.getSectorID(),
                                                               false );
      if( ( ! data.Endcap6.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap6.getPtCand1(),
                                                               data.Endcap6.getSectorID(),
                                                               true );
      if( ( ! data.Endcap6.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Endcap6.getPtCand2(),
                                                               data.Endcap6.getSectorID(),
                                                               false );

      //
      // Add the unsupressed forward candidates:
      //
      if( ( ! data.Forward1.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Forward1.getPtCand1(),
                                                               data.Forward1.getSectorID(),
                                                               true );
      if( ( ! data.Forward1.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Forward1.getPtCand2(),
                                                               data.Forward1.getSectorID(),
                                                               false );
      if( ( ! data.Forward2.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Forward2.getPtCand1(),
                                                               data.Forward2.getSectorID(),
                                                               true );
      if( ( ! data.Forward2.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Forward2.getPtCand2(),
                                                               data.Forward2.getSectorID(),
                                                               false );
      if( ( ! data.Forward3.getCand1Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Forward3.getPtCand1(),
                                                               data.Forward3.getSectorID(),
                                                               true );
      if( ( ! data.Forward3.getCand2Supressed() ) || m_flagMode )
         result += m_multiplicityLogic->calculateMultiplicity( data.Forward3.getPtCand2(),
                                                               data.Forward3.getSectorID(),
                                                               false );

      return result;
   }

   bool operator== ( const LUTMultiplicityCalculator& calc1,
                     const LUTMultiplicityCalculator& calc2 ) {

      if( ( calc1.m_bb_calc_1 == calc2.m_bb_calc_1 ) &&
          ( calc1.m_bb_calc_2 == calc2.m_bb_calc_2 ) &&
          ( calc1.m_ee_calc_1 == calc2.m_ee_calc_1 ) &&
          ( calc1.m_ee_calc_2 == calc2.m_ee_calc_2 ) &&
          ( calc1.m_ee_calc_3 == calc2.m_ee_calc_3 ) &&
          ( calc1.m_ee_calc_4 == calc2.m_ee_calc_4 ) &&
          ( calc1.m_ee_calc_5 == calc2.m_ee_calc_5 ) &&
          ( calc1.m_ff_calc_1 == calc2.m_ff_calc_1 ) &&
          ( calc1.m_ff_calc_2 == calc2.m_ff_calc_2 ) ) {

         return true;
      } else {

         return false;
      }
   }

} // namespace LVL1MUCTPI
