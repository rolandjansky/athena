/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaHepMCInterface_H
#define AthenaHepMCInterface_H

#include "FadsKinematics/FadsGenerator.h"
#include "AthenaKernel/MsgStreamMember.h"
//#include "GaudiKernel/MsgStream.h"
#include <string>

class AthenaHepMCInterface: public FADS::FadsGenerator
{
  public:
    AthenaHepMCInterface(std::string);
    ~AthenaHepMCInterface();
    void Initialize();
    void Terminate();
    const HepMC::GenEvent* GenerateAnEvent();
    /// Log a message using the Athena controlled logging system
    MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
    /// Check whether the logging system is active at the provided verbosity level
    bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
  private:
    /// Private message stream member
    mutable Athena::MsgStreamMember m_msg;
};

#endif
