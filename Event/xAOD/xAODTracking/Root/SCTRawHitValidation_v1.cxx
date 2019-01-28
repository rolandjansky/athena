/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTracking/versions/SCTRawHitValidation_v1.h"

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

namespace xAOD {
  SCTRawHitValidation_v1::SCTRawHitValidation_v1() { }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(SCTRawHitValidation_v1, uint64_t, identifier, setIdentifier)

  static const SG::AuxElement::Accessor<uint32_t> word_acc("dataword");
  void SCTRawHitValidation_v1::setWord(uint32_t new_word) {
    word_acc(*this) = new_word;
  }

  uint32_t SCTRawHitValidation_v1::getWord() const {
    return word_acc(*this);
  }

  // decode size of group of strips information 
  int SCTRawHitValidation_v1::getGroupSize() const {
    return getWord() & 0x7FF;
  }

  // decode strip information
  int SCTRawHitValidation_v1::getStrip() const {
    return (getWord() >> 11) & 0x7FF;
  }

  // decode time bin information
  int SCTRawHitValidation_v1::getTimeBin() const {
    return (getWord() >> 22) & 0x7;
  }

  // returns a word incoding the errors
  int SCTRawHitValidation_v1::getErrors() const {
    return (getWord() >> 25) & 0x7;
  }

  // returns true if the time bin corresponding to the present BC is on
  bool SCTRawHitValidation_v1::OnTime() const {
    return (getWord() >> 23) & 0x1;
  }

  // returns true if there is an error in the first hit's data
  bool SCTRawHitValidation_v1::FirstHitError() const {
    return (getWord() >> 29) & 0x1;
  }

  // returns true if there is an error in the second hit's data
  bool SCTRawHitValidation_v1::SecondHitError() const {
    return (getWord() >> 30) & 0x1;
  }
} // namespace xAOD
