/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOMPOSITEUTILS_COMBINATIONS_H
#define TRIGCOMPOSITEUTILS_COMBINATIONS_H

#include "TrigCompositeUtils/IPartCombItr.h"
#include "TrigCompositeUtils/LinkInfo.h"
#include "xAODBase/IParticleContainer.h"
#include <functional>
#include <vector>

namespace TrigCompositeUtils
{

  class Combinations
  {
  public:
    using VecLInfo_t = std::vector<LinkInfo<xAOD::IParticleContainer>>;
    Combinations(
        const std::vector<std::size_t> &legMultiplicities,
        const std::vector<VecLInfo_t> &legFeatures,
        const std::function<bool(const VecLInfo_t &)> &filter);

    Combinations(
        const std::vector<std::size_t> &legMultiplicities,
        const std::vector<VecLInfo_t> &legFeatures,
        FilterType filter = FilterType::UniqueObjects);

    Combinations(const std::function<bool(const VecLInfo_t &)> &filter);

    Combinations(FilterType filter = FilterType::UniqueObjects);

    void reserve(std::size_t capacity);

    bool empty() const;

    std::size_t size() const;

    void addLeg(std::size_t multiplicity, const VecLInfo_t &features);
    void addLeg(const VecLInfo_t &features);

    IPartCombItr begin() const;
    IPartCombItr end() const;

  private:
    std::function<bool(const VecLInfo_t &)> m_filter;
    std::vector<std::size_t> m_legMultiplicities;
    std::vector<VecLInfo_t> m_legFeatures;
  }; //> end class Combinations
} // namespace TrigCompositeUtils

#endif //> TRIGCOMPOSITEUTILS_COMBINATIONS_H