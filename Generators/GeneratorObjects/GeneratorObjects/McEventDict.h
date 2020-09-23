// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOROBJECTS_MCEVENTDICT_H
# define GENERATOROBJECTS_MCEVENTDICT_H 1

#include <vector>
#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GeneratorObjects/HijingEventParams.h"
#include "RootUtils/PyROOTIteratorFuncs.h"

#include "AtlasHepMC/GenEvent.h"
namespace GeneratorObjectsMcEventDict {
  
  struct tmp {
    std::vector<HepMcParticleLink> m_hepMcParticleLinks;
    std::set<HepMC::GenParticle*>::const_iterator m_psetci;
    std::set<HepMC::GenVertex*>::const_iterator m_vsetci;
    std::set<HepMC::GenParticle*>::iterator m_pseti;
    std::set<HepMC::GenVertex*>::iterator m_vseti;

    std::iterator<std::forward_iterator_tag,HepMC::GenVertex*,int,HepMC::GenVertex**,HepMC::GenVertex*&> m_1;
    std::iterator<std::forward_iterator_tag,HepMC::GenParticle*,int,HepMC::GenParticle**,HepMC::GenParticle*&> m_2;
    
#ifdef HEPMC3 
    RootUtils::PyROOTIteratorFuncs<std::vector<HepMC::GenVertex>::const_iterator> m_it;
#else    
    RootUtils::PyROOTIteratorFuncs<
      HepMC::GenVertex::particles_out_const_iterator> m_it;
#endif
  };

}

#endif // GENERATOROBJECTS_MCEVENTDICT_H
