/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RECORDREADOUT_H
#define AFP_RECORDREADOUT_H

#include "AFP_ByteStream2RawCnv/IAFP_WordReadOut.h"
#include "AFP_ByteStream2RawCnv/AFP_LinkNumTranslator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>

/// Class capable spliting word into parts, used to fill raw containers.
class AFP_WordReadOut : public extends<AthAlgTool, IAFP_WordReadOut> {
public:
  AFP_WordReadOut(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~AFP_WordReadOut() override;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /// @brief Returns true if the word is marked as header word
  ///
  /// The word is marked as header if four most significant bits are
  /// equal #s_wordHeader.
  bool isHeader (uint32_t the_word) const override {return s_wordHeader == getBits(the_word, 31, 28);}

  /// @brief Returns true if the word is marked as data word
  ///
  /// The word is marked as data word if two most significant bits are
  /// equal #s_wordData.
  bool isData(uint32_t the_word) const override {return s_wordData == getBits(the_word, 31, 30);}

  /// @brief Returns true if the word is marked as service word
  ///
  /// The word is marked as service word if four most significant bits are
  /// equal #s_wordService.
  bool isService(uint32_t the_word) const override {return s_wordService == getBits(the_word, 31, 28);}

  /// @brief Value of 5-8 most significant bits
  ///
  /// In the following word: `xxxx LLLL xxxx xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `L`.
  uint32_t link(uint32_t the_word) const override {return m_linkNumTrans->translate(getBits (the_word, 27, 24));}

  /// @brief Returns integer value of the selcted bits
  ///
  /// Bits are numbered starting from 0 for the least significant bit
  /// and raising as bits significance rises.
  ///
  /// @param start the most significant bit from which decoding should
  /// begin (the start bit will be included)
  ///
  /// @param stop the least significant bit where decoding should end
  /// (the stop bit will be included)
  ///
  /// For example for word `0011 1010` the function will give following results:
  /// - `getBits (7, 4) = 3`
  /// - `getBits (3, 0) = 10`
  /// - `getBits (4, 4) = 1`
  /// - `getBits (2, 2) = 0`
  /// - `getBits (5, 3) = 7`
  uint32_t getBits(uint32_t the_word, const uint16_t start, const uint16_t stop) const override;

  /// Header word is marked with four most significant bits set to 0011
  static constexpr uint16_t s_wordHeader = 3;

  /// Data word is marked with two most significant bits set to 11
  static constexpr uint16_t s_wordData = 3;

  /// Service word is marked with four most significant bits set to 0000
  static constexpr uint16_t s_wordService = 0;

private:
  ToolHandle<AFP_LinkNumTranslator> m_linkNumTrans {this, "AFP_LinkNumTranslator", "AFP_LinkNumTranslator", "Tool that translates link numbers"};
};

#endif
