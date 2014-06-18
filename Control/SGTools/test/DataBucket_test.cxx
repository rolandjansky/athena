/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "SGTools/BuiltinsClids.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/StlMapClids.h"
#include "SGTools/DataBucket.h"
#include "SGTools/StorableConversions.h"
#include "AthenaKernel/ILockable.h"
#include <iostream>
using namespace std;
using namespace SG;

/*FIXME can get it from TestTools circ package dep */
#include <cassert>
#define SGASSERTERROR( FALSEEXPR )   \
    std::cerr << "Now we expect to see an error message:" << std::endl \
              << "----Error Message Starts--->>" << std::endl; \
    assert(!FALSEEXPR); \
    std::cerr<< "<<---Error Message Ends-------" << std::endl


#include "GaudiKernel/DataObject.h"
class GaudiDataObj : public DataObject {
 
public: 
  static int count;
 
  GaudiDataObj(): DataObject(), m_val(0) { ++count; };
  GaudiDataObj(int i): DataObject(), m_val(i) { ++count; };
  virtual ~GaudiDataObj(){ --count; };

  static const CLID& classID() {
    static const CLID _ID = 8010;
    return _ID; }
  virtual const CLID& clID() const { return classID(); }

  void val(int i) { m_val = i; }
  int val() const { return m_val; }

private:
  int m_val;
};
int GaudiDataObj::count = 0;

class WrongType {};

class AbstractType {
public:
  virtual ~AbstractType() {}
  virtual void abstractMethod()=0;
};

class AbstractDataObj : public DataObject {
public:
  static const CLID& classID() {
    static const CLID _ID = 8011;
    return _ID; }
  virtual const CLID& clID() const { return classID(); }
  virtual void abstractMethod()=0;
};

class MyDataObj {
 
public: 
  static int count;

  MyDataObj(): m_val(0) { ++count; };
  MyDataObj(int i): m_val(i) { ++count; }
  ~MyDataObj(){ --count; }

  void val(int i) { m_val = i; }
  int val() const { return m_val; }

private:
  int m_val;
};
int MyDataObj::count = 0;

#include "SGTools/CLASS_DEF.h"
CLASS_DEF(MyDataObj, 8000, 3) 

#include <cassert>
#include <vector>
#include <map>

#include "TestTools/initGaudi.h"

//CLASSID_DEF(int, 34)
//CLASSVERSION_DEF(int, 5)
// CLASS_DEF(int, 8105, 1)
// CLASS_DEF(std::vector<int>, 8002, 1)
// CLASS_DEF2(std::map<int, float>, 8003, 1)
CLASS_DEF(AbstractType, 8004, 1)
CLASS_DEF(WrongType, 8005, 1)

struct X1
{
  ~X1() {}
  int a;
};

struct X2
  : public virtual X1
{
  int b;
};

CLASS_DEF(X1, 8011, 1)
CLASS_DEF(X2, 8012, 1)
SG_BASE(X2, X1);

CLASS_DEF( const int* , 206871866 , 1 )

class X3
{
public:
  int a;
};
CLASS_DEF(X3, 8013, 1)


class XCopyConversion
  : public SG::CopyConversion<X1, X3>
{
public:
  void convert (const X1& src, X3& dst) const
  { dst.a = src.a; }
};

SG_ADD_COPY_CONVERSION(X1, XCopyConversion);

class X4
{
public:
  int a;
};
CLASS_DEF(X4, 8014, 1)


class X5
{
public:
  X5 (int the_a) : a(the_a){}
  ~X5() { log.push_back (a); }
  int a;
  static std::vector<int> log;
};
std::vector<int> X5::log;
CLASS_DEF(X5, 8015, 1)


class XLock : public ILockable
{
public:
  XLock() : m_locked (false) {}
  void lock() { m_locked = true; std::cout << "lock\n"; }
  bool m_locked;
};
CLASS_DEF(XLock, 8114, 1)



class TestRegisterTransient
  : public IRegisterTransient
{
public:
  virtual void registerTransient (void* trans) { m_xtrans.push_back (trans); }

  std::vector<void*> m_xtrans;
};



