/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PADREADOUT_H
#define PADREADOUT_H

#include "TrigT1RPClogic/RPCtrigDataObject.h"

#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/PadReadOut.h"

#include <array>
#include <memory>

class PADreadout : public RPCtrigDataObject
{
    private:
    int m_sector;
    int m_PAD;

    MatrixReadOut* m_matrices_readout[8] = {nullptr};
    std::unique_ptr<PadReadOut> m_pad_readout;

    public:
    PADreadout(int,int);
    PADreadout(const PADreadout&);

    PADreadout operator=(const PADreadout&);

    bool operator==(const PADreadout&) const;
    bool operator!=(const PADreadout&) const;
    bool operator< (const PADreadout&) const;

    void load_readout(std::array<MatrixReadOut*, 2>&);

    int sector(void) const {return m_sector;}
    int PAD(void)    const {return m_PAD;}
    MatrixReadOut* matrices_readout(int) const;
    PadReadOut* give_pad_readout(void);
};

#endif
