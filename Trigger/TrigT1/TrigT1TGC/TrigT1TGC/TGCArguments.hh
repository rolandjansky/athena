/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1TGC_TGCARGUMENTS_HH
#define TRIGT1TGC_TGCARGUMENTS_HH

namespace LVL1TGCTrigger {

  class TGCArguments{
  public:
    TGCArguments();
    ~TGCArguments();

    void set_DEBUGLEVEL(bool v);
    void set_SHPT_ORED(bool v);
    void set_USE_INNER(bool v);
    void set_INNER_VETO(bool v);
    void set_TILE_MU(bool v);
    void set_USE_CONDDB(bool v);
    void set_useRun3Config(bool v);

    bool DEBUGLEVEL() const;
    bool SHPT_ORED() const;
    bool USE_INNER() const;
    bool INNER_VETO() const;
    bool TILE_MU() const;
    bool USE_CONDDB() const;
    bool useRun3Config() const;

  private:
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

