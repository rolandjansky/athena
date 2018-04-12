//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <PATInterfaces/SystematicSet.h>

#include <RootCoreUtils/Assert.h>
#include <cmath>
#include <gtest/gtest.h>

using namespace CP;

//
// method implementations
//

#define ASSERT_SUCCESS(x)			\
  ASSERT_EQ (asg::CheckHelper<decltype(x)>::successCode(), x)

#define ASSERT_FAILURE(x)			\
  ASSERT_EQ (asg::CheckHelper<decltype(x)>::failureCode(), x)

TEST (SystematicSetTest, state_clear)
{
  SystematicVariation sys("TEST_VARIATION", 1);
  SystematicSet set;
  set.insert( SystematicVariation("TEST_VARIATION", 1) );
  const std::string sysName = sys.name();
  const std::string setName = set.name();
  // First assert that the sys name matches the single-sys-set name
  ASSERT_EQ( sysName, setName );
  // Assert that the state is properly cleared
  set.clear();
  ASSERT_EQ( set.name(), std::string() );
}

TEST (SystematicSetTest, getToyVariation_empty)
{
  SystematicSet set;
  ASSERT_EQ (0u, set.getToyVariationByBaseName("TEST").first);
  ASSERT_EQ (0, set.getToyVariationByBaseName("TEST").second);
}

TEST (SystematicSetTest, getToyVariation_single)
{
  SystematicSet set;
  set.insert (SystematicVariation::makeToyVariation ("TEST", 3, 2.1234));
  ASSERT_EQ (3u, set.getToyVariationByBaseName("TEST").first);

  // Systematic variations are accurate up to four digits after the
  // period, so I check that it is accurate as an integer, instead of
  // accurate as a float.
  ASSERT_EQ (21234, rint (set.getToyVariationByBaseName("TEST").second * 10000));
}

struct SystematicSetFilterTest : testing::Test
{
  SystematicVariation continuous_variation;
  SystematicVariation continuous_ensemble;
  SystematicVariation toy_variation;
  SystematicVariation toy_ensemble;
  SystematicSet full_set;

  SystematicSetFilterTest ()
  {
    continuous_variation = SystematicVariation ("CONTINUOUS_VARIATION", 1);
    continuous_ensemble = SystematicVariation ("CONTINUOUS_ENSEMBLE", SystematicVariation::CONTINUOUS);
    toy_variation = SystematicVariation::makeToyVariation ("TOY_VARIATION", 1, 2);
    toy_ensemble = SystematicVariation::makeToyEnsemble ("TOY_ENSEMBLE");
    full_set.insert (continuous_variation);
    full_set.insert (continuous_ensemble);
    full_set.insert (toy_variation);
    full_set.insert (toy_ensemble);
  }
};

TEST_F (SystematicSetFilterTest, empty)
{
  SystematicSet affecting, filtered;
  ASSERT_SUCCESS (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
  ASSERT_EQ (0u, filtered.size());
}

TEST_F (SystematicSetFilterTest, continuous_variation)
{
  SystematicSet affecting, filtered;
  affecting.insert (continuous_variation);
  ASSERT_SUCCESS (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
  ASSERT_EQ (1u, filtered.size());
  ASSERT_EQ (*filtered.begin(), continuous_variation);
}

TEST_F (SystematicSetFilterTest, continuous_variation_from_ensemble)
{
  SystematicSet affecting, filtered;
  affecting.insert (SystematicVariation::makeContinuousEnsemble (continuous_variation.basename()));
  ASSERT_SUCCESS (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
  ASSERT_EQ (1u, filtered.size());
  ASSERT_EQ (*filtered.begin(), continuous_variation);
}

TEST_F (SystematicSetFilterTest, continuous_variation_from_ensemble_discrete)
{
  SystematicSet affecting, filtered;
  affecting.insert (SystematicVariation::makeContinuousEnsemble (continuous_variation.basename()));
  affecting.insert (continuous_variation);
  ASSERT_SUCCESS (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
  ASSERT_EQ (1u, filtered.size());
  ASSERT_EQ (*filtered.begin(), continuous_variation);
}

TEST_F (SystematicSetFilterTest, continuous_variation_parameter_missmatch)
{
  SystematicSet affecting, filtered;
  affecting.insert (SystematicVariation (continuous_variation.basename(), continuous_variation.parameter()+1));
  ASSERT_FAILURE (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
}

TEST_F (SystematicSetFilterTest, continuous_variation_toy_missmatch)
{
  SystematicSet affecting, filtered;
  affecting.insert (SystematicVariation::makeToyEnsemble (continuous_variation.basename()));
  ASSERT_FAILURE (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
}

TEST_F (SystematicSetFilterTest, continuous_ensemble)
{
  SystematicSet affecting, filtered;
  affecting.insert (continuous_ensemble);
  ASSERT_SUCCESS (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
  ASSERT_EQ (1u, filtered.size());
  ASSERT_EQ (*filtered.begin(), continuous_ensemble);
}

TEST_F (SystematicSetFilterTest, toy_variation)
{
  SystematicSet affecting, filtered;
  affecting.insert (toy_variation);
  ASSERT_SUCCESS (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
  ASSERT_EQ (1u, filtered.size());
  ASSERT_EQ (*filtered.begin(), toy_variation);
}

TEST_F (SystematicSetFilterTest, toy_variation_from_ensemble)
{
  SystematicSet affecting, filtered;
  affecting.insert (SystematicVariation::makeToyEnsemble (toy_variation.basename()));
  ASSERT_SUCCESS (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
  ASSERT_EQ (1u, filtered.size());
  ASSERT_EQ (*filtered.begin(), toy_variation);
}

TEST_F (SystematicSetFilterTest, toy_variation_continuous_missmatch)
{
  SystematicSet affecting, filtered;
  affecting.insert (SystematicVariation::makeContinuousEnsemble (toy_variation.basename()));
  ASSERT_FAILURE (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
}

TEST_F (SystematicSetFilterTest, toy_ensemble)
{
  SystematicSet affecting, filtered;
  affecting.insert (toy_ensemble);
  ASSERT_SUCCESS (SystematicSet::filterForAffectingSystematics (full_set, affecting, filtered));
  ASSERT_EQ (1u, filtered.size());
  ASSERT_EQ (*filtered.begin(), toy_ensemble);
}

int main (int argc, char **argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
