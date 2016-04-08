/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCcablingInterface/CMAprogram.h"
#include <string.h>

using namespace std;

CMAprogram::CMAprogram() :
    m_isnewcab(false), m_overlap1(0xff), m_overlap2(0xff000000),
    m_trig_local_direc_i(0x1), m_trig_local_direc_j(0x1), m_trig_k_readout(0x0),

    m_main_control(0x60), m_main_count(0x0), m_main_status(0x0),
    
    m_pipe_i0_mask0_in(0x0), m_pipe_i0_edge(0x07070707),
    m_pipe_i0_ipb_regdepth(0x0101), m_pipe_i0_shape(0x07070707),
    m_pipe_i0_mask0_readout(0x0), m_pipe_i0_mask0_trig(0x0),
    
    m_pipe_i1_mask0_in(0x0), m_pipe_i1_edge(0x07070707),
    m_pipe_i1_ipb_regdepth(0x0101), m_pipe_i1_shape(0x07070707),
    m_pipe_i1_mask0_readout(0x0), m_pipe_i1_mask0_trig(0x0),
    
    m_pipe_j0_mask0_in(0x0),
    m_pipe_j0_ipb_regdepth(0x01010101),
    m_pipe_j0_mask0_readout(0x0), m_pipe_j0_mask0_trig(0x0),
    
    m_pipe_j1_mask0_in(0x0), m_pipe_j1_edge(0x0),
    m_pipe_j1_ipb_regdepth(0x01010101),
    m_pipe_j1_mask0_readout(0x0), m_pipe_j1_mask0_trig(0x0),

    m_trig_thr0_maj_reg(0x2),
    m_trig_thr0_mask_1_i_ge1_reg(0x0), m_trig_thr0_mask_1_i_eq2_reg(0x0),
    m_trig_thr0_mask_1_j_ge1_reg(0x0), m_trig_thr0_mask_1_j_eq2_reg(0x0),

    m_trig_thr1_maj_reg(0x2),
    m_trig_thr1_mask_1_i_ge1_reg(0x0), m_trig_thr1_mask_1_i_eq2_reg(0x0),
    m_trig_thr1_mask_1_j_ge1_reg(0x0), m_trig_thr1_mask_1_j_eq2_reg(0x0),

    m_trig_thr2_maj_reg(0x2),
    m_trig_thr2_mask_1_i_ge1_reg(0x0), m_trig_thr2_mask_1_i_eq2_reg(0x0),
    m_trig_thr2_mask_1_j_ge1_reg(0x0), m_trig_thr2_mask_1_j_eq2_reg(0x0),
    
    m_trig_declu_i0_clsize(0x2), m_trig_declu_i1_clsize(0x2),
    m_trig_declu_j0_clsize(0x2), m_trig_declu_j1_clsize(0x2),
    
    m_trig_prp_i_eta_reg(0x1), m_trig_prp_j_eta_reg(0x1),
    
    m_trig_shape_k_reg(0x07070707), 

    m_trig_ovl_sx_reg(0xff000000),
    m_trig_ovl_dx_reg(0x000000ff),

    m_readout_buffer_empty(0x0), m_readout_buffer_almost_empty(0x0),
    m_readout_buffer_half_full(0x0), m_readout_buffer_almost_full(0x0),
    m_readout_buffer_full(0x0), m_readout_buffer_latreg(0x67),
    m_readout_buffer_hireg(0x67), m_readout_buffer_lowreg(0x60),
    m_readout_serializer_dslink(0x1),
    m_l1c_pre(0x1ff), m_bcc_pre(0x0)
{
    union data {
        uint64_t DoubleWord;
        uint8_t  Bytes[8];
    } Data;

    for (int i=0;i<6;++i) Data.Bytes[i] = 0x07;
    for (int i=6;i<8;++i) Data.Bytes[i] = 0x0;
    
    m_trig_edge_reg = Data.DoubleWord;
    
    for (int i=6;i<8;++i) Data.Bytes[i] = 0x07;
    
    m_pipe_j0_edge  = Data.DoubleWord;
    m_pipe_j0_shape = Data.DoubleWord;
    m_pipe_j1_shape = Data.DoubleWord;

    for (int th=0; th<3; ++th) {
      for (int ch=0; ch<32; ++ch) {
	for (int by=0; by<2; ++by) {
	  m_program_bytes[th][ch][by]=0xffffffff;
	}
      }
    }
    for(int i=0;i<3;++i)
        for(int j=0;j<CMAparameters::pivot_channels;++j)
            for(int k=0;k<CMAparameters::confirm_channels;++k)
	      m_threshold_registers[i][j][k] = true;
    m_status = false;
}

CMAprogram::CMAprogram(ifstream& file) :

  m_overlap1(0xff),m_overlap2(0xff000000),m_trig_local_direc_i(0x1),
  m_trig_local_direc_j(0x1),m_trig_k_readout(0x0),

  m_main_control(0x60), m_main_count(0x0), m_main_status(0x0),
    
  m_pipe_i0_mask0_in(0x0), m_pipe_i0_edge(0x07070707),
  m_pipe_i0_ipb_regdepth(0x0101), m_pipe_i0_shape(0x07070707),
  m_pipe_i0_mask0_readout(0x0), m_pipe_i0_mask0_trig(0x0),
  
  m_pipe_i1_mask0_in(0x0), m_pipe_i1_edge(0x07070707),
  m_pipe_i1_ipb_regdepth(0x0101), m_pipe_i1_shape(0x07070707),
  m_pipe_i1_mask0_readout(0x0), m_pipe_i1_mask0_trig(0x0),
  
  m_pipe_j0_mask0_in(0x0),
  m_pipe_j0_ipb_regdepth(0x01010101),
  m_pipe_j0_mask0_readout(0x0), m_pipe_j0_mask0_trig(0x0),
  
  m_pipe_j1_mask0_in(0x0), m_pipe_j1_edge(0x0),
  m_pipe_j1_ipb_regdepth(0x01010101),
  m_pipe_j1_mask0_readout(0x0), m_pipe_j1_mask0_trig(0x0),
  
  m_trig_thr0_maj_reg(0x2),
  m_trig_thr0_mask_1_i_ge1_reg(0x0), m_trig_thr0_mask_1_i_eq2_reg(0x0),
  m_trig_thr0_mask_1_j_ge1_reg(0x0), m_trig_thr0_mask_1_j_eq2_reg(0x0),
  
  m_trig_thr1_maj_reg(0x2),
  m_trig_thr1_mask_1_i_ge1_reg(0x0), m_trig_thr1_mask_1_i_eq2_reg(0x0),
  m_trig_thr1_mask_1_j_ge1_reg(0x0), m_trig_thr1_mask_1_j_eq2_reg(0x0),
  
  m_trig_thr2_maj_reg(0x2),
  m_trig_thr2_mask_1_i_ge1_reg(0x0), m_trig_thr2_mask_1_i_eq2_reg(0x0),
  m_trig_thr2_mask_1_j_ge1_reg(0x0), m_trig_thr2_mask_1_j_eq2_reg(0x0),
  
  m_trig_declu_i0_clsize(0x2), m_trig_declu_i1_clsize(0x2),
  m_trig_declu_j0_clsize(0x2), m_trig_declu_j1_clsize(0x2),
  
  m_trig_prp_i_eta_reg(0x1), m_trig_prp_j_eta_reg(0x1),
  
  m_trig_shape_k_reg(0x07070707), m_trig_ovl_sx_reg(0xff000000),
  m_trig_ovl_dx_reg(0xff),

  m_readout_buffer_empty(0x0), m_readout_buffer_almost_empty(0x0),
  m_readout_buffer_half_full(0x0), m_readout_buffer_almost_full(0x0),
  m_readout_buffer_full(0x0), m_readout_buffer_latreg(0x67),
  m_readout_buffer_hireg(0x67), m_readout_buffer_lowreg(0x60),
  m_readout_serializer_dslink(0x1),
  m_l1c_pre(0x1ff), m_bcc_pre(0x0)
  
