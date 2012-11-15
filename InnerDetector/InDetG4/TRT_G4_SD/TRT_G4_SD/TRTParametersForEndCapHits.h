/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTParametersForEndCapHits_hh
#define TRTParametersForEndCapHits_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTParameters;
class TRTProcessingOfEndCapHits;


class TRTParametersForEndCapHits
{
  public:
    TRTParametersForEndCapHits(TRTProcessingOfEndCapHits*);
    ~TRTParametersForEndCapHits();

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl) const   { return m_msg.get().level() <= lvl; }

  private:
    void DefineParameters() const;
    void PrintParameters() const;

    int printMessages;

    TRTParameters* pParameters;
    TRTProcessingOfEndCapHits* pProcessingOfEndCapHits;

    mutable Athena::MsgStreamMember m_msg;

};

#endif

