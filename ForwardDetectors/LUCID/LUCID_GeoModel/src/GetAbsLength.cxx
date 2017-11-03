/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <cassert>
#include "GetAbsLength.h"

namespace {

  struct L2A {
    double lambda;
    double absLen;
  };

  L2A const gas[] = {
    {150., .001},
    {172.2, .82},
    {175.0, 4.0},
    {200.0, 6.0},
    {1200., 6.0}
  };
  bool less_than(L2A const& lhs, double l) { return lhs.lambda < l; }
  
  L2A const* const gas_begin = gas;
  L2A const* const gas_end   = gas + sizeof(gas) / sizeof(L2A);
}

double GetAbsLength(double lambda, L2A const* first, L2A const* last) {
  
  double const lambda_min = first->lambda;
  double const lambda_max = (last - 1)->lambda;
  
  if (lambda < lambda_min || lambda > lambda_max) return 0;
  
  L2A const* const p(std::lower_bound(first, last, lambda, less_than));
  
  if (p == first) return p->absLen;
  else {
    L2A const* const q = p - 1;
    return (lambda - q->lambda)*(p->absLen - q->absLen)/(p->lambda - q->lambda) + q->absLen;
  }
}

double GetAbsLengthGas(double lambda) { return GetAbsLength(lambda, gas_begin, gas_end); }
