/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <cassert>
#include "GetQuantumEfficiency.h"

namespace {
  
  struct L2QE {
    double lambda;
    double qe;
  };

  L2QE const r762[] = {
    {160., 0.063096},
    {170., 0.152522},
    {180., 0.188365},
    {190., 0.211349},
    {200., 0.237137},
    {210., 0.232631},
    {220., 0.223872},
    {230., 0.207332},
    {240., 0.203392},
    {250., 0.199526},
    {260., 0.192014},
    {270., 0.188365},
    {280., 0.192014},
    {290., 0.207332},
    {300., 0.211349},
    {310., 0.215443},
    {320., 0.223872},
    {330., 0.228209},
    {340., 0.232631},
    {350., 0.237137},
    {360., 0.241732},
    {370., 0.246415},
    {380., 0.256055},
    {390., 0.251189},
    {400., 0.251189},
    {410., 0.251189},
    {420., 0.251189},
    {430., 0.246415},
    {440., 0.246415},
    {450., 0.237137},
    {460., 0.232631},
    {470., 0.219617},
    {480., 0.207332},
    {490., 0.195734},
    {500., 0.177828},
    {510., 0.161560},
    {520., 0.146780},
    {530., 0.133352},
    {540., 0.125893},
    {550., 0.114376},
    {560., 0.101937},
    {570., 0.077923},
    {580., 0.055165},
    {590., 0.044668},
    {600., 0.036869},
    {610., 0.029854},
    {620., 0.021962},
    {630., 0.017113},
    {640., 0.012115},
    {650., 0.008912},
    {660., 0.005623},
    {670., 0.003687},
    {680., 0.002073},
    {690., 0.001188},
    {700., 0.000764}
  };
  
  bool less_than(L2QE const& lhs, double l) { return lhs.lambda < l; }
  
  L2QE const* const r762_begin = r762;
  L2QE const* const r762_end   = r762 + sizeof(r762) / sizeof(L2QE);
}

double GetQuantumEfficiency(double lambda, L2QE const* first, L2QE const* last) {
  
  double const lambda_min = first->lambda;
  double const lambda_max = (last - 1)->lambda;
  
  if (lambda < lambda_min || lambda > lambda_max) return 0;
  
  L2QE const* const p(std::lower_bound(first, last, lambda, less_than));
  
  if (p == first) return p->qe;
  else {
    
    L2QE const* const q = p - 1;
    return (lambda - q->lambda)*(p->qe - q->qe)/(p->lambda - q->lambda) + q->qe;
  }
}

double GetQuantumEfficiencyR762(double lambda) { return GetQuantumEfficiency(lambda, r762_begin, r762_end); }
