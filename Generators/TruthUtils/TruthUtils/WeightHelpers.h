/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef TRUTHUTILS_WEIGHTHELPERS_H
#define TRUTHUTILS_WEIGHTHELPERS_H

/// @file WeightHelpers.h
/// @brief Postprocess generator weight names to be name/ROOT friendly.

#include <string>

namespace MC
{

/// \brief the prefix for generator weights
constexpr char generatorWeightsPrefix[] {"GEN_"};

/// @brief cleanup the weight name
std::string weightNameCleanup(const std::string &name);

/// @brief cleanup the weight name and add prefix
std::string weightNameWithPrefix(const std::string &name);

} // namespace MC

#endif // TRUTHUTILS_WEIGHTHELPERS_H
