/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonSimple_test.cxx 602896 2014-06-20 12:48:42Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"

int main() {

   xAOD::TrigPhoton ph1;
   ph1.makePrivateStore();
   ph1.init( 0x10101010,
             0.1, 0.2,
             xAOD::TrigPhoton::EMClusterLink_t() );
   ph1.setPt( 10000 );

   std::cout << "xAOD::TrigPhoton:" << std::endl;
   std::cout << "  type = " << ph1.type() << std::endl;
   std::cout << "  eta = " << ph1.eta() << std::endl;
   std::cout << "  phi = " << ph1.phi() << std::endl;
   std::cout << "  pt  = " << ph1.pt() << std::endl;
   std::cout << "  rapidity = " << ph1.rapidity() << std::endl;

   xAOD::TrigPhotonContainer c;
   xAOD::TrigPhotonAuxContainer aux;
   c.setStore( &aux );

   xAOD::TrigPhoton* ph2 = new xAOD::TrigPhoton();
   c.push_back( ph2 );
   ph2->init( 0x20202020,
              0.3, 0.4,
              xAOD::TrigPhoton::EMClusterLink_t() );
   ph2->setPt( 20000 );

   std::cout << "xAOD::TrigPhoton:" << std::endl;
   std::cout << "  type = " << ph2->type() << std::endl;
   std::cout << "  eta = " << ph2->eta() << std::endl;
   std::cout << "  phi = " << ph2->phi() << std::endl;
   std::cout << "  pt  = " << ph2->pt() << std::endl;
   std::cout << "  rapidity = " << ph2->rapidity() << std::endl;

   return 0;
}
