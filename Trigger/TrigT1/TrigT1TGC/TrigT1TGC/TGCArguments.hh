/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// TGCArguments.h
#ifndef TRIGT1TGC_TGCARGUMENTS_HH
#define TRIGT1TGC_TGCARGUMENTS_HH

#include "TrigT1TGC/TGCCoincidence.hh"

namespace LVL1TGCTrigger {

  class TGCArguments{
  public:
    TGCArguments();
    ~TGCArguments();

    void set_STRICTWD(bool v);
    void set_STRICTWT(bool v);
    void set_STRICTSD(bool v);
    void set_STRICTST(bool v);
    void set_OUTCOINCIDENCE(bool v);
    void set_TGCCOIN(TGCCoincidences* v);
    void set_DEBUGLEVEL(bool v);
    void set_SHPT_ORED(bool v);
    void set_USE_INNER(bool v);
    void set_INNER_VETO(bool v);
    void set_TILE_MU(bool v);
    void set_USE_CONDDB(bool v);
    void set_useRun3Config(bool v);

    bool STRICTWD() const;
    bool STRICTWT() const;
    bool STRICTSD() const;
    bool STRICTST() const;
    bool OUTCOINCIDENCE() const;
    TGCCoincidences* TGCCOIN() const;
    bool DEBUGLEVEL() const;
    bool SHPT_ORED() const;
    bool USE_INNER() const;
    bool INNER_VETO() const;
    bool TILE_MU() const;
    bool USE_CONDDB() const;
    bool useRun3Config() const;
    
    void clear();

  private:
    bool m_STRICTWD;
    bool m_STRICTWT;
    bool m_STRICTSD;
    bool m_STRICTST;
    bool m_OUTCOINCIDENCE;
    TGCCoincidences * m_TGCCOIN;
    bool m_DEBUGLEVEL;
    bool m_SHPT_ORED;
    bool m_USE_INNER;
    bool m_INNER_VETO;
    bool m_TILE_MU;
    bool m_USE_CONDDB;
    bool m_useRun3Config;
  };
  
} // namespace LVL1TGCTrigger


#endif

