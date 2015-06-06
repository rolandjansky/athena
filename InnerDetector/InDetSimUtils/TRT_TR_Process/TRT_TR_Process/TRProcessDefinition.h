/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRPROCESSDEFINITION_H
#define TRPROCESSDEFINITION_H

#include "FadsPhysics/UserPhysicsDefinition.h"
#include "AthenaKernel/MsgStreamMember.h"

class TRProcessDefinition: public UserPhysicsDefinition {

 public:

  TRProcessDefinition();

  void ConstructProcess();

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

 private:

  G4VProcess* m_pXTR;
  mutable Athena::MsgStreamMember m_msg;
};

#endif
