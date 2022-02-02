#include "CxxUtils/checker_macros.h"
#include "TrkExTools/ObjContainer.h"
#include <iostream>
#include <memory>
#include <atomic>

class TestObj {
public:
   TestObj()                     { ++s_ctorCounter; std::cout << "TestObj::ctor " << static_cast<const void *> (this) << std::endl;}
   TestObj(const TestObj &)      { ++s_ctorCounter; std::cout << "TestObj::ctor " << static_cast<const void *> (this) << std::endl;}
   TestObj(TestObj &&)            noexcept { ++s_ctorCounter; std::cout << "TestObj::ctor " << static_cast<const void *> (this) << std::endl;}
   ~TestObj()                    { ++s_dtorCounter; std::cout << "TestObj::DTOR " << static_cast<const void *> (this) << std::endl;}

   static bool isCleanedup() {
      if (s_ctorCounter != s_dtorCounter) {
         std::cout << "TestObj::isCleanedup: constructor and destructor count mismatch: "
                   << s_ctorCounter << " != " << s_dtorCounter << std::endl;
      }
      return s_ctorCounter == s_dtorCounter;
   }
   static void stat(const std::string& head="") {
      std::cout << head
                << "TestObj::stat: constructor and destructor counts "
                << s_ctorCounter << " : " << s_dtorCounter
                << (s_ctorCounter == s_dtorCounter ? " match" : " DO NOT match")
                << "."
                << std::endl;
   }

private:
   static std::atomic<unsigned int> s_ctorCounter ATLAS_THREAD_SAFE;
   static std::atomic<unsigned int> s_dtorCounter ATLAS_THREAD_SAFE;
};

std::atomic<unsigned int> TestObj::s_ctorCounter;
std::atomic<unsigned int> TestObj::s_dtorCounter;

template <> TestObj *cloneObj<TestObj>( const TestObj *a) { return (a) ? new TestObj(*a) : nullptr; }
std::unique_ptr<TestObj> uniqueForTest( const TestObj *a) { return (a) ? std::make_unique<TestObj>(*a) : nullptr; }

using Container  = ObjContainer<const TestObj>;
using Ptr        = ObjRef<>;
using GuardedPtr = ObjPtr<const TestObj>;

// NOTE return Ptr does not provide any guarantee that the object
//   remains alive !!
Ptr func(Container &container, Ptr in) {
   return GuardedPtr(container,in).index();
}

void test0()
{
   Container container;

   TestObj external_obj;
   GuardedPtr gptr(container,external_obj);
   [[maybe_unused]] Ptr out (func(container,gptr.index()));
}

void test1()
{
   Container container;

   TestObj external_obj;
   GuardedPtr eobj(container,external_obj);
   (void) eobj;
}

void test2()
{
   Container container;

   TestObj external_obj;
   GuardedPtr eobj(container, external_obj);
   GuardedPtr clone( GuardedPtr::recapture(eobj, uniqueForTest(eobj.get()) ));
   (void) clone;
}



const TestObj *test1HelperG(const TestObj &external_obj) {
   Container container;
   GuardedPtr ret(container);
   {
   GuardedPtr ptr(container,external_obj);
   GuardedPtr new_obj2(GuardedPtr::recapture(GuardedPtr(container),std::make_unique<TestObj>()));
   GuardedPtr shared_obj1(new_obj2);
   shared_obj1 = GuardedPtr::recapture(GuardedPtr(container),std::make_unique<TestObj>());
   ret=shared_obj1;
   }
   return ret.release();
}

void test1G()
{

   TestObj external_obj;
   std::unique_ptr<const TestObj> result(test1HelperG(external_obj));
   TestObj::stat("INFO ");
   (void) result.get();
}

