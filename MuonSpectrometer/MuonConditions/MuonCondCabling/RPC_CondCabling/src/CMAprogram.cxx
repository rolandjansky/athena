/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_CondCabling/CMAprogram.h"

#include <string>

CMAprogram::CMAprogram() {

    // initialize multi-dimensional arrays:
    std::fill_n(reinterpret_cast<uint32_t*>(m_program_bytes), 
                3 * CMAparameters::pivot_channels * 2, 
                0xffffffff);

    std::fill_n(reinterpret_cast<bool*>(m_threshold_registers), 
                3 * CMAparameters::pivot_channels * CMAparameters::confirm_channels, 
                true);
}

CMAprogram::CMAprogram(std::ifstream& file, bool NewCab) :
    CMAprogram() {

    m_isnewcab = NewCab;
    DBline data(file);
    m_status = read_v02(data);
}

CMAprogram::CMAprogram(std::istringstream& filestr, bool NewCab) :
    CMAprogram() {

    m_isnewcab = NewCab;
    DBline data(filestr);
    m_status = read_v02(data);
}

bool CMAprogram::read_v02(DBline& data) {
    uint32_t right_bit = 0x1;
    std::string ignore;

    ++data;

    while (data) {
        data("set_main_control") >> data.dbhex() >> m_main_control >> data.dbdec();
        data("set_main_count") >> data.dbhex() >> m_main_count >> data.dbdec();
        data("set_main_status") >> data.dbhex() >> m_main_status >> data.dbdec();

        data("set_pipe_i0_mask0_in") >> data.dbhex() >> m_pipe_i0_mask0_in >> data.dbdec();
        data("set_pipe_i0_edge") >> data.dbhex() >> m_pipe_i0_edge >> data.dbdec();
        data("set_pipe_i0_ipb_regdepth") >> data.dbhex() >> m_pipe_i0_ipb_regdepth >> data.dbdec();
        data("set_pipe_i0_shape") >> data.dbhex() >> m_pipe_i0_shape >> data.dbdec();
        data("set_pipe_i0_mask0_readout") >> data.dbhex() >> m_pipe_i0_mask0_readout >> data.dbdec();
        data("set_pipe_i0_mask0_trig") >> data.dbhex() >> m_pipe_i0_mask0_trig >> data.dbdec();

        data("set_pipe_i1_mask0_in") >> data.dbhex() >> m_pipe_i1_mask0_in >> data.dbdec();
        data("set_pipe_i1_edge") >> data.dbhex() >> m_pipe_i1_edge >> data.dbdec();
        data("set_pipe_i1_ipb_regdepth") >> data.dbhex() >> m_pipe_i1_ipb_regdepth >> data.dbdec();
        data("set_pipe_i1_shape") >> data.dbhex() >> m_pipe_i1_shape >> data.dbdec();
        data("set_pipe_i1_mask0_readout") >> data.dbhex() >> m_pipe_i1_mask0_readout >> data.dbdec();
        data("set_pipe_i1_mask0_trig") >> data.dbhex() >> m_pipe_i1_mask0_trig >> data.dbdec();

        data("set_pipe_j0_mask0_in") >> data.dbhex() >> m_pipe_j0_mask0_in >> data.dbdec();
        data("set_pipe_j0_edge") >> data.dbhex() >> m_pipe_j0_edge >> data.dbdec();
        data("set_pipe_j0_ipb_regdepth") >> data.dbhex() >> m_pipe_j0_ipb_regdepth >> data.dbdec();
        data("set_pipe_j0_shape") >> data.dbhex() >> m_pipe_j0_shape >> data.dbdec();
        data("set_pipe_j0_mask0_readout") >> data.dbhex() >> m_pipe_j0_mask0_readout >> data.dbdec();
        data("set_pipe_j0_mask0_trig") >> data.dbhex() >> m_pipe_j0_mask0_trig >> data.dbdec();

        data("set_pipe_j1_mask0_in") >> data.dbhex() >> m_pipe_j1_mask0_in >> data.dbdec();
        data("set_pipe_j1_edge") >> data.dbhex() >> m_pipe_j1_edge >> data.dbdec();
        data("set_pipe_j1_ipb_regdepth") >> data.dbhex() >> m_pipe_j1_ipb_regdepth >> data.dbdec();
        data("set_pipe_j1_shape") >> data.dbhex() >> m_pipe_j1_shape >> data.dbdec();
        data("set_pipe_j1_mask0_readout") >> data.dbhex() >> m_pipe_j1_mask0_readout >> data.dbdec();
        data("set_pipe_j1_mask0_trig") >> data.dbhex() >> m_pipe_j1_mask0_trig >> data.dbdec();

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

        data("set_trig_prp_i_eta_reg") >> data.dbhex() >> m_trig_local_direc_i >> data.dbdec();
        data("set_trig_prp_j_eta_reg") >> data.dbhex() >> m_trig_local_direc_j >> data.dbdec();
        data("set_trig_shape_k_reg") >> data.dbhex() >> m_trig_shape_k_reg >> data.dbdec();
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

        if (m_isnewcab) {
            // case RPC_CondCabling
            if (data("set_trig_thr0_thr_reg_00")) {
                for (int i = 0; i < 32; ++i) {
                    ignore.clear();
                    if (i == 0)
                        data >> data.dbhex() >> twowords >> data.dbdec();
                    else
                        data >> ignore >> data.dbhex() >> twowords >> data.dbdec();

                    union Data {
                        uint64_t bits;
                        uint32_t words[2];
                    } dataun{};

                    dataun.bits = twowords;

                    m_program_bytes[0][i][0] = dataun.words[0];
                    m_program_bytes[0][i][1] = dataun.words[1];

                    for (int bit = 0; bit < 32; ++bit) {
                        m_threshold_registers[0][i][bit] = ((dataun.words[0] >> bit) & right_bit) != 0;
                        m_threshold_registers[0][i][bit + 32] = ((dataun.words[1] >> bit) & right_bit) != 0;
                    }
                    if (i < 31) ++data;
                }
            }

            if (data("set_trig_thr1_thr_reg_00")) {
                ignore.clear();
                for (int i = 0; i < 32; ++i) {
                    if (i == 0)
                        data >> data.dbhex() >> twowords >> data.dbdec();
                    else
                        data >> ignore >> data.dbhex() >> twowords >> data.dbdec();

                    union Data {
                        uint64_t bits;
                        uint32_t words[2];
                    } dataun{};

                    dataun.bits = twowords;

                    m_program_bytes[1][i][0] = dataun.words[0];
                    m_program_bytes[1][i][1] = dataun.words[1];

                    for (int bit = 0; bit < 32; ++bit) {
                        m_threshold_registers[1][i][bit] = ((dataun.words[0] >> bit) & right_bit) != 0;
                        m_threshold_registers[1][i][bit + 32] = ((dataun.words[1] >> bit) & right_bit) != 0;
                    }

                    if (i < 31) ++data;
                }
            }

            if (data("set_trig_thr2_thr_reg_00")) {
                ignore.clear();
                for (int i = 0; i < 32; ++i) {
                    if (i == 0)
                        data >> data.dbhex() >> twowords >> data.dbdec();
                    else
                        data >> ignore >> data.dbhex() >> twowords >> data.dbdec();

                    union Data {
                        uint64_t bits;
                        uint32_t words[2];
                    } dataun{};

                    dataun.bits = twowords;

                    m_program_bytes[2][i][0] = dataun.words[0];
                    m_program_bytes[2][i][1] = dataun.words[1];

                    for (int bit = 0; bit < 32; ++bit) {
                        m_threshold_registers[2][i][bit] = ((dataun.words[0] >> bit) & right_bit) != 0;
                        m_threshold_registers[2][i][bit + 32] = ((dataun.words[1] >> bit) & right_bit) != 0;
                    }

                    if (i < 31) ++data;
                }
            }
            ++data;
        }

        else {
            // case RPCcablingSim
            // old format ///////////////////////////////////////////////////////
            for (int i = 1; i <= 3; ++i) {
                int majority;

                if (data("th #", i) && data("maj_reg") >> majority) {
                    int th = 0;
                    int ch = 0;
                    uint32_t first_word = 0;
                    uint32_t second_word = 0;

                    ++data;

                    while (data("th") >> th >> "thr_reg" >> ch >> data.dbhex() >> first_word >> second_word >> data.dbdec()) {
                        if (th < 1) return false;
                        m_program_bytes[th - 1][ch][1] = first_word;
                        m_program_bytes[th - 1][ch][0] = second_word;

                        for (int bit = 0; bit < 32; ++bit) {
                            m_threshold_registers[th - 1][ch][bit] = ((second_word >> bit) & right_bit) != 0;
                            m_threshold_registers[th - 1][ch][bit + 32] = ((first_word >> bit) & right_bit) != 0;
                        }

                        ++data;
                    }
                }
            }

            data("overlap1") >> data.dbhex() >> m_overlap1 >> data.dbdec();
            data("overlap2") >> data.dbhex() >> m_overlap2 >> data.dbdec();
            data("trig_local_direc_i") >> m_trig_local_direc_i;
            data("trig_local_direc_j") >> m_trig_local_direc_j;
            data("trig_k_readout") >> m_trig_k_readout;

            ++data;
        }
    }

    return true;
}

