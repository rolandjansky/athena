/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1Interfaces/BitOp.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Result/JetEnergyRoI.h"
#include "TrigT1Interfaces/RoIFormatUpdater.h"

using namespace std;
using namespace LVL1;

namespace ROIB {

  JetEnergyRoI::JetEnergyRoI( uint32_t RoIWord )
    : m_roIWord( RoIWord )
  {
  }

  JetEnergyRoI::JetEnergyRoI()
    : m_roIWord( 0 )
  {
  }

  void JetEnergyRoI::update() {
    /// This was obselete a long time ago
    m_roIWord = RoIFormatUpdater::UpdateJetEnergyRoI( m_roIWord );
    return;
  }

  uint32_t JetEnergyRoI::roIWord() const {
    return m_roIWord;
  }
  
  unsigned int JetEnergyRoI::jetRoIVersion() const {
    int type = BitOp::getValue( &m_roIWord, 0xe0000000 );
    
    if (type == TrigT1CaloDefs::Run1JetType)      return 1;
    else if (type == TrigT1CaloDefs::Run2JetType) return 2;
    
    return 999;
  }

  unsigned int JetEnergyRoI::roIType() const {
    
    if ( m_roIWord != 0 ) {

      int type = BitOp::getValue( &m_roIWord, 0xe0000000 );
    
      if (type == TrigT1CaloDefs::Run1JetType || type == TrigT1CaloDefs::Run2JetType) return TrigT1CaloDefs::JetRoIWordType;
      else if (type == TrigT1CaloDefs::JetEtType)                                     return TrigT1CaloDefs::JetEtRoIWordType;
      else {
        int type2 = BitOp::getValue( &m_roIWord, 0xf0000000 );
        if (type2 == TrigT1CaloDefs::energyRoIWord0)      return TrigT1CaloDefs::EnergyRoIWordType0;
        else if (type2 == TrigT1CaloDefs::energyRoIWord1) return TrigT1CaloDefs::EnergyRoIWordType1;
        else if (type2 == TrigT1CaloDefs::energyRoIWord2) return TrigT1CaloDefs::EnergyRoIWordType2;     
      }
    
    }
    
    return TrigT1CaloDefs::RoIWordTypeError;
  }

  unsigned int JetEnergyRoI::electronicsID() const {
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::JetRoIWordType )
      return 9999;
    else {
      return BitOp::getValue( &m_roIWord, 0x1ff10000 );
    }
  }

  vector< unsigned int > JetEnergyRoI::thresholds() const {
    vector< unsigned int > returnVec;
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::JetRoIWordType )
      returnVec.push_back( 9999 );
    else if ( jetRoIVersion() > 1 ) returnVec.push_back( 0 );
    else {
      for ( unsigned int i = 0;i < TrigT1CaloDefs::numOfJetThresholds+TrigT1CaloDefs::numOfFwdJetThresholds;i++ )
        returnVec.push_back( static_cast< unsigned int >( BitOp::isSet( &m_roIWord, i ) ) );
    }
    return returnVec;
  }

  unsigned int JetEnergyRoI::etLarge() const {
    if (jetRoIVersion() < 2) return 0;
    else                     return BitOp::getValue( &m_roIWord, 0x3ff );
  }

  unsigned int JetEnergyRoI::etSmall() const {
    if (jetRoIVersion() < 2) return 0;
    else                     return BitOp::getValue( &m_roIWord, 0x7fc00 );
  }

  unsigned int JetEnergyRoI::jetEt() const {
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::JetEtRoIWordType  )
      return 9999;
    else {
      return BitOp::getValue( &m_roIWord, 0x0000000f );
    }
  }

  unsigned int JetEnergyRoI::energyX() const {
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::EnergyRoIWordType0 )
      return 9999;
    else {
      return BitOp::getValue( &m_roIWord, 0x0000ffff );
    }
  }

  unsigned int JetEnergyRoI::energyY() const {
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::EnergyRoIWordType1 )
      return 9999;
    else {
      return BitOp::getValue( &m_roIWord, 0x0000ffff );
    }
  }

  unsigned int JetEnergyRoI::energySum() const {
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::EnergyRoIWordType2 )
      return 9999;
    else {
      return BitOp::getValue( &m_roIWord, 0x0000ffff );
    }
  }

  unsigned int JetEnergyRoI::etSumType() const {
    if ( roIType() != TrigT1CaloDefs::EnergyRoIWordType0 &&
	 roIType() != TrigT1CaloDefs::EnergyRoIWordType1 &&
	 roIType() != TrigT1CaloDefs::EnergyRoIWordType2 )
      return 0;
    else {
      return BitOp::getValue( &m_roIWord, 0x04000000 );
    }
  }

  vector< unsigned int > JetEnergyRoI::etSumThresholds() const {
    vector< unsigned int > returnVec;
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::EnergyRoIWordType1 )
      returnVec.push_back( 9999 );
    else  {      
      const unsigned int result = BitOp::getValue( &m_roIWord, 0x00ff0000 );
      for ( unsigned int i = 0;i < TrigT1CaloDefs::numOfSumEtThresholds;i++ )
        returnVec.push_back( static_cast< unsigned int >( BitOp::isSet( &result, i ) ) );
    } 
    return returnVec;
  }

  vector< unsigned int > JetEnergyRoI::etMissThresholds() const {
    vector< unsigned int > returnVec;
    
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::EnergyRoIWordType2 )
      returnVec.push_back( 9999 );
    else {
      const unsigned int result = BitOp::getValue( &m_roIWord, 0x00ff0000 );
      for ( unsigned int i = 0;i < TrigT1CaloDefs::numOfMissingEtThresholds;i++ )
        returnVec.push_back( static_cast< unsigned int >( BitOp::isSet( &result, i ) ) );
    }
    return returnVec;
  }

  vector< unsigned int > JetEnergyRoI::metSigThresholds() const {
    vector< unsigned int > returnVec;
    if ( m_roIWord == 0 || roIType() != TrigT1CaloDefs::EnergyRoIWordType0 )
      returnVec.push_back( 9999 );
    else {
      const unsigned int result = BitOp::getValue( &m_roIWord, 0x00ff0000 );
      for ( unsigned int i = 0;i < TrigT1CaloDefs::numOfMEtSigThresholds;i++ )
        returnVec.push_back( static_cast< unsigned int >( BitOp::isSet( &result, i ) ) );
    }
    return returnVec;
  }


} // namespace ROIB