{
  union data {
    uint64_t DoubleWord;
    uint8_t  Bytes[8];
  } Data;
  
  m_isnewcab = false;

  for (int i=0;i<6;++i) Data.Bytes[i] = 0x07;
  for (int i=6;i<8;++i) Data.Bytes[i] = 0x0;
  
  m_trig_edge_reg = Data.DoubleWord;
  
  for (int i=6;i<8;++i) Data.Bytes[i] = 0x07;
  
  m_pipe_j0_edge  = Data.DoubleWord;
  m_pipe_j0_shape = Data.DoubleWord;
  m_pipe_j1_shape = Data.DoubleWord;
  
  for (int th=0; th<3; ++th) {
    for (int ch=0; ch<32; ++ch) {
      for (int by=0; by<2; ++by) {
	m_program_bytes[th][ch][by]=0xffffffff;
      }
    }
  }
  for(int i=0;i<3;++i)
    for(int j=0;j<CMAparameters::pivot_channels;++j)
      for(int k=0;k<CMAparameters::confirm_channels;++k)
	m_threshold_registers[i][j][k] = true;
  
  DBline data(file);
  m_status = read_v02(data);
}

CMAprogram::CMAprogram(ifstream& file, bool NewCab) :

  m_overlap1(0xff),m_overlap2(0xff000000),m_trig_local_direc_i(0x1),
  m_trig_local_direc_j(0x1),m_trig_k_readout(0x0),

  m_main_control(0x60), m_main_count(0x0), m_main_status(0x0),
    
  m_pipe_i0_mask0_in(0x0), m_pipe_i0_edge(0x07070707),
  m_pipe_i0_ipb_regdepth(0x0101), m_pipe_i0_shape(0x07070707),
  m_pipe_i0_mask0_readout(0x0), m_pipe_i0_mask0_trig(0x0),
  
  m_pipe_i1_mask0_in(0x0), m_pipe_i1_edge(0x07070707),
  m_pipe_i1_ipb_regdepth(0x0101), m_pipe_i1_shape(0x07070707),
  m_pipe_i1_mask0_readout(0x0), m_pipe_i1_mask0_trig(0x0),
  
  m_pipe_j0_mask0_in(0x0),
  m_pipe_j0_ipb_regdepth(0x01010101),
  m_pipe_j0_mask0_readout(0x0), m_pipe_j0_mask0_trig(0x0),
  
  m_pipe_j1_mask0_in(0x0), m_pipe_j1_edge(0x0),
  m_pipe_j1_ipb_regdepth(0x01010101),
  m_pipe_j1_mask0_readout(0x0), m_pipe_j1_mask0_trig(0x0),
  
  m_trig_thr0_maj_reg(0x2),
  m_trig_thr0_mask_1_i_ge1_reg(0x0), m_trig_thr0_mask_1_i_eq2_reg(0x0),
  m_trig_thr0_mask_1_j_ge1_reg(0x0), m_trig_thr0_mask_1_j_eq2_reg(0x0),
  
  m_trig_thr1_maj_reg(0x2),
  m_trig_thr1_mask_1_i_ge1_reg(0x0), m_trig_thr1_mask_1_i_eq2_reg(0x0),
  m_trig_thr1_mask_1_j_ge1_reg(0x0), m_trig_thr1_mask_1_j_eq2_reg(0x0),
  
  m_trig_thr2_maj_reg(0x2),
  m_trig_thr2_mask_1_i_ge1_reg(0x0), m_trig_thr2_mask_1_i_eq2_reg(0x0),
  m_trig_thr2_mask_1_j_ge1_reg(0x0), m_trig_thr2_mask_1_j_eq2_reg(0x0),
  
  m_trig_declu_i0_clsize(0x2), m_trig_declu_i1_clsize(0x2),
  m_trig_declu_j0_clsize(0x2), m_trig_declu_j1_clsize(0x2),
  
  m_trig_prp_i_eta_reg(0x1), m_trig_prp_j_eta_reg(0x1),
  
  m_trig_shape_k_reg(0x07070707), m_trig_ovl_sx_reg(0xff000000),
  m_trig_ovl_dx_reg(0xff),

  m_readout_buffer_empty(0x0), m_readout_buffer_almost_empty(0x0),
  m_readout_buffer_half_full(0x0), m_readout_buffer_almost_full(0x0),
  m_readout_buffer_full(0x0), m_readout_buffer_latreg(0x67),
  m_readout_buffer_hireg(0x67), m_readout_buffer_lowreg(0x60),
  m_readout_serializer_dslink(0x1),
  m_l1c_pre(0x1ff), m_bcc_pre(0x0)
  
{
  union data {
    uint64_t DoubleWord;
    uint8_t  Bytes[8];
  } Data;
  
  m_isnewcab=NewCab;

  for (int i=0;i<6;++i) Data.Bytes[i] = 0x07;
  for (int i=6;i<8;++i) Data.Bytes[i] = 0x0;
  
  m_trig_edge_reg = Data.DoubleWord;
  
  for (int i=6;i<8;++i) Data.Bytes[i] = 0x07;
  
  m_pipe_j0_edge  = Data.DoubleWord;
  m_pipe_j0_shape = Data.DoubleWord;
  m_pipe_j1_shape = Data.DoubleWord;
  
  for (int th=0; th<3; ++th) {
    for (int ch=0; ch<32; ++ch) {
      for (int by=0; by<2; ++by) {
	m_program_bytes[th][ch][by]=0xffffffff;
      }
    }
  }
  for(int i=0;i<3;++i)
    for(int j=0;j<CMAparameters::pivot_channels;++j)
      for(int k=0;k<CMAparameters::confirm_channels;++k)
	m_threshold_registers[i][j][k] = true;
  
  DBline data(file);
  m_status = read_v02(data);
}

