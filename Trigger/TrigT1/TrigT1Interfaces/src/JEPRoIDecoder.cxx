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

using std::cout;
using std::endl;

namespace LVL1 {

  JEPRoIDecoder::JEPRoIDecoder()
  {
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

  CoordinateRange JEPRoIDecoder::coordinate( const unsigned int roiWord ) const {

    const unsigned int jem = this->module (roiWord);
    unsigned int temp = jem % 8;
    if ( temp == 0 ){
        return leftEndJEMCoordinate(jem, roiWord);
    } else if( temp == 7 ){
        return rightEndJEMCoordinate(jem, roiWord);
    } else return midJEMCoordinate(jem, roiWord);

  }

  /** Extract crate number from Jet RoI word */
  unsigned int JEPRoIDecoder::crate( const unsigned int roiWord ) const {
    return extractBits( roiWord, 29, 1 );
  }

  /** Extract module number from Jet RoI word */
  unsigned int JEPRoIDecoder::module( const unsigned int roiWord ) const {
    return extractBits( roiWord, 25, 4 );
  }

  /** Extract RoI row number within module from Jet RoI word */
  unsigned int JEPRoIDecoder::row( const unsigned int roiWord ) const {
    unsigned int rl = extractBits( roiWord, 20, 2 );
    unsigned int frame = extractBits( roiWord, 22, 3 );
    return ( frame & 3 ) * 2 + ( rl >> 1 );
  }

  /** Extract RoI column number within module from Jet RoI word */
  unsigned int JEPRoIDecoder::column( const unsigned int roiWord ) const {
    unsigned int rl = extractBits( roiWord, 20, 2 );
    unsigned int frame = extractBits( roiWord, 22, 3 );
    return ( frame >> 2 ) * 2 + ( rl & 1 );
  }

  /** Check on Run 1/Run 2*/
  const std::vector<unsigned int> JEPRoIDecoder::thresholdsPassed( const unsigned int word ) const {

    std::vector<unsigned int> threshPassedVec;
    
    if (jetRoIVersion(word) == 1) {
      unsigned int thresholdsPassed = extractBits( word, 1, 8 );
      for ( unsigned int thresh = 0; thresh < 16; ++thresh ) {
        if ( ( 1 << thresh ) & thresholdsPassed ) {
           threshPassedVec.push_back( thresh + 1 );
        }
      }
    }
    
   return threshPassedVec;

  }

  /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5
      it means that this RoI passed thresholds 1,3 and 5.*/
  const std::vector<unsigned int> JEPRoIDecoder::fwdThresholdsPassed( const unsigned int word ) const {

    std::vector<unsigned int> fwdThreshPassedVec;
    
    if (jetRoIVersion(word) == 1) {
      unsigned int fwdThresholdsPassed = extractBits( word, 9, 4 );
      for ( unsigned int thresh = 0; thresh < 4;thresh++ ) {
        if ( ( 1 << thresh ) & fwdThresholdsPassed ) {
          fwdThreshPassedVec.push_back( thresh + 1 );
        }
      } //endfor
    }
    
    return fwdThreshPassedVec;

  }
  

  /** Extract larger cluster ET value from Jet RoI word */
  unsigned int JEPRoIDecoder::etLarge( const unsigned int word ) const {
    if (jetRoIVersion(word) == 1) return 0;
    return extractBits( word, 1, 10 );
  }

  /** Extract smaller cluster ET value from Jet RoI word */
  unsigned int JEPRoIDecoder::etSmall( const unsigned int word ) const {
    if (jetRoIVersion(word) == 1) return 0;
    return extractBits( word, 11, 9 );
  }


  /** return a CoordinateRange for the JEMs coving -2.4<eta<2.4 */
  CoordinateRange JEPRoIDecoder::midJEMCoordinate(const unsigned int jem,
                                                  const unsigned int roiWord) const {

    double phiMin = 0.0;
    double phiMax = 0.0;
    setPhiCoords( jem, roiWord, phiMin, phiMax );

    //eta
    const double JEMEtaSize = 0.8;
    const double jeEtaSize = 0.2;

    const unsigned int col = this->column (roiWord);
    int etaBin = ( jem % 8 ) - 4; //bins range from -4 to +3
    double etaMin = ( etaBin * JEMEtaSize ) + ( jeEtaSize * col );
    double etaMax = etaMin + 2. * jeEtaSize;

    // there is one nasty "special case" as well
    if ( etaMax > 2.5 && etaMax < 2.7 ) etaMax = 2.7;

    CoordinateRange coord;
    coord.setRanges( phiMin, phiMax, etaMin, etaMax );
    return coord;
  }

  /** returns a CoordinateRange for the end JEMs, i.e. 2.4<eta<4.9 */
  CoordinateRange JEPRoIDecoder::leftEndJEMCoordinate(const unsigned int jem,
                                                      const unsigned int roiWord) const {

    double phiMin = 0.0;
    double phiMax = 0.0;
    setPhiCoords( jem, roiWord, phiMin, phiMax );

    double etaMin = 0.0; double etaMax = 0.0;
    switch ( this->column (roiWord) ) {
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
  CoordinateRange JEPRoIDecoder::rightEndJEMCoordinate(const unsigned int jem,
                                                       const unsigned int roiWord) const {

    double phiMin = 0.0;
    double phiMax = 0.0;
    setPhiCoords( jem, roiWord, phiMin, phiMax );
    double etaMin = 0.0; double etaMax = 0.0;
    switch ( this->column (roiWord) ) {
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
  void JEPRoIDecoder::setPhiCoords( const unsigned int jem, const unsigned int roiWord,
                                    double& phiMin, double& phiMax ) const {

    const unsigned int row = this->row (roiWord);
    const unsigned int crate = this->crate (roiWord);

    const double jemPhiSize = M_PI / 2;
    const double jePhiSize = M_PI / 16;
    int crateModifier = ( jem / 8 ) * 2; //=0 or 2
    phiMin = ( crate + crateModifier ) * jemPhiSize + ( jePhiSize * row );
    phiMax = phiMin + 2. * jePhiSize;

    return;
  }

  /** returns the (signed) Ex energy projection. If the RoIWord looks invalid, then zero will be returned. */
  int JEPRoIDecoder::energyX( unsigned int energyRoIWord0 ) const {

    if ( roiType( energyRoIWord0 ) != TrigT1CaloDefs::EnergyRoIWordType0 ) return 0; // wrong type of word
    return decodeEnergyComponent( energyRoIWord0 );
  }

  /** returns the (signed) Ey energy projection. If the RoIWord looks invalid, then zero will be returned. */
  int JEPRoIDecoder::energyY( unsigned int energyRoIWord1 ) const {

    if ( roiType( energyRoIWord1 ) != TrigT1CaloDefs::EnergyRoIWordType1 ) return 0; // wrong type of word
    return decodeEnergyComponent( energyRoIWord1 );
  }

  /** returns the ETSum value. If the RoIWord looks invalid, then zero will be returned. */
  int JEPRoIDecoder::energyT( unsigned int energyRoIWord2 ) const {

    if ( roiType( energyRoIWord2 ) != TrigT1CaloDefs::EnergyRoIWordType2 ) return 0; // wrong type of word
    return extractBits( energyRoIWord2, 1, 15 );
  }

  /** returns the overflow flag for an Energy RoIWord */
  bool JEPRoIDecoder::energyOverflow( unsigned int energyRoIWord ) const {

    if ( extractBits( energyRoIWord, 16, 1 ) != 0 ) {
      return true;
    } else {
      return false;
    }
  }

  /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5 it
      means that this RoI passed thresholds 1,3 and 5.*/
  std::vector<unsigned int> JEPRoIDecoder::mEtSigThresholdsPassed( const unsigned int energyRoIWord0 ) const {

    std::vector<unsigned int> mEtSigThreshPassedVec;
    unsigned int passed = mEtSigThresholdsFlags( energyRoIWord0 );
    for ( unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfMEtSigThresholds; thresh++ ) {
      if ( passed & (1<<thresh)) {
        mEtSigThreshPassedVec.push_back( thresh + 1 );
      }
    } //endfor

    return mEtSigThreshPassedVec;
  }

  /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5 it
      means that this RoI passed thresholds 1,3 and 5.*/
  std::vector<unsigned int> JEPRoIDecoder::etSumThresholdsPassed( const unsigned int energyRoIWord1 ) const {

    std::vector<unsigned int> sumEtThreshPassedVec;
    unsigned int passed = sumEtThresholdsFlags( energyRoIWord1 );
for ( unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfSumEtThresholds; thresh++ ) {
      if ( passed & (1<<thresh)) {
        sumEtThreshPassedVec.push_back( thresh + 1 );
      }
    } //endfor

    return sumEtThreshPassedVec;
  }

  /** returns a vector containing the numbers of threshold passed i.e. if the vector contains 1,3,5 it
      means that this RoI passed thresholds 1,3 and 5.*/
  std::vector<unsigned int> JEPRoIDecoder::etMissThresholdsPassed( const unsigned int energyRoIWord2 ) const {

    std::vector<unsigned int> missEtThreshPassedVec;
    unsigned int passed = missEtThresholdsFlags( energyRoIWord2 );
    for ( unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfMissingEtThresholds; thresh++ ) {
      if ( passed & (1<<thresh)) {
        missEtThreshPassedVec.push_back( thresh + 1 );
      }
    } //endfor

    return missEtThreshPassedVec;
  }

  /** returns true if thresh is passed*/
  bool JEPRoIDecoder::mEtSigThresholdPassed( const unsigned int energyRoIWord0, const unsigned int thresh ) const {

    return ( ( 1 << thresh ) & mEtSigThresholdsFlags( energyRoIWord0 ) );
  }

  /** returns true if thresh is passed*/
  bool JEPRoIDecoder::sumEtThresholdPassed( const unsigned int energyRoIWord1, const unsigned int thresh ) const {

    return ( ( 1 << thresh ) & sumEtThresholdsFlags( energyRoIWord1) );
  }

  /** returns true if thresh is passed*/
  bool JEPRoIDecoder::etMissThresholdPassed( const unsigned int energyRoIWord2, const unsigned int thresh ) const {

    return ( ( 1 << thresh ) & missEtThresholdsFlags( energyRoIWord2 ) );
  }

  unsigned int JEPRoIDecoder::mEtSigThresholdsFlags( const unsigned int energyRoIWord0 ) const {

    return extractBits( energyRoIWord0, 17, TrigT1CaloDefs::numOfMEtSigThresholds );
  }

  unsigned int JEPRoIDecoder::sumEtThresholdsFlags( const unsigned int energyRoIWord1 ) const {

    return extractBits( energyRoIWord1, 17, TrigT1CaloDefs::numOfSumEtThresholds );
  }

  unsigned int JEPRoIDecoder::missEtThresholdsFlags( const unsigned int energyRoIWord2 ) const {

    return extractBits( energyRoIWord2, 17, TrigT1CaloDefs::numOfMissingEtThresholds );
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
