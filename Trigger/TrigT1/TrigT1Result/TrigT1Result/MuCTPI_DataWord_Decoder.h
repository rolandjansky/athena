// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULT_MUCTPI_DATAWORD_DECODER_H
#define TRIGT1RESULT_MUCTPI_DATAWORD_DECODER_H

// System include(s):
#include <cstdint>

// Local include(s):
#include "TrigT1Result/MuCTPI_RDO.h"

/**
 *   $Date: 2007-07-05 13:26:22 $
 *
 *   @short Class helping to decode the MuCTPI data words
 *
 *          The data words of the MuCTPI readout data contain a lot of information
 *          about the LVL1 muon candidates in a tightly encoded fashion. This class
 *          can be used to easily extract all stored information from these 32-bit
 *          words.
 *
 *          For a detailed description of the data format of the MuCTPI, see
 *          the MIROD documentation (https://edms.cern.ch/file/248757/1/mirod.pdf).
 *
 *  @author David Berge <David.Berge@cern.ch>
 *
 * @version $Revision: 1.5 $
 */
class MuCTPI_DataWord_Decoder {

public:
  /// Constructor
  MuCTPI_DataWord_Decoder( uint32_t dataword = 0 );
  /// Destructor
  ~MuCTPI_DataWord_Decoder() = default;

  /// Flag showing whether there were more than 2 candidates in this candidate's sector
  uint16_t getSectorOverflow() const;
  /// Flag showing whether there was more than 1 candidate in this candidate's RoI
  uint16_t getRoiOverflow() const;
  /// RoI number (sub-sector in which the candidate was detected)
  uint16_t getRoiNumber() const;
  /// 2-bit overlap bits given to the candidate by the SL
  uint16_t getOverlapBits() const;
  /// The system that detected the muon candidate
  MuCTPI_RDO::SectorLocation getSectorLocation() const;
  /// 8-bit address of the sector that detected the candidate
  uint16_t getSectorAddress() const;
  /// Number of the sector that detected the candidate
  uint16_t getSectorID(bool newScheme=0) const;
  /// The hemisphere in which the candidate was detected
  uint16_t getHemisphere() const;
  /// 3-bit p<sub>T</sub> threshold of the candidate
  uint16_t getPt() const;
  /// 3-bit BCID fragment saved with the candidate
  uint16_t getBCID() const;
  /// Flag showing whether the candidate had the highest p<sub>T</sub> in its sector
  uint16_t getCandidateIsHighestPt() const;
  /// Flag showing whether the candidate was sent to the RoIB
  uint16_t getSentRoi() const;
  /// Get the candidate's sign (0=negative, 1=positive, 100=undefined)
  uint16_t getSign() const;
  /// Flag showing if the candidate was vetoed in the multiplicity sum
  uint16_t getVetoed() const;

  /// Function setting the MuCTPI data word
  void setWord( uint32_t word ) { m_dataWord = word; }
  /// Function returning the MuCTPI data word
  uint32_t getWord() const { return m_dataWord; }

  /// Function dumping the stored information to the message stream
  void dumpData() const;
  /// Function dumping the stored information to the message stream
  void dumpData( MsgStream& log ) const;

private:
  /// The 32-bit data word to decode
  uint32_t m_dataWord;
  // Add bitset here?

}; // class MuCTPI_DataWord_Decoder

#endif // TRIGT1RESULT_MUCTPI_DATAWORD_DECODER_H
