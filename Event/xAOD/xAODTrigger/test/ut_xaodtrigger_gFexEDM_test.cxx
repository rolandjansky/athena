/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#undef NDEBUG
#include <iostream>
#include <sstream>
#include <cassert>

// Local include(s):
#include "xAODTrigger/gFexJetRoI.h"
#include "xAODTrigger/versions/gFexJetRoIAuxContainer_v1.h"
#include "xAODTrigger/gFexJetRoIAuxContainer.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/gFexGlobalRoI.h"
#include "xAODTrigger/versions/gFexGlobalRoIAuxContainer_v1.h"
#include "xAODTrigger/gFexGlobalRoIAuxContainer.h"
#include "xAODTrigger/gFexGlobalRoIContainer.h"

/// Helper macro for testing the code
#define SIMPLE_ASSERT( EXP )                                   \
   do {                                                        \
      const bool result = EXP;                                 \
      if( ! result ) {                                         \
         std::cerr << __FILE__ << ":" << __LINE__              \
                   << " Failed test: " << #EXP << std::endl;   \
         throw std::runtime_error( "Failed " #EXP );           \
      }                                                        \
   } while( 0 )


/// Testing the 32-bit TOB
void testgFexJetRoI() {

    // Create the container that we want to test:
   xAOD::gFexJetRoIAuxContainer aux;
   xAOD::gFexJetRoIContainer c;
   c.setStore( &aux );

   // Create a test object:
   xAOD::gFexJetRoI* obj = new xAOD::gFexJetRoI();
   c.push_back( obj );

   std::cout << "Initializing test object: jet TOB" << std::endl;
   uint32_t word = 205353989;
   obj->initialize(word);

   SIMPLE_ASSERT( obj->isgRho() == 0 );
   SIMPLE_ASSERT( obj->isgBlockLead() == 0 );
   SIMPLE_ASSERT( obj->isgBlockSub() == 0 );
   SIMPLE_ASSERT( obj->isgJet() == 1 );
   SIMPLE_ASSERT( obj->gFexType() == 3 );
   
   SIMPLE_ASSERT( obj->etMin() == 5510400 );
   SIMPLE_ASSERT( obj->etMax() == 5512000 );
   SIMPLE_ASSERT( obj->eta() == static_cast<float>(-3.4) );
   SIMPLE_ASSERT( obj->etaMin() == static_cast<float>(-3.5) );
   SIMPLE_ASSERT( obj->etaMax() == static_cast<float>(-3.3) );
   SIMPLE_ASSERT( obj->phi_gFex() == static_cast<float>(3*(2*M_PI/16)+(2*M_PI/(16*2))) );
   SIMPLE_ASSERT( obj->phiMin_gFex() == static_cast<float>(3*(2*M_PI/16)) );
   SIMPLE_ASSERT( obj->phiMax_gFex() == static_cast<float>(3*(2*M_PI/16)+(2*M_PI/16)) );

   std::cout << "Test jet TOB completed!" << std::endl;


}

void testgFexGlobalRoI() {

    // Create the container that we want to test:
   xAOD::gFexGlobalRoIAuxContainer aux;
   xAOD::gFexGlobalRoIContainer c;
   c.setStore( &aux );

   // Create a test object:
   xAOD::gFexGlobalRoI* obj = new xAOD::gFexGlobalRoI();
   c.push_back( obj );

   std::cout << "Initializing test object: global TOB" << std::endl;
   uint32_t word = 192035055;
   obj->initialize(word);

   SIMPLE_ASSERT( obj->isgScalar() == 0 );
   SIMPLE_ASSERT( obj->isgMET() == 1 );
   SIMPLE_ASSERT( obj->isgMHT() == 0 );
   SIMPLE_ASSERT( obj->isgMST() == 0 );
   SIMPLE_ASSERT( obj->globalType() == 2 );

   SIMPLE_ASSERT( obj->METquantityOne() == 1461600 );
   SIMPLE_ASSERT( obj->METquantityTwo() == 1829600 );
   SIMPLE_ASSERT( obj->SumEt() == -999 );

   std::cout << "Test global TOB completed!" << std::endl;


}
int main() {

   // Run the tests:
   testgFexJetRoI();
   testgFexGlobalRoI();

   // Return gracefully:
   return 0;


}
