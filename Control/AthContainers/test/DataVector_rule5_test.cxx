/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Check if DataVector of a dummy type
 * is
 * - default_constructible
 * - copy_constructible
 * - copy_assignable
 * - nothrow_move_constructible
 * - nothrow_move_assignable
 * - is_destructible
 */
#undef NDEBUG
#include "AthContainers/DataVector.h"
#include <cstdlib> //rand
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
struct apple
{
  int size = 0;
  int colour = 0;
};
constexpr size_t bushel{20};

using apples = DataVector<const apple>;

struct applePie
{
  static_assert(std::is_default_constructible<apples>::value,
                "DataVector fails is_default_constructible");
  static_assert(std::is_copy_constructible<apples>::value,
                "DataVector fails is_copy_constructible");
  static_assert(std::is_copy_assignable<apples>::value,
                "DataVector fails is_copy_assignable");
  static_assert(std::is_nothrow_move_constructible<apples>::value,
                "DataVector fails is_nothrow_move_constructible");
  static_assert(std::is_nothrow_move_assignable<apples>::value,
                "DataVector fails is_nothrow_move_assignable ");
  static_assert(std::is_destructible<apples>::value,
                "DataVector fails is_destructible");
};

void fillWithApples(apples & appleDataVec){
  for (size_t i{0};i!=bushel;++i){
    const apple * newApple = new apple{rand() % 20, rand() % 20};
    appleDataVec.push_back(newApple);
  }
}

int
main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "AthContainers/DataVector_rule5_test\n";

  [[maybe_unused]] applePie myPie;
  
  { //straight assignment, both vectors prefilled
    Athena_test::Leakcheck check;
    apples braeburn;
    apples jonagold;
    fillWithApples(braeburn);
    fillWithApples(jonagold);
    braeburn=jonagold;
  }
  { //move-assigned from filled DV to empty DV
    Athena_test::Leakcheck check2;
    apples braeburn;
    apples jonagold;
    fillWithApples(braeburn);
    jonagold=std::move(braeburn);
  }
  { //move-assigned from filled DV to filled DV
    Athena_test::Leakcheck check3;
    apples braeburn;
    apples jonagold;
    fillWithApples(braeburn);
    fillWithApples(jonagold);
    jonagold=std::move(braeburn);
  }
  return 0;
}
