/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/// stolen from http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODTrigger/test/ut_xaodtrigger_trigcomposite_test.cxx
/// Helper macro for testing the code
//      std::cerr << __FILE__ << ":" << __LINE__		


#define SIMPLE_ASSERT( EXP )						\
  do {	                                                                \
    const bool result = EXP;						\
    if( ! result ) {                                                    \
          std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__; \
          std::cerr << " inside " << __FUNCTION__ ;                     \
          std::cerr << ": Condition: " << #EXP << std::endl;            \
          abort();                                                      \
    }									\
  } while( 0 )








 
