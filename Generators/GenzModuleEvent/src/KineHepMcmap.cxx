/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GenzModuleEvent/KineHepMcmap.h"

KineHepMcmap::KineHepMcmap ( const HepMC::GenEvent* evt )
    : m_evt			(evt),
      //m_vertex_offset		(-10000),
      m_kine_offset		(100000)
      //m_mbias_int_kine_offset  	(200000),
      //m_mbias_noint_kine_offset	(300000)
{}

int
KineHepMcmap::giveParticle_getkine	( const HepMC::GenParticle* p ) const
{
    int barcode	= p->barcode();
    if (barcode < m_kine_offset)
    {
	if (p->status() > 1000) return p->status() / 1000;
	return 0;
    }
    else
    {
	return barcode%m_kine_offset;
    }
}

HepMC::GenParticle*
KineHepMcmap::givekine_getParticle	( const int kine ) const
{
    if (kine <= 0) return 0;
    HepMC::GenParticle*	part =	0;
    bool not_found = true;
    HepMC::GenEvent::particle_const_iterator p = m_evt->particles_begin();
    do
    {
	int pkine = giveParticle_getkine(*p);
	if (pkine == kine)
	{
	    not_found = false;
	    part = *p;
	}
	++p;
    } while (p != m_evt->particles_end() && not_found);

    return part;
}



/////////////////////////////////////////////////////////////
//  How to use this ?                                      //
/////////////////////////////////////////////////////////////


//  Providing you have GenEvent* evt

//         int kine = 120;
	 
//	 HepMC::KineHepMcmap  kinm(evt )   ;
//	 HepMC::GenParticle* myparticle = kinm.givekine_getParticle( kine);
//	 int mykine = kinm.giveParticle_getkine(  myparticle);
//	 cout << "KineHepMcmap geant particle 120 =  " << mykine << endl; 
//	 if (myparticle) myparticle->print();


	 

	      

		
	  



