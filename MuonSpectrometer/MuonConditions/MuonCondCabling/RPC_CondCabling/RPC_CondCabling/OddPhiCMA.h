/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ODDPHICMA_H
#define ODDPHICMA_H

#include <algorithm>
#include <map>

#include "RPC_CondCabling/CMAparameters.h"
#include "RPC_CondCabling/WiredOR.h"

class MsgStream;

namespace RPC_CondCabling {

    class SectorLogicSetup;

    class OddPhiCMA : public CMAparameters {
    private:
        typedef std::map<int, WiredOR*, std::less<int> > WORlink;

        WORlink m_pivot_WORs;
        WORlink m_lowPt_WORs;
        WORlink m_highPt_WORs;

        bool m_inversion{false};

        bool cable_CMA_channels(void);
        bool connect(SectorLogicSetup&);
        bool doInversion(SectorLogicSetup&);
        void get_confirm_strip_boundaries(int, int);
        int get_max_strip_readout(int);

    public:
        OddPhiCMA(parseParams parse);
        OddPhiCMA(const OddPhiCMA&);
        virtual ~OddPhiCMA();

        OddPhiCMA& operator=(const OddPhiCMA&);

        const WORlink& pivot_WORs(void) const;
        const WORlink& lowPt_WORs(void) const;
        const WORlink& highPt_WORs(void) const;

        bool inversion(void) const;

        bool setup(SectorLogicSetup&, MsgStream&);
    };

}  // namespace RPC_CondCabling
#endif
