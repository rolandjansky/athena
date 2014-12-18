/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetConstitRemover.h"

JetConstitRemover::JetConstitRemover(const std::string& myname): JetModifierBase(myname) { }

int JetConstitRemover::modifyJet(xAOD::Jet& jet) const {

   /// The accessor for the cluster element links
  static SG::AuxElement::Accessor< std::vector< ElementLink< xAOD::IParticleContainer > > >
    constituentAcc( "constituentLinks" );
  static SG::AuxElement::Accessor< std::vector< float> >
    constituentWeightAcc( "constituentWeights" );
  

  if( constituentAcc.isAvailable(jet) ) constituentAcc( jet ).resize(0);    
  if( constituentWeightAcc.isAvailable(jet) ) constituentWeightAcc( jet ).resize(0);  

  return 0;
}
