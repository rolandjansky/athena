/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GenzModuleEvent/KineHepMcmap.h"

KineHepMcmap::KineHepMcmap ( const HepMC::GenEvent* evt )
    : m_evt			(evt),
      m_kine_offset		(100000)
{}

int
KineHepMcmap::giveParticle_getkine	(HepMC::ConstGenParticlePtr p ) const
{
    int barcode	= HepMC::barcode(p);
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

HepMC::ConstGenParticlePtr
KineHepMcmap::givekine_getParticle	( const int kine ) const
{
    if (kine <= 0) return nullptr;
    for (auto  part: *m_evt)
    {
         if (giveParticle_getkine(part) == kine )  return part;
    }
    return  nullptr;
}



/////////////////////////////////////////////////////////////
//  How to use this ?                                      //
/////////////////////////////////////////////////////////////


//  Providing you have GenEvent* evt

//         int kine = 120;
	 
//	 HepMC::KineHepMcmap  kinm(evt )   ;
//	 HepMC::GenParticlePtr myparticle = kinm.givekine_getParticle( kine);
//	 int mykine = kinm.giveParticle_getkine(  myparticle);
//	 cout << "KineHepMcmap geant particle 120 =  " << mykine << endl; 
//	 if (myparticle) myparticle->print();


	 

	      

		
	  



