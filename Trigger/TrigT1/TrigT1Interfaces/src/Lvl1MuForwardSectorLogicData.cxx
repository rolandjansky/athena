/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <iomanip>
#include <iostream>

#include "TrigT1Interfaces/Lvl1MuForwardSectorLogicData.h"

namespace LVL1MUONIF {

  Lvl1MuForwardSectorLogicData::Lvl1MuForwardSectorLogicData()
      : Lvl1MuSectorLogicData() {
    m_id_system = 2;
  }

  Lvl1MuForwardSectorLogicData& Lvl1MuForwardSectorLogicData::operator=( const Lvl1MuSectorLogicData& right ) {
    if ( ( right.getSystemId() == m_id_system ) &&
         ( this != &right ) ) {
      m_bcid = right.bcid();
      m_2candidatesInSector = right.is2candidatesInSector();
      for ( size_t i = 0; i < 2; ++i ) {
        m_roi[ i ] = right.roi( i );
        m_ovl[ i ] = right.ovl( i );
        m_pt[ i ] = right.pt( i );
        m_charge[ i ] = right.charge( i );
        m_2candidates[ i ] = right.is2candidates( i );
      }
    }
    return *this;
  }

  Lvl1MuSectorLogicData::SectorLogicWord Lvl1MuForwardSectorLogicData::getWordFormat() const {

    unsigned int val = 0;
    // Bit 0
    if ( is2candidatesInSector() ) {
      val = 1;
    }
    // Bit 1-6
    if ( roi( 0 ) < 0 ) {
      val |= 0x007E;
    } else {
      val |= ( roi( 0 ) & 0x03F ) << 1;
    }
    // Bit 7-9
    val |= 0x0380;
    // Bit 10-15
    if ( roi( 1 ) < 0 ) {
      val |= 0x00FC00;
    } else {
      val |= ( roi( 1 ) & 0x003F ) << 10;
    }
    // Bit 16-18
    val |= 0x070000;
    // bit 19-21
    if ( pt( 0 ) < 0 ) {
      val |= 0x0380000;
    } else {
      val |= ( pt( 0 ) & 0x0007 ) << 19;
    }
    // bit 22-24
    if ( pt( 1 ) < 0 ) {
      val |= 0x01C00000;
    } else {
      val |= ( pt( 1 ) & 0x0007 ) << 22;
    }
    // bit 25
    val |= 0x02000000;
    // bit 26
    val |= 0x04000000;
    // bit 27-29
    val |= ( bcid() & 0x07 ) << 27;
    // bit 30
    if ( charge( 0 ) > 0 ) val |= 0x040000000;
    // bit 31
    if ( charge( 1 ) > 0 ) val |= 0x040000000;

    return val;
  }

  const Lvl1MuSectorLogicData& Lvl1MuForwardSectorLogicData::convertFromWordFormat( const SectorLogicWord& value ) {

    unsigned int val;
    // Bit 0
    if ( value & 0x0001 ) {
      set2candidatesInSector();
    } else {
      clear2candidatesInSector();
    }
    // Bit 1-9
    val = ( value >> 1 ) & 0x003F;
    if ( val == 0x003F ) {
      roi( 0, -1 );
      ovl( 0, -1 );
    } else {
      roi( 0, val );
      ovl( 0, -1 );
    }
    // Bit 10-18
    val = ( value >> 10 ) & 0x003F;
    if ( val == 0x003F ) {
      roi( 1, -1 );
      ovl( 1, -1 );
    } else {
      roi( 1, val );
      ovl( 1, -1 );
    }
    // bit 19-21
    val = ( value >> 19 ) & 0x0007;
    if ( val == 0x0007 ) {
      pt( 0, -1 );
    } else {
      pt( 0, val );
    }
    // bit 22-24
    val = ( value >> 22 ) & 0x0007;
    if ( val == 0x0007 ) {
      pt( 1, -1 );
    } else {
      pt( 1, val );
    }
    // bit 25
    clear2candidates( 0 );
    // bit 26
    clear2candidates( 1 );
    // bit 27-29
    val = ( value >> 27 ) & 0x0007;
    bcid( val );
    // bit 30
    val = ( value >> 30 ) & 0x0001;
    if ( val != 0 ) charge( 0, + 1 );
    // bit 31
    val = ( value >> 31 ) & 0x0001;
    if ( val != 0 ) charge( 1, + 1 );

    return *this;
  }

  void Lvl1MuForwardSectorLogicData::printOut( std::ostream& out ) const {

    out << "Bit 0     " << " >2candidates in a sector ";
    out << std::setw( 8 ) << m_2candidatesInSector << std::endl;

    out << "Bit 1-6   " << " ROI<1>                   ";
    if ( m_roi[ 0 ] == -1 ) {
      out << std::setw( 8 ) << "NA" << std::endl;
    } else {
      out << std::setw( 8 ) << m_roi[ 0 ] << std::endl;
    }
    out << "Bit  7-9  " << " NA    " << std::endl;
    out << "Bit 10-15 " << " ROI<2>                   ";
    if ( m_roi[ 1 ] == -1 ) {
      out << std::setw( 8 ) << "NA" << std::endl;
    } else {
      out << std::setw( 8 ) << m_roi[ 1 ] << std::endl;
    }
    out << "Bit 16-18 " << " NA    " << std::endl;

    out << "Bit 19-21 " << " Pt1                      ";
    if ( m_pt[ 0 ] == -1 ) {
      out << std::setw( 8 ) << "No Hit" << std::endl;
    } else {
      out << std::setw( 8 ) << m_pt[ 0 ] << std::endl;
    }
    out << "Bit 22-24 " << " Pt2                      ";
    if ( m_pt[ 1 ] == -1 ) {
      out << std::setw( 8 ) << "No Hit" << std::endl;
    } else {
      out << std::setw( 8 ) << m_pt[ 1 ] << std::endl;
    }

    out << "Bit 25-26 " << " NA    " << std::endl;
    out << "Bit 27-29 " << " BCID                     ";
    out << std::setw( 8 ) << m_bcid << std::endl;
    out << "Bit 30    " << " charge in ROI1           ";
    out << std::setw( 8 ) << m_charge[ 0 ] << std::endl;
    out << "Bit 31    " << " charge in ROI2           ";
    out << std::setw( 8 ) << m_charge[ 1 ] << std::endl;

  }

} // namespace LVL1MUONIF
