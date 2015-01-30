/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARD_TRANSPORT_MODEL_H
#define FORWARD_TRANSPORT_MODEL_H

#include "AthenaKernel/MsgStreamMember.h"
#include "G4FastSimulation/FastSimModel.h"
#include "ForwardTransportSvc/IForwardTransportSvc.h"
#include "ForwardTracker/ForwardTrack.h"

class ForwardTransportModel: public FastSimModel {
  
 public:
  
  ForwardTransportModel(std::string);

  // methods being inherited from base class
  G4bool IsApplicable(const G4ParticleDefinition&) { return true; } // IDLE: we do selection in DoIt method
  G4bool ModelTrigger(const G4FastTrack&)          { return true; } // IDLE: we do selection in DoIt method
  void   DoIt        (const G4FastTrack&, G4FastStep&);             // Actual selection and parametrization

  void KillPrimaryTrack(G4FastStep&, double energy);

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }

private:

  IForwardTransportSvc*    m_fwdSvc;
  ForwardTrack             m_fwdTrack;
  ForwardTracker::Particle m_fwdParticle;

  mutable Athena::MsgStreamMember m_msg;
};

#endif
