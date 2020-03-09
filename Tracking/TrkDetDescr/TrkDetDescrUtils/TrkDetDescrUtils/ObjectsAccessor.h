/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef _ObjectsAccessor_H_
#define _ObjectsAccessor_H_

#include "ObjectAccessor.h"
#include <array>
#include <utility>

namespace Trk {
using ThreeObjectsAccessor = std::array<ObjectAccessor::value_type, 3>;
using FourObjectsAccessor =
  std::pair<std::array<ObjectAccessor::value_type, 4>, bool>; // bool to indicate inverseRetrieval
using FiveObjectsAccessor = std::array<ObjectAccessor::value_type, 5>;
using SixObjectsAccessor = std::array<ObjectAccessor::value_type, 6>;
using EightObjectsAccessor = std::array<ObjectAccessor::value_type, 8>;
}
#endif
