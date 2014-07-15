/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectronSimple_test.cxx 602876 2014-06-20 11:21:26Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"

int main() {

   xAOD::TrigElectron el1;
   el1.makePrivateStore();
   el1.init( 0x10101010,
             0.1, 0.2, 1.2,
             xAOD::TrigElectron::EMClusterLink_t(),
             xAOD::TrigElectron::TrackParticleLink_t() );
   el1.setPt( 10000 );

   std::cout << "xAOD::TrigElectron:" << std::endl;
   std::cout << "  type = " << el1.type() << std::endl;
   std::cout << "  eta = " << el1.eta() << std::endl;
   std::cout << "  phi = " << el1.phi() << std::endl;
   std::cout << "  pt  = " << el1.pt() << std::endl;
   std::cout << "  rapidity = " << el1.rapidity() << std::endl;

   xAOD::TrigElectronContainer c;
   xAOD::TrigElectronAuxContainer aux;
   c.setStore( &aux );

   xAOD::TrigElectron* el2 = new xAOD::TrigElectron();
   c.push_back( el2 );
   el2->init( 0x20202020,
              0.3, 0.4, 0.9,
              xAOD::TrigElectron::EMClusterLink_t(),
              xAOD::TrigElectron::TrackParticleLink_t() );
   el2->setPt( 20000 );

   std::cout << "xAOD::TrigElectron:" << std::endl;
   std::cout << "  type = " << el2->type() << std::endl;
   std::cout << "  eta = " << el2->eta() << std::endl;
   std::cout << "  phi = " << el2->phi() << std::endl;
   std::cout << "  pt  = " << el2->pt() << std::endl;
   std::cout << "  rapidity = " << el2->rapidity() << std::endl;

   return 0;
}