// Test copying conversion.
void test2()
{
  std::cout << "test2\n";
  TestRegisterTransient trt;

  X1* x1 = new X1;
  x1->a = 10;
  DataObject* xbucket = asStorable(x1);

  const X3* x3 = Storable_cast<X3>(xbucket, false, &trt);
  assert ((char*)x3 != (char*)x1);
  assert (x3->a == 10);

  assert (trt.m_xtrans.size() == 1);
  assert (trt.m_xtrans[0] == x3);

  const X4* x4 = Storable_cast<X4>(xbucket, true, &trt);
  assert (x4 == 0);

  assert (Storable_cast<X3>(xbucket, false, &trt, false) == 0);

  SG::DataBucket<X1>* xb = dynamic_cast<SG::DataBucket<X1>*> (xbucket);
  assert (xb->cast (ClassID_traits<X3>::ID(), &trt) == x3);
  assert (xb->cast (typeid(X3), &trt) == x3);

  assert (xb->cast (ClassID_traits<X4>::ID(), &trt) == 0);
  assert (xb->cast (typeid(X4), &trt) == 0);

  assert (xb->cast (ClassID_traits<X3>::ID(), &trt, false) == 0);
  assert (xb->cast (typeid(X3), &trt, false) == 0);
}


// Test lock()
void test3()
{
  std::cout << "test3\n";

  X1* x1 = new X1;
  XLock* xlock = new XLock;

  DataObject* b1 = asStorable (x1);
  DataObject* b2 = asStorable (xlock);

  DataBucketBase* bb1 = dynamic_cast<DataBucketBase*> (b1);
  DataBucketBase* bb2 = dynamic_cast<DataBucketBase*> (b2);

  bb1->lock();
  assert (!xlock->m_locked);
  bb2->lock();
  assert (xlock->m_locked);
}


