/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MAKEUNIQUE_H
#define TRIGHLTJETHYPO_MAKEUNIQUE_H


// supply missing factory function (present in C++14 only)

#include <memory>
template<typename T, typename... Ts>
  std::unique_ptr<T> make_unique(Ts&&... params)
{
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}
#endif
