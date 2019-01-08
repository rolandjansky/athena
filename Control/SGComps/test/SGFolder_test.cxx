/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** @file SGFolder_test.cxx
 * @brief unit test for SG::Folder
 * @author ATLAS Collaboration
 */
#undef NDEBUG

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ToolHandle.h"

// TEST_FOOBAR_H

#include "AthenaKernel/CLASS_DEF.h"
struct Foo{
  void doNothing() const {};
};
CLASS_DEF( Foo, 8101, 0) 
struct Bar{
  void doNothing() const {};
};
CLASS_DEF( Bar, 8107, 0) 

// TEST_FOOBAR_H

#include "../src/SGFolder.h"

namespace SG{
  /// hack to access SG::Folder::add(clid,key,checkValidclid) which is private
  class Folder_tester {
    Folder& m_SGF;
  public:
    Folder_tester(IFolder& aIF) : 
      m_SGF(dynamic_cast<Folder&>(aIF)) {}
    ///add a data object identifier to the list
    StatusCode add(const CLID& clid, const std::string& skey, 
		   bool checkValidCLID) {
      return m_SGF.add(clid, skey, checkValidCLID, false);
    }
  };
}
#include "SGTools/SGIFolder.h"

class ISvcLocator;

using namespace std;
using namespace SG;

int main () {
  cerr << "*** SG::Folder_test starts ***" <<endl;
  ISvcLocator* pDummy;
  if (!Athena_test::initGaudi("SGFolder_test.txt", pDummy)) {
    cerr << " This test cannot be run without init Gaudi" << endl;
    return -1;
  }
  ToolHandle<IFolder> ipFolder("SG::Folder/MyFolder");
  assert( ipFolder.retrieve().isSuccess() );
  //this of course depends on the job opts
  assert( 4 == std::distance(ipFolder->begin(), ipFolder->end()) );

  //add a good entry
  assert( (ipFolder->add("Bar", "abcd")).isSuccess() );
  //and some bad ones
  assert( (ipFolder->add("basfbojjvd", "a4")).isFailure() );  
  assert( (ipFolder->add("cucu", "")).isFailure() );
  Folder_tester tFolder(*(ipFolder));
  assert( (tFolder.add(56789401, "34", true)).isFailure() ); 
  //only added "Bar/abcd"
  assert( 5 == std::distance(ipFolder->begin(), ipFolder->end()) );
  
  assert( (ipFolder->add(558401, "asdf")).isSuccess() );  
  //added 558401/asdf
  assert( 6 == std::distance(ipFolder->begin(), ipFolder->end()) );

  assert( (ipFolder->add("Bar!", "poiu")).isSuccess() );

  IFolder::const_iterator i(ipFolder->begin()), iEnd(ipFolder->end());
  while (i != iEnd) {
    cout << i->id() << "/" << i->key();
    if (i->exact())
      cout << " [exact]";
    cout << endl;
    ++i;
  }

  ipFolder->clear();
  assert( 0 == std::distance(ipFolder->begin(), ipFolder->end()) );

  cerr << "*** SG::Folder_test OK ***" <<endl;
  return 0;

}

