/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CMAPROGRAM_H
#define CMAPROGRAM_H

#include "RPCcablingInterface/CMAparameters.h"
#include "MuonCablingTools/dbline.h"


class CMAprogram
{
    private:

    bool m_isnewcab;

    bool m_status;

    bool m_threshold_registers[3][CMAparameters::pivot_channels]
                                 [CMAparameters::confirm_channels];

    uint32_t m_program_bytes [3][CMAparameters::pivot_channels][2];

    uint32_t m_overlap1;
    uint32_t m_overlap2;

    uint32_t m_trig_local_direc_i;
    uint32_t m_trig_local_direc_j;
    uint32_t m_trig_k_readout;

    uint32_t m_main_control;
    uint8_t  m_main_count;
    uint8_t  m_main_status;
    
    uint32_t m_pipe_i0_mask0_in;
    uint32_t m_pipe_i0_edge;
    uint16_t m_pipe_i0_ipb_regdepth;
    uint32_t m_pipe_i0_shape;
    uint32_t m_pipe_i0_mask0_readout;
    uint32_t m_pipe_i0_mask0_trig;
    
    uint32_t m_pipe_i1_mask0_in;
    uint32_t m_pipe_i1_edge;
    uint16_t m_pipe_i1_ipb_regdepth;
    uint32_t m_pipe_i1_shape;
    uint32_t m_pipe_i1_mask0_readout;
    uint32_t m_pipe_i1_mask0_trig;
    
    uint64_t m_pipe_j0_mask0_in;
    uint64_t m_pipe_j0_edge;
    uint32_t m_pipe_j0_ipb_regdepth;
    uint64_t m_pipe_j0_shape;
    uint64_t m_pipe_j0_mask0_readout;
    uint64_t m_pipe_j0_mask0_trig;
    
    uint64_t m_pipe_j1_mask0_in;
    uint64_t m_pipe_j1_edge;
    uint32_t m_pipe_j1_ipb_regdepth;
    uint64_t m_pipe_j1_shape;
    uint64_t m_pipe_j1_mask0_readout;
    uint64_t m_pipe_j1_mask0_trig;

    uint8_t  m_trig_thr0_maj_reg;
    uint32_t m_trig_thr0_mask_1_i_ge1_reg;
    uint32_t m_trig_thr0_mask_1_i_eq2_reg;
    uint64_t m_trig_thr0_mask_1_j_ge1_reg;
    uint64_t m_trig_thr0_mask_1_j_eq2_reg;

    uint8_t  m_trig_thr1_maj_reg;
    uint32_t m_trig_thr1_mask_1_i_ge1_reg;
    uint32_t m_trig_thr1_mask_1_i_eq2_reg;
    uint64_t m_trig_thr1_mask_1_j_ge1_reg;
    uint64_t m_trig_thr1_mask_1_j_eq2_reg;

    uint8_t  m_trig_thr2_maj_reg;
    uint32_t m_trig_thr2_mask_1_i_ge1_reg;
    uint32_t m_trig_thr2_mask_1_i_eq2_reg;
    uint64_t m_trig_thr2_mask_1_j_ge1_reg;
    uint64_t m_trig_thr2_mask_1_j_eq2_reg;
    
    uint8_t  m_trig_declu_i0_clsize;
    uint8_t  m_trig_declu_i1_clsize;
    uint8_t  m_trig_declu_j0_clsize;
    uint8_t  m_trig_declu_j1_clsize;
    
    uint8_t  m_trig_prp_i_eta_reg;
    uint8_t  m_trig_prp_j_eta_reg;
    
    uint32_t m_trig_shape_k_reg;
    uint32_t m_trig_ovl_sx_reg;
    uint32_t m_trig_ovl_dx_reg;
    uint64_t m_trig_edge_reg;
    
    uint16_t m_readout_buffer_empty;
    uint16_t m_readout_buffer_almost_empty;
    uint16_t m_readout_buffer_half_full;
    uint16_t m_readout_buffer_almost_full;
    uint16_t m_readout_buffer_full;
    uint8_t  m_readout_buffer_latreg;
    uint8_t  m_readout_buffer_hireg;
    uint8_t  m_readout_buffer_lowreg;
    uint8_t  m_readout_serializer_dslink;
    
    uint16_t m_l1c_pre;
    uint16_t m_bcc_pre;


