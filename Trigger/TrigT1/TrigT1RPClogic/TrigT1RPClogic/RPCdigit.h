/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCDIGIT_H
#define RPCDIGIT_H

#include <list>

#include "TrigT1RPClogic/RPCtrigDataObject.h"
#include "MuonCablingTools/RPCdecoder.h"
//#include "RPCgeometry/RPCGeometry.h"
#include "TrigT1RPClogic/MuonSimuTrack.h"

class RPCdigit : public RPCtrigDataObject
{
    private:
    RPCdecoder m_coding;

    float m_x;
    float m_y;
    float m_z;

    float m_time;
    
    float m_station_radius;
    float m_station_phi;
    
    //const RPCGeometry* m_rpcgeo;  
    const int* m_rpcgeo;  // NEEDED TO AVOID MYSTERYOUS CRASH !, TO BE DEBUGGED AND REMOVED 

    std::list<unsigned int> m_codes;

    public:
    RPCdigit();
    RPCdigit(std::string,int,unsigned int, float vec[4]);
        
    /*
    RPCdigit(std::string,int,unsigned int, float vec[4],
             const RPCGeometry* rpcgeo);
    RPCdigit(std::string,int,std::list<unsigned int>,float, 
             const RPCGeometry* rpcgeo);
    */
    RPCdigit(const RPCdigit&);
    virtual ~RPCdigit() {}
   
    RPCdigit operator = (const RPCdigit&);
    
    bool operator==(const RPCdigit&) const;

    const RPCdecoder& decoding(void) const {return m_coding;}
    //const RPCGeometry* rpcgeo(void) const {return m_rpcgeo;}
    const std::list<unsigned int> codes(void) const {return m_codes;}

    float time (void)      const {return m_time;}
    float x(void)          const {return m_x;}
    float y(void)          const {return m_y;}
    float z(void)          const {return m_z;}

    void xyz(float vec[3]) const;
    bool radius(float&)    const;
    bool theta(float&)     const;
    bool eta(float&)       const;
    bool phi(float&)       const;

    bool station_radius(float&) const;
    bool station_eta(float&) const;
    bool station_phi(float&) const;

    
    //bool set_to_chamber(const int rpc_index);
   
    virtual int particle_code(void) const {return 0;}
    virtual int process_type(void)  const {return 0;}
    virtual int track_number(void)  const {return 0;}

    virtual const MuonSimuTrack* muon(void) const {return 0;}

    void Print(std::ostream&,bool) const;
};


inline bool
RPCdigit::station_radius(float& rad) const
{    
    if(m_station_radius)
    {
        rad = m_station_radius;
	return true;
    }
    return false;
}


inline bool
RPCdigit::station_phi(float& phi) const
{    
    if(m_station_phi >= 0.)
    {
        phi = m_station_phi;
	return true;
    }
    return false;
}




template <class X> X& operator<<(X& stream,const RPCdigit& data)
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[30000];
    for (int i=0;i<30000;++i) buffer[i] = '\0';
    __osstream display(buffer,30000);
#endif

    data.Print(display,false);
    stream << display.str();
    return stream;
}

#endif