CMAprogram::CMAprogram(istringstream& filestr) :

  m_overlap1(0xff),m_overlap2(0xff000000),m_trig_local_direc_i(0x1),
  m_trig_local_direc_j(0x1),m_trig_k_readout(0x0),

  m_main_control(0x60), m_main_count(0x0), m_main_status(0x0),
    
  m_pipe_i0_mask0_in(0x0), m_pipe_i0_edge(0x07070707),
  m_pipe_i0_ipb_regdepth(0x0101), m_pipe_i0_shape(0x07070707),
  m_pipe_i0_mask0_readout(0x0), m_pipe_i0_mask0_trig(0x0),
  
  m_pipe_i1_mask0_in(0x0), m_pipe_i1_edge(0x07070707),
  m_pipe_i1_ipb_regdepth(0x0101), m_pipe_i1_shape(0x07070707),
  m_pipe_i1_mask0_readout(0x0), m_pipe_i1_mask0_trig(0x0),
  
  m_pipe_j0_mask0_in(0x0),
  m_pipe_j0_ipb_regdepth(0x01010101),
  m_pipe_j0_mask0_readout(0x0), m_pipe_j0_mask0_trig(0x0),
  
  m_pipe_j1_mask0_in(0x0), m_pipe_j1_edge(0x0),
  m_pipe_j1_ipb_regdepth(0x01010101),
  m_pipe_j1_mask0_readout(0x0), m_pipe_j1_mask0_trig(0x0),
  
  m_trig_thr0_maj_reg(0x2),
  m_trig_thr0_mask_1_i_ge1_reg(0x0), m_trig_thr0_mask_1_i_eq2_reg(0x0),
  m_trig_thr0_mask_1_j_ge1_reg(0x0), m_trig_thr0_mask_1_j_eq2_reg(0x0),
  
  m_trig_thr1_maj_reg(0x2),
  m_trig_thr1_mask_1_i_ge1_reg(0x0), m_trig_thr1_mask_1_i_eq2_reg(0x0),
  m_trig_thr1_mask_1_j_ge1_reg(0x0), m_trig_thr1_mask_1_j_eq2_reg(0x0),
  
  m_trig_thr2_maj_reg(0x2),
  m_trig_thr2_mask_1_i_ge1_reg(0x0), m_trig_thr2_mask_1_i_eq2_reg(0x0),
  m_trig_thr2_mask_1_j_ge1_reg(0x0), m_trig_thr2_mask_1_j_eq2_reg(0x0),
  
  m_trig_declu_i0_clsize(0x2), m_trig_declu_i1_clsize(0x2),
  m_trig_declu_j0_clsize(0x2), m_trig_declu_j1_clsize(0x2),
  
  m_trig_prp_i_eta_reg(0x1), m_trig_prp_j_eta_reg(0x1),
  
  m_trig_shape_k_reg(0x07070707), m_trig_ovl_sx_reg(0xff000000),
  m_trig_ovl_dx_reg(0xff),

  m_readout_buffer_empty(0x0), m_readout_buffer_almost_empty(0x0),
  m_readout_buffer_half_full(0x0), m_readout_buffer_almost_full(0x0),
  m_readout_buffer_full(0x0), m_readout_buffer_latreg(0x67),
  m_readout_buffer_hireg(0x67), m_readout_buffer_lowreg(0x60),
  m_readout_serializer_dslink(0x1),
  m_l1c_pre(0x1ff), m_bcc_pre(0x0)
  
{
  union data {
    uint64_t DoubleWord;
    uint8_t  Bytes[8];
  } Data;
  
  m_isnewcab=false;

  for (int i=0;i<6;++i) Data.Bytes[i] = 0x07;
  for (int i=6;i<8;++i) Data.Bytes[i] = 0x0;
  
  m_trig_edge_reg = Data.DoubleWord;
  
  for (int i=6;i<8;++i) Data.Bytes[i] = 0x07;
  
  m_pipe_j0_edge  = Data.DoubleWord;
  m_pipe_j0_shape = Data.DoubleWord;
  m_pipe_j1_shape = Data.DoubleWord;
  
  for (int th=0; th<3; ++th) {
    for (int ch=0; ch<32; ++ch) {
      for (int by=0; by<2; ++by) {
	m_program_bytes[th][ch][by]=0xffffffff;
      }
    }
  }
  for(int i=0;i<3;++i)
    for(int j=0;j<CMAparameters::pivot_channels;++j)
      for(int k=0;k<CMAparameters::confirm_channels;++k)
	m_threshold_registers[i][j][k] = true;
  
  DBline data(filestr);
  m_status = read_v02(data);
}

CMAprogram::CMAprogram(istringstream& filestr, bool NewCab) :

  m_overlap1(0xff),m_overlap2(0xff000000),m_trig_local_direc_i(0x1),
  m_trig_local_direc_j(0x1),m_trig_k_readout(0x0),

  m_main_control(0x60), m_main_count(0x0), m_main_status(0x0),
    
  m_pipe_i0_mask0_in(0x0), m_pipe_i0_edge(0x07070707),
  m_pipe_i0_ipb_regdepth(0x0101), m_pipe_i0_shape(0x07070707),
  m_pipe_i0_mask0_readout(0x0), m_pipe_i0_mask0_trig(0x0),
  
  m_pipe_i1_mask0_in(0x0), m_pipe_i1_edge(0x07070707),
  m_pipe_i1_ipb_regdepth(0x0101), m_pipe_i1_shape(0x07070707),
  m_pipe_i1_mask0_readout(0x0), m_pipe_i1_mask0_trig(0x0),
  
  m_pipe_j0_mask0_in(0x0),
  m_pipe_j0_ipb_regdepth(0x01010101),
  m_pipe_j0_mask0_readout(0x0), m_pipe_j0_mask0_trig(0x0),
  
  m_pipe_j1_mask0_in(0x0), m_pipe_j1_edge(0x0),
  m_pipe_j1_ipb_regdepth(0x01010101),
  m_pipe_j1_mask0_readout(0x0), m_pipe_j1_mask0_trig(0x0),
  
  m_trig_thr0_maj_reg(0x2),
  m_trig_thr0_mask_1_i_ge1_reg(0x0), m_trig_thr0_mask_1_i_eq2_reg(0x0),
  m_trig_thr0_mask_1_j_ge1_reg(0x0), m_trig_thr0_mask_1_j_eq2_reg(0x0),
  
  m_trig_thr1_maj_reg(0x2),
  m_trig_thr1_mask_1_i_ge1_reg(0x0), m_trig_thr1_mask_1_i_eq2_reg(0x0),
  m_trig_thr1_mask_1_j_ge1_reg(0x0), m_trig_thr1_mask_1_j_eq2_reg(0x0),
  
  m_trig_thr2_maj_reg(0x2),
  m_trig_thr2_mask_1_i_ge1_reg(0x0), m_trig_thr2_mask_1_i_eq2_reg(0x0),
  m_trig_thr2_mask_1_j_ge1_reg(0x0), m_trig_thr2_mask_1_j_eq2_reg(0x0),
  
  m_trig_declu_i0_clsize(0x2), m_trig_declu_i1_clsize(0x2),
  m_trig_declu_j0_clsize(0x2), m_trig_declu_j1_clsize(0x2),
  
  m_trig_prp_i_eta_reg(0x1), m_trig_prp_j_eta_reg(0x1),
  
  m_trig_shape_k_reg(0x07070707), m_trig_ovl_sx_reg(0xff000000),
  m_trig_ovl_dx_reg(0xff),

  m_readout_buffer_empty(0x0), m_readout_buffer_almost_empty(0x0),
  m_readout_buffer_half_full(0x0), m_readout_buffer_almost_full(0x0),
  m_readout_buffer_full(0x0), m_readout_buffer_latreg(0x67),
  m_readout_buffer_hireg(0x67), m_readout_buffer_lowreg(0x60),
  m_readout_serializer_dslink(0x1),
  m_l1c_pre(0x1ff), m_bcc_pre(0x0)
  
