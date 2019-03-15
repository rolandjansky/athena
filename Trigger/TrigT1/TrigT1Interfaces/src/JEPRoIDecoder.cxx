/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         JEPRoIDecoder.cxx  -  description
                            -------------------
   begin                : Fri Apr 19 2002
   email                : moyse@ph.qmw.ac.uk
***************************************************************************/


#include <cmath>
#include <iostream>

#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

using namespace std;

namespace LVL1 {

  JEPRoIDecoder::JEPRoIDecoder()
    : m_fwdThresholdsPassed( 0 ), m_jem( 0 ), m_row( 0 ), m_col( 0 ),
      m_mEtSigThresholdsPassed( 0 ), m_sumEtThresholdsPassed( 0 ), m_missEtThresholdsPassed( 0 ),
      m_etLarge(0), m_etSmall(0), m_ex( 0 ), m_ey( 0 ), m_et( 0 ) {

  }

  JEPRoIDecoder::~JEPRoIDecoder() {

  }
  

/** Override base method - allows us to handle both Run 1 & Run 2 data for JEP (up to a point)*/
LVL1::TrigT1CaloDefs::RoIType LVL1::JEPRoIDecoder::roiType( unsigned int word ) const{
   unsigned int temp=extractBits(word, 30, 3);
   if (temp == TrigT1CaloDefs::Run1JetType) {
      return TrigT1CaloDefs::JetRoIWordType;
   }
   else if (temp == TrigT1CaloDefs::Run2JetType) {
      return TrigT1CaloDefs::JetRoIWordType;
   }
   else if (temp == TrigT1CaloDefs::JetEtType) {
      return TrigT1CaloDefs::JetEtRoIWordType;
   }
   else {
      unsigned int temp2 = extractBits(word, 29, 4);
      if (temp2 == TrigT1CaloDefs::energyRoIWord0) {
         return TrigT1CaloDefs::EnergyRoIWordType0;
      }
      else if (temp2 == TrigT1CaloDefs::energyRoIWord1) {
         return TrigT1CaloDefs::EnergyRoIWordType1;
      }
      else if (temp2 == TrigT1CaloDefs::energyRoIWord2) {
         return TrigT1CaloDefs::EnergyRoIWordType2;
      }
   }
   return TrigT1CaloDefs::RoIWordTypeError;
}

unsigned int LVL1::JEPRoIDecoder::jetRoIVersion( unsigned int word ) const {
   unsigned int temp=extractBits(word, 30, 3);
   if (temp == TrigT1CaloDefs::Run1JetType)      return 1;
   else if (temp == TrigT1CaloDefs::Run2JetType) return 2;
   else                                          return 999;
}

  CoordinateRange JEPRoIDecoder::coordinate( const unsigned int roiWord ) {

    decodeWord( roiWord );
    unsigned int temp = m_jem % 8;
    if ( temp == 0 ){
        return leftEndJEMCoordinate();
    } else if( temp == 7 ){
        return rightEndJEMCoordinate();
    } else return midJEMCoordinate();

  }

  /** Extract crate number from Jet RoI word */
  unsigned int JEPRoIDecoder::crate( const unsigned int roiWord ) {
    decodeWord( roiWord );
    return m_crate;
  }

  /** Extract module number from Jet RoI word */
  unsigned int JEPRoIDecoder::module( const unsigned int roiWord ) {
    decodeWord( roiWord );
    return m_jem;
  }

  /** Extract RoI row number within module from Jet RoI word */
  unsigned int JEPRoIDecoder::row( const unsigned int roiWord ) {
    decodeWord( roiWord );
    return m_row;
  }

  /** Extract RoI column number within module from Jet RoI word */
  unsigned int JEPRoIDecoder::column( const unsigned int roiWord ) {
    decodeWord( roiWord );
    return m_col;
  }

