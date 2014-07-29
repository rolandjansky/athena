/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HYBRID_H
#define HYBRID_H

#include <cmath>
#include <vector>
#include <deque>

struct hybridPart_t {
  hybridPart_t() :
    part_ID(0), part_barcode(0), part_inV(0), part_outV(0), part_ipos(0),
    part_istat(0), part_moth1(0), part_moth2(0), part_daugh1(0), part_daugh2(0),
    part_Px(0.0), part_Py(0.0), part_Pz(0.0), part_E(0.0), part_M(0.0),
    part_Vx(0.0), part_Vy(0.0), part_Vz(0.0), part_Vt(0.0)
  { }

  int part_ID;
  int part_barcode;
  int part_inV;
  int part_outV;
  int part_ipos;
  int part_istat;
  int part_moth1;
  int part_moth2;
  int part_daugh1;
  int part_daugh2;

  double part_Px;
  double part_Py;
  double part_Pz;
  double part_E;
  double part_M;
  double part_Vx;
  double part_Vy;
  double part_Vz;
  double part_Vt;
};

typedef std::deque<hybridPart_t>             hybridPartSet_t;
typedef hybridPartSet_t::iterator            hybridPartSet_ptr_t;
typedef hybridPartSet_t::const_iterator      hybridPartSet_const_ptr_t;
typedef hybridPartSet_t::reverse_iterator    hybridPartSet_rptr_t;

#endif
