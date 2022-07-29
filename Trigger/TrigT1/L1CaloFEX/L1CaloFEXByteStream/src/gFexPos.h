/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef L1CALOFEXBYTESTREAM_GFEXPOS_H
#define L1CALOFEXBYTESTREAM_GFEXPOS_H

#include <cstdint>

namespace LVL1::gFEXPos {
//The following definitions reflect the position of the 32-bits words in the gFEX bytestream
        constexpr std::array<unsigned int, 3> START_JET_TOB           = {  1, 16, 31 };//position of word in bytestream of the start of the Jet TOB dataframe
        constexpr std::array<unsigned int, 3> START_GLOBAL_TOB        = { 46, 61, 76 };//position of word in bytestream of the start of the MET TOB dataframe

        constexpr std::array<unsigned int, 2> TRAILER_POSITION        = { 7, 14 };//position of trailer in Jet/Global TOB dataframe, relative to start of TOB 
        constexpr std::array<unsigned int, 5> JET_UNUSED_POSITION     = { 1, 5, 6, 12, 13 };//position of unused word in Jet TOB dataframe, relative to start of TOB 
        constexpr std::array<unsigned int, 4> GLOBAL_UNUSED_POSITION  = { 5, 6, 12, 13 };//position of unused word in Global TOB dataframe, relative to start of TOB 

        constexpr std::array<unsigned int, 4> GBLOCK_POSITION         = { 2, 3, 9, 10 };//position of gBlocks word in Jet TOB dataframe, relative to dataframe start
        constexpr std::array<unsigned int, 2> GJET_POSITION           = { 4, 11 };//position of gBlocks word in Jet TOB dataframe, relative to dataframe start
        constexpr unsigned int GRHO_POSITION                          = 8 ;//position of gRho word in Jet TOB dataframe, relative to dataframe start
 
        constexpr unsigned int JWOJ_MHT_POSITION    = 1 ;//position of JwoJ MHT word in Global TOB dataframe, relative to dataframe start
        constexpr unsigned int JWOJ_MST_POSITION    = 2 ;//position of JwoJ MST word in Global TOB dataframe, relative to dataframe start
        constexpr unsigned int JWOJ_MET_POSITION    = 3 ;//position of JwoJ MET word in Global TOB dataframe, relative to dataframe start
        constexpr unsigned int JWOJ_SCALAR_POSITION = 4 ;//position of JwoJ Scalar word in Global TOB dataframe, relative to dataframe start

        constexpr unsigned int NC_MET_POSITION      = 8 ;//position of Noise Cut MET word in Global TOB dataframe, relative to dataframe start
        constexpr unsigned int NC_SCALAR_POSITION   = 10;//position of Noise Cut Scalar word in Global TOB dataframe, relative to dataframe start
 
        constexpr unsigned int RMS_MET_POSITION     = 9 ;//position of Rho+RMS MET word in Global TOB dataframe, relative to dataframe start
        constexpr unsigned int RMS_SCALAR_POSITION  = 11;//position of Rho+RMS Scalar word in Global TOB dataframe, relative to dataframe start


} // namespace LVL1::gFEXPos

#endif // L1CALOFEXBYTESTREAM_GFEXPOS_H