    bool read(DBline&);
    bool read_v02(DBline&);
    void copy_threshold(const bool*);
    void copy_program(const uint32_t*);

    public:
    CMAprogram();
    CMAprogram(std::ifstream&);
    CMAprogram(std::ifstream&, bool);
    CMAprogram(std::istringstream&);
    CMAprogram(std::istringstream&, bool);
    CMAprogram(const CMAprogram&);
    ~CMAprogram();

    CMAprogram& operator=(const CMAprogram&);

    const bool* registers(void) const 
        {return reinterpret_cast<const bool*>(m_threshold_registers);}
    const uint32_t* bytes(void) const 
        {return reinterpret_cast<const uint32_t*>(m_program_bytes);}

    bool check(void) const {return m_status;}
    uint32_t overlap1 (void) const {return m_overlap1;}
    uint32_t overlap2 (void) const {return m_overlap2;} 
    uint32_t  local_direc_i(void) const {return m_trig_local_direc_i;}
    uint32_t  local_direc_j(void) const {return m_trig_local_direc_j;}    
    uint32_t  k_readout(void)     const {return m_trig_k_readout;}
    
    uint32_t main_control()       const ;
    inline uint8_t  main_count()  const;
    inline uint8_t  main_status() const;

    uint32_t pipe_i0_mask0_in()      const;
    uint32_t pipe_i0_edge()          const;
    uint16_t pipe_i0_ipb_regdepth()  const;
    uint32_t pipe_i0_shape()         const;
    uint32_t pipe_i0_mask0_readout() const;
    uint32_t pipe_i0_mask0_trig()    const;

    uint32_t pipe_i1_mask0_in()      const;
    uint32_t pipe_i1_edge()          const;
    uint16_t pipe_i1_ipb_regdepth()  const;
    uint32_t pipe_i1_shape()         const;
    uint32_t pipe_i1_mask0_readout() const;
    uint32_t pipe_i1_mask0_trig()    const;

    uint64_t pipe_j0_mask0_in()      const;
    uint64_t pipe_j0_edge()          const;
    uint32_t pipe_j0_ipb_regdepth()  const;
    uint64_t pipe_j0_shape()         const;
    uint64_t pipe_j0_mask0_readout() const;
    uint64_t pipe_j0_mask0_trig()    const;

    uint64_t pipe_j1_mask0_in()      const;
    uint64_t pipe_j1_edge()          const;
    uint32_t pipe_j1_ipb_regdepth()  const;
    uint64_t pipe_j1_shape()         const;
    uint64_t pipe_j1_mask0_readout() const;
    uint64_t pipe_j1_mask0_trig()    const;

    uint8_t  trig_thr0_maj_reg()          const;
    uint32_t trig_thr0_mask_1_i_ge1_reg() const;
    uint32_t trig_thr0_mask_1_i_eq2_reg() const;
    uint64_t trig_thr0_mask_1_j_ge1_reg() const;
    uint64_t trig_thr0_mask_1_j_eq2_reg() const;

    uint8_t  trig_thr1_maj_reg()          const;
    uint32_t trig_thr1_mask_1_i_ge1_reg() const;
    uint32_t trig_thr1_mask_1_i_eq2_reg() const;
    uint64_t trig_thr1_mask_1_j_ge1_reg() const;
    uint64_t trig_thr1_mask_1_j_eq2_reg() const;

    uint8_t  trig_thr2_maj_reg()          const;
    uint32_t trig_thr2_mask_1_i_ge1_reg() const;
    uint32_t trig_thr2_mask_1_i_eq2_reg() const;
    uint64_t trig_thr2_mask_1_j_ge1_reg() const;
    uint64_t trig_thr2_mask_1_j_eq2_reg() const;

    uint8_t  trig_declu_i0_clsize() const;
    uint8_t  trig_declu_i1_clsize() const;
    uint8_t  trig_declu_j0_clsize() const;
    uint8_t  trig_declu_j1_clsize() const;

    uint8_t  trig_prp_i_eta_reg() const;
    uint8_t  trig_prp_j_eta_reg() const;

    uint32_t trig_shape_k_reg() const;
    uint32_t trig_ovl_sx_reg()  const;
    uint32_t trig_ovl_dx_reg()  const;
    uint64_t trig_edge_reg()    const;

