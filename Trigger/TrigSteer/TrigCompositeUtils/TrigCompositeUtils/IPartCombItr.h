/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOMPOSITEUTILS_IPARTCOMBITR_H
#define TRIGCOMPOSITEUTILS_IPARTCOMBITR_H

#include <iterator>
#include <vector>
#include <tuple>
#include <utility>
#include <algorithm>
#include <functional>
#include "xAODBase/IParticleContainer.h"
#include "TrigCompositeUtils/KFromNItr.h"
#include "TrigCompositeUtils/LinkInfo.h"

namespace TrigCompositeUtils
{
  enum class FilterType
  {
    /// Allow all combinations
    All,
    /// Do not allow any repeated objects
    UniqueObjects,
    /// Do not allow any two objects to share an RoI
    UniqueRoIs,
    /// Do not allow any two objects to share an initial RoI
    UniqueInitialRoIs
  };
  /// Helper fucntion that returns true if no objects are repeated
  bool uniqueObjects(const std::vector<LinkInfo<xAOD::IParticleContainer>> &links);
  /// Helper function that returns true if no objects share an initial RoI
  bool uniqueInitialRoIs(const std::vector<LinkInfo<xAOD::IParticleContainer>> &links);
  /// Helper function that returns true if no objects share a final RoI
  bool uniqueRoIs(const std::vector<LinkInfo<xAOD::IParticleContainer>> &links);

  /// Get a lambda corresponding to the specified FilterType enum.
  std::function<bool(const std::vector<LinkInfo<xAOD::IParticleContainer>> &)> getFilter(FilterType filter);
  class IPartCombItr
  {
  public:
    using VecLInfo_t = std::vector<LinkInfo<xAOD::IParticleContainer>>;
    using LInfoItr_t = VecLInfo_t::const_iterator;

    using iterator_category = std::input_iterator_tag;
    using value_type = VecLInfo_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using difference_type = std::ptrdiff_t;

    /**
     * @brief The direct constructor
     * 
     * Takes a vector of tuples.
     * The last two elements of each tuple are start and end iterators
     * describing ranges of IParticleContainer LinkInfos.
     * The first element describes how many particles from this range are used
     * in the output combinations
     */
    IPartCombItr(
        const std::vector<std::tuple<std::size_t, LInfoItr_t, LInfoItr_t>> &pieces,
        const std::function<bool(const VecLInfo_t &)> filter);

    /**
     * @brief The direct constructor
     * 
     * Takes a vector of tuples.
     * The last two elements of each tuple are start and end iterators
     * describing ranges of IParticleContainer LinkInfos.
     * The first element describes how many particles from this range are used
     * in the output combinations
     */
    IPartCombItr(
        const std::vector<std::tuple<std::size_t, LInfoItr_t, LInfoItr_t>> &pieces,
        FilterType filter = FilterType::UniqueObjects);

    /// Base case constructor for the variadic constructors
    IPartCombItr(const std::function<bool(const VecLInfo_t &)> filter);

    /// Base case constructor for the variadict constructors
    IPartCombItr(FilterType filter = FilterType::UniqueObjects);

    template <typename... Ts>
    IPartCombItr(std::size_t k, const LInfoItr_t &begin, const LInfoItr_t &end, Ts &&... args)
        : IPartCombItr(std::forward<Ts>(args)...)
    {
      m_itrs.insert(m_itrs.begin(), std::make_pair(KFromNItr(k, std::distance(begin, end)), begin));
      m_current.insert(m_current.begin(), k, {});
      const KFromNItr &idxItr = std::get<0>(m_itrs.front());
      if (!idxItr.exhausted())
        std::transform(idxItr->begin(), idxItr->end(), m_current.begin(),
                       [begin](std::size_t idx) { return *(begin + idx); });
    }

    template <typename... Ts>
    IPartCombItr(const LInfoItr_t &begin, const LInfoItr_t &end, Ts &&... args)
        : IPartCombItr(1, begin, end, std::forward<Ts>(args)...)
    {
    }

    template <typename... Ts>
    IPartCombItr(std::size_t k, const VecLInfo_t &linfos, Ts &&... args)
        : IPartCombItr(k, linfos.begin(), linfos.end(), std::forward<Ts>(args)...)
    {
    }

    template <typename... Ts>
    IPartCombItr(const VecLInfo_t &linfos, Ts &&... args)
        : IPartCombItr(1, linfos.begin(), linfos.end(), std::forward<Ts>(args)...)
    {
    }

    /// The size of each combination
    std::size_t size() const { return m_current.size(); }

    /**
     * @brief Reset the iterator to its starting point
     * 
     * Note that this resets all component iterators, so it will not reset this
     * object to its starting point if any of the iterators passed to it were not
     * initially in their starting positions
     */
    void reset();

    /// True if this iterator is past the end
    bool exhausted() const;

    /// Dereference
    reference operator*() const;
    pointer operator->() const;

    /// Pre-increment operator
    IPartCombItr &operator++();

    /// Post-increment operator
    IPartCombItr operator++(int);

    /// Iterator comparison functions
    bool operator==(const IPartCombItr &other) const;
    bool operator!=(const IPartCombItr &other) const;

  private:
    std::function<bool(const VecLInfo_t &)> m_filter;
    std::vector<std::pair<KFromNItr, const LInfoItr_t>> m_itrs;
    VecLInfo_t m_current;

  }; //> end class IPartCombItr
} // namespace TrigCompositeUtils

#endif //> !TRIGCOMPOSITEUTILS_IPARTCOMBITR_H