void CMAprogram::open_threshold(int th) {
    if (th >= 3) return;
    for (int ch = 0; ch < CMAparameters::pivot_channels; ++ch) {
        m_program_bytes[th][ch][1] = 0xffffffff;
        m_program_bytes[th][ch][0] = 0xffffffff;

        for (int bit = 0; bit < 32; ++bit) {
            m_threshold_registers[th][ch][bit] = true;
            m_threshold_registers[th][ch][bit + 32] = true;
        }
    }
}

void CMAprogram::copy_over(int source, int dest) {
    if (source >= 3 || dest >= 3) return;
    if (source == dest) return;

    for (int ch = 0; ch < CMAparameters::pivot_channels; ++ch) {
        m_program_bytes[dest][ch][1] = m_program_bytes[source][ch][1];
        m_program_bytes[dest][ch][0] = m_program_bytes[source][ch][0];

        for (int bit = 0; bit < 32; ++bit) {
            m_threshold_registers[dest][ch][bit] = m_threshold_registers[source][ch][bit];
            m_threshold_registers[dest][ch][bit + 32] = m_threshold_registers[source][ch][bit + 32];
        }
    }
}

bool CMAprogram::hasProgrammed(unsigned int th) const {
    if (th >= 3) return false;
    for (int ch = 0; ch < CMAparameters::pivot_channels; ++ch) {
        if (m_program_bytes[th][ch][1] != 0x0) return true;
        if (m_program_bytes[th][ch][0] != 0x0) return true;
    }
    return false;
}

std::ostream& operator<<(std::ostream& stream, const CMAprogram& program) {
    unsigned int no_coincidence = 183;

    for (int piv = CMAparameters::pivot_channels - 1; piv >= 0; --piv) {
        for (int con = 0; con < CMAparameters::confirm_channels; ++con) {
            for (int th = 2; th >= 0; --th) {
                if (program.m_threshold_registers[th][piv][con]) {
                    stream << th + 1;
                    break;
                }
                if (th == 0) stream << (char)no_coincidence;
            }
        }
        stream << std::endl;
    }
    return stream;
}