  /** Override default version by adding a check on Run 1/Run 2*/
  const std::vector<unsigned int>& JEPRoIDecoder::thresholdsPassed( const unsigned int word ) {

    m_threshPassedVec.clear();
    
    if (jetRoIVersion(word) == 1) {
      decodeWord( word );
      for ( unsigned int thresh = 0; thresh < 16; ++thresh ) {
        if ( ( 1 << thresh ) & m_thresholdsPassed ) {
           m_threshPassedVec.push_back( thresh + 1 );
        }
      }
    }
    
   return m_threshPassedVec;

  }

  /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5
      it means that this RoI passed thresholds 1,3 and 5.*/
  std::vector<unsigned int>& JEPRoIDecoder::fwdThresholdsPassed( const unsigned int word ) {

    m_fwdThreshPassedVec.clear();
    
    if (jetRoIVersion(word) == 1) {
      decodeWord( word );
      for ( unsigned int thresh = 0; thresh < 4;thresh++ ) {
        if ( ( 1 << thresh ) & m_fwdThresholdsPassed ) {
          m_fwdThreshPassedVec.push_back( thresh + 1 );
        }
      } //endfor
    }
    
    return m_fwdThreshPassedVec;

  }
  

  /** Extract larger cluster ET value from Jet RoI word */
  unsigned int JEPRoIDecoder::etLarge( const unsigned int word ) {
    if (jetRoIVersion(word) == 1) return 0;
    decodeWord( word );
    return m_etLarge;
  }

  /** Extract smaller cluster ET value from Jet RoI word */
  unsigned int JEPRoIDecoder::etSmall( const unsigned int word ) {
    if (jetRoIVersion(word) == 1) return 0;
    decodeWord( word );
    return m_etSmall;
  }


  /** get information from RoI word and store in member variables. */
  void JEPRoIDecoder::decodeWord( const unsigned int word ) {

    // Run 1 data
    m_thresholdsPassed = extractBits( word, 1, 8 );
    m_fwdThresholdsPassed = extractBits( word, 9, 4 );
    
    // Run 2 data
    m_etLarge = extractBits( word, 1, 10 );
    m_etSmall = extractBits( word, 11, 9 );
    
    // Common to both
    unsigned int rl = extractBits( word, 20, 2 );
    unsigned int frame = extractBits( word, 22, 3 );
    m_jem = extractBits( word, 25, 4 );
    m_crate = extractBits( word, 29, 1 );

    m_col = ( frame >> 2 ) * 2 + ( rl & 1 );
    m_row = ( frame & 3 ) * 2 + ( rl >> 1 );


    if ( RoIDecoder::m_DEBUG ) {
      cout << "JEPRoIDecoder: RoIWord being analysed - " << hex << word << dec << endl;
      cout << "Crate : " << m_crate << endl
           << "jem   : " << m_jem << endl
           << "row    : " << m_row << endl
           << "col    : " << m_col << endl;
    } //end debug output

    return;
  }


  /** return a CoordinateRange for the JEMs coving -2.4<eta<2.4 */
  CoordinateRange JEPRoIDecoder::midJEMCoordinate() const {

    double phiMin = 0.0;
    double phiMax = 0.0;
    setPhiCoords( phiMin, phiMax );

    //eta
    const double JEMEtaSize = 0.8;
    const double jeEtaSize = 0.2;

    int etaBin = ( m_jem % 8 ) - 4; //bins range from -4 to +3
    double etaMin = ( etaBin * JEMEtaSize ) + ( jeEtaSize * m_col );
    double etaMax = etaMin + 2. * jeEtaSize;

    // there is one nasty "special case" as well
    if ( etaMax > 2.5 && etaMax < 2.7 ) etaMax = 2.7;

    CoordinateRange coord;
    coord.setRanges( phiMin, phiMax, etaMin, etaMax );
    return coord;
  }

