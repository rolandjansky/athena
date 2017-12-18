/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasAlg_AthenaTrackingAction_H
#define G4AtlasAlg_AthenaTrackingAction_H

#include "AthenaKernel/MsgStreamMember.h"

#include "G4UserTrackingAction.hh"

namespace G4UA
{

  /// @class AthenaTrackingAction
  /// @brief User action for pre/post tracking truth handling.
  ///
  class AthenaTrackingAction : public G4UserTrackingAction
  {

    public:

      /// Constructor
      AthenaTrackingAction(MSG::Level lvl, int secondarySavingLevel, int subDetVolLevel);

      /// @brief Called before tracking a new particle.
      ///
      /// If the track meets certain conditions, we save it in the
      /// EventInformation and possibly construct a new AtlasTrajectory
      /// which will be used for writing out truth particles later.
      virtual void PreUserTrackingAction(const G4Track*) override final;

      /// @brief Called after tracking a particle.
      ///
      /// Here we reset the AtlasTrajectory if it was created.
      virtual void PostUserTrackingAction(const G4Track*) override final;

    private:

      /// Log a message using the Athena controlled logging system
      MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
      bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
      mutable Athena::MsgStreamMember m_msg;

      /// The saving level for secondaries.
      int m_secondarySavingLevel;
      /// The level in the G4 volume hierarchy at which can we find the sub-detector name
      int m_subDetVolLevel;

  }; // class AthenaTrackingAction

} // namespace G4UA

#endif
