/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

  /// Sets word which is to be processed
  void setWord (const uint32_t dataWord) override {m_word = dataWord;}

  /// @brief Returns true if the word is marked as header word
  ///
  /// The word is marked as header if four most significant bits are
  /// equal #s_wordHeader.
  bool isHeader () const override {return s_wordHeader == getBits(31, 28);}

  /// @brief Returns true if the word is marked as data word
  ///
  /// The word is marked as data word if two most significant bits are
  /// equal #s_wordData.
  bool isData() const override {return s_wordData == getBits(31, 30);}

  /// @brief Returns true if the word is marked as service word
  ///
  /// The word is marked as service word if four most significant bits are
  /// equal #s_wordService.
  bool isService() const override {return s_wordService == getBits(31, 28);}

  /// @brief Value of 5-8 most significant bits
  ///
  /// In the following word: `xxxx LLLL xxxx xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `L`.
  uint32_t link() const override {return m_linkNumTrans->translate(getBits (27, 24));}

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
  uint32_t getBits(const uint16_t start, const uint16_t stop) const override;

  /// Header word is marked with four most significant bits set to 0011
  static constexpr uint16_t s_wordHeader = 3;

  /// Data word is marked with two most significant bits set to 11
  static constexpr uint16_t s_wordData = 3;

  /// Service word is marked with four most significant bits set to 0000
  static constexpr uint16_t s_wordService = 0;

private:
  uint32_t m_word;

  ToolHandle<AFP_LinkNumTranslator> m_linkNumTrans {this, "AFP_LinkNumTranslator", "AFP_LinkNumTranslator", "Tool that translates link numbers"};
};

#endif
