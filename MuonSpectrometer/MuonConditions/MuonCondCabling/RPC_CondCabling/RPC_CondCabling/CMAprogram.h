/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMAPROGRAM_H
#define CMAPROGRAM_H

#include "MuonCablingTools/dbline.h"
#include "RPC_CondCabling/CMAparameters.h"

class CMAprogram {
private:
    /* Data members are arranged to reduce the amount of padding ("holes") */

    bool m_isnewcab{false};
    bool m_status{false};

    uint8_t m_main_count{0};
    uint8_t m_main_status{0};

    uint32_t m_main_control{0x60};

    uint32_t m_overlap1{0xff};
    uint32_t m_overlap2{0xff000000};

    uint32_t m_trig_local_direc_i{0x1};
    uint32_t m_trig_local_direc_j{0x1};
    uint32_t m_trig_k_readout{0};

    uint16_t m_pipe_i0_ipb_regdepth{0x0101};
    uint16_t m_pipe_i1_ipb_regdepth{0x0101};

    uint32_t m_pipe_i0_mask0_in{0};
    uint32_t m_pipe_i0_edge{0x07070707};
    uint32_t m_pipe_i0_shape{0x07070707};
    uint32_t m_pipe_i0_mask0_readout{0};
    uint32_t m_pipe_i0_mask0_trig{0};

    uint32_t m_pipe_i1_mask0_in{0};
    uint32_t m_pipe_i1_edge{0x07070707};
    uint32_t m_pipe_i1_shape{0x07070707};
    uint32_t m_pipe_i1_mask0_readout{0};
    uint32_t m_pipe_i1_mask0_trig{0};

    uint32_t m_pipe_j0_ipb_regdepth{0x01010101};
    uint32_t m_pipe_j1_ipb_regdepth{0x01010101};

    uint64_t m_pipe_j0_mask0_in{0};
    uint64_t m_pipe_j0_edge{0x0707070707070707};
    uint64_t m_pipe_j0_shape{0x0707070707070707};
    uint64_t m_pipe_j0_mask0_readout{0};
    uint64_t m_pipe_j0_mask0_trig{0};

    uint64_t m_pipe_j1_mask0_in{0};
    uint64_t m_pipe_j1_edge{0};
    uint64_t m_pipe_j1_shape{0x0707070707070707};
    uint64_t m_pipe_j1_mask0_readout{0};
    uint64_t m_pipe_j1_mask0_trig{0};

    uint32_t m_trig_thr0_mask_1_i_ge1_reg{0};
    uint32_t m_trig_thr0_mask_1_i_eq2_reg{0};
    uint64_t m_trig_thr0_mask_1_j_ge1_reg{0};
    uint64_t m_trig_thr0_mask_1_j_eq2_reg{0};

    uint32_t m_trig_thr1_mask_1_i_ge1_reg{0};
    uint32_t m_trig_thr1_mask_1_i_eq2_reg{0};
    uint64_t m_trig_thr1_mask_1_j_ge1_reg{0};
    uint64_t m_trig_thr1_mask_1_j_eq2_reg{0};

    uint32_t m_trig_thr2_mask_1_i_ge1_reg{0};
    uint32_t m_trig_thr2_mask_1_i_eq2_reg{0};
    uint64_t m_trig_thr2_mask_1_j_ge1_reg{0};
    uint64_t m_trig_thr2_mask_1_j_eq2_reg{0};

    uint64_t m_trig_edge_reg{0x070707070707};
    uint32_t m_trig_shape_k_reg{0x07070707};
    uint32_t m_trig_ovl_sx_reg{0xff000000};
    uint32_t m_trig_ovl_dx_reg{0x000000ff};

    uint16_t m_readout_buffer_empty{0};
    uint16_t m_readout_buffer_almost_empty{0};
    uint16_t m_readout_buffer_half_full{0};
    uint16_t m_readout_buffer_almost_full{0};
    uint16_t m_readout_buffer_full{0};

    uint8_t m_trig_declu_i0_clsize{0x2};
    uint8_t m_trig_declu_i1_clsize{0x2};
    uint8_t m_trig_declu_j0_clsize{0x2};
    uint8_t m_trig_declu_j1_clsize{0x2};

    uint8_t m_trig_prp_i_eta_reg{0x1};
    uint8_t m_trig_prp_j_eta_reg{0x1};

