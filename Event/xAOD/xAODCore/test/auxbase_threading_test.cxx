// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

// Local include(s).
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/AuxInfoBase.h"

// System include(s).
#include <memory>
#include <thread>
#include <vector>

/// Auxiliary store with a few static variables
class TestAuxContainer : public xAOD::AuxContainerBase {

public:
   /// Constructor setting up the auxiliary variables
   TestAuxContainer() {
      AUX_VARIABLE( var1 );
      AUX_VARIABLE( var2 );
      AUX_VARIABLE( var3 );
      AUX_VARIABLE( var4 );
      AUX_VARIABLE( var5 );
      AUX_VARIABLE( var6 );
      AUX_VARIABLE( var7 );
      AUX_VARIABLE( var8 );
      AUX_VARIABLE( var9 );
      AUX_VARIABLE( foo );
   }

private:
   std::vector< float > var1;
   std::vector< float > var2;
   std::vector< float > var3;
   std::vector< float > var4;
   std::vector< float > var5;
   std::vector< float > var6;
   std::vector< float > var7;
   std::vector< float > var8;
   std::vector< float > var9;
   std::vector< std::vector< int > > foo;

}; // class TestAuxContainer

/// Auxiliary store with a few static variables
class TestAuxInfo : public xAOD::AuxInfoBase {

public:
   /// Constructor setting up the auxiliary variables
   TestAuxInfo() {
      AUX_VARIABLE( var1 );
      AUX_VARIABLE( var2 );
      AUX_VARIABLE( var3 );
      AUX_VARIABLE( var4 );
      AUX_VARIABLE( var5 );
      AUX_VARIABLE( var6 );
      AUX_VARIABLE( var7 );
      AUX_VARIABLE( var8 );
      AUX_VARIABLE( var9 );
      AUX_VARIABLE( bar );
   }

private:
   float var1;
   float var2;
   float var3;
   float var4;
   float var5;
   float var6;
   float var7;
   float var8;
   float var9;
   std::vector< std::vector< int > > bar;

}; // class TestAuxInfo

/// Function executed in parallel threads at the same time
void execute() {

   // Create a *lot* of auxiliary store objects...
   for( int i = 0; i < 100000; ++i ) {
      auto container = std::make_unique< TestAuxContainer >();
      auto info = std::make_unique< TestAuxInfo >();
   }
   return;
}

int main() {

   std::cout << "xAODCore/auxbase_threading_test\n";

   /// The number of threads to use in the test
   static const int THREADS = 4;

   // Execute the test function in the requested number of threads.
   std::vector< std::thread > threads;
   threads.reserve(THREADS);

for( int i = 0; i < THREADS; ++i ) {
      threads.emplace_back( execute );
   }

   // Wait for the threads to finish.
   for( std::thread& thread : threads ) {
      thread.join();
   }
   return 0;
}
