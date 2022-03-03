// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODEVENTINFOCNV_EVENTINFOBEAMSPOTDECORATORALG_H
#define XAODEVENTINFOCNV_EVENTINFOBEAMSPOTDECORATORALG_H

// EDM include(s).
#include "xAODEventInfo/EventInfo.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

// Gaudi/Athena include(s).
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"

namespace xAODMaker {

   /// Algorithm for filling the beam position variables on @c xAOD::EventInfo
   ///
   /// These variables need to be filled as decorations for data. This algorithm
   /// takes care of doing that.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class EventInfoBeamSpotDecoratorAlg : public AthReentrantAlgorithm {

   public:
      // Inherit the base class's constructor.
      using AthReentrantAlgorithm::AthReentrantAlgorithm;

      /// @name Function(s) implemented from @c AthReentrantAlgorithm
      /// @{

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;
      /// Function executing the algorithm
      virtual StatusCode execute( const EventContext& ctx ) const override;

      /// @}

   private:
      /// @name Algorithm properties
      /// @{

      /// Conditions object with the beamspot information
      SG::ReadCondHandleKey< InDet::BeamSpotData > m_beamSpotKey{ this,
         "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

      /// Decorator handle for @c beamPosX
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamPosXKey{ this,
         "beamPosXKey", "EventInfo.beamPosX",
         "Key for the beamPosX decoration" };
      /// Decorator handle for @c beamPosY
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamPosYKey{ this,
         "beamPosYKey", "EventInfo.beamPosY",
         "Key for the beamPosY decoration" };
      /// Decorator handle for @c beamPosZ
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamPosZKey{ this,
         "beamPosZKey", "EventInfo.beamPosZ",
         "Key for the beamPosZ decoration" };

      /// Decorator handle for @c beamPosSigmaX
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamPosSigmaXKey{ this,
         "beamPosSigmaXKey", "EventInfo.beamPosSigmaX",
         "Key for the beamPosSigmaX decoration" };
      /// Decorator handle for @c beamPosSigmaY
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamPosSigmaYKey{ this,
         "beamPosSigmaYKey", "EventInfo.beamPosSigmaY",
         "Key for the beamPosSigmaY decoration" };
      /// Decorator handle for @c beamPosSigmaZ
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamPosSigmaZKey{ this,
         "beamPosSigmaZKey", "EventInfo.beamPosSigmaZ",
         "Key for the beamPosSigmaZ decoration" };
      /// Decorator handle for @c beamPosSigmaXY
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamPosSigmaXYKey{ this,
         "beamPosSigmaXYKey", "EventInfo.beamPosSigmaXY",
         "Key for the beamPosSigmaXY decoration" };

      /// Decorator handle for @c beamTiltXZ
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamTiltXZKey{ this,
         "beamTiltXZKey", "EventInfo.beamTiltXZ",
         "Key for the beamTiltXZ decoration" };
      /// Decorator handle for @c beamTiltYZ
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamTiltYZKey{ this,
         "beamTiltYZKey", "EventInfo.beamTiltYZ",
         "Key for the beamTiltYZ decoration" };

      /// Decorator handle for @c beamStatus
      SG::WriteDecorHandleKey< xAOD::EventInfo > m_beamStatusKey{ this,
         "beamStatusKey", "EventInfo.beamStatus",
         "Key for the beamStatus decoration" };

      /// @}

   }; // class EventInfoBeamSpotDecoratorAlg

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOBEAMSPOTDECORATORALG_H
