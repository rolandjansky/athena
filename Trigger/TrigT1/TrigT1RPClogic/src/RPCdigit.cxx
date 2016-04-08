/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iomanip>

#include "TrigT1RPClogic/RPCdigit.h"

using namespace std;

RPCdigit::RPCdigit() :
    RPCtrigDataObject(0,"RPC digit")
{
    m_coding(0);

    m_time = 0.0;
    
    m_x = 0.0;
    m_y = 0.0;
    m_z = 0.0;
    
    m_station_radius = 0;
    m_station_phi = -1.;
    
    //m_rpcgeo = 0;
    m_codes.clear();  
}


RPCdigit::RPCdigit(std::string name,int number,unsigned int code,float vec[4]):
    RPCtrigDataObject(number,name)
{
    m_coding(code);

    m_time = vec[0];

    m_x = vec[1];
    m_y = vec[2];
    m_z = vec[3];
    
    //m_rpcgeo = 0;    
    
    m_station_radius = 0.;
    m_station_phi = -1.;

    m_codes.clear();  
}

/*
RPCdigit::RPCdigit(std::string name,int number,unsigned int code,float vec[4],
                   const RPCGeometry* rpcgeo):
    RPCtrigDataObject(number,name)
{
    while (code%100 && !rpcgeo->existCode(code)) {code -= 1;}
    m_coding(code);

    m_time = vec[0];

    m_x = vec[1];
    m_y = vec[2];
    m_z = vec[3];
    
    m_rpcgeo = rpcgeo;    
    
    if(!m_rpcgeo->give_station_radius(m_coding.code(),m_station_radius))
        m_station_radius = 0.;
    if(!m_rpcgeo->give_station_phi(m_coding.code(),m_station_phi)) 
        m_station_phi = -1.;

    m_codes.clear();  
}

RPCdigit::RPCdigit(std::string name,int number,std::list<unsigned int> code,
                   float time, const RPCGeometry* rpcgeo):
    RPCtrigDataObject(number,name)
{
    unsigned int Code = code.front();
    while (Code%100 && !rpcgeo->existCode(Code)) {Code -= 1;}
    
    m_coding(Code);

    m_time = time;
   
    m_rpcgeo = rpcgeo;
    m_codes  = code;

    float xyz[3] = {0.,0.,0.};
    m_rpcgeo->give_strip_coordinates(m_coding.code(),xyz);
    m_x = xyz[0];
    m_y = xyz[1];
    m_z = xyz[2];
    
    if(!m_rpcgeo->give_station_radius(m_coding.code(),m_station_radius))
        m_station_radius = 0.;
    if(!m_rpcgeo->give_station_phi(m_coding.code(),m_station_phi))
        m_station_phi = -1.;
}


bool
RPCdigit::set_to_chamber(const int rpc_index)
{
    if(m_coding.rpc_z_index() == rpc_index) return true;
    if(!m_rpcgeo) return false;

    std::list<unsigned int>::const_iterator it = m_codes.begin();
    
    while(it != m_codes.end())
    {
        unsigned int code = *it; 
	while (code%100 && !m_rpcgeo->existCode(code)) {code -= 1;}
        RPCdecoder tmp(code);
	if(tmp.rpc_z_index() == rpc_index)
	{
	    m_coding = tmp;
            float xyz[3] = {0.,0.,0.};
            m_rpcgeo->give_strip_coordinates(m_coding.code(),xyz);
            m_x = xyz[0];
            m_y = xyz[1];
            m_z = xyz[2];
    
            if(!m_rpcgeo->give_station_radius(m_coding.code(),m_station_radius))
                m_station_radius = 0.;
            if(!m_rpcgeo->give_station_phi(m_coding.code(),m_station_phi))
                m_station_phi = -1.;
	       
	    return true;
	}
	++it;
    }
    return false;
}
*/

RPCdigit::RPCdigit(const RPCdigit& digit) :
    RPCtrigDataObject(digit.number(),digit.name())
{
    m_coding = digit.decoding();

    m_time   = digit.time();

    m_x = digit.x();
    m_y = digit.y();
    m_z = digit.z();

    //m_rpcgeo =0 ;
    
    //m_rpcgeo = digit.rpcgeo();
    
    if(!digit.station_radius(m_station_radius)) m_station_radius = 0.;
    if(!digit.station_phi(m_station_phi)) m_station_phi = 0.;
    
    m_codes  = digit.codes();
}

RPCdigit RPCdigit::operator = (const RPCdigit& digit)
{
    static_cast<RPCtrigDataObject&>(*this) = 
        static_cast<const RPCtrigDataObject&>(digit);
    m_coding = digit.decoding();

    m_time   = digit.time();

    m_x = digit.x();
    m_y = digit.y();
    m_z = digit.z();

    //m_rpcgeo=0;
    
    //    m_rpcgeo = digit.rpcgeo();

    
    if(!digit.station_radius(m_station_radius)) m_station_radius = 0.;
    if(!digit.station_phi(m_station_phi)) m_station_phi = 0.;
   
    m_codes  = digit.codes();
    
    return *this;
}


bool
RPCdigit::operator==(const RPCdigit& digit) const
{
    if(m_coding.code() == digit.decoding().code()) return true;
    else return false;
}

void RPCdigit::xyz(float vec[3]) const
{
    vec[0] = m_x;
    vec[1] = m_y;
    vec[2] = m_z;
}

bool RPCdigit::radius(float& rad) const
{
    if(m_x != 0. || m_y != 0.)
    {
	rad = sqrt( pow(m_x,2) + pow(m_y,2) );
	return true;
    }
    return false;
}


bool RPCdigit::theta(float& theta) const
{
    float radius;
    if(this->radius(radius))
    {
        if(m_z != 0.) theta = atan( radius/ fabsf(m_z) );
        else theta = asin(1.);
        return true;
    }
    return false;
}
    
bool RPCdigit::eta(float& eta) const
{
    float theta;
    if(this->theta(theta))
    {
        eta = -log(tan(theta/2.));
        if(m_z < 0.) eta = -eta; 
        return true;
    }
    return false;
}
    
bool RPCdigit::phi(float& phi) const
{
    if(m_x != 0. || m_y != 0.)
    {
        phi = atan2(m_y,m_x);
	return true;
    }
    return false;   
}

bool RPCdigit::station_eta(float& eta) const
{
    if(m_station_radius)
    {
        if(m_z != 0.) eta = -log(tan(atan(m_station_radius/fabsf(m_z))/2.));
        if(m_z < 0. ) eta = -eta;
        return true;
    }
    return false;
}

void RPCdigit::Print(ostream& stream,bool detail) const
{
    //unsigned int upper = 19;
    //unsigned int lower = 17;

    unsigned int upper = 95;
    unsigned int lower = 45;
    

    std::string type = (decoding().view() == Eta)? "ETA " : "PHI ";

    for(int i=0;i<61;++i) stream << (char) upper;
    stream << endl;

    stream << type << name() << " number " << number() << ",  time = "
	   << time() << " ns" << endl;
    if (particle_code())
    {
	stream << "particle code = "    << particle_code() << ","
	       << " physics process = " << process_type()  << ","
	       << " track number = "    << track_number()  << endl;
    }
    stream << setiosflags(ios::fixed);

    stream << "GlobalPosition (cm): " 
        << setiosflags(ios::fixed) << setprecision(3) << setw(12) << x() << ","
	<< setiosflags(ios::fixed) << setprecision(3) << setw(12) << y() << ","
	<< setiosflags(ios::fixed) << setprecision(3) << setw(12) << z()
        << endl;

    stream.unsetf(ios::fixed);

    if(detail)
    {
        decoding().Print(stream,detail);
    }

    for(int i=0;i<61;++i) stream << (char) lower;
    stream << endl;
}
