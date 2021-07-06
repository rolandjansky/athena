/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/IPartCombItr.h"
#include <set>
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

namespace TrigCompositeUtils
{
  bool uniqueObjects(const std::vector<LinkInfo<xAOD::IParticleContainer>> &links)
  {
    std::set<const xAOD::IParticle *> seen;
    for (const auto &info : links)
      if (!seen.insert(*info.link).second)
        return false;
    return true;
  }

  bool uniqueInitialRoIs(const std::vector<LinkInfo<xAOD::IParticleContainer>> &links)
  {
    std::set<std::pair<uint32_t, uint32_t>> seen;
    for (const auto &info : links)
    {
      LinkInfo<TrigRoiDescriptorCollection> roi = findLink<TrigRoiDescriptorCollection>(info.source, initialRoIString());
      if (!seen.insert(std::make_pair(roi.link.persKey(), roi.link.persIndex())).second)
        // Insert returns false if that item already exists in it
        return false;
    }
    return true;
  }

  bool uniqueRoIs(const std::vector<LinkInfo<xAOD::IParticleContainer>> &links)
  {
    std::set<std::pair<uint32_t, uint32_t>> seen;
    for (const auto &info : links)
    {
      LinkInfo<TrigRoiDescriptorCollection> roi = findLink<TrigRoiDescriptorCollection>(info.source, "roi");
      if (!seen.insert(std::make_pair(roi.link.persKey(), roi.link.persIndex())).second)
        // Insert returns false if that item already exists in it
        return false;
    }
    return true;
  }

  std::function<bool(const std::vector<LinkInfo<xAOD::IParticleContainer>> &)> getFilter(FilterType filter)
  {
    switch (filter)
    {
    case FilterType::All:
      return [](const std::vector<LinkInfo<xAOD::IParticleContainer>> &) { return true; };
    case FilterType::UniqueObjects:
      return uniqueObjects;
    case FilterType::UniqueRoIs:
      return uniqueRoIs;
    default:
      throw std::runtime_error("Unhandled FilterType enum value!");
      return {};
    }
  }

  IPartCombItr::IPartCombItr(
      const std::vector<std::tuple<std::size_t, LInfoItr_t, LInfoItr_t>> &pieces,
      const std::function<bool(const VecLInfo_t &)> filter)
      : m_filter(filter)
  {
    m_itrs.reserve(pieces.size());
    auto currentItr = std::back_inserter(m_current);
    for (const auto &tup : pieces)
    {
      LInfoItr_t begin = std::get<1>(tup);
      LInfoItr_t end = std::get<2>(tup);
      m_itrs.push_back(std::make_pair(
          KFromNItr(std::get<0>(tup), std::distance(begin, end)), begin));
      const KFromNItr &idxItr = std::get<0>(m_itrs.back());
      if (idxItr.exhausted())
        for (std::size_t ii = 0; ii < idxItr.size(); ++ii)
          *(currentItr++) = LinkInfo<xAOD::IParticleContainer>{};
      else
        std::transform(
            idxItr->begin(), idxItr->end(), currentItr,
            [begin](std::size_t idx) { return *(begin + idx); });
    }
    // make sure that the starting set makes sense
    if (!exhausted() && !m_filter(m_current))
      this->operator++();
  }

  IPartCombItr::IPartCombItr(
      const std::vector<std::tuple<std::size_t, LInfoItr_t, LInfoItr_t>> &pieces,
      FilterType filter)
      : IPartCombItr(pieces, getFilter(filter))
  {
  }

  IPartCombItr::IPartCombItr(const std::function<bool(const VecLInfo_t &)> filter)
      : m_filter(filter) {}

  IPartCombItr::IPartCombItr(FilterType filter)
      : m_filter(getFilter(filter)) {}

  void IPartCombItr::reset()
  {
    // Reset each individual iterator and set our current value accordingly
    auto outItr = m_current.begin();
    for (auto &itrPair : m_itrs)
    {
      KFromNItr &idxItr = itrPair.first;
      idxItr.reset();
      if (idxItr.exhausted())
        for (std::size_t ii = 0; ii < idxItr.size(); ++ii)
          *(outItr++) = LinkInfo<xAOD::IParticleContainer>{};
      else
        for (std::size_t idx : *idxItr)
          *(outItr++) = *(itrPair.second + idx);
    }
    // make sure that the starting set makes sense
    if (!exhausted() && !m_filter(m_current))
      this->operator++();
  }

  bool IPartCombItr::exhausted() const
  {
    return m_itrs.size() == 0 ||
           std::any_of(m_itrs.begin(), m_itrs.end(),
                       [](const auto &itrPair) { return itrPair.first.exhausted(); });
  }

  IPartCombItr::reference IPartCombItr::operator*() const
  {
    if (exhausted())
      throw std::runtime_error("Dereferencing past-the-end iterator");
    return m_current;
  }

  IPartCombItr::pointer IPartCombItr::operator->() const
  {
    if (exhausted())
      throw std::runtime_error("Dereferencing past-the-end iterator");
    return &m_current;
  }

  IPartCombItr &IPartCombItr::operator++()
  {
    if (exhausted())
      // Don't iterate an iterator that is already past the end
      return *this;
    auto backItr = m_itrs.rbegin();
    std::size_t step = 0;
    while (backItr != m_itrs.rend())
    {
      KFromNItr &idxItr = backItr->first;
      step += idxItr.size();
      if (!(++backItr->first).exhausted())
      {
        // This is the starting point for a good combination
        // We need to update the current value
        auto outItr = m_current.end() - step;
        for (std::size_t idx : *idxItr)
          *(outItr++) = *(backItr->second + idx);

        // Any iterators we passed by up to this point were exhausted so we have
        // to reset them before we use their values
        for (auto itr = backItr.base(); itr != m_itrs.end(); ++itr)
        {
          itr->first.reset();
          for (std::size_t idx : *itr->first)
            *(outItr++) = *(itr->second + idx);
        }
        break;
      }
      ++backItr;
    }
    if (!m_filter(m_current))
      // If we fail the filter condition, advance the iterator again
      this->operator++();
    return *this;
  }

  IPartCombItr IPartCombItr::operator++(int)
  {
    IPartCombItr ret(*this);
    this->operator++();
    return ret;
  }

  bool IPartCombItr::operator==(const IPartCombItr &other) const
  {
    // All past-the-end iterators compare equal
    if (exhausted() && other.exhausted())
      return true;
    return m_itrs == other.m_itrs;
  }

  bool IPartCombItr::operator!=(const IPartCombItr &other) const
  {
    return !(*this == other);
  }

} // namespace TrigCompositeUtils