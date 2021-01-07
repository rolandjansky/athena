/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <vector>
#include <memory>
#include "TrigCompositeUtils/IPartCombItr.h"
#include "TrigCompositeUtils/Combinations.h"
#include "xAODTrigger/TrigComposite.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/IParticleContainer.h"
#include "TestTools/expect.h"
#include "CxxUtils/checker_macros.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


using namespace TrigCompositeUtils;
/// Helper functions in anonymous namespace
namespace
{
  /// Create a unique LinkInfo
  LinkInfo<xAOD::IParticleContainer> createLinkInfo(std::vector<std::unique_ptr<Decision>> &owning)
  {
    static std::size_t idxBase = 0;
    // Make a new decision object
    owning.push_back(std::make_unique<Decision>());
    std::size_t key = idxBase++;
    std::size_t index = idxBase++;
    return LinkInfo<xAOD::IParticleContainer>(
        owning.back().get(),
        ElementLink<xAOD::IParticleContainer>(key, index));
  }

  /// Allow comparing link info objects
  bool operator==(const LinkInfo<xAOD::IParticleContainer> &lhs, const LinkInfo<xAOD::IParticleContainer> &rhs)
  {
    return lhs.source == rhs.source &&
           lhs.link.persKey() == rhs.link.persKey() &&
           lhs.link.persIndex() == rhs.link.persIndex();
  }
  bool operator!=(const LinkInfo<xAOD::IParticleContainer> &lhs, const LinkInfo<xAOD::IParticleContainer> &rhs)
  {
    return !(lhs == rhs);
  }

  std::vector<LinkInfo<xAOD::IParticleContainer>> createLinkInfoVec(
      std::size_t n,
      std::vector<std::unique_ptr<Decision>> &owning)
  {
    std::vector<LinkInfo<xAOD::IParticleContainer>> ret;
    for (std::size_t ii = 0; ii < n; ++ii)
      ret.push_back(createLinkInfo(owning));
    return ret;
  }

  std::vector<std::size_t> getIndices(
      const std::vector<LinkInfo<xAOD::IParticleContainer>> &infos,
      const std::vector<std::vector<LinkInfo<xAOD::IParticleContainer>>::const_iterator> &startItrs)
  {
    assert(infos.size() == startItrs.size());
    std::vector<std::size_t> ret;
    auto infoItr = infos.begin();
    auto startsItr = startItrs.begin();
    for (; infoItr != infos.end(); ++infoItr, ++startsItr)
    {
      std::size_t distance = 0;
      std::vector<LinkInfo<xAOD::IParticleContainer>>::const_iterator itr = *startsItr;
      while (*itr != *infoItr)
      {
        ++distance;
        ++itr;
      }
      ret.push_back(distance);
    }
    return ret;
  }

  std::vector<std::vector<std::size_t>> getAllIndices(
      const Combinations &combinations,
      const std::vector<std::vector<LinkInfo<xAOD::IParticleContainer>>::const_iterator> &startItrs)
  {
    std::vector<std::vector<std::size_t>> ret;
    for (const auto &combo : combinations)
      ret.push_back(getIndices(combo, startItrs));
    return ret;
  }

  /// Allow printing vector<T>
  template <typename T>
  std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
  {
    os << "[";
    if (v.size() > 0)
    {
      for (auto itr = v.begin(); itr != v.end() - 1; ++itr)
        os << *itr << ", ";
      os << v.back();
    }
    return os << "]";
  }
} // namespace

void pickOne()
{
  std::cout << "Pick 1 from 6" << std::endl;
  std::vector<std::unique_ptr<Decision>> owning;

  std::vector<LinkInfo<xAOD::IParticleContainer>> features = createLinkInfoVec(6, owning);
  Combinations combos({1}, {features}, FilterType::All);
  auto indices = getAllIndices(combos, {features.begin()});
  VALUE(indices) EXPECTED({{0}, {1}, {2}, {3}, {4}, {5}});
}

void pickTwoSingleLeg()
{
  std::cout << "Pick 2 from 4" << std::endl;
  std::vector<std::unique_ptr<Decision>> owning;

  std::vector<LinkInfo<xAOD::IParticleContainer>> features = createLinkInfoVec(4, owning);
  Combinations combos({2}, {features}, FilterType::All);
  auto indices = getAllIndices(combos, {features.begin(), features.begin()});
  VALUE( indices ) EXPECTED( {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}} ); 
}

void pickTwoOneMultiLeg()
{
  std::cout << "Pick 2 from 3, 1 from 2" << std::endl;
  std::vector<std::unique_ptr<Decision>> owning;

  std::vector<LinkInfo<xAOD::IParticleContainer>> features1 = createLinkInfoVec(3, owning);
  std::vector<LinkInfo<xAOD::IParticleContainer>> features2 = createLinkInfoVec(2, owning);
  Combinations combos({2, 1}, {features1, features2}, FilterType::All);
  auto indices = getAllIndices(combos, {features1.begin(), features1.begin(), features2.begin()});
  VALUE( indices ) EXPECTED( {{0, 1, 0}, {0, 1, 1}, {0, 2, 0}, {0, 2, 1}, {1, 2, 0}, {1, 2, 1}} );
}

void checkReset()
{
  std::cout << "Check resetting" << std::endl;
  std::vector<std::unique_ptr<Decision>> owning;
  std::vector<LinkInfo<xAOD::IParticleContainer>> features1 = createLinkInfoVec(3, owning);
  std::vector<LinkInfo<xAOD::IParticleContainer>> features2 = createLinkInfoVec(2, owning);
  Combinations combos({2, 1}, {features1, features2}, FilterType::All);
  auto itr = combos.begin();
  // Purposefully exhaust it
  while(!itr.exhausted())
    ++itr;
  itr.reset();
  VALUE( itr == combos.begin() ) EXPECTED( true );

}

int main()
{
  pickOne();
  pickTwoSingleLeg();
  pickTwoOneMultiLeg();
  checkReset();
  return 0;
}