  /** returns a CoordinateRange for the end JEMs, i.e. 2.4<eta<4.9 */
  CoordinateRange JEPRoIDecoder::leftEndJEMCoordinate() const {

    double phiMin = 0.0;
    double phiMax = 0.0;
    setPhiCoords( phiMin, phiMax );

    double etaMin = 0.0; double etaMax = 0.0;
    switch ( m_col ) {
    case 0:
      etaMin = -4.9; etaMax = -2.9;
      break;
    case 1:
      etaMin = -3.2; etaMax = -2.7;
      break;
    case 2:
      etaMin = -2.9; etaMax = -2.4;
      break;
    case 3:
      etaMin = -2.7; etaMax = -2.2;
      break;
    }

    CoordinateRange coord( phiMin, phiMax, etaMin, etaMax );
    return coord;
  }

  /** returns a CoordinateRange for the end JEMs, i.e. 2.4<eta<4.9 */
  CoordinateRange JEPRoIDecoder::rightEndJEMCoordinate() const {

    double phiMin = 0.0;
    double phiMax = 0.0;
    setPhiCoords( phiMin, phiMax );
    double etaMin = 0.0; double etaMax = 0.0;
    switch ( m_col ) {
    case 0:
      etaMin = 2.4; etaMax = 2.9;
      break;
    case 1:
      etaMin = 2.7; etaMax = 3.2;
      break;
    case 2:
      etaMin = 2.9; etaMax = 4.9;  // to put centre at HEC-FCAL boundary
      break;
    case 3:
      etaMin = 2.9; etaMax = 4.9;  // distinction between these 2 not useful to L2
      break;
    }

    CoordinateRange coord( phiMin, phiMax, etaMin, etaMax );
    return coord;
  }

  /** set phi coords for Jet RoI */
  void JEPRoIDecoder::setPhiCoords( double& phiMin, double& phiMax ) const {

    const double jemPhiSize = M_PI / 2;
    const double jePhiSize = M_PI / 16;
    int crateModifier = ( m_jem / 8 ) * 2; //=0 or 2
    phiMin = ( m_crate + crateModifier ) * jemPhiSize + ( jePhiSize * m_row );
    phiMax = phiMin + 2. * jePhiSize;

    return;
  }

  /** returns the (signed) Ex energy projection. If the RoIWord looks invalid, then zero will be returned. */
  int JEPRoIDecoder::energyX( unsigned int energyRoIWord0 ) {

    if ( roiType( energyRoIWord0 ) != TrigT1CaloDefs::EnergyRoIWordType0 ) return 0; // wrong type of word
    decodeEnergyWord0( energyRoIWord0 );
    return m_ex;
  }

  /** returns the (signed) Ey energy projection. If the RoIWord looks invalid, then zero will be returned. */
  int JEPRoIDecoder::energyY( unsigned int energyRoIWord1 ) {

    if ( roiType( energyRoIWord1 ) != TrigT1CaloDefs::EnergyRoIWordType1 ) return 0; // wrong type of word
    decodeEnergyWord1( energyRoIWord1 );
    return m_ey;
  }

  /** returns the ETSum value. If the RoIWord looks invalid, then zero will be returned. */
  int JEPRoIDecoder::energyT( unsigned int energyRoIWord2 ) {

    if ( roiType( energyRoIWord2 ) != TrigT1CaloDefs::EnergyRoIWordType2 ) return 0; // wrong type of word
    decodeEnergyWord2( energyRoIWord2 );
    return m_et;
  }

  /** returns the overflow flag for an Energy RoIWord */
  bool JEPRoIDecoder::energyOverflow( unsigned int energyRoIWord ) {

    if ( extractBits( energyRoIWord, 16, 1 ) != 0 ) {
      return true;
    } else {
      return false;
    }
  }

  /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5 it
      means that this RoI passed thresholds 1,3 and 5.*/
  std::vector<unsigned int>& JEPRoIDecoder::mEtSigThresholdsPassed( const unsigned int energyRoIWord0 ) {

    m_mEtSigThreshPassedVec.clear();
    for ( unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfMEtSigThresholds; thresh++ ) {
      if ( mEtSigThresholdPassed( energyRoIWord0, thresh ) ) {
        m_mEtSigThreshPassedVec.push_back( thresh + 1 );
      }
    } //endfor

    return m_mEtSigThreshPassedVec;
  }

