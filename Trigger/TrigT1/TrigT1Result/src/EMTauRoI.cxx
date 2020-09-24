/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1Result/EMTauRoI.h"
#include "TrigT1Interfaces/BitOp.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/RoIFormatUpdater.h"

using namespace LVL1;
using namespace std;

namespace ROIB {

  EMTauRoI::EMTauRoI( uint32_t RoIWord ) 
    : m_roIWord( RoIWord )
  {
  }

  EMTauRoI::EMTauRoI()
    : m_roIWord( 0 )
  {
  }

  void EMTauRoI::update() {
    // Obsolete a long time ago.
    m_roIWord = RoIFormatUpdater::UpdateEMTauRoI(m_roIWord) ;
    return;
  }

  uint32_t EMTauRoI::roIWord() const {
    return m_roIWord;
  }

  /** Return RoI type. Types defined in TrigT1Interfaces::RoIType.
     Run 1 = CpRoIWordType, Run 2 = EMRoIWordType or TauRoIWordType */
  unsigned int EMTauRoI::roIType() const {
    if ( m_roIWord != 0 ) {
      unsigned int type  = BitOp::getValue( &m_roIWord, 0xc0000000 );
      if (type == TrigT1CaloDefs::EMTauRoIWord) {
        return TrigT1CaloDefs::CpRoIWordType;
      }
      else {
        unsigned int temp2 = BitOp::getValue( &m_roIWord, 0xf0000000 );
        if (temp2 == TrigT1CaloDefs::EMRoIWord) {
           return TrigT1CaloDefs::EMRoIWordType;
        }
        else if (temp2 == TrigT1CaloDefs::TauRoIWord) {
           return TrigT1CaloDefs::TauRoIWordType;
        }        
      }
    }
    return TrigT1CaloDefs::RoIWordTypeError;
  }

  /** Return bitmask containing coordinate information (crate, module, chip, ocation) */
  unsigned int EMTauRoI::electronicsID() const {
    if ( roIType() == TrigT1CaloDefs::CpRoIWordType )
      return BitOp::getValue( &m_roIWord, 0x3ffc0000 );
    else if ( roIType() == TrigT1CaloDefs::EMRoIWordType || roIType() == TrigT1CaloDefs::TauRoIWordType )
      return BitOp::getValue( &m_roIWord, 0x0fff0000 );
    else
      return 9999;  
  }

  /** Return vector of thresholds passed for Run 1 RoI */
  vector< unsigned int > EMTauRoI::thresholds() const {

    std::vector<unsigned int> returnVec;
    if ( roIType() != TrigT1CaloDefs::CpRoIWordType )
      returnVec.push_back( 9999 );
    else {
      for ( unsigned int i = 0;i < TrigT1CaloDefs::numOfCPThresholds;i++ )
        returnVec.push_back( static_cast< unsigned int >( BitOp::isSet( &m_roIWord, i ) ) );
    }
    return returnVec;
  }

  /** Return cluster ET for Run 2 RoI */
  unsigned int EMTauRoI::et() const {

    if ( roIType() == TrigT1CaloDefs::EMRoIWordType || roIType() == TrigT1CaloDefs::TauRoIWordType )
      return BitOp::getValue( &m_roIWord, 0x000000ff );
    
    return 0;
  }

  /** Return isolation flags for Run 2 RoI */
  unsigned int EMTauRoI::isolation() const {

    if ( roIType() == TrigT1CaloDefs::EMRoIWordType || roIType() == TrigT1CaloDefs::TauRoIWordType )
      return BitOp::getValue( &m_roIWord, 0x00001f00 );
    
    return 0;
  }

} // namespace ROIB