{
  union data {
    uint64_t DoubleWord;
    uint8_t  Bytes[8];
  } Data;
  
  m_isnewcab=NewCab;

  for (int i=0;i<6;++i) Data.Bytes[i] = 0x07;
  for (int i=6;i<8;++i) Data.Bytes[i] = 0x0;
  
  m_trig_edge_reg = Data.DoubleWord;
  
  for (int i=6;i<8;++i) Data.Bytes[i] = 0x07;
  
  m_pipe_j0_edge  = Data.DoubleWord;
  m_pipe_j0_shape = Data.DoubleWord;
  m_pipe_j1_shape = Data.DoubleWord;
  
  for (int th=0; th<3; ++th) {
    for (int ch=0; ch<32; ++ch) {
      for (int by=0; by<2; ++by) {
	m_program_bytes[th][ch][by]=0xffffffff;
      }
    }
  }
  for(int i=0;i<3;++i)
    for(int j=0;j<CMAparameters::pivot_channels;++j)
      for(int k=0;k<CMAparameters::confirm_channels;++k)
	m_threshold_registers[i][j][k] = true;
  
  DBline data(filestr);
  m_status = read_v02(data);
}

CMAprogram::CMAprogram(const CMAprogram& program)
{

    m_status = program.check();
    m_overlap1 = program.overlap1();
    m_overlap2 = program.overlap2();
    m_trig_local_direc_i = program.local_direc_i();
    m_trig_local_direc_j = program.local_direc_j();
    m_trig_k_readout     = program.k_readout();

    m_main_control = program.main_control();
    m_main_count   = program.main_count();
    m_main_status  = program.main_status();
        
    m_pipe_i0_mask0_in      = program.pipe_i0_mask0_in();
    m_pipe_i0_edge          = program.pipe_i0_edge(); 
    m_pipe_i0_ipb_regdepth  = program.pipe_i0_ipb_regdepth();
    m_pipe_i0_shape         = program.pipe_i0_shape();
    m_pipe_i0_mask0_readout = program.pipe_i0_mask0_readout();
    m_pipe_i0_mask0_trig    = program.pipe_i0_mask0_trig();
    
    m_pipe_i1_mask0_in      = program.pipe_i1_mask0_in();
    m_pipe_i1_edge          = program.pipe_i1_edge();
    m_pipe_i1_ipb_regdepth  = program.pipe_i1_ipb_regdepth();
    m_pipe_i1_shape         = program.pipe_i1_shape();
    m_pipe_i1_mask0_readout = program.pipe_i1_mask0_readout();
    m_pipe_i1_mask0_trig    = program.pipe_i1_mask0_trig();
    
    m_pipe_j0_mask0_in      = program.pipe_j0_mask0_in();
    m_pipe_j0_edge          = program.pipe_j0_edge();
    m_pipe_j0_ipb_regdepth  = program.pipe_j0_ipb_regdepth();
    m_pipe_j0_shape         = program.pipe_j0_shape();
    m_pipe_j0_mask0_readout = program.pipe_j0_mask0_readout();
    m_pipe_j0_mask0_trig    = program.pipe_j0_mask0_trig();
    
    m_pipe_j1_mask0_in      = program.pipe_j1_mask0_in();
    m_pipe_j1_edge          = program.pipe_j1_edge();
    m_pipe_j1_ipb_regdepth  = program.pipe_j1_ipb_regdepth();
    m_pipe_j1_shape         = program.pipe_j1_shape();
    m_pipe_j1_mask0_readout = program.pipe_j1_mask0_readout();
    m_pipe_j1_mask0_trig    = program.pipe_j1_mask0_trig();

    m_trig_thr0_maj_reg          = program.trig_thr0_maj_reg();
    m_trig_thr0_mask_1_i_ge1_reg = program.trig_thr0_mask_1_i_ge1_reg();
    m_trig_thr0_mask_1_i_eq2_reg = program.trig_thr0_mask_1_i_eq2_reg();
    m_trig_thr0_mask_1_j_ge1_reg = program.trig_thr0_mask_1_j_ge1_reg();
    m_trig_thr0_mask_1_j_eq2_reg = program.trig_thr0_mask_1_j_eq2_reg();

    m_trig_thr1_maj_reg          = program.trig_thr1_maj_reg();
    m_trig_thr1_mask_1_i_ge1_reg = program.trig_thr1_mask_1_i_ge1_reg();
    m_trig_thr1_mask_1_i_eq2_reg = program.trig_thr1_mask_1_i_eq2_reg();
    m_trig_thr1_mask_1_j_ge1_reg = program.trig_thr1_mask_1_j_ge1_reg();
    m_trig_thr1_mask_1_j_eq2_reg = program.trig_thr1_mask_1_j_eq2_reg();

    m_trig_thr2_maj_reg          = program.trig_thr2_maj_reg();
    m_trig_thr2_mask_1_i_ge1_reg = program.trig_thr2_mask_1_i_ge1_reg();
    m_trig_thr2_mask_1_i_eq2_reg = program.trig_thr2_mask_1_i_eq2_reg();
    m_trig_thr2_mask_1_j_ge1_reg = program.trig_thr2_mask_1_j_ge1_reg();
    m_trig_thr2_mask_1_j_eq2_reg = program.trig_thr2_mask_1_j_eq2_reg();
    
    m_trig_declu_i0_clsize = program.trig_declu_i0_clsize();
    m_trig_declu_i1_clsize = program.trig_declu_i1_clsize();
    m_trig_declu_j0_clsize = program.trig_declu_j0_clsize();
    m_trig_declu_j1_clsize = program.trig_declu_j1_clsize();
    
    m_trig_prp_i_eta_reg = program.trig_prp_i_eta_reg();
    m_trig_prp_j_eta_reg = program.trig_prp_j_eta_reg();
    
    m_trig_shape_k_reg = program.trig_shape_k_reg();
    m_trig_ovl_sx_reg  = program.trig_ovl_sx_reg();
    m_trig_ovl_dx_reg  = program.trig_ovl_dx_reg();
    m_trig_edge_reg    = program.trig_edge_reg();
    
    m_readout_buffer_empty        = program.readout_buffer_empty();
    m_readout_buffer_almost_empty = program.readout_buffer_almost_empty();
    m_readout_buffer_half_full    = program.readout_buffer_half_full();
    m_readout_buffer_almost_full  = program.readout_buffer_almost_full();
    m_readout_buffer_full         = program.readout_buffer_full();
    m_readout_buffer_latreg       = program.readout_buffer_latreg();
    m_readout_buffer_hireg        = program.readout_buffer_hireg();
    m_readout_buffer_lowreg       = program.readout_buffer_lowreg();
    m_readout_serializer_dslink   = program.readout_serializer_dslink(); 
    
    m_l1c_pre = program.l1c_pre();
    m_bcc_pre = program.bcc_pre();
    m_isnewcab = program.m_isnewcab;

    copy_threshold(program.registers());
    copy_program(program.bytes());
 }

