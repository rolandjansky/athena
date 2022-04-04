/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_RPCReadOut_H
#define TrigT1RPChardware_RPCReadOut_H

#include <array>
#include <iostream>

#include "TrigT1RPChardware/Lvl1Def.h"

class RPCReadOut {
public:
    RPCReadOut() = default;
    ~RPCReadOut() = default;

    ubit16 getWord() const { return m_word; };
    char field() const { return m_field; };

protected:
    // create a 16 bits word of the CMA frame structure , starting from the fields:
    // ubit16 set16Bits(const ubit16 num, const ubit16 *pos, const ubit16 *val);
    // get the fields of the 16 bits word of the CMA frame structure:
    // ubit16 get16Bits(const ubit16 num, const ubit16  pos, const ubit16  val);

    ubit16 m_word{0xffff};
    char m_field{0};

    inline ubit16 set16Bits(const ubit16 n, const ubit16 *pos, const ubit16 *val) const {
        ubit16 output = 0;
        ubit16 p = 0, v = 0;
        for (ubit16 i = 0; i < n; i++) {
            v = *(val + i);
            p = *(pos + i);
            output = output | (v << p);
        }  // end-of-for
        return output;
    }  // end-of-MatrixReadOutStructure::set16Bits
    template <size_t n> ubit16 set16Bits(const std::array<ubit16, n> &pos, const std::array<ubit16, n> &val) const {
        ubit16 output = 0;
        for (size_t i = 0; i < n; ++i) { output |= (val[i] << pos[i]); }
        return output;
    }

    //---------------------------------------------------------------------------//
    inline ubit16 get16Bits(const ubit16 input, const ubit16 position, const ubit16 length) const {
        ubit16 output = 0;
        ubit16 CMfield = 0;
        for (ubit16 i = 0; i < length; i++) { CMfield = CMfield | 1 << (position + i); }
        output = (input & CMfield) >> position;
        return output;
    }  // end-of-MatrixReadOutStructure::get16Bits

    static constexpr ubit16 s_last2bitsON{0xc000};
    static constexpr ubit16 s_last4bitsON{0xf000};
    static constexpr ubit16 s_first8bitsON{0};
};
#endif
