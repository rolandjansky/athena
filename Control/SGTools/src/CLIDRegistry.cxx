/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/CLIDRegistry.h"
/* #include <algorithm> */
using namespace std;

const unsigned long CLIDRegistry::MINCLID = detail::MINCLID;
const unsigned long CLIDRegistry::MAXCLID = detail::MAXCLID;
unsigned int _alreadyDone (0);


//   bool 
//   CLIDRegistry::addEntry(unsigned long id, const string& typeName) {
//     assert( MINCLID <= id && id <= MAXCLID );
//     registry().push_back(pair<unsigned long, string>(id,typeName));
//     return true;
//   }


CLIDRegistry::const_iterator CLIDRegistry::begin() {
#ifdef CLIDREG_DEBUG
  cerr << "CLIDRegistry::begin: returns " 
	    << &*(registry().begin()) << " for registry " <<&registry() 
	    <<endl;
#endif	
  return registry().begin();
}


CLIDRegistry::const_iterator CLIDRegistry::end() {
#ifdef CLIDREG_DEBUG
  cerr << "CLIDRegistry::end: returns " 
	    << &*(registry().end()) << " for registry " <<&registry() 
	    <<endl;
#endif	
  return registry().end();
  //return unique(registry().begin(), registry().end());  //FIXME O(N)!
}

bool
CLIDRegistry::hasNewEntries() {
  return (_alreadyDone < CLIDRegistry::registry().size());
}

std::pair<CLIDRegistry::const_iterator, CLIDRegistry::const_iterator> 
CLIDRegistry::newEntries() {
  std::pair<CLIDRegistry::const_iterator, CLIDRegistry::const_iterator> ret =
     std::make_pair(CLIDRegistry::begin()+_alreadyDone, 
                    CLIDRegistry::end());
  _alreadyDone = CLIDRegistry::registry().size();
  return ret;
}


CLIDRegistry::CLIDRegistryImpl& CLIDRegistry::registry() {
  static CLIDRegistryImpl _reg;
  return _reg;
}