    uint16_t readout_buffer_empty()        const;
    uint16_t readout_buffer_almost_empty() const;
    uint16_t readout_buffer_half_full()    const;
    uint16_t readout_buffer_almost_full()  const;
    uint16_t readout_buffer_full()         const;
    uint8_t  readout_buffer_latreg()       const;
    uint8_t  readout_buffer_hireg()        const;
    uint8_t  readout_buffer_lowreg()       const;
    uint8_t  readout_serializer_dslink()   const;

    uint16_t l1c_pre() const;
    uint16_t bcc_pre() const;

    void open_threshold(int th);
    void copy_over(int,int);

    bool hasProgrammed(unsigned int th) const;

    friend std::ostream& operator<<(std::ostream&,const CMAprogram&);
};


inline uint32_t CMAprogram::main_control(void) const { return m_main_control; }
inline uint8_t  CMAprogram::main_count(void)   const { return m_main_count; }
inline uint8_t  CMAprogram::main_status(void)  const { return m_main_status; }

inline uint32_t CMAprogram::pipe_i0_mask0_in(void)      const { return m_pipe_i0_mask0_in; }
inline uint32_t CMAprogram::pipe_i0_edge(void)          const { return m_pipe_i0_edge; }
inline uint16_t CMAprogram::pipe_i0_ipb_regdepth(void)  const { return m_pipe_i0_ipb_regdepth; }
inline uint32_t CMAprogram::pipe_i0_shape(void)         const { return m_pipe_i0_shape; }
inline uint32_t CMAprogram::pipe_i0_mask0_readout(void) const { return m_pipe_i0_mask0_readout; }
inline uint32_t CMAprogram::pipe_i0_mask0_trig(void)    const { return m_pipe_i0_mask0_trig; }

inline uint32_t CMAprogram::pipe_i1_mask0_in(void)      const { return m_pipe_i1_mask0_in; }
inline uint32_t CMAprogram::pipe_i1_edge(void)          const { return m_pipe_i1_edge; }
inline uint16_t CMAprogram::pipe_i1_ipb_regdepth(void)  const { return m_pipe_i1_ipb_regdepth; }
inline uint32_t CMAprogram::pipe_i1_shape(void)         const { return m_pipe_i1_shape; }
inline uint32_t CMAprogram::pipe_i1_mask0_readout(void) const { return m_pipe_i1_mask0_readout; }
inline uint32_t CMAprogram::pipe_i1_mask0_trig(void)    const { return m_pipe_i1_mask0_trig; }

inline uint64_t CMAprogram::pipe_j0_mask0_in(void)      const { return m_pipe_j0_mask0_in; }
inline uint64_t CMAprogram::pipe_j0_edge(void)          const { return m_pipe_j0_edge; }
inline uint32_t CMAprogram::pipe_j0_ipb_regdepth(void)  const { return m_pipe_j0_ipb_regdepth; }
inline uint64_t CMAprogram::pipe_j0_shape(void)         const { return m_pipe_j0_shape; }
inline uint64_t CMAprogram::pipe_j0_mask0_readout(void) const { return m_pipe_j0_mask0_readout; }
inline uint64_t CMAprogram::pipe_j0_mask0_trig(void)    const { return m_pipe_j0_mask0_trig; }

inline uint64_t CMAprogram::pipe_j1_mask0_in(void)      const { return m_pipe_j1_mask0_in; }
inline uint64_t CMAprogram::pipe_j1_edge(void)          const { return m_pipe_j1_edge; }
inline uint32_t CMAprogram::pipe_j1_ipb_regdepth(void)  const { return m_pipe_j1_ipb_regdepth; }
inline uint64_t CMAprogram::pipe_j1_shape(void)         const { return m_pipe_j1_shape; }
inline uint64_t CMAprogram::pipe_j1_mask0_readout(void) const { return m_pipe_j1_mask0_readout; }
inline uint64_t CMAprogram::pipe_j1_mask0_trig(void)    const { return m_pipe_j1_mask0_trig; }

inline uint8_t  CMAprogram::trig_thr0_maj_reg(void)          const { return m_trig_thr0_maj_reg; }
inline uint32_t CMAprogram::trig_thr0_mask_1_i_ge1_reg(void) const { return m_trig_thr0_mask_1_i_ge1_reg; }
inline uint32_t CMAprogram::trig_thr0_mask_1_i_eq2_reg(void) const { return m_trig_thr0_mask_1_i_eq2_reg; }
inline uint64_t CMAprogram::trig_thr0_mask_1_j_ge1_reg(void) const { return m_trig_thr0_mask_1_j_ge1_reg; }
inline uint64_t CMAprogram::trig_thr0_mask_1_j_eq2_reg(void) const { return m_trig_thr0_mask_1_j_eq2_reg; }

