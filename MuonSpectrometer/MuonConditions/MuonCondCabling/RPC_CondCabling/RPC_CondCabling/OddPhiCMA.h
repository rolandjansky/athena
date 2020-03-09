/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ODDPHICMA_H
#define ODDPHICMA_H

#include <map>
#include <algorithm>
#include "RPC_CondCabling/CMAparameters.h"
#include "RPC_CondCabling/WiredOR.h"

class IMessageSvc;

namespace RPC_CondCabling {

class SectorLogicSetup;

class OddPhiCMA : public CMAparameters
{
     private:
     typedef std::map < int,WiredOR*,std::less < int > > WORlink;
 
     WORlink m_pivot_WORs;
     WORlink m_lowPt_WORs;
     WORlink m_highPt_WORs;     

     bool m_inversion;  

     bool cable_CMA_channels(void);
     bool connect(SectorLogicSetup&);
     bool doInversion(SectorLogicSetup&);
     void get_confirm_strip_boundaries(int,int);
     int  get_max_strip_readout(int);
     bool m_debug;
     bool m_verbose;
     IMessageSvc* m_msgSvc;

     public:
     OddPhiCMA(int,int,int,CMAcoverage,int,int,int,int,int,int,int,
               int,int,int,int);
     OddPhiCMA(const OddPhiCMA&);
     ~OddPhiCMA();

     OddPhiCMA& operator =(const OddPhiCMA&);

     const WORlink& pivot_WORs(void)  const {return m_pivot_WORs;}
     const WORlink& lowPt_WORs(void)  const {return m_lowPt_WORs;}
     const WORlink& highPt_WORs(void) const {return m_highPt_WORs;}

     bool inversion(void)             const { return m_inversion;}

     bool setup(SectorLogicSetup&);
};

}
#endif
