/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <cassert>
#include "GetReflectivity.h"

namespace {
  
  struct L2R {
    double lambda;
    double reflectivity;
  };
  
  L2R const alu[] = {
    {150,  0.022456},
    {175,  0.227736},
    {200,  0.388777},
    {225,  0.515113},
    {250,  0.614222},
    {275,  0.691972},
    {300,  0.752967},
    {325,  0.800817},
    {350,  0.838355},
    {375,  0.867803},
    {400,  0.890905},
    {425,  0.909028},
    {450,  0.923245},
    {475,  0.934399},
    {500,  0.943149},
    {525,  0.950013},
    {550,  0.955398},
    {575,  0.959622},
    {600,  0.962936},
    {625,  0.964399},
    {650,  0.967576},
    {675,  0.969176},
    {700,  0.970431},
    {725,  0.971416},
    {750,  0.972188},
    {775,  0.972794},
    {800,  0.973269},
    {1500, 0.973269}
  };
  
  bool less_than(L2R const& lhs, double l) { return lhs.lambda < l; }

  L2R const* const alu_begin = alu;
  L2R const* const alu_end   = alu + sizeof(alu) / sizeof(L2R);
}

double GetReflectivity(double lambda, L2R const* first, L2R const* last) {
  
  double const lambda_min = first->lambda;
  double const lambda_max = (last - 1)->lambda;
  
  if (lambda < lambda_min || lambda > lambda_max) return 0;
  
  L2R const* const p(std::lower_bound(first, last, lambda, less_than));
  
  if (p == first) return p->reflectivity;
  else {
    
    L2R const* const q = p - 1;
    return (lambda - q->lambda)*(p->reflectivity - q->reflectivity)/(p->lambda - q->lambda) + q->reflectivity;
  }
}

double GetReflectivity(double lambda) { return GetReflectivity(lambda, alu_begin, alu_end); }
