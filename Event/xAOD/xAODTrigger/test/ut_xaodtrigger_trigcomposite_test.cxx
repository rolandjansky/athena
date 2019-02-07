/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodtrigger_trigcomposite_test.cxx 761887 2016-07-14 13:16:16Z tbold $

// System include(s):
#include <cmath>
#include <iostream>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"
#include "xAODTrigger/MuonRoIContainer.h"

/// Helper macro for testing the code
#define SIMPLE_ASSERT( EXP )                                         \
   do {                                                              \
      const bool result = EXP;                                       \
      if( ! result ) {                                               \
         std::cerr << __FILE__ << ":" << __LINE__                    \
                   << "  Failed to execute: " << #EXP << std::endl;  \
         return 1;                                                   \
      }                                                              \
   } while( 0 )

/// Helper print operator
template< typename T >
std::ostream& operator<< ( std::ostream& out, const std::vector< T >& vec ) {

   out << "[";
   for( size_t i = 0; i < vec.size(); ++i ) {
      out << vec[ i ];
      if( ( i + 1 ) < vec.size() ) {
         out << ", ";
      }
   }
   out << "]";
   return out;
}

int populateObject(xAOD::TrigComposite* obj) {
   obj->setName( "TestObj" );
   obj->setDetail( "IntValue", 12 );
   obj->setDetail( "FloatValue", 3.14f );
   obj->setDetail( "IntVecValue", std::vector< int >( { 1, 2, 3 } ) );
   obj->setDetail( "FloatVecValue", std::vector< float >( { 1.23, 2.34 } ) );

   std::cout << "Set detail ok." << std::endl;

   // create a MuonRoIContainer in order to have "valid" ElementLinks
   // they will have made up SG key hashes but they are internally "valid"
   auto mrc = new xAOD::MuonRoIContainer();
   for (unsigned int i = 0; i<20; i++){
     xAOD::MuonRoI* roi = new xAOD::MuonRoI();
     mrc->push_back( roi );
   }
   // Now test the ElementLink functionality in a basic way:
   obj->setObjectLink( "MuonRoI",
                       ElementLink<xAOD::MuonRoIContainer>( 123, 11, mrc->at(11)) );

   // Test the ElementLinkVector functionality in a basic way:
   ElementLinkVector<xAOD::MuonRoIContainer> elementLinks;
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 789, 13, mrc->at(13) ) );
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 345, 17, mrc->at(17) ) );
   obj->addObjectCollectionLinks("ManyMuonRoIs", elementLinks);

   return 0;
}

int testDetails(const xAOD::TrigComposite* obj) {
   SIMPLE_ASSERT( obj->hasDetail<int>("IntValue") );
   SIMPLE_ASSERT( obj->hasDetail<float>("FloatValue") );
   SIMPLE_ASSERT( obj->hasDetail<std::vector<int> >("IntVecValue") );
   SIMPLE_ASSERT( obj->hasDetail<std::vector<float> >("FloatVecValue") );

   std::cout << "Has detail ok." << std::endl;

   // Check them:
   SIMPLE_ASSERT( obj->name() == "TestObj" );

   int intValue = 0;
   SIMPLE_ASSERT( obj->getDetail( "IntValue", intValue ) );
   SIMPLE_ASSERT( intValue == 12 );

   float floatValue = 0;
   SIMPLE_ASSERT( obj->getDetail( "FloatValue", floatValue ) );
   SIMPLE_ASSERT( std::abs( floatValue - 3.14 ) < 0.001 );

   std::vector< int > intVector;
   SIMPLE_ASSERT( obj->getDetail( "IntVecValue", intVector ) );
   SIMPLE_ASSERT( intVector == std::vector< int >( { 1, 2, 3 } ) );

   std::vector< float > floatVector;
   SIMPLE_ASSERT( obj->getDetail( "FloatVecValue", floatVector ) );
   // Simply just print the last one:
   std::cout << "FloatVecValue = " << floatVector << std::endl;

   int intValue2 = obj->getDetail<int>("IntValue");
   SIMPLE_ASSERT( intValue2 == 12 );
   float floatValue2 = obj->getDetail<float>("FloatValue");
   SIMPLE_ASSERT( std::abs( floatValue2 - 3.14 ) < 0.001 );
   std::vector<int> intVector2 =  obj->getDetail<std::vector<int>>( "IntVecValue");
   SIMPLE_ASSERT( intVector2 == std::vector< int >( { 1, 2, 3 } ) );
   std::vector< float > floatVector2 = obj->getDetail<std::vector<float>>( "FloatVecValue");
   std::cout << "Simple getDetail API ok." << std::endl;

   try {
     obj->getDetail<int>("NonExistent");
     SIMPLE_ASSERT(false);
   } catch (...) {}

   try {
     obj->getDetail<int>("FloatValue"); // confuse type
     SIMPLE_ASSERT(false);
   } catch (...) {}

   try {
     obj->getDetail<int>("IntVecValue"); // confuse cardinality
     SIMPLE_ASSERT(false);
   } catch (...) {}
   std::cout << "Missing details handled ok." << std::endl;

   return 0;
}

