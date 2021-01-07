/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/Combinations.h"
#include <tuple>
#include <algorithm>
#include <numeric>

namespace TrigCompositeUtils
{
  Combinations::Combinations(
      const std::vector<std::size_t> &legMultiplicities,
      const std::vector<VecLInfo_t> &legFeatures,
      const std::function<bool(const VecLInfo_t &)> &filter)
      : m_filter(filter),
        m_legMultiplicities(legMultiplicities),
        m_legFeatures(legFeatures)
  {
    if (legMultiplicities.size() != legFeatures.size())
      throw std::invalid_argument("Different numbers of multiplicities and features provided");
  }

  Combinations::Combinations(
      const std::vector<std::size_t> &legMultiplicities,
      const std::vector<VecLInfo_t> &legFeatures,
      FilterType filter)
      : Combinations(legMultiplicities, legFeatures, getFilter(filter))
  {
  }

  Combinations::Combinations(const std::function<bool(const VecLInfo_t &)> &filter)
      : Combinations(std::vector<std::size_t>{}, std::vector<VecLInfo_t>{}, filter)
  {
  }

  Combinations::Combinations(FilterType filter) : Combinations(getFilter(filter)) {}

  void Combinations::reserve(std::size_t capacity)
  {
    m_legMultiplicities.reserve(capacity);
    m_legFeatures.reserve(capacity);
  }

  bool Combinations::empty() const
  {
    return begin() == end();
  }

  std::size_t Combinations::size() const
  {
    return std::accumulate(m_legMultiplicities.begin(), m_legMultiplicities.end(), std::size_t(0));
  }

  void Combinations::addLeg(std::size_t multiplicity, const VecLInfo_t &features)
  {
    m_legMultiplicities.push_back(multiplicity);
    m_legFeatures.push_back(features);
  }

  void Combinations::addLeg(const VecLInfo_t &features)
  {
    addLeg(1, features);
  }

  IPartCombItr Combinations::begin() const
  {
    // Build up the constructor arguments
    std::vector<std::tuple<std::size_t, VecLInfo_t::const_iterator, VecLInfo_t::const_iterator>> args;
    for (std::size_t ii = 0; ii < m_legFeatures.size(); ++ii)
      args.push_back(std::make_tuple(
          m_legMultiplicities.at(ii),
          m_legFeatures.at(ii).begin(),
          m_legFeatures.at(ii).end()));
    return IPartCombItr(args, m_filter);
  }

  IPartCombItr Combinations::end() const
  {
    return IPartCombItr();
  }
} // namespace TrigCompositeUtils