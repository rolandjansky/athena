/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRUTHCLASSIFICATIONTOOL_H_
#define ITRUTHCLASSIFICATIONTOOL_H_

#include <iostream>
#include <type_traits>

#include <AsgAnalysisInterfaces/IClassificationTool.h>
#include <xAODBase/IParticle.h>


namespace Truth
{

/// \brief truth classification type enum
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



/// \brief a tool interface to classify particles into multiple categories
/// based on their truth information
/// 
/// The tools either assignes them an unsigned integer using the base
/// \ref CP::IClassificationTool interface or a \ref Truth::Type enum
/// representing possible truth classification classes.
class ITruthClassificationTool : virtual public CP::IClassificationTool
{
  ASG_TOOL_INTERFACE(ITruthClassificationTool)

public:
  virtual ~ITruthClassificationTool() = default;

  /// \brief classify and return unsigned int
  /// re-declaration needed due to the same name used
  virtual StatusCode classify(const xAOD::IParticle &particle,
                              unsigned int &classification) const = 0;

  /// \brief classify and return Truth::Type
  virtual StatusCode classify(const xAOD::IParticle &particle,
                              Truth::Type &classification) const = 0;
};

#endif // ITRUTHCLASSIFICATIONTOOL_H_