int testLinks(const xAOD::TrigComposite* obj, const size_t expectedSize = 3) {
   SIMPLE_ASSERT( obj->hasObjectLink( "MuonRoI" ) );
   SIMPLE_ASSERT( obj->linkColNames().size() == expectedSize );
   SIMPLE_ASSERT( obj->linkColKeys().size() == expectedSize );
   SIMPLE_ASSERT( obj->linkColIndices().size() == expectedSize );
   SIMPLE_ASSERT( obj->linkColClids().size() == expectedSize );
   SIMPLE_ASSERT( obj->linkColKeys()[ 0 ] == 123 );
   SIMPLE_ASSERT( obj->linkColIndices()[ 0 ] == 11 );
   SIMPLE_ASSERT( obj->linkColClids()[ 0 ] ==
                  ClassID_traits< xAOD::MuonRoIContainer >::ID() );

   std::cout << "Basic link functionality OK" << std::endl;

   ElementLink< xAOD::MuonRoIContainer > getMuonRoILink = obj->objectLink<xAOD::MuonRoIContainer>("MuonRoI");
   SIMPLE_ASSERT(getMuonRoILink == ElementLink<xAOD::MuonRoIContainer>( 123, 11 ));

   ElementLinkVector<xAOD::MuonRoIContainer> getMuonRoILinks = obj->objectCollectionLinks<xAOD::MuonRoIContainer>("ManyMuonRoIs");
   ElementLinkVector<xAOD::MuonRoIContainer> elementLinks;
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 789, 13 ) );
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 345, 17 ) );
   SIMPLE_ASSERT(getMuonRoILinks == elementLinks);

   std::cout << "Link recovery OK" << std::endl;
   return 0;
}

int testObject(const xAOD::TrigComposite* obj) {
   int ret = testDetails(obj);
   ret    |= testLinks(obj);
   return ret;
}