CMAprogram::~CMAprogram()
{}

CMAprogram&
CMAprogram::operator=(const CMAprogram& program)
{
    if (this!=&program) {
      m_status = program.check();
      m_overlap1 = program.overlap1();
      m_overlap2 = program.overlap2();
      m_trig_local_direc_i = program.local_direc_i();
      m_trig_local_direc_j = program.local_direc_j();
      m_trig_k_readout     = program.k_readout();

      m_main_control = program.main_control();
      m_main_count   = program.main_count();
      m_main_status  = program.main_status();
    
      m_pipe_i0_mask0_in      = program.pipe_i0_mask0_in();
      m_pipe_i0_edge          = program.pipe_i0_edge(); 
      m_pipe_i0_ipb_regdepth  = program.pipe_i0_ipb_regdepth();
      m_pipe_i0_shape         = program.pipe_i0_shape();
      m_pipe_i0_mask0_readout = program.pipe_i0_mask0_readout();
      m_pipe_i0_mask0_trig    = program.pipe_i0_mask0_trig();

      m_pipe_i1_mask0_in      = program.pipe_i1_mask0_in();
      m_pipe_i1_edge          = program.pipe_i1_edge();
      m_pipe_i1_ipb_regdepth  = program.pipe_i1_ipb_regdepth();
      m_pipe_i1_shape         = program.pipe_i1_shape();
      m_pipe_i1_mask0_readout = program.pipe_i1_mask0_readout();
      m_pipe_i1_mask0_trig    = program.pipe_i1_mask0_trig();

      m_pipe_j0_mask0_in      = program.pipe_j0_mask0_in();
      m_pipe_j0_edge          = program.pipe_j0_edge();
      m_pipe_j0_ipb_regdepth  = program.pipe_j0_ipb_regdepth();
      m_pipe_j0_shape         = program.pipe_j0_shape();
      m_pipe_j0_mask0_readout = program.pipe_j0_mask0_readout();
      m_pipe_j0_mask0_trig    = program.pipe_j0_mask0_trig();
    
      m_pipe_j1_mask0_in      = program.pipe_j1_mask0_in();
      m_pipe_j1_edge          = program.pipe_j1_edge();
      m_pipe_j1_ipb_regdepth  = program.pipe_j1_ipb_regdepth();
      m_pipe_j1_shape         = program.pipe_j1_shape();
      m_pipe_j1_mask0_readout = program.pipe_j1_mask0_readout();
      m_pipe_j1_mask0_trig    = program.pipe_j1_mask0_trig();

      m_trig_thr0_maj_reg          = program.trig_thr0_maj_reg();
      m_trig_thr0_mask_1_i_ge1_reg = program.trig_thr0_mask_1_i_ge1_reg();
      m_trig_thr0_mask_1_i_eq2_reg = program.trig_thr0_mask_1_i_eq2_reg();
      m_trig_thr0_mask_1_j_ge1_reg = program.trig_thr0_mask_1_j_ge1_reg();
      m_trig_thr0_mask_1_j_eq2_reg = program.trig_thr0_mask_1_j_eq2_reg();

      m_trig_thr1_maj_reg          = program.trig_thr1_maj_reg();
      m_trig_thr1_mask_1_i_ge1_reg = program.trig_thr1_mask_1_i_ge1_reg();
      m_trig_thr1_mask_1_i_eq2_reg = program.trig_thr1_mask_1_i_eq2_reg();
      m_trig_thr1_mask_1_j_ge1_reg = program.trig_thr1_mask_1_j_ge1_reg();
      m_trig_thr1_mask_1_j_eq2_reg = program.trig_thr1_mask_1_j_eq2_reg();

      m_trig_thr2_maj_reg          = program.trig_thr2_maj_reg();
      m_trig_thr2_mask_1_i_ge1_reg = program.trig_thr2_mask_1_i_ge1_reg();
      m_trig_thr2_mask_1_i_eq2_reg = program.trig_thr2_mask_1_i_eq2_reg();
      m_trig_thr2_mask_1_j_ge1_reg = program.trig_thr2_mask_1_j_ge1_reg();
      m_trig_thr2_mask_1_j_eq2_reg = program.trig_thr2_mask_1_j_eq2_reg();

      m_trig_declu_i0_clsize = program.trig_declu_i0_clsize();
      m_trig_declu_i1_clsize = program.trig_declu_i1_clsize();
      m_trig_declu_j0_clsize = program.trig_declu_j0_clsize();
      m_trig_declu_j1_clsize = program.trig_declu_j1_clsize();

      m_trig_prp_i_eta_reg = program.trig_prp_i_eta_reg();
      m_trig_prp_j_eta_reg = program.trig_prp_j_eta_reg();

      m_trig_shape_k_reg = program.trig_shape_k_reg();
      m_trig_ovl_sx_reg  = program.trig_ovl_sx_reg();
      m_trig_ovl_dx_reg  = program.trig_ovl_dx_reg();
      m_trig_edge_reg    = program.trig_edge_reg();

      m_readout_buffer_empty        = program.readout_buffer_empty();
      m_readout_buffer_almost_empty = program.readout_buffer_almost_empty();
      m_readout_buffer_half_full    = program.readout_buffer_half_full();
      m_readout_buffer_almost_full  = program.readout_buffer_almost_full();
      m_readout_buffer_full         = program.readout_buffer_full();
      m_readout_buffer_latreg       = program.readout_buffer_latreg();
      m_readout_buffer_hireg        = program.readout_buffer_hireg();
      m_readout_buffer_lowreg       = program.readout_buffer_lowreg();
      m_readout_serializer_dslink   = program.readout_serializer_dslink(); 

      m_l1c_pre = program.l1c_pre();
      m_bcc_pre = program.bcc_pre();

      copy_threshold(program.registers());
      copy_program(program.bytes());
    }
    return *this;
}

