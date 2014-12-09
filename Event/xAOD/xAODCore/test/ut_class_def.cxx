/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_class_def.cxx 613558 2014-08-26 17:16:16Z krasznaa $
//
// Simple unit testing for the standalone CLASS_DEF/ClassID_traits code.
//

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODCore/CLASS_DEF.h"

class TestClassA {
public:
   TestClassA() {}
};
class TestClassB{};

namespace xAOD {
   class TestClassC {};
}

CLASS_DEF( TestClassA, 887887, 1 )
CLASS_DEF( xAOD::TestClassC, 11223344, 1 )

int main() {

   if( ClassID_traits< TestClassA >::ID() != 887887 ) {
      std::cout << "Wrong class ID for TestClassA" << std::endl;
      return -1;
   }
   std::cout << "Class ID worked" << std::endl;

   if( ClassID_traits< TestClassA >::typeName() != "TestClassA" ) {
      std::cout << "Wrong type name for TestClassA" << std::endl;
      return -1;
   }
   std::cout << "Class typeName worked" << std::endl;

   if( ClassID_traits< xAOD::TestClassC >::typeName() != "xAOD::TestClassC" ) {
      std::cout << "Wrong type name for xAOD::TestClassC" << std::endl;
      return -1;
   }
   std::cout << "Namespaces class typeName worked" << std::endl;

   //ClassID_traits<TestClassB>::ID(); // this should fail to compile with readable message

   std::cout << "Test ok" << std::endl;
   return 0;
}