    uint8_t m_trig_thr0_maj_reg{0x2};
    uint8_t m_trig_thr1_maj_reg{0x2};
    uint8_t m_trig_thr2_maj_reg{0x2};

    uint8_t m_readout_buffer_latreg{0x67};
    uint8_t m_readout_buffer_hireg{0x67};
    uint8_t m_readout_buffer_lowreg{0x60};
    uint8_t m_readout_serializer_dslink{0x1};

    uint16_t m_l1c_pre{0x1ff};
    uint16_t m_bcc_pre{0};

    bool m_threshold_registers[3][CMAparameters::pivot_channels][CMAparameters::confirm_channels]{};
    uint32_t m_program_bytes[3][CMAparameters::pivot_channels][2]{};

    bool read_v02(DBline&);

public:
    CMAprogram();
    CMAprogram(std::ifstream& file, bool NewCab = false);
    CMAprogram(std::istringstream& filestr, bool NewCab = false);

    CMAprogram(const CMAprogram&) = default;
    CMAprogram& operator=(const CMAprogram&) = default;

    const bool* registers(void) const { return reinterpret_cast<const bool*>(m_threshold_registers); }
    const uint32_t* bytes(void) const { return reinterpret_cast<const uint32_t*>(m_program_bytes); }

    bool check(void) const { return m_status; }
    uint32_t overlap1(void) const { return m_overlap1; }
    uint32_t overlap2(void) const { return m_overlap2; }
    uint32_t local_direc_i(void) const { return m_trig_local_direc_i; }
    uint32_t local_direc_j(void) const { return m_trig_local_direc_j; }
    uint32_t k_readout(void) const { return m_trig_k_readout; }

    uint32_t main_control() const;
    inline uint8_t main_count() const;
    inline uint8_t main_status() const;

    uint32_t pipe_i0_mask0_in() const;
    uint32_t pipe_i0_edge() const;
    uint16_t pipe_i0_ipb_regdepth() const;
    uint32_t pipe_i0_shape() const;
    uint32_t pipe_i0_mask0_readout() const;
    uint32_t pipe_i0_mask0_trig() const;

    uint32_t pipe_i1_mask0_in() const;
    uint32_t pipe_i1_edge() const;
    uint16_t pipe_i1_ipb_regdepth() const;
    uint32_t pipe_i1_shape() const;
    uint32_t pipe_i1_mask0_readout() const;
    uint32_t pipe_i1_mask0_trig() const;

    uint64_t pipe_j0_mask0_in() const;
    uint64_t pipe_j0_edge() const;
    uint32_t pipe_j0_ipb_regdepth() const;
    uint64_t pipe_j0_shape() const;
    uint64_t pipe_j0_mask0_readout() const;
    uint64_t pipe_j0_mask0_trig() const;

    uint64_t pipe_j1_mask0_in() const;
    uint64_t pipe_j1_edge() const;
    uint32_t pipe_j1_ipb_regdepth() const;
    uint64_t pipe_j1_shape() const;
    uint64_t pipe_j1_mask0_readout() const;
    uint64_t pipe_j1_mask0_trig() const;

    uint8_t trig_thr0_maj_reg() const;
    uint32_t trig_thr0_mask_1_i_ge1_reg() const;
    uint32_t trig_thr0_mask_1_i_eq2_reg() const;
    uint64_t trig_thr0_mask_1_j_ge1_reg() const;
    uint64_t trig_thr0_mask_1_j_eq2_reg() const;

    uint8_t trig_thr1_maj_reg() const;
    uint32_t trig_thr1_mask_1_i_ge1_reg() const;
    uint32_t trig_thr1_mask_1_i_eq2_reg() const;
    uint64_t trig_thr1_mask_1_j_ge1_reg() const;
    uint64_t trig_thr1_mask_1_j_eq2_reg() const;

    uint8_t trig_thr2_maj_reg() const;
    uint32_t trig_thr2_mask_1_i_ge1_reg() const;
    uint32_t trig_thr2_mask_1_i_eq2_reg() const;
    uint64_t trig_thr2_mask_1_j_ge1_reg() const;
    uint64_t trig_thr2_mask_1_j_eq2_reg() const;

    uint8_t trig_declu_i0_clsize() const;
    uint8_t trig_declu_i1_clsize() const;
    uint8_t trig_declu_j0_clsize() const;
    uint8_t trig_declu_j1_clsize() const;

    uint8_t trig_prp_i_eta_reg() const;
    uint8_t trig_prp_j_eta_reg() const;

