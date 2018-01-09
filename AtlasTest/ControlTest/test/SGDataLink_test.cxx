/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "TestTools/SGassert.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/tools/MapIndexingPolicy.h"
#include "AthLinks/tools/SetIndexingPolicy.h"
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/DefaultKey.h"
#include "AthenaKernel/errorcheck.h"
#include "SGTools/CLASS_DEF.h"
#include "SGTools/BuiltinsClids.h"


#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <string>
#include <stdexcept>

#include <sstream>

#include "TestTools/initGaudi.h"
using namespace Athena_test;

using std::ostringstream;
using std::istringstream;

using std::cerr;
using std::cout;
using std::endl;
using std::deque;
using std::map;
using std::set;
using std::string;
using std::vector;
using std::runtime_error;


typedef DataVector<int> IntVector;
//typedef vector<int*> IntVector;

class IntVectorInherit : public IntVector {};
class FloatVectorInherit : public DataVector<float> {};
class IntFloatMapInherit : public map<int, float> {/*ouch!*/};
CONTAINER_IS_MAP(IntFloatMapInherit)

class Bar {};

class Foo {
public:
  Foo(double d=0.0) : m_d(d) {};
  double d() const { return m_d; }
private:
  double m_d;
};

CLASS_DEF( IntVector , 7368883 , 1 )
CLASS_DEF(Foo, 8101, 1)
CLASS_DEF(Bar, 8107, 5)
CLASS_DEF2( map<int,string> , 231411637 , 1 )

class FooDeque : public deque<Foo> {};

namespace Athena_test 
{
  void linkDataProxyStorage(StoreGateSvc& rSG) 
  {  
    //
    //part 2 DataProxyStorage
    //
    cout << "*** DataLink_test - testing DataProxyStorage ***" << endl;
  
  //a link to a primitive type 
  //should not work DataLink<int> li(1);
    int* pint = new int(1);
    assert (rSG.record (pint, "pint").isSuccess());
    DataLink<int> testI(pint);
    assert(testI.isValid());
    assert(*testI == 1);

    cerr << "Now we expect to see an error message:" << endl \
              << "----Error Message Starts--->>" << endl;
    DataLink<int> badI((int*)nullptr);
    cerr << "<<----Error Message Ends-----" << endl;
    assert(!badI.isValid());

  //a link to an object
    Foo* pFoo = new Foo;
    assert (rSG.record (pFoo, "pFoo").isSuccess());
    DataLink<Foo> fLink(*pFoo), copyfLink(fLink);
    assert(pFoo == &*fLink);
    assert(pFoo == &*copyfLink);
    
    std::cout << "*** IntVector test starts ***" << std::endl;
    IntVector* vint = new IntVector;
    vint->push_back(new int(10));
    vint->push_back(new int(20));
    vint->push_back(new int(30));
    assert (rSG.record (vint, "vint").isSuccess());
    //a link to a container 
    DataLink< IntVector > lvint(*vint), copylvint;
    copylvint=lvint;
    cout<< "vector [" << 1 << "] = " << (*copylvint)[1] << endl; 
    assert(&*lvint == vint);
    assert(&*copylvint == &*lvint);
    assert((*vint)[1] == (*lvint)[1]);
    assert(copylvint->operator[](1) == (*vint)[1]);

  //a link to a vector *element*. 
    typedef ElementLink<IntVector> VectorIntLink; 
    VectorIntLink liint(*vint, 1), copyliint;
    copyliint=liint;
    assert(*liint == (*vint)[1]);
    assert(*copyliint == (*vint)[1]);
    assert(copyliint.index() == 1);

    // this will fail as we don't specify an IndexingPolicy
    //  DataLink<IntVector >::type wrongvint(vint,1);
    std::cout << "*** IntVector test OK ***" << std::endl;

    std::cout << "*** Map test starts ***" << std::endl;
    map<int,string>* pmint = new std::map<int,string>;
    assert (rSG.record (pmint, "pmint").isSuccess());
    map<int,string>& mint = *pmint;
    mint[10] = "dieci";
    mint[20] = "venti";
    mint[30] = "trenta";
    //now a link to a map element
    typedef ElementLink<map<int,string> > MapElementLink;
    MapElementLink lmint(mint, **liint) , copylmint(lmint);
    cout<< "map [" << lmint.index() << "] = " << *lmint << endl; 
    assert(*copylmint == *lmint);
    assert(copylmint.index() == 20);
    assert(mint[20] == *lmint);

    // this will fail as we specify the wrong indexing policy
    //  DataLink<map<int,string>, SequenceIndexing>::type wronglmint(mint, *liint);
    std::cout << "*** Map test OK ***" << std::endl;

    std::cout << "*** test reverse lookup static functions ***" << std::endl;
    int key(-1);
    string mstr("venti");
    SG::MapIndexingPolicy<map<int,string> >::reverseLookup(mint,mstr,key);
    cout<< "venti has key " << key << endl;
    assert(key==20);

    set<string> aset;
    aset.insert("dieci");
    aset.insert("venti");
    aset.insert("trenta");
    aset.insert("trenta");
    string keyel("");
    string sstr("venti");
    SG::SetIndexingPolicy<set<string> >::reverseLookup(aset,sstr,keyel);
    cout<< "found set element " << keyel << endl;
    assert(keyel=="venti");

    VectorIntLink::index_type idx(999999999);
    VectorIntLink::IndexingPolicy::reverseLookup(*vint, (*vint)[1], idx);
    assert(idx==1);

    MapElementLink mLink;
    assert( mLink.toPersistent() );
    std::string myStr("cinquanta");
    bool caught = false;
    try {
      mLink.toContainedElement(mint, myStr);
    }
    catch (SG::ExcElementNotFound&) {
      caught = true;
    }
    assert (caught);

    std::cout << "*** test reverse lookup static functions OK ***" << std::endl;

    //now try to set the link using const pointer
    int* novantanove = new int(99);
    assert (rSG.record (novantanove, "novantanove").isSuccess());
    
    const int& cref(*novantanove);
    DataLink<int> lcint(cref);
    assert (lcint.cptr() == novantanove);
    const DataLink<int> copylcint(lcint);
    const DataLink<int> clcint(cref);
    assert (clcint == copylcint);
    assert (*clcint == 99);
    assert (*clcint == *copylcint);
    cout << "*** DataLink_test DataProxyStorage OK ***\n\n" <<endl;
  }