void
CMAprogram::copy_threshold(const bool* registers)
{
    const int pivch = CMAparameters::pivot_channels;
    const int conch = CMAparameters::confirm_channels;

    for(int th=0;th<3;++th)
    {
	for (int piv=0;piv<pivch;++piv)
	{
	    for (int con=0;con<conch;++con)
	    {
		int pos = th*pivch*conch + piv*conch + con;
		m_threshold_registers[th][piv][con] = *(registers+pos);
	    }
	}
    }
}

void
CMAprogram::copy_program(const uint32_t* bytes)
{
    const int pivch = CMAparameters::pivot_channels;

    for(int th=0;th<3;++th)
    {
	for (int piv=0;piv<pivch;++piv)
	{
	    for (int byt=0;byt<2;++byt)
	    {
		int pos = th*pivch*2 + piv*2 + byt;
		m_program_bytes[th][piv][byt] = *(bytes+pos);
	    }
	}
    }
}
bool
CMAprogram::read(DBline& data)
{
    //unsigned long left_bit  = 0x80000000;
    unsigned long right_bit = 0x1;

    // switch to new DBline I/O format 
    data.setdbfmtflag(0);

    ++data;

    while(data)
      {
        // start read the threshold registers
        for(int i=1;i<=3;++i)
	  {
	    int majority;
	    
	    if(data("th #",i) && data("maj_reg") >> majority)
	      {
	        int th = 0;
	        int ch = 0;
	        uint32_t  first_word = 0;
	        uint32_t second_word = 0;
		
	        ++data;
		
	        while (data("th") >> th >> "thr_reg" >> ch >> data.dbhex() 
		       >> first_word >> second_word >> data.dbdec())
		  {
		    if(th < 1) return false;
		    m_program_bytes[th-1][ch][1] = first_word;
		    m_program_bytes[th-1][ch][0] = second_word;
		    
		    for(int bit=0;bit<32;++bit)
		      {
			m_threshold_registers[th-1][ch][bit] = 
			  ((second_word >> bit) & right_bit)? true : false;
			m_threshold_registers[th-1][ch][bit+32] = 
			  ((first_word >> bit) & right_bit)? true : false;
		      }
		    
		    ++data;
		  }
	      }
	  }
	
	data("overlap1") >> data.dbhex() >> m_overlap1;
	data("overlap2") >> data.dbhex() >> m_overlap2;
	data("trig_local_direc_i") >> m_trig_local_direc_i;
	data("trig_local_direc_j") >> m_trig_local_direc_j;
	data("trig_k_readout") >> m_trig_k_readout;
	
        ++data;
      }
    
    // switch back to previous DBline I/O format  
    data.setdbfmtflag(1);
    return true;
}

