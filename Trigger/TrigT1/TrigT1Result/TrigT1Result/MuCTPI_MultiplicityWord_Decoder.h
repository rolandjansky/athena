// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULT_MUCTPI_MULTIPLICITYWORD_DECODER_H
#define TRIGT1RESULT_MUCTPI_MULTIPLICITYWORD_DECODER_H

// System include(s):
#include <cstdint>

/**
 *   $Date: 2007-09-27 15:22:34 $
 *
 *   @short Class helping to decode the MuCTPI multiplicity words
 *
 *          The multiplicity word(s) in the MuCTPI readout data store the number
 *          of collected LVL1 muon candidates in 6 p<sub>T</sub> thresholds in (a)
 *          32-bit word(s). This class can help in decoding such a multiplicity word.
 *
 *          For a detailed description of the data format of the MuCTPI, see
 *          the MIROD documentation (https://edms.cern.ch/file/248757/1/mirod.pdf).
 *
 *  @author David Berge <David.Berge@cern.ch>
 *
 * @version $Revision: 1.5 $
 */
class MuCTPI_MultiplicityWord_Decoder {

public:
  /// Possible trigger threshold modes
  enum TriggerMode_t{
    inclusive, exclusive
  };

  /// Constructor
  MuCTPI_MultiplicityWord_Decoder( const uint32_t multword = 0, const TriggerMode_t mode = exclusive );
  /// Constructor
  MuCTPI_MultiplicityWord_Decoder( const uint32_t multword, const bool inclusiveTriggerMode);
  /// Destructor
  ~MuCTPI_MultiplicityWord_Decoder() = default;

  /// Function returning the total number of muon candidates
  uint16_t getNCandidates() const;
  /// Function returning the 3-bit candidate multiplicity for the specified threshold
  uint16_t getMultiplicity( const uint16_t candidateNumber ) const;
  /// Function returning the 3-bit BCID fragment in the multiplicity word
  uint16_t getBCID() const;

  /// Function setting the MuCTPI multiplicity word
  void setWord( const uint32_t word ) { m_multiplicityWord = word; }
  /// Function returning the MuCTPI multiplicity word
  uint32_t getWord() const { return m_multiplicityWord; }

  /// Function dumping the stored information to the message stream
  void dumpData() const;
  /// Function dumping the stored information to the message stream
  void dumpData( MsgStream& log ) const;

private:
  /// The 32-bit multiplicity word to decode
  uint32_t m_multiplicityWord;
  /// Trigger threshold mode (inclusive/exclusive)
  TriggerMode_t m_triggerMode;
        
}; // class MuCTPI_MultiplicityWord_Decoder

#endif // TRIGT1RESULT_MUCTPI_MULTIPLICITYWORD_DECODER_H
