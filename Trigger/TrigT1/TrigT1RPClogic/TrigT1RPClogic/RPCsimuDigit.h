/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMUDIGIT_H
#define SIMUDIGIT_H

#include <string>

#include "TrigT1RPClogic/RPCdigit.h"
#include "TrigT1RPClogic/MuonSimuTrack.h"

class RPCsimuData;

class RPCsimuDigit: public RPCdigit
{ 
    private:

    int m_particle_code;
    int m_process_type;
    int m_track_number;

    const RPCsimuData* m_data;

    public:
    RPCsimuDigit(int,unsigned int, int [3],float [4]);
    //    RPCsimuDigit(int,unsigned int, int [3],float [4],const RPCGeometry*);
    RPCsimuDigit(const RPCsimuDigit&);
    ~RPCsimuDigit() {}

    RPCsimuDigit operator=(const RPCsimuDigit&);

    void set_data(const RPCsimuData&);

    int particle_code(void) const {return m_particle_code;}
    int process_type(void)  const {return m_process_type;}
    int track_number(void)  const {return m_track_number;}

    const RPCsimuData* data(void) const {return m_data;} 

    const MuonSimuTrack* muon(void) const; 
};

#endif