    uint32_t trig_shape_k_reg() const;
    uint32_t trig_ovl_sx_reg() const;
    uint32_t trig_ovl_dx_reg() const;
    uint64_t trig_edge_reg() const;

    uint16_t readout_buffer_empty() const;
    uint16_t readout_buffer_almost_empty() const;
    uint16_t readout_buffer_half_full() const;
    uint16_t readout_buffer_almost_full() const;
    uint16_t readout_buffer_full() const;
    uint8_t readout_buffer_latreg() const;
    uint8_t readout_buffer_hireg() const;
    uint8_t readout_buffer_lowreg() const;
    uint8_t readout_serializer_dslink() const;

    uint16_t l1c_pre() const;
    uint16_t bcc_pre() const;

    void open_threshold(int th);
    void copy_over(int, int);

    bool hasProgrammed(unsigned int th) const;

    friend std::ostream& operator<<(std::ostream&, const CMAprogram&);
};

inline uint32_t CMAprogram::main_control(void) const { return m_main_control; }
inline uint8_t CMAprogram::main_count(void) const { return m_main_count; }
inline uint8_t CMAprogram::main_status(void) const { return m_main_status; }

inline uint32_t CMAprogram::pipe_i0_mask0_in(void) const { return m_pipe_i0_mask0_in; }
inline uint32_t CMAprogram::pipe_i0_edge(void) const { return m_pipe_i0_edge; }
inline uint16_t CMAprogram::pipe_i0_ipb_regdepth(void) const { return m_pipe_i0_ipb_regdepth; }
inline uint32_t CMAprogram::pipe_i0_shape(void) const { return m_pipe_i0_shape; }
inline uint32_t CMAprogram::pipe_i0_mask0_readout(void) const { return m_pipe_i0_mask0_readout; }
inline uint32_t CMAprogram::pipe_i0_mask0_trig(void) const { return m_pipe_i0_mask0_trig; }

inline uint32_t CMAprogram::pipe_i1_mask0_in(void) const { return m_pipe_i1_mask0_in; }
inline uint32_t CMAprogram::pipe_i1_edge(void) const { return m_pipe_i1_edge; }
inline uint16_t CMAprogram::pipe_i1_ipb_regdepth(void) const { return m_pipe_i1_ipb_regdepth; }
inline uint32_t CMAprogram::pipe_i1_shape(void) const { return m_pipe_i1_shape; }
inline uint32_t CMAprogram::pipe_i1_mask0_readout(void) const { return m_pipe_i1_mask0_readout; }
inline uint32_t CMAprogram::pipe_i1_mask0_trig(void) const { return m_pipe_i1_mask0_trig; }

inline uint64_t CMAprogram::pipe_j0_mask0_in(void) const { return m_pipe_j0_mask0_in; }
inline uint64_t CMAprogram::pipe_j0_edge(void) const { return m_pipe_j0_edge; }
inline uint32_t CMAprogram::pipe_j0_ipb_regdepth(void) const { return m_pipe_j0_ipb_regdepth; }
inline uint64_t CMAprogram::pipe_j0_shape(void) const { return m_pipe_j0_shape; }
inline uint64_t CMAprogram::pipe_j0_mask0_readout(void) const { return m_pipe_j0_mask0_readout; }
inline uint64_t CMAprogram::pipe_j0_mask0_trig(void) const { return m_pipe_j0_mask0_trig; }

inline uint64_t CMAprogram::pipe_j1_mask0_in(void) const { return m_pipe_j1_mask0_in; }
inline uint64_t CMAprogram::pipe_j1_edge(void) const { return m_pipe_j1_edge; }
inline uint32_t CMAprogram::pipe_j1_ipb_regdepth(void) const { return m_pipe_j1_ipb_regdepth; }
inline uint64_t CMAprogram::pipe_j1_shape(void) const { return m_pipe_j1_shape; }
inline uint64_t CMAprogram::pipe_j1_mask0_readout(void) const { return m_pipe_j1_mask0_readout; }
inline uint64_t CMAprogram::pipe_j1_mask0_trig(void) const { return m_pipe_j1_mask0_trig; }