inline uint8_t  CMAprogram::trig_thr1_maj_reg(void)          const { return m_trig_thr1_maj_reg; }
inline uint32_t CMAprogram::trig_thr1_mask_1_i_ge1_reg(void) const { return m_trig_thr1_mask_1_i_ge1_reg; }
inline uint32_t CMAprogram::trig_thr1_mask_1_i_eq2_reg(void) const { return m_trig_thr1_mask_1_i_eq2_reg; }
inline uint64_t CMAprogram::trig_thr1_mask_1_j_ge1_reg(void) const { return m_trig_thr1_mask_1_j_ge1_reg; }
inline uint64_t CMAprogram::trig_thr1_mask_1_j_eq2_reg(void) const { return m_trig_thr1_mask_1_j_eq2_reg; }

inline uint8_t  CMAprogram::trig_thr2_maj_reg(void)          const { return m_trig_thr2_maj_reg; }
inline uint32_t CMAprogram::trig_thr2_mask_1_i_ge1_reg(void) const { return m_trig_thr2_mask_1_i_ge1_reg; }
inline uint32_t CMAprogram::trig_thr2_mask_1_i_eq2_reg(void) const { return m_trig_thr2_mask_1_i_eq2_reg; }
inline uint64_t CMAprogram::trig_thr2_mask_1_j_ge1_reg(void) const { return m_trig_thr2_mask_1_j_ge1_reg; }
inline uint64_t CMAprogram::trig_thr2_mask_1_j_eq2_reg(void) const { return m_trig_thr2_mask_1_j_eq2_reg; }

inline uint8_t  CMAprogram::trig_declu_i0_clsize(void) const { return m_trig_declu_i0_clsize; }
inline uint8_t  CMAprogram::trig_declu_i1_clsize(void) const { return m_trig_declu_i1_clsize; }
inline uint8_t  CMAprogram::trig_declu_j0_clsize(void) const { return m_trig_declu_j0_clsize; }
inline uint8_t  CMAprogram::trig_declu_j1_clsize(void) const { return m_trig_declu_j1_clsize; }

inline uint8_t  CMAprogram::trig_prp_i_eta_reg(void) const { return m_trig_prp_i_eta_reg; }
inline uint8_t  CMAprogram::trig_prp_j_eta_reg(void) const { return m_trig_prp_j_eta_reg; }

inline uint32_t CMAprogram::trig_shape_k_reg(void) const { return m_trig_shape_k_reg; }
inline uint32_t CMAprogram::trig_ovl_sx_reg(void)  const { return m_trig_ovl_sx_reg; }
inline uint32_t CMAprogram::trig_ovl_dx_reg(void)  const { return m_trig_ovl_dx_reg; }
inline uint64_t CMAprogram::trig_edge_reg(void)    const { return m_trig_edge_reg; }

inline uint16_t CMAprogram::readout_buffer_empty(void)        const { return m_readout_buffer_empty; }
inline uint16_t CMAprogram::readout_buffer_almost_empty(void) const { return m_readout_buffer_almost_empty; }
inline uint16_t CMAprogram::readout_buffer_half_full(void)    const { return m_readout_buffer_half_full; }
inline uint16_t CMAprogram::readout_buffer_almost_full(void)  const { return m_readout_buffer_almost_full; }
inline uint16_t CMAprogram::readout_buffer_full(void)         const { return m_readout_buffer_full; }
inline uint8_t  CMAprogram::readout_buffer_latreg(void)       const { return m_readout_buffer_latreg; }
inline uint8_t  CMAprogram::readout_buffer_hireg(void)        const { return m_readout_buffer_hireg; }
inline uint8_t  CMAprogram::readout_buffer_lowreg(void)       const { return m_readout_buffer_lowreg; }
inline uint8_t  CMAprogram::readout_serializer_dslink(void)   const { return m_readout_serializer_dslink; }

inline uint16_t CMAprogram::l1c_pre(void) const { return m_l1c_pre; }
inline uint16_t CMAprogram::bcc_pre(void) const { return m_bcc_pre; }

#endif
