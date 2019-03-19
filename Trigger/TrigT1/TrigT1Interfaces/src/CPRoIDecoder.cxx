/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         CPRoIDecoder.cxx  -  description
                            -------------------
   begin                : Fri Apr 19 2002
   email                : moyse@ph.qmw.ac.uk
***************************************************************************/

#include <cmath>
#include <iostream>

#include "TrigT1Interfaces/CPRoIDecoder.h"

using std::cout;
using std::endl;

namespace LVL1 {

  CPRoIDecoder::CPRoIDecoder()
  {
  }

  CPRoIDecoder::~CPRoIDecoder() {

  }
  

   /** Override base method - allows us to handle both Run 1 & Run 2 data for CP system*/
  LVL1::TrigT1CaloDefs::RoIType LVL1::CPRoIDecoder::roiType( unsigned int word ) const{
   unsigned int temp=extractBits(word, 31, 2);
   if (temp == TrigT1CaloDefs::EMTauRoIWord) {
      return TrigT1CaloDefs::CpRoIWordType;
   }
   else {
      unsigned int temp2 = extractBits(word, 29, 4);
      if (temp2 == TrigT1CaloDefs::EMRoIWord) {
         return TrigT1CaloDefs::EMRoIWordType;
      }
      else if (temp2 == TrigT1CaloDefs::TauRoIWord) {
         return TrigT1CaloDefs::TauRoIWordType;
      }
   }
   return TrigT1CaloDefs::RoIWordTypeError;
  }


  /** Return coordinate information for RoI */
  CoordinateRange CPRoIDecoder::coordinate( const unsigned int roiWord ) const {

    //phi
    const double cratePhiSize = M_PI / 2.0;
    const double fpgaPhiSize = M_PI / 16.0;
    const double localCoordPhiSize = M_PI / 32.0;

    //eta
    const double cpmEtaSize = 0.4;
    const double localCoordEtaSize = 0.1;

    // the FPGAs local ROI coords are arranged like this:
    // 2 3 6 7 (top)
    // 0 1 4 5 (bot)
    // (where each cell is 0.1x0.1)
    // so

    unsigned int crate = this->crate (roiWord);
    unsigned int cpm   = this->module (roiWord);
    unsigned int cp    = this->chip (roiWord);
    unsigned int lc    = this->localcoord (roiWord);

    unsigned int top = ( lc & 2 ) >> 1; // top=1, bot=0
    double localCoordEta = ( ( ( lc & 4 ) >> 1 ) + ( lc & 1 ) ) * localCoordEtaSize;

    double phiMin = ( static_cast< double >( crate ) * cratePhiSize ) +
                    ( static_cast< double >( cp ) * fpgaPhiSize ) + ( top * localCoordPhiSize );
    double phiMax = ( static_cast< double >( crate ) * cratePhiSize ) +
                    ( static_cast< double >( cp ) * fpgaPhiSize ) + ( top * localCoordPhiSize ) + 2. * localCoordPhiSize;
    double etaMin = ( ( static_cast< double >( static_cast< int >( cpm ) - 8 ) ) * cpmEtaSize ) + localCoordEta;

    double etaMax = ( static_cast< double >( static_cast< int >( cpm ) - 8 ) * cpmEtaSize ) + localCoordEta + 2. * localCoordEtaSize;

    if ( RoIDecoder::m_DEBUG ) {

      cout << "phiMin : " << phiMin << "phiMax : " << phiMax << endl
           << "etaMin : " << etaMin << "etaMax : " << etaMax << endl
           << "Phi" << endl << "===" << endl
           << "Crate phi min   : " << ( crate * cratePhiSize ) << " (size: " << ( cratePhiSize ) << ")" << endl
           << "CP FPGA phi min : " << ( cp * fpgaPhiSize ) << " (size: " << ( fpgaPhiSize ) << ")" << endl
           << "Local coord min : " << ( top * localCoordPhiSize ) << " (size: " << ( localCoordPhiSize ) << ")" << endl
           << "Eta" << endl << "===" << endl
           << "CPM eta min     : " << ( ( cpm - 8 ) * cpmEtaSize ) << " (size: " << ( cpmEtaSize ) << ")" << endl
           << "CP FPGA phi min : " << localCoordEta << " (size: " << ( localCoordEtaSize ) << ")" << endl;

    }

    CoordinateRange coord;
    coord.setRanges( phiMin, phiMax, etaMin, etaMax );
    return coord;

  }

  /** Decode crate number from RoI word */
  unsigned int CPRoIDecoder::crate( const unsigned int roiWord ) const {
    int offset = 0;
    if (roiType( roiWord ) == TrigT1CaloDefs::CpRoIWordType) offset = 2;
    return extractBits( roiWord, 27+offset, 2 );
  }

  /** Decode module number from RoI word */
  unsigned int CPRoIDecoder::module( const unsigned int roiWord ) const {
    int offset = 0;
    if (roiType( roiWord ) == TrigT1CaloDefs::CpRoIWordType) offset = 2;
    return extractBits( roiWord, 23+offset, 4 );
  }

  /** Decode CP chip number from RoI word */
  unsigned int CPRoIDecoder::chip( const unsigned int roiWord ) const {
    int offset = 0;
    if (roiType( roiWord ) == TrigT1CaloDefs::CpRoIWordType) offset = 2;
    return extractBits( roiWord, 20+offset, 3 );
  }

  /** Decode local coordinate from RoI word */
  unsigned int CPRoIDecoder::localcoord( const unsigned int roiWord ) const {
    int offset = 0;
    if (roiType( roiWord ) == TrigT1CaloDefs::CpRoIWordType) offset = 2;
    return extractBits( roiWord, 17+offset, 3 );
  }

  /** Extract cluster ET from Run 2 RoI word */
  unsigned int CPRoIDecoder::et( const unsigned int roiWord ) const {
    unsigned int type = roiType( roiWord );
    
    if ( type == TrigT1CaloDefs::EMRoIWordType || type == TrigT1CaloDefs::TauRoIWordType )
       return extractBits(roiWord, 1, 8);
    
    return 0;
  }
  
  /** Extract isolation results from Run 2 RoI word */
  unsigned int CPRoIDecoder::isolationWord( const unsigned int roiWord ) const {
    unsigned int type = roiType( roiWord );
    
    if ( type == TrigT1CaloDefs::EMRoIWordType || type == TrigT1CaloDefs::TauRoIWordType )
       return extractBits(roiWord, 9, 5);
    
    return 0;
  }
  

  /** Override default version by adding a check on Run 1/Run 2*/
  const std::vector<unsigned int> CPRoIDecoder::thresholdsPassed( const unsigned int word ) const {

    std::vector<unsigned int> threshPassedVec;
    
    if (roiType(word) == TrigT1CaloDefs::CpRoIWordType) {
      unsigned int hitmask = word&0xffff;
      for ( unsigned int thresh = 0; thresh < 16; ++thresh ) {
        if ( ( 1 << thresh ) & hitmask ) {
          threshPassedVec.push_back( thresh + 1 );
        }
      }
    }
    
   return threshPassedVec;

  }

  

} // namespace LVL1
