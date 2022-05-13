/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_LOADGPUHIST_H
#define ISF_FASTCALOGPU_LOADGPUHIST_H

#include "FH_structs.h"

class LoadGpuHist {

public:
  LoadGpuHist();
  ~LoadGpuHist();

  void  set_hf1d( FH1D* hf_ptr ) { m_hf1d = hf_ptr; }
  void  set_d_hf1d( FH1D* hf_ptr ) { m_hf1d_d = hf_ptr; }
  FH1D* hf1d() const { return m_hf1d; }
  FH1D* hf1d_d() const { return m_hf1d_d; }
  FH1D* hf1d_h() const { return m_hf1d_h; } // device ptr on HOST

  void LD1D();

private:
  struct FH1D* m_hf1d{0};
  struct FH1D* m_hf1d_d{0}; // device pointer
  struct FH1D* m_hf1d_h{0}; // host pointer struct hold device param to be copied to device

};

#endif
