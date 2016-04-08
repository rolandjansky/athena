/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Reldis_HepMCInterface_H
#define Reldis_HepMCInterface_H

#include "HepMC/IO_BaseClass.h"
#include "HepPDT/ParticleDataTable.hh"

#include "Reldis_i/ReldisTree.h"

#include <iostream>

class IPartPropSvc;

namespace HepMC {
  
  class I_Reldis: public IO_BaseClass {
    
  public:
    
    I_Reldis() { m_particleTable = 0; };
    virtual ~I_Reldis() {};
    
    void fill_next_event(ReldisTree&, GenEvent*, int ievtnum);
  
  private: 
    
    // Following are not (yet?) implemented for this class.
    virtual bool fill_next_event(      GenEvent*) { return 0; }
    virtual void write_event    (const GenEvent*) {}
    
    double getParticleMass(int pdgID);
    void   addParticles   (GenVertex*, Int_t nParticles, Int_t* pdg, Float_t* px, Float_t* py, Float_t* pz, Int_t statusCode);

    HepPDT::ParticleDataTable* m_particleTable;
    
    // Use of copy constructor is not allowed
    I_Reldis(const I_Reldis&): IO_BaseClass() {} 
  };
}

#endif 
