/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTH_CLASSIFICATION__TRUTH_CLASSIFICATION_DEFS_H_
#define TRUTH_CLASSIFICATION__TRUTH_CLASSIFICATION_DEFS_H_

#include <iostream>
#include <type_traits>

namespace Truth
{

enum class Type {
  Unknown,
  KnownUnknown,
  IsoElectron,
  ChargeFlipIsoElectron,
  PromptMuon,
  PromptPhotonConversion,
  ElectronFromMuon,
  TauDecay,
  BHadronDecay,
  CHadronDecay,
  LightFlavorDecay,
};

} // namespace Truth

inline std::ostream &operator<<(std::ostream &os, const Truth::Type &obj)
{
  os << static_cast<std::underlying_type<Truth::Type>::type>(obj);
  return os;
}

#endif // TRUTH_CLASSIFICATION__TRUTH_CLASSIFICATION_DEFS_H_
