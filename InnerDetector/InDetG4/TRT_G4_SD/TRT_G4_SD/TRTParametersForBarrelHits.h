/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersForBarrelHits_hh
#define TRTParametersForBarrelHits_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;
class TRTProcessingOfBarrelHits;


class TRTParametersForBarrelHits
{
  public:
    TRTParametersForBarrelHits(TRTProcessingOfBarrelHits*);
    ~TRTParametersForBarrelHits();

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl)  const  { return m_msg.get().level() <= lvl; }

  private:
    void DefineParameters() const;
    void PrintParameters() const;

    int printMessages;

    TRTParameters* pParameters;
    TRTProcessingOfBarrelHits* pProcessingOfBarrelHits;

    mutable Athena::MsgStreamMember m_msg;

};

#endif
