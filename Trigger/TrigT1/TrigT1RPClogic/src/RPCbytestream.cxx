/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include <fstream>
#include "TrigT1RPClogic/RPCbytestream.h"

using namespace std;

RPCbytestream::RPCbytestream(CMAdata& data,std::string filename,MsgStream& log,
                             debu cma_debug,debu pad_debug,debu rx_debug,
                             debu sl_debug,debu cma_struc_debug,
                             debu pad_struc_debug,debu rx_struc_debug,
                             debu sl_struc_debug) :
    RPCtrigDataObject(0,"RPC byte stream"),
    m_cma_ro_debug(cma_debug),
    m_pad_ro_debug(pad_debug),
    m_rx_ro_debug(rx_debug),
    m_sl_ro_debug(sl_debug),
    m_cma_rostruct_debug(cma_struc_debug),
    m_pad_rostruct_debug(pad_struc_debug),
    m_rx_rostruct_debug(rx_struc_debug),
    m_sl_rostruct_debug(sl_struc_debug),
    m_filename(filename)
{
    CMAdata::PatternsList list = data.give_patterns();
    CMAdata::PatternsList::iterator it = list.begin();

    while(it != list.end())
    {
        int key = (*it)->sector() * 10000 +
                  (*it)->cma_parameters().id().PAD_index() * 100 +
                  (*it)->cma_parameters().id().type() * 10 +
                  (*it)->cma_parameters().id().Ixx_index();
		
        std::pair < CMA_Readout::iterator, bool> ins = 
        m_cma_readout.insert(CMA_Readout::value_type(key,CMAreadout(*it)));
        if(!ins.second)
	{
        log << MSG::ERROR << "Found duplicate CMA readout into CMA data!" << endmsg;
	}

	++it;
    }

    build_pad_readout(log);
    if(m_filename != "" ) dump_rpc_bytestream();
}

RPCbytestream::RPCbytestream(const RPCbytestream& readout) :
    RPCtrigDataObject(readout.number(),readout.name())
{
    m_filename           = readout.filename();
    m_cma_ro_debug       = readout.cma_ro_debug();
    m_pad_ro_debug       = readout.pad_ro_debug();
    m_rx_ro_debug        = readout.rx_ro_debug();
    m_sl_ro_debug        = readout.sl_ro_debug();
    m_cma_rostruct_debug = readout.cma_rostruct_debug();
    m_pad_rostruct_debug = readout.pad_rostruct_debug();
    m_rx_rostruct_debug  = readout.rx_rostruct_debug();
    m_sl_rostruct_debug  = readout.sl_rostruct_debug();

    m_cma_readout = readout.cma_readout();
    m_pad_readout = readout.pad_readout();
}


RPCbytestream::~RPCbytestream()
{
    m_cma_readout.clear();
}


void
RPCbytestream::build_pad_readout(MsgStream& log)
{
    for (int i=0;i<64;++i)
    {
        for(int j=0;j<=10;++j)   // 99 is the maximum number allowed
	{	        
            int kPAD_lw = i * 10000 + j * 100;
	    int kPAD_hg = i * 10000 + j * 100 + 99;

            CMA_Readout::iterator PAD_l = m_cma_readout.lower_bound(kPAD_lw);
            CMA_Readout::iterator PAD_h = m_cma_readout.upper_bound(kPAD_hg);

	    if(distance(PAD_l,PAD_h) )
	    {
                int SLid  = (*PAD_l).second.sector();
                int PADid = (*PAD_l).second.id().PAD_index();
                PADreadout pad_readout(SLid,PADid);

                int key = SLid * 100 + PADid;
					
                if(SLid != i)
	        {
                log << MSG::ERROR << "Key doesn't correspond to CMA Sector Id!" << endmsg;
	        }

                for (CMA_Readout::iterator it = PAD_l; it != PAD_h; ++it)
	        {
                    auto cma_readout = (*it).second.give_matrix_readout(log);
                    pad_readout.load_readout(cma_readout);
                    if (log.level() <= MSG::DEBUG) {
                        log << MSG::DEBUG << "Dump of the matrices readout into PAD " << PADid
                             << endl
                             << cma_readout[0]
                             << cma_readout[1] << endmsg;
                    }
	        }

                std::pair < PAD_Readout::iterator, bool> ins = 
                m_pad_readout.insert(PAD_Readout::value_type(key,pad_readout));
                if(!ins.second)
	        {
	            log << MSG::ERROR << "Found duplicate PAD readout into CMA data!" << endmsg;
	        }
	    }
        }
    }
}

void
RPCbytestream::dump_rpc_bytestream()
{
    ofstream out;
    out.open(m_filename.c_str(),ios::app);

    // dump the PAD chunk of data into the byte stream file
    PAD_Readout::iterator it = m_pad_readout.begin();
    while(it != m_pad_readout.end())
    {
	PadReadOut* readout = (*it).second.give_pad_readout();

        readout->bytestream(out);

        out << endl;

	++it;
    }
}