int main() {

   xAOD::TrigComposite::s_throwOnCopyError = true;

   // Create the container that we want to test:
   xAOD::TrigCompositeAuxContainer aux;
   xAOD::TrigCompositeContainer c;
   c.setStore( &aux );

   // Create a test object:
   xAOD::TrigComposite* obj = new xAOD::TrigComposite();
   c.push_back( obj );

   std::cout << "Populating initial TC object" << std::endl;
   populateObject(obj);

   // Standalone with copy constructor. Should get own store.
   xAOD::TrigComposite* copyConstructor = new xAOD::TrigComposite(*obj);

   // Standalone with assignment operator. Should get own store.
   xAOD::TrigComposite* assignmentOperator = new xAOD::TrigComposite();
   *assignmentOperator = *obj;

   // Creating a new object in a new store and copying to it
   xAOD::TrigCompositeAuxContainer aux2;
   xAOD::TrigCompositeContainer c2;
   c2.setStore( &aux2 );
   xAOD::TrigComposite* obj2 = new xAOD::TrigComposite();
   c2.push_back( obj2 );
   *obj2 = *obj;

   // Testing the v1 -> v2 EDM migration, change in Aux base class
   xAOD::TrigCompositeAuxContainer_v1 aux_v1;
   xAOD::TrigCompositeContainer c_edm_a;
   c_edm_a.setStore( &aux_v1 );
   xAOD::TrigComposite* obj_edm_a = new xAOD::TrigComposite();
   c_edm_a.push_back(obj_edm_a); // Now has an old-style store
   populateObject(obj_edm_a);
   // Creating a new object in a new store
   xAOD::TrigCompositeAuxContainer aux_vlatest;
   xAOD::TrigCompositeContainer c_edm_b;
   c_edm_b.setStore( &aux_vlatest );
   xAOD::TrigComposite* obj_edm_b = new xAOD::TrigComposite();
   c_edm_b.push_back(obj_edm_b);
   // Copy
   SG::copyAuxStoreThinned( aux_v1, aux_vlatest, 0 ); 

   std::cout << "Testing initial TC object" << std::endl;
   SIMPLE_ASSERT( testObject(obj) == 0 );

   std::cout << "Testing copy constructor" << std::endl;
   SIMPLE_ASSERT( testObject(copyConstructor) == 0 );

   std::cout << "Testing assignment operator (standalone object)" << std::endl;
   SIMPLE_ASSERT( testObject(assignmentOperator) == 0 );

   std::cout << "Testing assignment operator (object with store)" << std::endl;
   SIMPLE_ASSERT( testObject(obj2) == 0 );

   std::cout << "Testing assignment operator (EDM migration - new aux)" << std::endl;
   SIMPLE_ASSERT( testObject(obj_edm_b) == 0 );

   // Make new objects to test the link copy
   xAOD::TrigComposite* fullCopy = new xAOD::TrigComposite();
   c.push_back( fullCopy );

   // Copy over all other links
   SIMPLE_ASSERT( fullCopy->copyAllLinksFrom( obj ) == true );
   // Add another too
   auto mrc = new xAOD::MuonRoIContainer();
   for (unsigned int i = 0; i<20; i++){
     xAOD::MuonRoI* roi = new xAOD::MuonRoI();
     mrc->push_back( roi );
   }
   fullCopy->setObjectLink( "feature", ElementLink<xAOD::MuonRoIContainer>( 111, 19, mrc->at(0) ) );
   SIMPLE_ASSERT( testLinks(fullCopy, 4) == 0 );

   std::cout << "Full-copy of element links OK" << std::endl;

   // Check that we throw on attempted copy of link which exists in target or 
   // is non-existent in source
   try {
     fullCopy->copyLinkFrom(obj, "MuonRoI"); // Already copied
     SIMPLE_ASSERT(false);
   } catch (...) {}

   try {
     fullCopy->copyLinkFrom(obj, "NonExistent"); 
     SIMPLE_ASSERT(false);
   } catch (...) {}

   try {
     fullCopy->copyLinkCollectionFrom(obj, "ManyMuonRoIs");  // Already copied
     SIMPLE_ASSERT(false);
   } catch (...) {}

   try {
     fullCopy->copyLinkCollectionFrom(obj, "NonExistent"); 
     SIMPLE_ASSERT(false);
   } catch (...) {}

   std::cout << "Throw on copy of existing links or non-existent links OK" << std::endl;

   // Check we can still access the element link unique to fullCopy
   ElementLink<xAOD::MuonRoIContainer> getFeatureLink = fullCopy->objectLink<xAOD::MuonRoIContainer>("feature");
   SIMPLE_ASSERT(getFeatureLink == ElementLink<xAOD::MuonRoIContainer>( 111, 19, mrc->at(0) ));

   // Make new objects to test the manual link copy
   xAOD::TrigComposite* manualCopy = new xAOD::TrigComposite();
   c.push_back( manualCopy );

   SIMPLE_ASSERT( manualCopy->copyLinkFrom( fullCopy, "MuonRoI" ) == true );
   SIMPLE_ASSERT( manualCopy->copyLinkCollectionFrom( fullCopy, "ManyMuonRoIs" ) == true );
   // Test also link renaming 
   SIMPLE_ASSERT( manualCopy->copyLinkFrom( fullCopy, "feature", "featureWithNewName" ) == true );
   SIMPLE_ASSERT( testLinks(manualCopy, 4) == 0 );

   ElementLink<xAOD::MuonRoIContainer> getFeatureLinkAgain = manualCopy->objectLink<xAOD::MuonRoIContainer>("featureWithNewName");
   SIMPLE_ASSERT(getFeatureLinkAgain == ElementLink<xAOD::MuonRoIContainer>( 111, 19, mrc->at(0) ));

   std::cout << "Copy link-by-link OK" << std::endl;

   // Test single typeless add
   obj->typelessSetObjectLink( "typeless1", 123, ClassID_traits< xAOD::MuonRoIContainer >::ID(), 11, 12 );
   ElementLink< xAOD::MuonRoIContainer > getMuonRoILink = obj->objectLink<xAOD::MuonRoIContainer>("typeless1");
   SIMPLE_ASSERT(getMuonRoILink == ElementLink<xAOD::MuonRoIContainer>( 123, 11 ));

   // Test single typeless overwrite
   obj->typelessSetObjectLink( "typeless1", 123, ClassID_traits< xAOD::MuonRoIContainer >::ID(), 12, 13 );
   getMuonRoILink = obj->objectLink<xAOD::MuonRoIContainer>("typeless1");
   SIMPLE_ASSERT(getMuonRoILink != ElementLink<xAOD::MuonRoIContainer>( 123, 11 ));
   SIMPLE_ASSERT(getMuonRoILink == ElementLink<xAOD::MuonRoIContainer>( 123, 12 ));

   // Test typeless collection add
   obj->typelessSetObjectLink( "typeless2", 123, ClassID_traits< xAOD::MuonRoIContainer >::ID(), 12, 15 );
   ElementLinkVector<xAOD::MuonRoIContainer> getMuonRoILinks = obj->objectCollectionLinks<xAOD::MuonRoIContainer>("typeless2");
   ElementLinkVector<xAOD::MuonRoIContainer> elementLinks;
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 123, 12 ) );
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 123, 13 ) );
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 123, 14 ) );
   SIMPLE_ASSERT(getMuonRoILinks == elementLinks);

   // Test typeless collection overwrite
   obj->typelessSetObjectLink( "typeless2", 123, ClassID_traits< xAOD::MuonRoIContainer >::ID(), 5, 7 );
   getMuonRoILinks = obj->objectCollectionLinks<xAOD::MuonRoIContainer>("typeless2");
   SIMPLE_ASSERT(getMuonRoILinks != elementLinks);
   elementLinks.clear();
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 123, 5 ) );
   elementLinks.push_back( ElementLink<xAOD::MuonRoIContainer>( 123, 6 ) );
   SIMPLE_ASSERT(getMuonRoILinks == elementLinks);

   // Apparently everything went well:
   std::cout << "All tests successful." << std::endl;

   // Return gracefully:
   return 0;
}
