/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef KINEHEPMCMAP_H
#define KINEHEPMCMAP_H

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

class KineHepMcmap
{
public:
    KineHepMcmap(const HepMC::GenEvent* evt);
	
    int  			giveParticle_getkine	( const HepMC::GenParticle* p ) const;
    HepMC::GenParticle*  	givekine_getParticle	( const int ikine ) const;
private:
    const HepMC::GenEvent*	m_evt;
    //const int	m_vertex_offset;
    const int 	m_kine_offset;
    //const int 	m_mbias_int_kine_offset;
    //const int 	m_mbias_noint_kine_offset;
};


#endif  // KINEHEPMCMAP_H



