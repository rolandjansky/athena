/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCArguments.hh"

namespace LVL1TGCTrigger {
  
  TGCArguments::TGCArguments():
    m_STRICTWD(false),
    m_STRICTWT(false),
    m_STRICTSD(false),
    m_STRICTST(false),
    m_OUTCOINCIDENCE(false),
    m_TGCCOIN(nullptr),
    m_DEBUGLEVEL(false),
    m_SHPT_ORED(false),
    m_USE_INNER(false),
    m_INNER_VETO(false),
    m_TILE_MU(false),
    m_USE_CONDDB(false),
    m_useRun3Config(false)
  {
  }

  TGCArguments::~TGCArguments(){
  }

  void TGCArguments::clear(){
    if(m_TGCCOIN!=nullptr)delete m_TGCCOIN;
  }
  
  void TGCArguments::set_STRICTWD(bool v){ m_STRICTWD = v;}
  void TGCArguments::set_STRICTWT(bool v){ m_STRICTWT = v;}
  void TGCArguments::set_STRICTSD(bool v){ m_STRICTSD = v;}
  void TGCArguments::set_STRICTST(bool v){ m_STRICTST = v;}
  void TGCArguments::set_OUTCOINCIDENCE(bool v){ m_OUTCOINCIDENCE = v;}
  void TGCArguments::set_TGCCOIN(TGCCoincidences* v){ m_TGCCOIN = v;}
  void TGCArguments::set_DEBUGLEVEL(bool v){ m_DEBUGLEVEL = v;}
  void TGCArguments::set_SHPT_ORED(bool v){ m_SHPT_ORED = v;}
  void TGCArguments::set_USE_INNER(bool v){ m_USE_INNER = v;}
  void TGCArguments::set_INNER_VETO(bool v){ m_INNER_VETO = v;}
  void TGCArguments::set_TILE_MU(bool v){ m_TILE_MU = v;}
  void TGCArguments::set_USE_CONDDB(bool v){ m_USE_CONDDB = v;}
  void TGCArguments::set_useRun3Config(bool v){ m_useRun3Config = v;}
  
  bool TGCArguments::STRICTWD() const {return m_STRICTWD;}
  bool TGCArguments::STRICTWT() const {return m_STRICTWT;}
  bool TGCArguments::STRICTSD() const {return m_STRICTSD;}
  bool TGCArguments::STRICTST() const {return m_STRICTST;}
  bool TGCArguments::OUTCOINCIDENCE() const {return m_OUTCOINCIDENCE;}
  TGCCoincidences* TGCArguments::TGCCOIN() const {return m_TGCCOIN;}
  bool TGCArguments::DEBUGLEVEL() const {return m_DEBUGLEVEL;}
  bool TGCArguments::SHPT_ORED() const {return m_SHPT_ORED;}
  bool TGCArguments::USE_INNER() const {return m_USE_INNER;}
  bool TGCArguments::INNER_VETO() const {return m_INNER_VETO;}
  bool TGCArguments::TILE_MU() const {return m_TILE_MU;}
  bool TGCArguments::USE_CONDDB() const {return m_USE_CONDDB;}
  bool TGCArguments::useRun3Config() const {return m_useRun3Config;}

}