bool
CMAprogram::read_v02(DBline& data)
{
  uint32_t right_bit = 0x1;
  
  // switch to new DBline I/O format 
  data.setdbfmtflag(0);

  ++data;
  
  while(data)
    {
      data("set_main_control") >> data.dbhex() >> m_main_control >> data.dbdec();
      data("set_main_count")   >> data.dbhex() >> m_main_count   >> data.dbdec();
      data("set_main_status")  >> data.dbhex() >> m_main_status  >> data.dbdec();
      
      data("set_pipe_i0_mask0_in")      >> data.dbhex() >> m_pipe_i0_mask0_in >> data.dbdec();
      data("set_pipe_i0_edge")          >> data.dbhex() >> m_pipe_i0_edge >> data.dbdec();
      data("set_pipe_i0_ipb_regdepth")  >> data.dbhex() >> m_pipe_i0_ipb_regdepth >> data.dbdec();
      data("set_pipe_i0_shape")         >> data.dbhex() >> m_pipe_i0_shape >> data.dbdec();
      data("set_pipe_i0_mask0_readout") >> data.dbhex() >> m_pipe_i0_mask0_readout >> data.dbdec();
      data("set_pipe_i0_mask0_trig")    >> data.dbhex() >> m_pipe_i0_mask0_trig >> data.dbdec();
      
      data("set_pipe_i1_mask0_in")      >> data.dbhex() >> m_pipe_i1_mask0_in >> data.dbdec();
      data("set_pipe_i1_edge")          >> data.dbhex() >> m_pipe_i1_edge >> data.dbdec();
      data("set_pipe_i1_ipb_regdepth")  >> data.dbhex() >> m_pipe_i1_ipb_regdepth >> data.dbdec();
      data("set_pipe_i1_shape")         >> data.dbhex() >> m_pipe_i1_shape >> data.dbdec();
      data("set_pipe_i1_mask0_readout") >> data.dbhex() >> m_pipe_i1_mask0_readout >> data.dbdec();
      data("set_pipe_i1_mask0_trig")    >> data.dbhex() >> m_pipe_i1_mask0_trig >> data.dbdec();
      
      data("set_pipe_j0_mask0_in")      >> data.dbhex() >> m_pipe_j0_mask0_in >> data.dbdec();
      data("set_pipe_j0_edge")          >> data.dbhex() >> m_pipe_j0_edge >> data.dbdec();
      data("set_pipe_j0_ipb_regdepth")  >> data.dbhex() >> m_pipe_j0_ipb_regdepth >> data.dbdec();
      data("set_pipe_j0_shape")         >> data.dbhex() >> m_pipe_j0_shape >> data.dbdec();
      data("set_pipe_j0_mask0_readout") >> data.dbhex() >> m_pipe_j0_mask0_readout >> data.dbdec();
      data("set_pipe_j0_mask0_trig")    >> data.dbhex() >> m_pipe_j0_mask0_trig >> data.dbdec();
      
      data("set_pipe_j1_mask0_in")      >> data.dbhex() >> m_pipe_j1_mask0_in >> data.dbdec();
      data("set_pipe_j1_edge")          >> data.dbhex() >> m_pipe_j1_edge >> data.dbdec();
      data("set_pipe_j1_ipb_regdepth")  >> data.dbhex() >> m_pipe_j1_ipb_regdepth >> data.dbdec();
      data("set_pipe_j1_shape")         >> data.dbhex() >> m_pipe_j1_shape >> data.dbdec();
      data("set_pipe_j1_mask0_readout") >> data.dbhex() >> m_pipe_j1_mask0_readout >> data.dbdec();
      data("set_pipe_j1_mask0_trig")    >> data.dbhex() >> m_pipe_j1_mask0_trig >> data.dbdec();
      
      data("set_trig_thr0_maj_reg") >> data.dbhex() >> m_trig_thr0_maj_reg >> data.dbdec();
      data("set_trig_thr0_mask_1_i_ge1_reg") >> data.dbhex() >> m_trig_thr0_mask_1_i_ge1_reg >> data.dbdec();
      data("set_trig_thr0_mask_1_i_eq2_reg") >> data.dbhex() >> m_trig_thr0_mask_1_i_eq2_reg >> data.dbdec();
      data("set_trig_thr0_mask_1_j_ge1_reg") >> data.dbhex() >> m_trig_thr0_mask_1_j_ge1_reg >> data.dbdec();
      data("set_trig_thr0_mask_1_j_eq2_reg") >> data.dbhex() >> m_trig_thr0_mask_1_j_eq2_reg >> data.dbdec();
      
      data("set_trig_thr1_maj_reg") >> data.dbhex() >> m_trig_thr1_maj_reg >> data.dbdec();
      data("set_trig_thr1_mask_1_i_ge1_reg") >> data.dbhex() >> m_trig_thr1_mask_1_i_ge1_reg >> data.dbdec();
      data("set_trig_thr1_mask_1_i_eq2_reg") >> data.dbhex() >> m_trig_thr1_mask_1_i_eq2_reg >> data.dbdec();
      data("set_trig_thr1_mask_1_j_ge1_reg") >> data.dbhex() >> m_trig_thr1_mask_1_j_ge1_reg >> data.dbdec();
      data("set_trig_thr1_mask_1_j_eq2_reg") >> data.dbhex() >> m_trig_thr1_mask_1_j_eq2_reg >> data.dbdec();
      
      data("set_trig_thr2_maj_reg") >> data.dbhex() >> m_trig_thr2_maj_reg >> data.dbdec();
      data("set_trig_thr2_mask_1_i_ge1_reg") >> data.dbhex() >> m_trig_thr2_mask_1_i_ge1_reg >> data.dbdec();
      data("set_trig_thr2_mask_1_i_eq2_reg") >> data.dbhex() >> m_trig_thr2_mask_1_i_eq2_reg >> data.dbdec();
      data("set_trig_thr2_mask_1_j_ge1_reg") >> data.dbhex() >> m_trig_thr2_mask_1_j_ge1_reg >> data.dbdec();
      data("set_trig_thr2_mask_1_j_eq2_reg") >> data.dbhex() >> m_trig_thr2_mask_1_j_eq2_reg >> data.dbdec();
      
      data("set_trig_declu_i0_clsize") >> data.dbhex() >> m_trig_declu_i0_clsize >> data.dbdec();
      data("set_trig_declu_i1_clsize") >> data.dbhex() >> m_trig_declu_i1_clsize >> data.dbdec();
      data("set_trig_declu_j0_clsize") >> data.dbhex() >> m_trig_declu_j0_clsize >> data.dbdec();
      data("set_trig_declu_j1_clsize") >> data.dbhex() >> m_trig_declu_j1_clsize >> data.dbdec();
      
      //data("set_trig_prp_i_eta_reg") >> data.dbhex() >> m_trig_prp_i_eta_reg >> data.dbdec();
      //data("set_trig_prp_j_eta_reg") >> data.dbhex() >> m_trig_prp_j_eta_reg >> data.dbdec();
      data("set_trig_prp_i_eta_reg") >> data.dbhex() >> m_trig_local_direc_i >> data.dbdec();
      data("set_trig_prp_j_eta_reg") >> data.dbhex() >> m_trig_local_direc_j >> data.dbdec();
      
      data("set_trig_shape_k_reg") >> data.dbhex() >> m_trig_shape_k_reg >> data.dbdec();
      //      data("set_trig_ovl_sx_reg") >> data.dbhex() >> m_trig_ovl_sx_reg >> data.dbdec();
      //      data("set_trig_ovl_dx_reg") >> data.dbhex() >> m_trig_ovl_dx_reg >> data.dbdec();
      data("set_trig_ovl_sx_reg") >> data.dbhex() >> m_overlap2 >> data.dbdec();
      data("set_trig_ovl_dx_reg") >> data.dbhex() >> m_overlap1 >> data.dbdec();
      data("set_trig_edge_reg") >> data.dbhex() >> m_trig_edge_reg >> data.dbdec();
      
      data("set_readout_buffer_empty") >> data.dbhex() >> m_readout_buffer_empty >> data.dbdec();
      data("set_readout_buffer_almost_empty") >> data.dbhex() >> m_readout_buffer_almost_empty >> data.dbdec();
      data("set_readout_buffer_half_full") >> data.dbhex() >> m_readout_buffer_half_full >> data.dbdec();
      data("set_readout_buffer_almost_full") >> data.dbhex() >> m_readout_buffer_almost_full >> data.dbdec();
      data("set_readout_buffer_full") >> data.dbhex() >> m_readout_buffer_full >> data.dbdec();
      data("set_readout_buffer_latreg") >> data.dbhex() >> m_readout_buffer_latreg >> data.dbdec();
      data("set_readout_buffer_hireg") >> data.dbhex() >> m_readout_buffer_hireg >> data.dbdec();
      data("set_readout_buffer_lowreg") >> data.dbhex() >> m_readout_buffer_lowreg >> data.dbdec();
      data("set_readout_serializer_dslink") >> data.dbhex() >> m_readout_serializer_dslink >> data.dbdec();
      
      data("set_l1c_pre") >> data.dbhex() >> m_l1c_pre >> data.dbdec();
      data("set_bcc_pre") >> data.dbhex() >> m_bcc_pre >> data.dbdec();	
      
      
      // start reading the threshold registers 

      uint64_t twowords;
      
      if(m_isnewcab){

	// case MuonRPC_Cabling
        if (data("set_trig_thr0_thr_reg_00")) 
        {
	  std::string newString = "empty";
	  for (int i=0;i<32;++i) 
	  {
	      if (i==0) data >> data.dbhex() >> twowords >> data.dbdec();
	      else data >> newString >>data.dbhex() >> twowords >> data.dbdec();
	      
	      //std::cout<<" newString "<<newString<<" two words = "<<twowords<<std::endl;
	      
	      union Data {
		  uint64_t bits;
		  uint32_t words[2];
	      } dataun;
	  
	      dataun.bits = twowords;
		 
	      m_program_bytes[0][i][0] = dataun.words[0];
	      m_program_bytes[0][i][1] = dataun.words[1];
	      

	      for(int bit=0;bit<32;++bit)
	      {
		  m_threshold_registers[0][i][bit] = 
		      ((dataun.words[0] >> bit) & right_bit)? true : false;
		  m_threshold_registers[0][i][bit+32] = 
		      ((dataun.words[1]>> bit) & right_bit)? true : false;
		  //if (m_threshold_registers[0][i][bit]) std::cout<<" m_threshold_registers[0]["<<i<<"]["<<bit<<"] = "<<  m_threshold_registers[0][i][bit]<<std::endl;
		  //if (m_threshold_registers[0][i][bit+32]) std::cout<<" m_threshold_registers[0]["<<i<<"]["<<bit+32<<"] = "<<  m_threshold_registers[0][i][bit+32]<<std::endl;
	      }
	      if (i<31) ++data;
	  }
	}

	
        if (data("set_trig_thr1_thr_reg_00")) 
        {
	  std::string newString = "empty";
	  for (int i=0;i<32;++i) 
	  {
	      if (i==0) data >> data.dbhex() >> twowords >> data.dbdec();
	      else data >> newString >>data.dbhex() >> twowords >> data.dbdec();
	      
	      //std::cout<<" newString "<<newString<<" two words = "<<twowords<<std::endl;
	      
	      union Data {
		  uint64_t bits;
		  uint32_t words[2];
	      } dataun;
	  
	      dataun.bits = twowords;
		 
	      m_program_bytes[1][i][0] = dataun.words[0];
	      m_program_bytes[1][i][1] = dataun.words[1];
	      

	      for(int bit=0;bit<32;++bit)
	      {
		  m_threshold_registers[1][i][bit] = 
		      ((dataun.words[0] >> bit) & right_bit)? true : false;
		  m_threshold_registers[1][i][bit+32] = 
		      ((dataun.words[1]>> bit) & right_bit)? true : false;
		  //if (m_threshold_registers[1][i][bit]) std::cout<<" m_threshold_registers[1]["<<i<<"]["<<bit<<"] = "<<  m_threshold_registers[1][i][bit]<<std::endl;
		  //if (m_threshold_registers[1][i][bit+32]) std::cout<<" m_threshold_registers[1]["<<i<<"]["<<bit+32<<"] = "<<  m_threshold_registers[1][i][bit+32]<<std::endl;
	      }
	  
	      if (i<31) ++data;
	  }
	}

        if (data("set_trig_thr2_thr_reg_00")) 
        {
	  std::string newString = "empty";
	  for (int i=0;i<32;++i) 
	  {
	      if (i==0) data >> data.dbhex() >> twowords >> data.dbdec();
	      else data >> newString >>data.dbhex() >> twowords >> data.dbdec();
	      
	      //std::cout<<" newString "<<newString<<" two words = "<<twowords<<std::endl;
	      
	      union Data {
		  uint64_t bits;
		  uint32_t words[2];
	      } dataun;
	  
	      dataun.bits = twowords;
		 
	      m_program_bytes[2][i][0] = dataun.words[0];
	      m_program_bytes[2][i][1] = dataun.words[1];
	      

	      for(int bit=0;bit<32;++bit)
	      {
		  m_threshold_registers[2][i][bit] = 
		      ((dataun.words[0] >> bit) & right_bit)? true : false;
		  m_threshold_registers[2][i][bit+32] = 
		      ((dataun.words[1]>> bit) & right_bit)? true : false;
		  //if (m_threshold_registers[2][i][bit]) std::cout<<" m_threshold_registers[2]["<<i<<"]["<<bit<<"] = "<<  m_threshold_registers[2][i][bit]<<std::endl;
		  //if (m_threshold_registers[2][i][bit+32]) std::cout<<" m_threshold_registers[2]["<<i<<"]["<<bit+32<<"] = "<<  m_threshold_registers[2][i][bit+32]<<std::endl;
	      }
	  
	      if (i<31) ++data;
	  }
	}
        ++data;
      }

      else
      {
        // case RPCcablingSim
        // old format ///////////////////////////////////////////////////////
        for(int i=1;i<=3;++i)
	{
	  int majority;
	  
	  if(data("th #",i) && data("maj_reg") >> majority)
	    {
	      int th = 0;
	      int ch = 0;
	      uint32_t  first_word = 0;
	      uint32_t second_word = 0;
	      
	      ++data;
	      
	      while (data("th") >> th >> "thr_reg" >> ch >> data.dbhex() 
		     >> first_word >> second_word >> data.dbdec())
		{
		  if(th < 1) return false;
		  m_program_bytes[th-1][ch][1] = first_word;
		  m_program_bytes[th-1][ch][0] = second_word;
		  
		  for(int bit=0;bit<32;++bit)
		    {
		      m_threshold_registers[th-1][ch][bit] = 
			((second_word >> bit) & right_bit)? true : false;
		      m_threshold_registers[th-1][ch][bit+32] = 
			((first_word >> bit) & right_bit)? true : false;
		    }
		  
		  ++data;
		}
	    }
	}

        data("overlap1") >> data.dbhex()  >> m_overlap1 >> data.dbdec();
        data("overlap2") >> data.dbhex()  >> m_overlap2 >> data.dbdec();
        data("trig_local_direc_i") >> m_trig_local_direc_i;
        data("trig_local_direc_j") >> m_trig_local_direc_j;
        data("trig_k_readout")     >> m_trig_k_readout;

      ++data;
      }
    }
  // switch back to previous DBline I/O format  
  data.setdbfmtflag(1);
  return true;
}


