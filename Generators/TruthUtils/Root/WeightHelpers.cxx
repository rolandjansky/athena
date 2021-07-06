/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


#include <unordered_map>

#include "CxxUtils/StringUtils.h"
#include "TruthUtils/WeightHelpers.h"


namespace MC
{

std::string weightNameCleanup(const std::string &name)
{
  // empty weight is nominal
  if (name.empty())
  {
    return {};
  }

  // Trim trailing whitespace
  std::string out = CxxUtils::StringUtils::trim (name);
  std::string outLowercase = out;
  std::transform(outLowercase.begin(), outLowercase.end(), outLowercase.begin(),
    [](unsigned char c){ return std::tolower(c); });

  // more cases of nominal weights
  if (outLowercase == "nominal" // Powheg calls it "nominal"
    || outLowercase == "weight" // Sherpa names the nominal weight just "Weight"
    || outLowercase == "default"
    || outLowercase == "0")
  {
    return {};
  }

  static const std::vector<std::pair<std::string, std::string>> substitutions =
  {
    {" set = ", "_"}, // Powheg
    {" = ", "_"}, // Powheg
    {"=", ""},
    {",", ""},
    {".", ""},
    {":", ""},
    {" ", "_"},
    {"#", "num"},
    {"\n", "_"},
    {"/", "over"}, // MadGraph
  };

  for (const auto &[before, after] : substitutions)
  {
    size_t location = out.find(before);
    while (location != std::string::npos)
    {
      out.replace(location, before.length(), after);
      location = out.find(before);
    }
  }

  return out;
}

std::string weightNameWithPrefix(const std::string &name)
{
  std::string out = weightNameCleanup(name);
  if (out.empty())
  {
    return out;
  }

  return generatorWeightsPrefix + out;
}

} // namespace MC
