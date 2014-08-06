/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombLinksFillerTool.cxx 504000 2012-06-05 16:28:50Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TriggerMenuAnalysis/CombLinksDef.h"

// Local include(s):
#include "CombLinksFillerTool.h"

namespace D3PD {

   CombLinksFillerTool::CombLinksFillerTool( const std::string& type, 
                                             const std::string& name, 
                                             const IInterface* parent )
      : BlockFillerTool< CombLinks >( type, name, parent ) {

      declareProperty( "RoIType", m_roiType, 
                       "RoI type defined in ChainEntry::RoIType" );

      m_type = 0;
      m_active = 0;
      m_lastStep = 0;
      m_TENumber = 0;
      m_RoINumber = 0;
   }

   StatusCode CombLinksFillerTool::book() {

      // Find out what kind of RoI are we looking at:
      ChainEntry::RoIType roi_type = static_cast< ChainEntry::RoIType >( m_roiType );
      const CombLinksDef* def = CombLinksDef::getCombLinksDef( roi_type );

      if( ! def ) {
         REPORT_MESSAGE( MSG::ERROR ) << "Cannot retrieve CombLinksDef for RoI type: "
                                      << m_roiType;
         return StatusCode::FAILURE;
      }

      int feature_id = 0;
      std::vector< std::string > vi_names  = def->typeLabelNamesInt();
      std::vector< std::string > vvi_names = def->typeLabelNamesVectorOfInt();
      std::vector< std::string >::const_iterator name_itr, name_end;
      
      ATH_MSG_INFO( "Booking branches for RoIType: " << m_roiType );

      CHECK( addVariable( "type", m_type ) );
      CHECK( addVariable( "active", m_active ) );
      CHECK( addVariable( "lastStep", m_lastStep ) );
      CHECK( addVariable( "TENumber", m_TENumber ) );
      CHECK( addVariable( "roiNumber", m_RoINumber ) );

      name_itr = vi_names.begin();
      name_end = vi_names.end();
      for( ; name_itr != name_end; ++name_itr ) {

         REPORT_MESSAGE( MSG::DEBUG ) << "Booking branches for '"
                                      << *name_itr << "' objects";

         feature_id = CombLinks::featureId( *name_itr );

         CHECK( addVariable( ( *name_itr ), m_featureIndex[ feature_id ] ) );
         CHECK( addVariable( ( *name_itr ) + "Status", 
                             m_featureStatus[ feature_id ] ) );

      }

      name_itr = vvi_names.begin();
      name_end = vvi_names.end();
      for( ; name_itr != name_end; ++name_itr ) {

         REPORT_MESSAGE( MSG::DEBUG ) << "Booking branches for '"
                                      << *name_itr << "' objects";

         feature_id = CombLinks::featureId( *name_itr );

         CHECK( addVariable( ( *name_itr ), m_featureIndexVec[ feature_id ] ) );
         CHECK( addVariable( ( *name_itr ) + "Status", 
                             m_featureStatusVec[ feature_id ] ) );

      }

      return StatusCode::SUCCESS;
   }

   StatusCode CombLinksFillerTool::fill( const CombLinks& x ) {

      *m_type     = x.RoIType();
      *m_active   = x.active();
      *m_lastStep = x.lastStep();
      *m_TENumber = x.TENumber();
      *m_RoINumber = x.RoINumber();

      ChainEntry::RoIType roi_type = static_cast< ChainEntry::RoIType >( m_roiType );
      const CombLinksDef* def = CombLinksDef::getCombLinksDef( roi_type );
      if( ! def ) {
         REPORT_MESSAGE( MSG::ERROR ) << "Cannot retrieve CombLinksDef for RoI type: "
                                      << m_roiType;
         return StatusCode::FAILURE;
      }

      int feature_id = 0;
      std::vector< std::string > vi_names  = def->typeLabelNamesInt();
      std::vector< std::string > vvi_names = def->typeLabelNamesVectorOfInt();
      std::vector< std::string >::const_iterator name_itr, name_end;

      name_itr = vi_names.begin();
      name_end = vi_names.end();
      for( ; name_itr != name_end; ++name_itr ) {

         feature_id = CombLinks::featureId( *name_itr );
         const FeatureIndex* fi = x.index( feature_id );
         if( ! fi ) continue;

         *( m_featureIndex[ feature_id ] )  = static_cast< index_t >( fi->getIndex() );
         *( m_featureStatus[ feature_id ] ) = static_cast< index_t >( fi->getStatus() );
      }

      name_itr = vvi_names.begin();
      name_end = vvi_names.end();
      for( ; name_itr != name_end; ++name_itr ) {

         feature_id = CombLinks::featureId( *name_itr );
         const std::vector< FeatureIndex >* fiv = x.indexVec( feature_id );
         if( ! fiv ) continue;

         m_featureIndexVec[ feature_id ]->clear();
         m_featureStatusVec[ feature_id ]->clear();
         for( size_t i = 0; i < fiv->size(); ++i ) {
            m_featureIndexVec[ feature_id ]->push_back( static_cast< index_t >( ( *fiv )[ i ].getIndex() ) );
            m_featureStatusVec[ feature_id ]->push_back( static_cast< index_t >( ( *fiv )[ i ].getStatus() ) );
         }

      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
