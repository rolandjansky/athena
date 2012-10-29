/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StaticBunchCrossingTool.cxx 457114 2011-09-05 09:35:49Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "StaticBunchCrossingTool.h"
#include "TrigBunchCrossingTool/MsgWriter.h"

namespace Trig {

   StaticBunchCrossingTool::StaticBunchCrossingTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      declareProperty( "MaxBunchSpacing", m_maxBunchSpacing = 500,
                       "Maximum bunch spacing in the trains in nanoseconds" );
      declareProperty( "FrontLength", m_frontLength = 300,
                       "Length of the front part of a train in nanoseconds" );
      declareProperty( "TailLength", m_tailLength = 300,
                       "Length of the tail part of a train in nanoseconds" );

      declareProperty( "BGKey", m_bgKey = 0,
                       "DB key of the bunch group configuration to load" );
   }

   StatusCode StaticBunchCrossingTool::queryInterface( const InterfaceID& riid,
                                                       void** ppvIf ) {

      if( riid == IBunchCrossingTool::interfaceID() ) {
         *ppvIf = static_cast< IBunchCrossingTool* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }

      return AlgTool::queryInterface( riid, ppvIf );
   }

   StatusCode StaticBunchCrossingTool::initialize() {

      // Configure the underlying logger:
      MsgWriter::instance()->setMinType( msg().level() );
      MsgWriter::instance()->setSource( name() );

      // Report about the initialization"
      ATH_MSG_INFO( "Initializing StaticBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );
      ATH_MSG_INFO( "  Maximal bunch spacing: " << m_maxBunchSpacing << " ns" );
      ATH_MSG_INFO( "  Length of train front: " << m_frontLength << " ns" );
      ATH_MSG_INFO( "  Length of train tail : " << m_tailLength << " ns" );
      ATH_MSG_INFO( "  Bunch group config   : " << m_bgKey );

      // Configure the base class:
      setMaxBunchSpacing( m_maxBunchSpacing );
      setFrontLength( m_frontLength );
      setTailLength( m_tailLength );

      // Now try to load the requested configuration:
      if( ! loadConfig( m_bgKey ) ) {
         REPORT_ERROR( StatusCode::FAILURE ) << "Couldn't load BG key = " << m_bgKey;
         return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
   }

   StatusCode StaticBunchCrossingTool::finalize() {

      ATH_MSG_INFO( "Finalizing StaticBunchCrossingTool - package version: "
                    << PACKAGE_VERSION );

      return StatusCode::SUCCESS;
   }

   void StaticBunchCrossingTool::setMaxBunchSpacing( int spacing ) {

      BunchCrossingToolBase::setMaxBunchSpacing( spacing );
      return;
   }

   int StaticBunchCrossingTool::maxBunchSpacing() const {

      return BunchCrossingToolBase::maxBunchSpacing();
   }

   void StaticBunchCrossingTool::setFrontLength( int length ) {

      BunchCrossingToolBase::setFrontLength( length );
      return;
   }

   int StaticBunchCrossingTool::frontLength() const {

      return BunchCrossingToolBase::frontLength();
   }

   void StaticBunchCrossingTool::setTailLength( int length ) {

      BunchCrossingToolBase::setTailLength( length );
      return;
   }

   int StaticBunchCrossingTool::tailLength() const {

      return BunchCrossingToolBase::tailLength();
   }

} // namespace Trig