const TestObj *test3HelperG(const TestObj &external_obj) {
   Container container;
   GuardedPtr ret(container);
   {
   GuardedPtr ptr(container,external_obj);
   GuardedPtr empty(container);
   GuardedPtr ptr2( GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr new_obj1(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr new_obj4(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr new_obj2(GuardedPtr::recapture(empty,std::make_unique<TestObj>()));
   GuardedPtr new_obj3(GuardedPtr::recapture(empty,std::make_unique<TestObj>()));
   GuardedPtr shared_obj3(new_obj2); 
   shared_obj3 = new_obj4;
   shared_obj3 = GuardedPtr::recapture(empty,std::make_unique<TestObj>());

   TestObj::stat("INFO ");
   ret=shared_obj3;
   }
   return ret.release();
}

void test3G()
{

   TestObj external_obj;
   std::unique_ptr<const TestObj> result(test3HelperG(external_obj));
   TestObj::stat("INFO ");
   (void) result.get();
}

const TestObj *test5HelperG(const TestObj &external_obj) {
   Container container;
   GuardedPtr ret(container);
   {
   GuardedPtr eobj(container,external_obj);
   GuardedPtr empty(container);
   GuardedPtr nobj(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));

   GuardedPtr shared_nobj(nobj);
   GuardedPtr shared_eobj2(eobj);
   GuardedPtr shared_nobj2(nobj);
   GuardedPtr nobj3(GuardedPtr::recapture(empty, uniqueForTest(&external_obj)));

   shared_nobj  = GuardedPtr::recapture(empty, std::make_unique<TestObj>());
   TestObj::stat("INFO ");
   ret=shared_nobj2;
   }
   return ret.release();
}

void test5G()
{

   TestObj external_obj;
   std::unique_ptr<const TestObj> result(test5HelperG(external_obj) );
   TestObj::stat("INFO ");
   (void) result.get();
}


const TestObj *ret_func(Container &container_in, Ptr idx) {
   GuardedPtr obj(container_in, idx);
   return obj.get();
}

const TestObj *test6Helper(const TestObj &external_obj) {
   Container container;
   GuardedPtr ret(container);
   {
   GuardedPtr eobj(container,external_obj);
   GuardedPtr empty(container);
   GuardedPtr nobj(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr nobj2(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr nobj3(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr nobj4(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr nobj5(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr nobj6(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   TestObj::stat("INFO ");
   ret=nobj4;
   }
   TestObj::stat("INFO ");
   return ret.release();
}

void test6()
{

   TestObj external_obj;
   std::unique_ptr<const TestObj> result(test6Helper(external_obj) );
   TestObj::stat("INFO ");
   (void) result.get();
}


const TestObj *test7Helper(const TestObj &external_obj) {
   Container container;
   GuardedPtr ret(container);
   {
   GuardedPtr empty(container);
   GuardedPtr nobj(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   GuardedPtr nobj2(GuardedPtr::recapture(empty,uniqueForTest(&external_obj)));
   if (nobj==nobj2) {
      std::abort();
   }
   if (nobj.index()==nobj2.index()) {
      std::abort();
   }
   
   GuardedPtr nobj3(nobj);
   if (nobj!=nobj3) {
      std::abort();
   }
   if (nobj.index()!=nobj3.index()) {
      std::abort();
   }

   nobj2=std::move(nobj);
   TestObj::stat("INFO ");
   ret=nobj2;
   }
   TestObj::stat("INFO ");
   return ret.release();
}

void test7 ATLAS_NOT_THREAD_SAFE()
{

   TestObj external_obj;
   std::unique_ptr<const TestObj> result(test7Helper(external_obj) );
   TestObj::stat("INFO ");
   (void) result.get();
}


bool run(const std::string &label, void (*func_p)() ) {
   (*func_p)();
   TestObj::stat();
   std::cout << " --> " << (TestObj::isCleanedup() ? "passed" : "FAILED") << " " << label << std::endl;
   return TestObj::isCleanedup();
}

#define RUN(a)  run(#a,&a)

int main() {
   bool passed=true;
   passed &= RUN(test0);
   passed &= RUN(test1);
   passed &= RUN(test1G);
   passed &= RUN(test2);
   passed &= RUN(test3G);
   passed &= RUN(test5G);
   passed &= RUN(test6);
   passed &= RUN(test7);
   TestObj::stat();
   return passed ? 0 : 1;
}
