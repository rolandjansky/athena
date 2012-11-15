/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTPrintingOfHits_hh
#define TRTPrintingOfHits_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class TRTUncompressedHit;
class TRTOutputFile;


class TRTPrintingOfHits
{
  public:
    TRTPrintingOfHits();
    ~TRTPrintingOfHits();

    void PrintUncompressedHit(TRTUncompressedHit*);

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

  private:
    int printMessages;

    int mask;
    int shift;

    TRTOutputFile* pOutputFile;

    mutable Athena::MsgStreamMember m_msg;

};

#endif
