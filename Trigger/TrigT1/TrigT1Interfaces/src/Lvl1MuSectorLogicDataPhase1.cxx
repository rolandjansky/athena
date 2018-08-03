
#include <cstdio>
#include <iostream>
#include <iomanip>

#include "TrigT1Interfaces/Lvl1MuSectorLogicDataPhase1.h"
#include "TrigT1Interfaces/Lvl1MuSectorLogicConstantsPhase1.h"

namespace LVL1MUONIF {

  Lvl1MuSectorLogicDataPhase1::Lvl1MuSectorLogicDataPhase1()
      : 
    m_2candidatesInSector ( false ), 
    m_roi(NULL),
    m_ovl(NULL),
    m_pt(NULL),
    m_2candidates(NULL),
    m_bcid( 0 ),
    m_charge(NULL),
    m_id_system( -1 )
  {
  }
  Lvl1MuSectorLogicDataPhase1::~Lvl1MuSectorLogicDataPhase1()
  {
    delete m_roi;
    delete m_ovl;
    delete m_pt;
    delete m_charge;
    delete m_2candidates;
  }

  void Lvl1MuSectorLogicDataPhase1::initialize()
  {
    if (m_id_system == -1) return;

    size_t ncand = NCAND[m_id_system];
    m_roi = new int[ncand];
    m_ovl = new int[ncand];
    m_pt = new int[ncand];
    m_charge = new int[ncand];
    m_2candidates = new bool[ncand];
    for ( size_t i = 0; i < ncand; ++i ) {
      m_roi[ i ] = -1;
      m_ovl[ i ] = -1;
      m_pt[ i ] = -1;
      m_charge[ i ] = + 1;
      m_2candidates[ i ] = false;
    }
  }


  Lvl1MuSectorLogicDataPhase1::Lvl1MuSectorLogicDataPhase1( const Lvl1MuSectorLogicDataPhase1& right ) {
    *this = right;
  }

  Lvl1MuSectorLogicDataPhase1& Lvl1MuSectorLogicDataPhase1::operator=( const Lvl1MuSectorLogicDataPhase1& right ) {
    if ( this != &right ) {
      m_id_system = right.m_id_system;
      m_bcid = right.m_bcid;
      m_2candidatesInSector = right.m_2candidatesInSector;
      for ( size_t i = 0; i < NCAND[m_id_system]; ++i ) {
        m_roi[ i ] = right.m_roi[ i ];
        m_ovl[ i ] = right.m_ovl[ i ];
        m_pt[ i ] = right.m_pt[ i ];
        m_charge[ i ] = right.m_charge[ i ];
        m_2candidates[ i ] = right.m_2candidates[ i ];
      }
    }
    return *this;
  }

  std::ostream& operator<<( std::ostream& out, const Lvl1MuSectorLogicDataPhase1& right ) {

    right.printOut( out );
    return out;

  }

  // void Lvl1MuSectorLogicDataPhase1::printOut( std::ostream& out ) const {

  //   out << "Bit 0     " << " >2candidates in a sector ";
  //   out << std::setw( 8 ) << m_2candidatesInSector << std::endl;

  //   out << "Bit 1-9   " << " ROI<1>  +         OVL<1> ";
  //   out << std::setw( 8 ) << m_roi[ 0 ] << " : " << m_ovl[ 0 ] << std::endl;
  //   out << "Bit 10-18 " << " ROI<2>  +         OVL<2> ";
  //   out << std::setw( 8 ) << m_roi[ 1 ] << " : " << m_ovl[ 1 ] << std::endl;
  //   out << "Bit 19-21 " << " Pt1                      ";
  //   out << std::setw( 8 ) << m_pt[ 0 ] << std::endl;
  //   out << "Bit 22-24 " << " Pt2                      ";
  //   out << std::setw( 8 ) << m_pt[ 1 ] << std::endl;
  //   out << "Bit 25    " << " >2candidates in ROI1     ";
  //   out << std::setw( 8 ) << m_2candidates[ 0 ] << std::endl;
  //   out << "Bit 26    " << " >2candidates in ROI2     ";
  //   out << std::setw( 8 ) << m_2candidates[ 1 ] << std::endl;
  //   out << "Bit 27-29 " << " BCID                     ";
  //   out << std::setw( 8 ) << m_bcid << std::endl;
  //   out << "Bit 30    " << " charge in ROI1           ";
  //   out << std::setw( 8 ) << m_charge[ 0 ] << std::endl;
  //   out << "Bit 31    " << " charge in ROI2           ";
  //   out << std::setw( 8 ) << m_charge[ 1 ] << std::endl;

  // }

  void Lvl1MuSectorLogicDataPhase1::clear() {
    m_2candidatesInSector = false;
    m_bcid = 0;
    for ( size_t i = 0; i < NCAND[m_id_system]; ++i ) {
      m_roi[ i ] = -1;
      m_ovl[ i ] = -1;
      m_pt[ i ] = -1;
      m_charge[ i ] = -1;
      m_2candidates[ i ] = false;
    }
  }

} // namespace LVL1MUONIF