  /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5 it
      means that this RoI passed thresholds 1,3 and 5.*/
  std::vector<unsigned int>& JEPRoIDecoder::etSumThresholdsPassed( const unsigned int energyRoIWord1 ) {

    m_sumEtThreshPassedVec.clear();
    for ( unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfSumEtThresholds; thresh++ ) {
      if ( sumEtThresholdPassed( energyRoIWord1, thresh ) ) {
        m_sumEtThreshPassedVec.push_back( thresh + 1 );
      }
    } //endfor

    return m_sumEtThreshPassedVec;
  }

  /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5 it
      means that this RoI passed thresholds 1,3 and 5.*/
  std::vector<unsigned int>& JEPRoIDecoder::etMissThresholdsPassed( const unsigned int energyRoIWord2 ) {

    m_missEtThreshPassedVec.clear();
    for ( unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfMissingEtThresholds; thresh++ ) {
      if ( etMissThresholdPassed( energyRoIWord2, thresh ) ) {
        m_missEtThreshPassedVec.push_back( thresh + 1 );
      }
    } //endfor

    return m_missEtThreshPassedVec;
  }

  /** returns true if thresh is passed*/
  bool JEPRoIDecoder::mEtSigThresholdPassed( const unsigned int energyRoIWord0, const unsigned int thresh ) {

    decodeEnergyWord0( energyRoIWord0 );
    return ( ( 1 << thresh ) & m_mEtSigThresholdsPassed );
  }

  /** returns true if thresh is passed*/
  bool JEPRoIDecoder::sumEtThresholdPassed( const unsigned int energyRoIWord1, const unsigned int thresh ) {

    decodeEnergyWord1( energyRoIWord1 );
    return ( ( 1 << thresh ) & m_sumEtThresholdsPassed );
  }

  /** returns true if thresh is passed*/
  bool JEPRoIDecoder::etMissThresholdPassed( const unsigned int energyRoIWord2, const unsigned int thresh ) {

    decodeEnergyWord2( energyRoIWord2 );
    return ( ( 1 << thresh ) & m_missEtThresholdsPassed );
  }

  void JEPRoIDecoder::decodeEnergyWord0( const unsigned int energyRoIWord0 ) {

    m_ex = decodeEnergyComponent( energyRoIWord0 );
    m_mEtSigThresholdsPassed = extractBits( energyRoIWord0, 17, TrigT1CaloDefs::numOfMEtSigThresholds );
    return;
  }

  void JEPRoIDecoder::decodeEnergyWord1( const unsigned int energyRoIWord1 ) {

    m_ey = decodeEnergyComponent( energyRoIWord1 );
    m_sumEtThresholdsPassed = extractBits( energyRoIWord1, 17, TrigT1CaloDefs::numOfSumEtThresholds );
    return;
  }

  void JEPRoIDecoder::decodeEnergyWord2( const unsigned int energyRoIWord2 ) {

    m_et = extractBits( energyRoIWord2, 1, 15 );
    m_missEtThresholdsPassed = extractBits( energyRoIWord2, 17, TrigT1CaloDefs::numOfMissingEtThresholds );
    return;
  }

  int JEPRoIDecoder::decodeEnergyComponent( const unsigned int energyRoIWord ) const {

    /** Ex, Ey values are stored in 15 bit twos complement format */
    int value = extractBits( energyRoIWord, 1, 15 );
    int negative = extractBits( energyRoIWord, 15, 1 );

    /** Convert negative 15-bit complement values to ints */
    if ( negative != 0 ) {
      int complement = ~value;
      value = -( (complement+1)&0x7FFF) ;
    }
    
    return value;
  }

} // namespace LVL1
