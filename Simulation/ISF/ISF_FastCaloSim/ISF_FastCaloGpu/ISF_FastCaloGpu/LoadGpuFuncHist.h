/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_LOADGPUFUNCHIST_H
#define ISF_FASTCALOGPU_LOADGPUFUNCHIST_H

#include "FH_structs.h"

class LoadGpuFuncHist {

  public:
    LoadGpuFuncHist();
    ~LoadGpuFuncHist();

    void  set_hf( FHs* hf_ptr ) { m_hf = hf_ptr; }
    void  set_d_hf( FHs* hf_ptr ) { m_hf_d = hf_ptr; }
    void  set_hf2d( FH2D* hf_ptr ) { m_hf2d = hf_ptr; }
    void  set_d_hf2d( FH2D* hf_ptr ) { m_hf2d_d = hf_ptr; }
    FHs*  hf() const { return m_hf; }
    FHs*  hf_h() const { return m_hf_h; } // device ptr on HOST
    FHs*  hf_d() const { return m_hf_d; } // device ptr on DEVICE
    FH2D* hf2d() const { return m_hf2d; }
    FH2D* hf2d_h() const { return m_hf2d_h; } // device ptr on HOST
    FH2D* hf2d_d() const { return m_hf2d_d; } // device ptr on DEVICE

    void LD();
    void LD2D();

  private:
    struct FHs* m_hf{0};
    struct FHs* m_hf_d{0}; // device pointer
    struct FHs* m_hf_h{0}; // host pointer to struct hold device param that is copied to device

    struct FH2D* m_hf2d{0};
    struct FH2D* m_hf2d_d{0}; // device pointer
    struct FH2D* m_hf2d_h{0}; // host pointer struct hold device param to be copied to device

};


#endif
