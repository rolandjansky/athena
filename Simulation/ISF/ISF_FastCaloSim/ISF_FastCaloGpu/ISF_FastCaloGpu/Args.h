/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_GPUARGS_H
#define ISF_FASTCALOGPU_GPUARGS_H

#include "FH_structs.h"
#include "GpuGeneral_structs.h"

#define MAXHITS 200000
#define MAXBINS 1024
#define MAXHITCT 2000

struct GeoGpu;

typedef struct Chain0_Args {

  bool debug;

  double extrapol_eta_ent;
  double extrapol_phi_ent;
  double extrapol_r_ent;
  double extrapol_z_ent;
  double extrapol_eta_ext;
  double extrapol_phi_ext;
  double extrapol_r_ext;
  double extrapol_z_ext;

  float extrapWeight;
  float E;

  int    pdgId;
  double charge;
  int    cs;
  bool   is_phi_symmetric;
  float* rand;
  int    nhits;
  void*  rd4h;

  FH1D* fh1d;//for energy reweigth
  FH2D* fh2d;
  FHs*  fhs;
  FH1D  fh1d_h; // for energy reweight
  FH2D  fh2d_h; // host struct so we have info
  FHs   fhs_h;  // host struct

  GeoGpu* geo;

  bool is_first; // first event
  bool is_last;  // last event

  int*          hitcells_ct; // GPU pointer for number of uniq hit cells
  unsigned long ncells;
  unsigned int  maxhitct;

  float*  cells_energy; // big, all cells, ~ 200K array
  Cell_E* hitcells_E;   // array with only hit cells (Mem maxhitct )
  Cell_E* hitcells_E_h; // host array

  unsigned int* hitcounts_b; // GPU pointer for interm blockwise result of hit counts

  unsigned int ct; // cells got hit for the event

  int reweight; // 1: use gaus log weight for pion, 0: no reweight

} Chain0_Args;

#endif
