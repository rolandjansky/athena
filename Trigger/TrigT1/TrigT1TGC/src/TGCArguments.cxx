/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGCTrigger {
  
TGCArguments::TGCArguments()
 : m_MSGLEVEL(MSG::INFO),
   m_SHPT_ORED(false),
   m_USE_INNER(false),
   m_INNER_VETO(false),
   m_TILE_MU(false),
   m_USE_CONDDB(false),
   m_useRun3Config(false),
   m_USE_NSW(false)
{
}

TGCArguments::~TGCArguments()
{
}

  void TGCArguments::set_MSGLEVEL(const MSG::Level v){ m_MSGLEVEL = v;}
  void TGCArguments::set_SHPT_ORED(bool v){ m_SHPT_ORED = v;}
  void TGCArguments::set_USE_INNER(bool v){ m_USE_INNER = v;}
  void TGCArguments::set_INNER_VETO(bool v){ m_INNER_VETO = v;}
  void TGCArguments::set_TILE_MU(bool v){ m_TILE_MU = v;}
  void TGCArguments::set_USE_CONDDB(bool v){ m_USE_CONDDB = v;}
  void TGCArguments::set_useRun3Config(bool v){ m_useRun3Config = v;}
  void TGCArguments::set_USE_NSW(bool v){ m_USE_NSW = v;}

  
  MSG::Level TGCArguments::MSGLEVEL() const {return m_MSGLEVEL;}
  bool TGCArguments::SHPT_ORED() const {return m_SHPT_ORED;}
  bool TGCArguments::USE_INNER() const {return m_USE_INNER;}
  bool TGCArguments::INNER_VETO() const {return m_INNER_VETO;}
  bool TGCArguments::TILE_MU() const {return m_TILE_MU;}
  bool TGCArguments::USE_CONDDB() const {return m_USE_CONDDB;}
  bool TGCArguments::useRun3Config() const {return m_useRun3Config;}
  bool TGCArguments::USE_NSW() const {return m_USE_NSW;}


}
