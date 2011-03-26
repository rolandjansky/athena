/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RPCbytestream_H
#define RPCbytestream_H 

#include <algorithm>
#include <map>
#include "TrigT1RPClogic/CMAdata.h"
#include "TrigT1RPClogic/CMAreadout.h"
#include "TrigT1RPClogic/PADreadout.h"


class RPCbytestream : public RPCtrigDataObject
{
    public:
    typedef unsigned long int debu;
    typedef std::map < int, CMAreadout, std::less <int> > CMA_Readout;
    typedef std::map < int, PADreadout, std::less <int> > PAD_Readout;

    private:
    debu m_cma_ro_debug;       // bits for debugging CMA readout
    debu m_pad_ro_debug;       // bits for debugging PAD readout
    debu m_rx_ro_debug;        // bits for debugging Rx readout
    debu m_sl_ro_debug;        // bits for debugging SL readout

    debu m_cma_rostruct_debug; // bits for debugging CMA ro struct
    debu m_pad_rostruct_debug; // bits for debugging PAD ro struct
    debu m_rx_rostruct_debug;  // bits for debugging Rx ro struct
    debu m_sl_rostruct_debug;  // bits for debugging SL ro struct 

    std::string m_filename;

    CMA_Readout m_cma_readout;
    PAD_Readout m_pad_readout;

    void build_pad_readout(void);

    void dump_rpc_bytestream(void);

    public:
    RPCbytestream(CMAdata&,std::string,debu,debu,debu,debu,debu,debu,debu,
                  debu);
    RPCbytestream(const RPCbytestream&);
    ~RPCbytestream();

    debu cma_ro_debug(void) const {return m_cma_ro_debug;}
    debu pad_ro_debug(void) const {return m_pad_ro_debug;}
    debu rx_ro_debug(void)  const {return m_rx_ro_debug;}
    debu sl_ro_debug(void)  const {return m_sl_ro_debug;}

    debu cma_rostruct_debug(void) const {return m_cma_rostruct_debug;}
    debu pad_rostruct_debug(void) const {return m_pad_rostruct_debug;}
    debu rx_rostruct_debug(void)  const {return m_rx_rostruct_debug;}
    debu sl_rostruct_debug(void)  const {return m_sl_rostruct_debug;}

    std::string filename(void) const {return m_filename;}

    CMA_Readout cma_readout(void) const {return m_cma_readout;}
    PAD_Readout pad_readout(void) const {return m_pad_readout;}
};


#endif
