/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <iomanip>
#include <iostream>

#include "TrigT1Interfaces/Lvl1MuBarrelSectorLogicData.h"

namespace LVL1MUONIF {

  Lvl1MuBarrelSectorLogicData::Lvl1MuBarrelSectorLogicData()
      : Lvl1MuSectorLogicData() {

    m_id_system = 0;
  }

  Lvl1MuBarrelSectorLogicData& Lvl1MuBarrelSectorLogicData::operator=( const Lvl1MuSectorLogicData& right ) {

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

  Lvl1MuSectorLogicData::SectorLogicWord Lvl1MuBarrelSectorLogicData::getWordFormat() const {

    unsigned int val = 0;
    // Bit 0
    if ( is2candidatesInSector() ) {
      val = 1;
    }
    // Bit 1-7
    if ( roi( 0 ) < 0 ) {
      val |= 0x00FE;
    } else {
      val |= ( 0x00C0 | ( roi( 0 ) & 0x01F ) << 1 );
    }
    // Bit 8-9
    if ( ovl( 0 ) < 0 ) {
      val |= 0x0300;
    } else {
      val |= ( ovl( 0 ) & 0x0003 ) << 8;
    }
    // Bit 10-16
    if ( roi( 1 ) < 0 ) {
      val |= 0x01FC00;
    } else {
      val |= ( 0x018000 | ( roi( 1 ) & 0x001F ) << 10 );
    }
    // Bit 17-18
    if ( ovl( 1 ) < 0 ) {
      val |= 0x060000;
    } else {
      val |= ( ovl( 1 ) & 0x0003 ) << 17;
    }
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
    if ( is2candidates( 0 ) ) {
      val |= 0x02000000;
    }
    // bit 26
    if ( is2candidates( 1 ) ) {
      val |= 0x04000000;
    }
    // bit 27-29
    val |= ( bcid() & 0x07 ) << 27;
    // bit 30
    if ( charge( 0 ) > 0 ) val |= 0x040000000;
    // bit 31
    if ( charge( 1 ) > 0 ) val |= 0x080000000;

    return val;
  }


  const Lvl1MuSectorLogicData& Lvl1MuBarrelSectorLogicData::convertFromWordFormat( const SectorLogicWord& value ) {

    unsigned int val;
    // Bit 0
    if ( value & 0x0001 ) {
      set2candidatesInSector();
    } else {
      clear2candidatesInSector();
    }
    // Bit 1-9
    val = ( value >> 1 ) & 0x001F;
    if ( val == 0x001F ) {
      roi( 0, -1 );
      ovl( 0, -1 );
    } else {
      roi( 0, val );
      ovl( 0, ( value >> 8 ) & 0x0003 );
    }
    // Bit 10-18
    val = ( value >> 10 ) & 0x001F;
    if ( val == 0x001F ) {
      roi( 1, -1 );
      ovl( 1, -1 );
    } else {
      roi( 1, val );
      ovl( 1, ( value >> 17 ) & 0x0003 );
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
    val = ( value >> 25 ) & 0x0001;
    if ( val ) {
      set2candidates( 0 );
    } else {
      clear2candidates( 0 );
    }
    // bit 26
    val = ( value >> 26 ) & 0x0001;
    if ( val ) {
      set2candidates( 1 );
    } else {
      clear2candidates( 1 );
    }
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


  void Lvl1MuBarrelSectorLogicData::printOut( std::ostream& out ) const {

    out << "Bit 0     " << " >2candidates in a sector ";
    out << std::setw( 8 ) << m_2candidatesInSector << std::endl;

    out << "Bit 1-5   " << " ROI<1>                   ";
    if ( m_roi[ 0 ] == -1 ) {
      out << std::setw( 8 ) << "NA" << std::endl;
    } else {
      out << std::setw( 8 ) << m_roi[ 0 ] << std::endl;
    }
    out << "Bit  6-7  " << " NA    " << std::endl;
    out << "Bit  8-9  " << " OVL<1>                   ";
    if ( m_ovl[ 0 ] == 0 ) {
      out << std::setw( 8 ) << "No Ovl" << std::endl;
    } else if ( m_ovl[ 0 ] == 1 ) {
      out << std::setw( 8 ) << "Phi" << std::endl;
    } else if ( m_ovl[ 0 ] == 2 ) {
      out << std::setw( 8 ) << "Eta" << std::endl;
    } else if ( m_ovl[ 0 ] == 3 ) {
      out << std::setw( 8 ) << "Phi,Eta" << std::endl;
    } else {
      out << std::setw( 8 ) << "NA" << std::endl;
    }
    out << "Bit 10-14 " << " ROI<2>                   ";
    if ( m_roi[ 1 ] == -1 ) {
      out << std::setw( 8 ) << "NA" << std::endl;
    } else {
      out << std::setw( 8 ) << m_roi[ 1 ] << std::endl;
    }
    out << "Bit 15-16 " << " NA    " << std::endl;
    out << "Bit 17-18 " << " OVL<2>                   ";
    if ( m_ovl[ 1 ] == 0 ) {
      out << std::setw( 8 ) << "No Ovl" << std::endl;
    } else if ( m_ovl[ 1 ] == 1 ) {
      out << std::setw( 8 ) << "Phi" << std::endl;
    } else if ( m_ovl[ 1 ] == 2 ) {
      out << std::setw( 8 ) << "Eta" << std::endl;
    } else if ( m_ovl[ 1 ] == 3 ) {
      out << std::setw( 8 ) << "Phi,Eta" << std::endl;
    } else {
      out << std::setw( 8 ) << "NA" << std::endl;
    }
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

    return;
  }

} // namespace LVL1MUONIF
