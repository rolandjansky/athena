/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef KINEHEPMCMAP_H
#define KINEHEPMCMAP_H

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

class KineHepMcmap
{
public:
    KineHepMcmap(const HepMC::GenEvent* evt);
	
    int  		                  	giveParticle_getkine	(HepMC::ConstGenParticlePtr p ) const;
    HepMC::ConstGenParticlePtr  	givekine_getParticle	( const int ikine ) const;
private:
    const HepMC::GenEvent*	m_evt;
    const int 	m_kine_offset;
};


#endif  // KINEHEPMCMAP_H



