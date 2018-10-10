/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdio>
#include <iostream>
#include <iomanip>

#include "TrigT1Interfaces/Lvl1MuSectorLogicData.h"

namespace LVL1MUONIF {

  Lvl1MuSectorLogicData::Lvl1MuSectorLogicData()
      : m_2candidatesInSector ( false ), m_bcid( 0 ),
        m_id_system( -1 ) {

    for ( size_t i = 0; i < 2; ++i ) {
      m_roi[ i ] = -1;
      m_ovl[ i ] = -1;
      m_pt[ i ] = -1;
      m_charge[ i ] = + 1;
      m_2candidates[ i ] = false;
    }
  }


  Lvl1MuSectorLogicData::Lvl1MuSectorLogicData( const Lvl1MuSectorLogicData& right ) {
    *this = right;
  }

  Lvl1MuSectorLogicData& Lvl1MuSectorLogicData::operator=( const Lvl1MuSectorLogicData& right ) {
    if ( this != &right ) {
      m_id_system = right.m_id_system;
      m_bcid = right.m_bcid;
      m_2candidatesInSector = right.m_2candidatesInSector;
      for ( size_t i = 0; i < 2; ++i ) {
        m_roi[ i ] = right.m_roi[ i ];
        m_ovl[ i ] = right.m_ovl[ i ];
        m_pt[ i ] = right.m_pt[ i ];
        m_charge[ i ] = right.m_charge[ i ];
        m_2candidates[ i ] = right.m_2candidates[ i ];
      }
    }
    return *this;
  }

  std::ostream& operator<<( std::ostream& out, const Lvl1MuSectorLogicData& right ) {

    right.printOut( out );
    return out;

  }

  void Lvl1MuSectorLogicData::printOut( std::ostream& out ) const {

    out << "Bit 0     " << " >2candidates in a sector ";
    out << std::setw( 8 ) << m_2candidatesInSector << std::endl;

    out << "Bit 1-9   " << " ROI<1>  +         OVL<1> ";
    out << std::setw( 8 ) << m_roi[ 0 ] << " : " << m_ovl[ 0 ] << std::endl;
    out << "Bit 10-18 " << " ROI<2>  +         OVL<2> ";
    out << std::setw( 8 ) << m_roi[ 1 ] << " : " << m_ovl[ 1 ] << std::endl;
    out << "Bit 19-21 " << " Pt1                      ";
    out << std::setw( 8 ) << m_pt[ 0 ] << std::endl;
    out << "Bit 22-24 " << " Pt2                      ";
    out << std::setw( 8 ) << m_pt[ 1 ] << std::endl;
    out << "Bit 25    " << " >2candidates in ROI1     ";
    out << std::setw( 8 ) << m_2candidates[ 0 ] << std::endl;
    out << "Bit 26    " << " >2candidates in ROI2     ";
    out << std::setw( 8 ) << m_2candidates[ 1 ] << std::endl;
    out << "Bit 27-29 " << " BCID                     ";
    out << std::setw( 8 ) << m_bcid << std::endl;
    out << "Bit 30    " << " charge in ROI1           ";
    out << std::setw( 8 ) << m_charge[ 0 ] << std::endl;
    out << "Bit 31    " << " charge in ROI2           ";
    out << std::setw( 8 ) << m_charge[ 1 ] << std::endl;

  }

  void Lvl1MuSectorLogicData::clear() {
    m_2candidatesInSector = false;
    m_bcid = 0;
    for ( size_t i = 0; i < 2; ++i ) {
      m_roi[ i ] = -1;
      m_ovl[ i ] = -1;
      m_pt[ i ] = -1;
      m_charge[ i ] = -1;
      m_2candidates[ i ] = false;
    }
  }

} // namespace LVL1MUONIF