  void linkStoreGate(StoreGateSvc& rSG)
  {
    //a link to an object
    Bar *pBar(new Bar());
    assert((rSG.record(pBar, "a Bar")).isSuccess());
    DataLink<Bar> fLink("a Bar");
    assert(fLink.isValid());
    assert(pBar == &*fLink);

    //a link to an element
    IntVector* pvint(new IntVector);
    pvint->push_back(new int(10));
    pvint->push_back(new int(20));
    assert((rSG.record(pvint, "a vector")).isSuccess());
    ElementLink<IntVector> liint("a vector", 1);
    assert(liint.isValid());
    assert(*liint == (*pvint)[1]);
    assert(**liint == 20);
    assert(liint.index() == 1);
    assert(liint.dataID() == "a vector");
    cout << "*** DataLink_test StoreGate OK ***\n\n" <<endl;
  }

  void dummyToRemoveWarning(const Foo&) 
  {}
    
  void linkStateMachine(StoreGateSvc& rSG) 
  {
  
    cout << "\n\nDataLink_test testing StateMachine" <<endl;

    //creating an invalid link
    DataLink<Foo> lFoo;
    assert(!lFoo.isValid());
  
    //transition to accessible
    Foo* pFoo = new Foo();
    assert (rSG.record (pFoo, "pFoo3").isSuccess());
    Foo& aFoo(*pFoo);
    lFoo.toStorableObject(aFoo);
    assert(lFoo.isValid());
    const Foo& rFoo = *lFoo;
    dummyToRemoveWarning(rFoo);
    
    cout << "transition invalid to Accessible OK" <<endl;
    

    //create an IdentifiedDefault link
    //FIXME     DataLink<Foo> dFoo(SG::DefaultObject_tag());
    //    assert(!dFoo.isValid());


    //clear the original link
    lFoo.clear();
    assert(!lFoo.isValid());
    cout << "transition Accessible to invalid OK" <<endl;
    //transition to IdentifiedDefault

    lFoo.toDefaultObject();
    assert(!lFoo.isValid()); //LINK not valid until accessible
    pFoo = new Foo();
    assert(rSG.record(pFoo, SG::DEFAULTKEY).isSuccess());

    assert(lFoo.isValid()); //LINK now valid (initial->persistable->identifiedDefault
 
    cout << "transition invalid to IdentifiedDefault OK" <<endl;

    //transition toAccessible
    const Foo& r2Foo = *lFoo;
    dummyToRemoveWarning(r2Foo);

    cout << "transition IdentifiedDefault to Accessible OK" <<endl;

    //transition to Persistable
    lFoo.dataID();
    cout << "transition Accessible to Persistable OK" <<endl;

    //clear the original link
    lFoo.clear(); // now in default state.

    //transition to Identified
    lFoo.toIdentifiedObject("foo");
    assert(!lFoo.isValid());   // invalid because object foo does not exist yet
    assert("foo" == lFoo.dataID());
    cout << "transition invalid to Identified OK" <<endl;
    //transition to accessible
    //too early not yet registered
    try {   
      const Foo& r3Foo = *lFoo;
      dummyToRemoveWarning(r3Foo);
      cout << "ERROR should never get here" <<endl;
      assert(0);
    } catch (const runtime_error& e) {
      cout << "foo Foo not yet registered. Caught exception -"
 	   << e.what()<< "- as expected" << endl;
    }	
    rSG.record(new Foo(), "foo");
    //try again transition toAccessible
    const Foo& r4Foo = *lFoo;
    dummyToRemoveWarning(r4Foo);
    cout << "transition Identified to Accessible OK" <<endl;

#if 0
    //reset the accessible link (this leaves it valid and identified)
    lFoo.reset();
    assert(lFoo.isValid());
    assert("foo" == lFoo.dataID());
#endif
    cout << "reset Accessible OK" <<endl;    

    cout << "*** DataLink_test StateMachine OK ***\n\n" <<endl;
  }
} //end namespace


int main() {
  ISvcLocator* pSvcLoc;
  if (!initGaudi("DataLink_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  

  errorcheck::ReportMessage::hideErrorLocus();

  assert(pSvcLoc);
  StoreGateSvc* pSG;
  
  const bool CREATEIF(true);
  assert((pSvcLoc->service("StoreGateSvc", pSG, CREATEIF)).isSuccess());
  assert(pSG);

  Foo* pFoo = new Foo;
  assert (pSG->record (pFoo, "foo2").isSuccess());

  linkDataProxyStorage(*pSG);
  linkStoreGate(*pSG);
  linkStateMachine(*pSG);

  cout << "*** DataLink_test OK ***" <<endl;
  return 0;
}












