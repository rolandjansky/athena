/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdio>
#include <iostream>
#include <iomanip>

#include "TrigT1Interfaces/Lvl1MuSectorLogicDataPhase1.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"

namespace LVL1MUONIF {

  Lvl1MuSectorLogicDataPhase1::Lvl1MuSectorLogicDataPhase1()
      : 
    m_2candidatesInSector ( false ), 
    m_bcid( 0 ),
    m_nsw( 0 ),
    m_id_system( -1 )
  {
  }
  Lvl1MuSectorLogicDataPhase1::~Lvl1MuSectorLogicDataPhase1()
  {
    m_roi.clear();
    m_ovl.clear();
    m_pt.clear();
    m_charge.clear();
    m_2candidates.clear();
    m_goodmf.clear();
    m_innercoin.clear();
    m_bw2or3.clear();
    m_veto.clear();
  }

  void Lvl1MuSectorLogicDataPhase1::initialize()
  {
    if (m_id_system == -1) return;

    size_t ncand = NCAND[m_id_system];
    for ( size_t i = 0; i < ncand; ++i ) {
      m_roi.push_back(-1);
      m_ovl.push_back(-1);
      m_pt.push_back(-1);
      m_charge.push_back(+1);
      m_2candidates.push_back(false);
      m_goodmf.push_back(-1);
      m_innercoin.push_back(-1);
      m_bw2or3.push_back(-1);
      m_veto.push_back(0);
    }
  }


  Lvl1MuSectorLogicDataPhase1::Lvl1MuSectorLogicDataPhase1( const Lvl1MuSectorLogicDataPhase1& right ) {
    *this = right;
  }

  Lvl1MuSectorLogicDataPhase1& Lvl1MuSectorLogicDataPhase1::operator=( const Lvl1MuSectorLogicDataPhase1& right ) {
    if ( this != &right ) {
      m_id_system = right.m_id_system;
      m_bcid = right.m_bcid;
      m_nsw = right.m_nsw;
      m_2candidatesInSector = right.m_2candidatesInSector;
      for ( size_t i = 0; i < NCAND[m_id_system]; ++i ) {
        m_roi[ i ] = right.m_roi[ i ];
        m_ovl[ i ] = right.m_ovl[ i ];
        m_pt[ i ] = right.m_pt[ i ];
        m_charge[ i ] = right.m_charge[ i ];
        m_2candidates[ i ] = right.m_2candidates[ i ];
        m_goodmf[ i ] = right.m_goodmf[ i ];
        m_innercoin[ i ] = right.m_innercoin[ i ];
        m_bw2or3[ i ] = right.m_bw2or3[ i ];
        m_veto[ i ] = right.m_veto[ i ];
      }
    }
    return *this;
  }

  std::ostream& operator<<( std::ostream& out, const Lvl1MuSectorLogicDataPhase1& right ) {

    right.printOut( out );
    return out;

  }

  void Lvl1MuSectorLogicDataPhase1::clear() {
    m_2candidatesInSector = false;
    m_bcid = 0;
    m_nsw = 0;
    for ( size_t i = 0; i < NCAND[m_id_system]; ++i ) {
      m_roi[ i ] = -1;
      m_ovl[ i ] = -1;
      m_pt[ i ] = -1;
      m_charge[ i ] = -1;
      m_2candidates[ i ] = false;
      m_goodmf[ i ] = -1;
      m_innercoin[ i ] = -1;
      m_bw2or3[ i ] = -1;
      m_veto[ i ] = 0;
    }
  }

} // namespace LVL1MUONIF