void 
CMAprogram::open_threshold(int th){
    if (th>=3) return;
    for (int ch=0;ch<CMAparameters::pivot_channels;++ch) {
      m_program_bytes[th][ch][1] = 0xffffffff;
      m_program_bytes[th][ch][0] = 0xffffffff;
 	  	 
        for(int bit=0;bit<32;++bit) {
	  m_threshold_registers[th][ch][bit] = true;
	  m_threshold_registers[th][ch][bit+32] = true;
        }
    }
}

void
CMAprogram::copy_over(int source, int dest) {
    if (source>=3 || dest>=3) return;
    if (source == dest ) return;
    
    for (int ch=0;ch<CMAparameters::pivot_channels;++ch) {
        m_program_bytes[dest][ch][1] = m_program_bytes[source][ch][1];
        m_program_bytes[dest][ch][0] = m_program_bytes[source][ch][0];
		    
        for(int bit=0;bit<32;++bit) {
            m_threshold_registers[dest][ch][bit] = m_threshold_registers[source][ch][bit] ; 
            m_threshold_registers[dest][ch][bit+32] = m_threshold_registers[source][ch][bit+32]; 
        }
    }
}

bool
CMAprogram::hasProgrammed(unsigned int th) const {
    if (th>=3) return false;
    for (int ch=0;ch<CMAparameters::pivot_channels;++ch) {
        if( m_program_bytes[th][ch][1] != 0x0) return true;
        if( m_program_bytes[th][ch][0] != 0x0) return true;
    }
    return false;
}

ostream& operator<<(ostream& stream,const CMAprogram& program)
{
    unsigned int no_coincidence = 183;

    for(int piv=CMAparameters::pivot_channels-1;piv>=0;--piv)
    {
        for(int con=0;con<CMAparameters::confirm_channels;++con)
	{
	    for (int th=2;th>=0;--th)
	    {
		if(program.m_threshold_registers[th][piv][con])
		{
		    stream << th+1;
                    break;
	        }
                if(th==0)  stream << (char)no_coincidence;
	    }
	}
	stream << endl;
    }
    return stream;
}
