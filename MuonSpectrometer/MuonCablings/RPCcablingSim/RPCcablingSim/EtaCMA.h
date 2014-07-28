/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ETACMA_H
#define ETACMA_H

#include <map>
#include <algorithm>
#include "RPCcablingInterface/CMAparameters.h"
#include "RPCcablingSim/RPCchamber.h"


namespace RPCcablingSim {

class SectorLogicSetup;

class EtaCMA : public CMAparameters
{
     private:
     typedef std::map < int,RPCchamber*,std::less < int > > RPClink;
 
     RPClink m_pivot_RPCs;
     RPClink m_lowPt_RPCs;
     RPClink m_highPt_RPCs;     

     bool cable_CMA_channels(HalfType);
     bool connect(SectorLogicSetup&);
     bool got_confirm_cabling(SectorLogicSetup&,int);
     bool end_at_RPC_Z_boundary(void) const;
     bool begin_at_RPC_Z_boundary(void) const;

     public:
     EtaCMA(int,int,int,CMAcoverage,int,int,int,int,int,int,int,
            int,int,int,int);
     EtaCMA(int,int,int,int,int,int,int,int,int,int,int);
     EtaCMA(const EtaCMA&);
     ~EtaCMA();

     EtaCMA& operator =(const EtaCMA&);

     const RPClink& pivot_RPCs(void)  const {return m_pivot_RPCs;}
     const RPClink& lowPt_RPCs(void)  const {return m_lowPt_RPCs;}
     const RPClink& highPt_RPCs(void) const {return m_highPt_RPCs;}

     bool setup(SectorLogicSetup&);

};

}
#endif