inline uint8_t CMAprogram::trig_thr0_maj_reg(void) const { return m_trig_thr0_maj_reg; }
inline uint32_t CMAprogram::trig_thr0_mask_1_i_ge1_reg(void) const { return m_trig_thr0_mask_1_i_ge1_reg; }
inline uint32_t CMAprogram::trig_thr0_mask_1_i_eq2_reg(void) const { return m_trig_thr0_mask_1_i_eq2_reg; }
inline uint64_t CMAprogram::trig_thr0_mask_1_j_ge1_reg(void) const { return m_trig_thr0_mask_1_j_ge1_reg; }
inline uint64_t CMAprogram::trig_thr0_mask_1_j_eq2_reg(void) const { return m_trig_thr0_mask_1_j_eq2_reg; }

inline uint8_t CMAprogram::trig_thr1_maj_reg(void) const { return m_trig_thr1_maj_reg; }
inline uint32_t CMAprogram::trig_thr1_mask_1_i_ge1_reg(void) const { return m_trig_thr1_mask_1_i_ge1_reg; }
inline uint32_t CMAprogram::trig_thr1_mask_1_i_eq2_reg(void) const { return m_trig_thr1_mask_1_i_eq2_reg; }
inline uint64_t CMAprogram::trig_thr1_mask_1_j_ge1_reg(void) const { return m_trig_thr1_mask_1_j_ge1_reg; }
inline uint64_t CMAprogram::trig_thr1_mask_1_j_eq2_reg(void) const { return m_trig_thr1_mask_1_j_eq2_reg; }

inline uint8_t CMAprogram::trig_thr2_maj_reg(void) const { return m_trig_thr2_maj_reg; }
inline uint32_t CMAprogram::trig_thr2_mask_1_i_ge1_reg(void) const { return m_trig_thr2_mask_1_i_ge1_reg; }
inline uint32_t CMAprogram::trig_thr2_mask_1_i_eq2_reg(void) const { return m_trig_thr2_mask_1_i_eq2_reg; }
inline uint64_t CMAprogram::trig_thr2_mask_1_j_ge1_reg(void) const { return m_trig_thr2_mask_1_j_ge1_reg; }
inline uint64_t CMAprogram::trig_thr2_mask_1_j_eq2_reg(void) const { return m_trig_thr2_mask_1_j_eq2_reg; }

inline uint8_t CMAprogram::trig_declu_i0_clsize(void) const { return m_trig_declu_i0_clsize; }
inline uint8_t CMAprogram::trig_declu_i1_clsize(void) const { return m_trig_declu_i1_clsize; }
inline uint8_t CMAprogram::trig_declu_j0_clsize(void) const { return m_trig_declu_j0_clsize; }
inline uint8_t CMAprogram::trig_declu_j1_clsize(void) const { return m_trig_declu_j1_clsize; }

inline uint8_t CMAprogram::trig_prp_i_eta_reg(void) const { return m_trig_prp_i_eta_reg; }
inline uint8_t CMAprogram::trig_prp_j_eta_reg(void) const { return m_trig_prp_j_eta_reg; }

inline uint32_t CMAprogram::trig_shape_k_reg(void) const { return m_trig_shape_k_reg; }
inline uint32_t CMAprogram::trig_ovl_sx_reg(void) const { return m_trig_ovl_sx_reg; }
inline uint32_t CMAprogram::trig_ovl_dx_reg(void) const { return m_trig_ovl_dx_reg; }
inline uint64_t CMAprogram::trig_edge_reg(void) const { return m_trig_edge_reg; }

inline uint16_t CMAprogram::readout_buffer_empty(void) const { return m_readout_buffer_empty; }
inline uint16_t CMAprogram::readout_buffer_almost_empty(void) const { return m_readout_buffer_almost_empty; }
inline uint16_t CMAprogram::readout_buffer_half_full(void) const { return m_readout_buffer_half_full; }
inline uint16_t CMAprogram::readout_buffer_almost_full(void) const { return m_readout_buffer_almost_full; }
inline uint16_t CMAprogram::readout_buffer_full(void) const { return m_readout_buffer_full; }
inline uint8_t CMAprogram::readout_buffer_latreg(void) const { return m_readout_buffer_latreg; }
inline uint8_t CMAprogram::readout_buffer_hireg(void) const { return m_readout_buffer_hireg; }
inline uint8_t CMAprogram::readout_buffer_lowreg(void) const { return m_readout_buffer_lowreg; }
inline uint8_t CMAprogram::readout_serializer_dslink(void) const { return m_readout_serializer_dslink; }

inline uint16_t CMAprogram::l1c_pre(void) const { return m_l1c_pre; }
inline uint16_t CMAprogram::bcc_pre(void) const { return m_bcc_pre; }

#endif
