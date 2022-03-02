//
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "EventInfoBeamSpotDecoratorAlg.h"

// Gaudi/Athena include(s).
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteDecorHandle.h"

// System include(s).
#include <cstdint>

// Declare the algorithm to Gaudi.
DECLARE_COMPONENT( xAODMaker::EventInfoBeamSpotDecoratorAlg )

namespace xAODMaker {

   StatusCode EventInfoBeamSpotDecoratorAlg::initialize() {

      // Initialise all of the keys.
      ATH_CHECK( m_beamSpotKey.initialize() );

      ATH_CHECK( m_beamPosXKey.initialize() );
      ATH_CHECK( m_beamPosYKey.initialize() );
      ATH_CHECK( m_beamPosZKey.initialize() );

      ATH_CHECK( m_beamPosSigmaXKey.initialize() );
      ATH_CHECK( m_beamPosSigmaYKey.initialize() );
      ATH_CHECK( m_beamPosSigmaZKey.initialize() );
      ATH_CHECK( m_beamPosSigmaXYKey.initialize() );

      ATH_CHECK( m_beamTiltXZKey.initialize() );
      ATH_CHECK( m_beamTiltYZKey.initialize() );

      ATH_CHECK( m_beamStatusKey.initialize() );

      // Greet the user.
      ATH_MSG_INFO( "Creating beamspot decorations on "
                    << m_beamPosXKey.contHandleKey().fullKey() << " from "
                    << m_beamSpotKey.fullKey() );
      ATH_MSG_DEBUG( "Keys used by the algorithm:" );
      ATH_MSG_DEBUG( "  - " << m_beamSpotKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamPosXKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamPosYKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamPosZKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamPosSigmaXKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamPosSigmaYKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamPosSigmaZKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamPosSigmaXYKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamTiltXZKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamTiltYZKey.fullKey() );
      ATH_MSG_DEBUG( "  - " << m_beamStatusKey.fullKey() );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode
   EventInfoBeamSpotDecoratorAlg::execute( const EventContext& ctx ) const {

      // Read in the condition object.
      SG::ReadCondHandle< InDet::BeamSpotData >
         beamSpotHandle( m_beamSpotKey, ctx );
      const InDet::BeamSpotData* beamSpotData = beamSpotHandle.retrieve();
      ATH_CHECK( beamSpotData != nullptr );

      // Set all of the decorations on the EventInfo object.
      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamPosXHandle( m_beamPosXKey, ctx );
      beamPosXHandle( *beamPosXHandle ) = beamSpotData->beamPos()[ Amg::x ];
      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamPosYHandle( m_beamPosYKey, ctx );
      beamPosYHandle( *beamPosYHandle ) = beamSpotData->beamPos()[ Amg::y ];
      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamPosZHandle( m_beamPosZKey, ctx );
      beamPosZHandle( *beamPosZHandle ) = beamSpotData->beamPos()[ Amg::z ];

      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamPosSigmaXHandle( m_beamPosSigmaXKey, ctx );
      beamPosSigmaXHandle( *beamPosSigmaXHandle ) =
         beamSpotData->beamSigma( 0 );
      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamPosSigmaYHandle( m_beamPosSigmaYKey, ctx );
      beamPosSigmaYHandle( *beamPosSigmaYHandle ) =
         beamSpotData->beamSigma( 1 );
      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamPosSigmaZHandle( m_beamPosSigmaZKey, ctx );
      beamPosSigmaZHandle( *beamPosSigmaZHandle ) =
         beamSpotData->beamSigma( 2 );
      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamPosSigmaXYHandle( m_beamPosSigmaXYKey, ctx );
      beamPosSigmaXYHandle( *beamPosSigmaXYHandle ) =
         beamSpotData->beamSigmaXY();

      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamTiltXZHandle( m_beamTiltXZKey, ctx );
      beamTiltXZHandle( *beamTiltXZHandle ) = beamSpotData->beamTilt( 0 );
      SG::WriteDecorHandle< xAOD::EventInfo, float >
         beamTiltYZHandle( m_beamTiltYZKey, ctx );
      beamTiltYZHandle( *beamTiltYZHandle ) = beamSpotData->beamTilt( 1 );

      SG::WriteDecorHandle< xAOD::EventInfo, uint32_t >
         beamStatusHandle( m_beamStatusKey, ctx );
      beamStatusHandle( *beamStatusHandle ) = beamSpotData->beamStatus();

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