int main () {
  ISvcLocator* pDum;
  Athena_test::initGaudi(pDum); //need the MessageSvc
  assert( pDum );
  DataBucket<int> intBucket(0);
  DataBucket<const int*> pintBucket(0);
  DataBucket<vector<int> > vintBucket(0);
  cerr << "int has_classID " <<  intBucket.clID() << " version "
       << ClassID_traits<int>::s_version << " and " 
       << (ClassID_traits<int>::s_isDataObject ? "does" : "does not")
       << " inherit from DataObject" <<endl;
  cerr << "const int* has_classID " <<  pintBucket.clID() <<endl;
  assert(intBucket.clID() == pintBucket.clID());

  cerr << "vector<int> has_classID " <<  vintBucket.clID() <<endl;
  DataBucket<GaudiDataObj> gdobjBucket(0);
  cerr << "GaudiDataObj has_classID " <<  gdobjBucket.clID() << " and " 
       << (ClassID_traits<GaudiDataObj>::s_isDataObject ? "does" : "does not")
       << " inherit from DataObject" <<endl;
  assert(gdobjBucket.clID() == GaudiDataObj::classID());
  assert(ClassID_traits<GaudiDataObj>::s_isDataObject);

  DataBucket<MyDataObj> dobjBucket(0);
  cerr << "MyDataObj has_classID " <<  dobjBucket.clID() << " and " 
       << (ClassID_traits<MyDataObj>::s_isDataObject ? "does" : "does not")
       << " inherit from DataObject" <<endl;
  assert(dobjBucket.clID() == ClassID_traits<MyDataObj>::ID());
  assert(!ClassID_traits<MyDataObj>::s_isDataObject);

  DataBucket<AbstractDataObj> absdobjBucket(0);
  cerr << "AbstractDataObj has_classID " <<  absdobjBucket.clID() <<endl;
  assert(absdobjBucket.clID() == AbstractDataObj::classID());
  assert(ClassID_traits<AbstractDataObj>::s_isDataObject);

  DataBucket<AbstractType> absBucket(0);
  cerr << "AbstractType has_classID " <<  absBucket.clID() <<endl;
  assert(absBucket.clID() == ClassID_traits<AbstractType>::ID());
  assert(!ClassID_traits<AbstractType>::s_isDataObject);

  assert("int" == ClassID_traits<int>::typeName());
  assert("GaudiDataObj" == ClassID_traits<GaudiDataObj>::typeName());
  if("std::map<int,float>" != 
     ClassID_traits<map<int, float> >::typeName()) {
    cerr << "error checking type name for map<int,float>: ClassID has it as " << ClassID_traits<map<int, float> >::typeName() <<endl;
    assert(0);
  }

  MyDataObj* pMDO = new MyDataObj(1);
  MyDataObj* pRes(0);

  DataObject* pBucket(asStorable(pMDO));
  assert(fromStorable(pBucket, pRes));
  //FIXME asStorable above creates a DataBucket which is not deleted by anybody
  WrongType* pWrong(0);
  SGASSERTERROR(fromStorable(pBucket, pWrong));
  assert(pMDO == pRes);
  assert(pRes->val()==1);

  pRes = 0;
  //  static const bool QUIET(true);
  static const bool VERBOSE(false);
  assert(0 != (pRes = Storable_cast<MyDataObj>(pBucket, VERBOSE)));
  
  cerr << "Now we expect to see an error message:" << endl 
	    << "----Error Message Starts--->>" << endl; 
  pWrong = Storable_cast<WrongType>(pBucket, VERBOSE);
  assert(0 == pWrong);
  cerr<< "<<---Error Message Ends-------" << endl;
  cerr << "Now we expect to see an error message:" << endl 
	    << "----Error Message Starts--->>" << endl; 
  pWrong = Storable_cast<WrongType>((DataObject*)0);
  assert(0 == pWrong);
  cerr<< "<<---Error Message Ends-------" << endl;
  delete pBucket;
	 
  GaudiDataObj* pGDO = new GaudiDataObj(2);
  GaudiDataObj* pGRes(0);
  DataObject* DBGDO(asStorable(pGDO));
  assert(fromStorable(DBGDO, pGRes));
  SGASSERTERROR(fromStorable(DBGDO, pWrong));
  assert(pGDO == pGRes);
  assert(pGRes->val()==2);

  DataObject* pDO(0);
  assert (0 != (pDO = asStorable(new GaudiDataObj(3))));
  pGRes = 0;
  assert(0 != (pGRes = Storable_cast<GaudiDataObj>(pDO, VERBOSE)));

  const DataObject& rDO(*pDO);
  const GaudiDataObj& rGRes(Storable_cast<GaudiDataObj>(rDO, VERBOSE));
  
  try {
    cerr << "Now we expect to see an error message:" << endl 
              << "----Error Message Starts--->>" << endl; 
    const WrongType& rWr = Storable_cast<WrongType>(rGRes, VERBOSE);
    pWrong=const_cast<WrongType*>(&rWr); //remove warning
  } catch (bad_Storable_cast ) {
    cerr << "<<---Error Message Ends-------" << endl;
  }

  const DataObject* cpDO(pDO);
  const GaudiDataObj* cpGRes(Storable_cast<GaudiDataObj>(cpDO, VERBOSE));
  assert( 0 != cpGRes );
  delete pDO;
  delete DBGDO;

  //----------------------------------------------------------
  X2* x2 = new X2;
  X1* x1 = x2;
  assert ((long)x1 != (long)x2);
  DataObject* xbucket = asStorable(x2);
  X1* x1b = 0;
  assert (fromStorable (xbucket, x1b));
  assert (x1 == x1b);
  delete xbucket;
  //----------------------------------------------------------
  // Test refcounting.
  int c1 = GaudiDataObj::count;
  int c2 = MyDataObj::count;
  GaudiDataObj* gdo = new GaudiDataObj;
  assert(1 == gdo->addRef());
  DataBucketBase* b1 = dynamic_cast<DataBucketBase*>(asStorable(gdo));
  assert(1 == b1->addRef());
  assert(1 == b1->refCount());
  assert(2 == gdo->refCount());  //notice how DB and DO have indep refcounts
  assert(1 == gdo->release());
  assert(1 == gdo->refCount());
  assert(1 == b1->refCount());
  assert(0 == b1->release());
  assert (c1 == GaudiDataObj::count);
  //another combination
  gdo = new GaudiDataObj;
  assert(1 == gdo->addRef());
  DataObject* b2(asStorable(gdo));
  assert(3 == gdo->addRef());
  delete b2; //this must not delete gdo, only decrease its refcount
  assert(2 == gdo->refCount()); 
  assert (c1 + 1 == GaudiDataObj::count);
  //this is simpler: no DataObj in play
  b2 = asStorable(new MyDataObj);
  delete b2;
  assert (c2 == MyDataObj::count);
  //cleanup
  delete gdo;
  //----------------------------------------------------------

#if __cplusplus > 201100
  {
    std::unique_ptr<X5> p (new X5(10));
    DataBucketBase* b3 = new DataBucket<X5> (std::move(p));
    assert (p.get() == 0);
    assert (X5::log.empty());
    delete b3;
    assert (X5::log == std::vector<int> {10});
    X5::log.clear();
  }

  {
    std::unique_ptr<X5> p (new X5(11));
    DataObject* b4 = asStorable (std::move(p));
    assert (p.get() == 0);
    assert (X5::log.empty());
    delete b4;
    assert (X5::log == std::vector<int> {11});
  }
#endif

  test2();
  test3();

  cerr << "*** DataBucket_test OK ***" <<endl;
  return 0;

}


