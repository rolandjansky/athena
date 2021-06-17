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

#include "AthContainers/DataVector.h"
struct apple
{
  int size = 0;
  int colour = 0;
};

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

int
main()
{
  [[maybe_unused]] applePie myPie;
  return 0;
}
