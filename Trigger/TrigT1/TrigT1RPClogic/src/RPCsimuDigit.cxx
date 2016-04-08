/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/RPCsimuDigit.h"
#include "TrigT1RPClogic/RPCsimuData.h"
/*
RPCsimuDigit::RPCsimuDigit(int number,unsigned int code,int parameters[3],
                     float vector[4],const RPCGeometry* rpcgeo) :
                     RPCdigit("Simulated RPC digit",number,code,vector,rpcgeo),
                     m_particle_code(parameters[0]),
                     m_process_type(parameters[1]),
                     m_track_number(parameters[2]),
                     m_data(0)
{
    //Insert code for xyz, time and radius decoding
    //Match the xyz decoding with the simulated xyz position
}
*/

RPCsimuDigit::RPCsimuDigit(int number,unsigned int code,int parameters[3],
                     float vector[4]) :
                     RPCdigit("Simulated RPC digit",number,code,vector),
                     m_particle_code(parameters[0]),
                     m_process_type(parameters[1]),
                     m_track_number(parameters[2]),
                     m_data(0)
{
    //Insert code for xyz, time and radius decoding
    //Match the xyz decoding with the simulated xyz position
}

RPCsimuDigit::RPCsimuDigit(const RPCsimuDigit& digit) : RPCdigit(digit)
{
    m_particle_code = digit.particle_code();
    m_process_type  = digit.process_type();
    m_track_number  = digit.track_number();
    m_data          = digit.data();
}

RPCsimuDigit
RPCsimuDigit::operator=(const RPCsimuDigit& digit)
{
    static_cast<RPCdigit&>(*this) = static_cast<const RPCdigit&>(digit);
    m_particle_code = digit.particle_code();
    m_process_type  = digit.process_type();
    m_track_number  = digit.track_number();
    m_data          = digit.data();
    return *this;
}

const MuonSimuTrack*
RPCsimuDigit::muon() const
{
    const RPCsimuData::MuonList& muons = m_data->muon_tracks();
    RPCsimuData::MuonList::const_iterator it = muons.begin();
    while(it != muons.end())
    {
	if((*it).track_number() == m_track_number) return &(*it);
    }
    return 0;
}

void
RPCsimuDigit::set_data(const RPCsimuData& data)
{
    m_data = &data;
}
