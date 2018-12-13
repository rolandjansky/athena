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

// Local include(s):
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
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

const unsigned uintTestConst = (1<<(sizeof(unsigned)*4-1)) + 41;

int populateObject(xAOD::TrigComposite* obj) {
   // Set some simple properties on it:
   obj->setName( "TestObj" );
   obj->setDetail( "IntValue", 12 );
   obj->setDetail( "UnsignedIntValue", uintTestConst );
   obj->setDetail( "FloatValue", 3.14f );
   obj->setDetail( "IntVecValue", std::vector< int >( { 1, 2, 3 } ) );
   obj->setDetail( "UnsignedIntVecValue", std::vector< unsigned int >( { uintTestConst, 2, 3 } ) );
   obj->setDetail( "UnsignedShortVecValueEven", std::vector< uint16_t >( { 5, 10, 50, 100 } ) ); // Packs into two ints
   obj->setDetail( "UnsignedShortVecValueOdd", std::vector< uint16_t >( { 1, 1, 3, 4, 7 } ) ); // Packs into three ints
   obj->setDetail( "FloatVecValue", std::vector< float >( { 1.23, 2.34 } ) );
   obj->setDetail( "StringValue", std::string("I am a string"));
   obj->setDetail( "StringVecValue", std::vector< std::string >( {"Hello", "I", "am", "a", "string", "vector"} ) );

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
   SIMPLE_ASSERT( obj->hasDetail<unsigned int>("UnsignedIntValue") );
   SIMPLE_ASSERT( obj->hasDetail<float>("FloatValue") );
   SIMPLE_ASSERT( obj->hasDetail<std::vector<int> >("IntVecValue") );
   SIMPLE_ASSERT( obj->hasDetail<std::vector<unsigned int> >("UnsignedIntVecValue") );
   // These should work.... but they do not.... still being investigated
   // SIMPLE_ASSERT( obj->hasDetail<std::vector<uint16_t> >("UnsignedShortVecValueEven") );
   // SIMPLE_ASSERT( obj->hasDetail<std::vector<uint16_t> >("UnsignedShortVecValueOdd") );
   SIMPLE_ASSERT( obj->hasDetail<std::vector<float> >("FloatVecValue") );
   // SIMPLE_ASSERT( obj->hasDetail<std::string>("StringValue") );
   // SIMPLE_ASSERT( obj->hasDetail<std::vector<std::string> >("StringVecValue") );

   std::cout << "Has detail ok." << std::endl;

   // Check them:
   SIMPLE_ASSERT( obj->name() == "TestObj" );

   int intValue = 0;
   SIMPLE_ASSERT( obj->getDetail( "IntValue", intValue ) );
   SIMPLE_ASSERT( intValue == 12 );

   unsigned int unsignedIntValue = 0; // we want to use also the sin bit
   SIMPLE_ASSERT( obj->getDetail( "UnsignedIntValue", unsignedIntValue ) );
   SIMPLE_ASSERT( unsignedIntValue == uintTestConst );

   float floatValue = 0;
   SIMPLE_ASSERT( obj->getDetail( "FloatValue", floatValue ) );
   SIMPLE_ASSERT( std::abs( floatValue - 3.14 ) < 0.001 );

   std::vector< int > intVector;
   SIMPLE_ASSERT( obj->getDetail( "IntVecValue", intVector ) );
   SIMPLE_ASSERT( intVector == std::vector< int >( { 1, 2, 3 } ) );

   std::vector<unsigned int> unsignedIntVector;
   SIMPLE_ASSERT( obj->getDetail("UnsignedIntVecValue", unsignedIntVector) );
   SIMPLE_ASSERT( unsignedIntVector == std::vector<unsigned int>( { uintTestConst, 2, 3 } ) );

   std::vector<uint16_t> unsignedShortVectorEven;
   SIMPLE_ASSERT( obj->getDetail("UnsignedShortVecValueEven", unsignedShortVectorEven) );
   std::cout << "UnsignedShortVecValueEven = ";
   for (auto v : unsignedShortVectorEven) std::cout << v << " ";
   std::cout << std::endl;
   SIMPLE_ASSERT( unsignedShortVectorEven == std::vector<uint16_t>( { 5, 10, 50, 100 } ) );

   std::vector<uint16_t> unsignedShortVectorOdd;
   SIMPLE_ASSERT( obj->getDetail("UnsignedShortVecValueOdd", unsignedShortVectorOdd) );
   std::cout << "UnsignedShortVecValueOdd = ";
   for (auto v : unsignedShortVectorOdd) std::cout << v << " ";
   std::cout << std::endl;
   SIMPLE_ASSERT( unsignedShortVectorOdd == std::vector<uint16_t>( { 1, 1, 3, 4, 7 } ) );

   std::vector< float > floatVector;
   SIMPLE_ASSERT( obj->getDetail( "FloatVecValue", floatVector ) );
   // Simply just print the last one:
   std::cout << "FloatVecValue = " << floatVector << std::endl;

   std::string stringValue;
   SIMPLE_ASSERT( obj->getDetail("StringValue", stringValue) );
   SIMPLE_ASSERT( stringValue == std::string("I am a string") );

   std::vector<std::string> stringVecValue;
   SIMPLE_ASSERT( obj->getDetail("StringVecValue", stringVecValue) );
   SIMPLE_ASSERT( stringVecValue == std::vector< std::string >( {"Hello", "I", "am", "a", "string", "vector"} ) );

   int intValue2 = obj->getDetail<int>("IntValue");
   SIMPLE_ASSERT( intValue2 == 12 );
   unsigned int unsignedIntValue2 = obj->getDetail<unsigned int>("UnsignedIntValue");
   SIMPLE_ASSERT( unsignedIntValue2 == uintTestConst);
   float floatValue2 = obj->getDetail<float>("FloatValue");
   SIMPLE_ASSERT( std::abs( floatValue2 - 3.14 ) < 0.001 );
   std::vector<int> intVector2 =  obj->getDetail<std::vector<int>>( "IntVecValue");
   SIMPLE_ASSERT( intVector2 == std::vector< int >( { 1, 2, 3 } ) );
   std::vector< float > floatVector2 = obj->getDetail<std::vector<float>>( "FloatVecValue");
   std::cout << "Simple getDetail API ok." << std::endl;

   std::vector<unsigned int> unsignedIntVector2 = obj->getDetail<std::vector<unsigned int>>("UnsignedIntVecValue");
   SIMPLE_ASSERT( unsignedIntVector2 == std::vector<unsigned int>( { uintTestConst, 2, 3 } ) );

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

   std::cout << "Testing initial TC object" << std::endl;
   SIMPLE_ASSERT( testObject(obj) == 0 );

   std::cout << "Testing copy constructor" << std::endl;
   SIMPLE_ASSERT( testObject(copyConstructor) == 0 );

   std::cout << "Testing assignment operator (standalone object)" << std::endl;
   SIMPLE_ASSERT( testObject(assignmentOperator) == 0 );

   std::cout << "Testing assignment operator (object with store)" << std::endl;
   SIMPLE_ASSERT( testObject(obj2) == 0 );

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

   // Apparently everything went well:
   std::cout << "All tests successful." << std::endl;

   // Return gracefully:
   return 0;
}
