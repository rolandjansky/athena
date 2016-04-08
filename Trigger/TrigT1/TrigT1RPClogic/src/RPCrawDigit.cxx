/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/RPCrawDigit.h"
#include "TrigT1RPClogic/RPCrawData.h"

/*
RPCrawDigit::RPCrawDigit(int number,std::list<unsigned int> code,float time,
                         const RPCGeometry* rpcgeo) :
                           RPCdigit("Raw RPC digit",number,code,time,rpcgeo),
                           m_data(0)
{
    //Insert code for xyz, time and radius decoding
    //Match the xyz decoding with the simulated xyz position
}
*/

RPCrawDigit::RPCrawDigit(int number,unsigned int code,float xyz[4])
                          : RPCdigit("Raw RPC digit",number,code,xyz),
                           m_data(0)
{

}


RPCrawDigit::RPCrawDigit(const RPCrawDigit& digit) : RPCdigit(digit)
{
    m_data          = digit.data();
}

RPCrawDigit
RPCrawDigit::operator=(const RPCrawDigit& digit)
{
    static_cast<RPCdigit&>(*this) = static_cast<const RPCdigit&>(digit);
    m_data          = digit.data();
    return *this;
}


void
RPCrawDigit::set_data(const RPCrawData& data)
{
    m_data = &data;
}
