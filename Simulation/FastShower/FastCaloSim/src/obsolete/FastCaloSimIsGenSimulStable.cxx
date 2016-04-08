/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>

#include "FastCaloSim/FastCaloSimIsGenSimulStable.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

  using std::vector;
  using std::abs;

  //****************************************************************
  //*                    FastCaloSimIsGenSimulStable                             *
  //****************************************************************
  FastCaloSimIsGenSimulStable::FastCaloSimIsGenSimulStable()  {}

  bool FastCaloSimIsGenSimulStable::operator()( const HepMC::GenParticle* const p ) const {
    int status=p->status();
//    int barcode=p->barcode();
//    int pdg_id=abs(p->pdg_id());
    HepMC::GenVertex* vertex = p->end_vertex();
// we want to keep primary particle with status==2 but without vertex in HepMC
    int vertex_barcode=-999999;
    if (vertex) vertex_barcode=vertex->barcode();

    return (
             (status%1000 == 1) ||
             (status%1000 == 2 && status > 1000) ||
             (status==2 && vertex_barcode<-200000)
           ) ? true:false;
  }
  
  bool FastCaloSimIsGenSimulStable::operator() ( const HepMC::GenParticle& p ) const {
    return this->operator()(&p);
  } 
  
  GenIMCselector* FastCaloSimIsGenSimulStable::create() const {return new FastCaloSimIsGenSimulStable(*this);}
