#ifndef ROOT_CORE_UTILS__ASSERT_H
#define ROOT_CORE_UTILS__ASSERT_H

//          Copyright Nils Krumnack 2010 - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a variety of assert style macros.  The
/// interface in this module is indended for experts only.  This module
/// is considered to be in the pre-alpha stage.

/// This module defines a variety of assert style macros for different
/// situations, i.e. macros that test whether a particular condition
/// has been met and that trigger an error if the condition is not met.
/// The exact error behavior depends on the configuration options as
/// well as the macro called.

/// The main distinction is the purpose of the test:
/// RCU_REQUIRE: checks on the arguments to a function
/// RCU_PROVIDE: checks on the results of a function
/// RCU_ASSERT: checks that happen in the middle of a function
/// RCU_INVARIANT: checks that happen in the invariant test

/// These can be further modified using postfixes:
/// _SLOW: tests that are in the critical path of a program
/// _SOFT: tests that are expected to fail in correctly implemented
///   programs, i.e. should trigger an exception
/// 2: tests that have a textual description of the condition
/// 0: tests that always fail


/// Furthermore there are also invariant tests in here (calling
///   MyClass::testInvariant()):
/// RCU_READ_INVARIANT: an invariant is read, but not modified
/// RCU_CHANGE_INVARIANT: an invariant is read and modified
/// RCU_NEW_INVARIANT: an invariant has been newly established
/// RCU_DESTROY_INVARIANT: an invariant is about to be decomissioned





//protect
#include <RootCoreUtils/Global.h>

namespace RCU
{
  namespace Check
  {
    enum Type
      {
	/// regular old style assertion
	assert_soft,
	assert_hard,
	assert_noimp,

	/// function parameter requirement
	require_soft,
	require_hard,
	require_noimp,

	/// function postcondition
	provide_soft,
	provide_hard,
	provide_noimp,

	/// invariant violation
	invariant
      };
    const int typeNum = invariant + 1;
    extern const char *typeLiteral [typeNum];
    extern bool typeAbort [typeNum];


    /// effects: report the error and abort either by exception or
    ///   assert like
    /// guarantee: all-fail
    /// failures: as requested
    /// requires: file != 0
    /// requires: line != 0
    /// requires: type < typeNum
    /// requires: error != 0
    /// availability: experts only
    void fail (const char *package, const char *file, unsigned line,
	       Type type, const char *error);


    /// effects: apply the test invariant method on an object
    /// guarantee: no-fail
    /// availability: experts only
    /// rationale: this is a templated wrapper for the class specific
    ///   invariant testers
    typedef void (*TestInvariantFunction) (const void *object);
    template<class T> void
    testInvariantVoid (const void *object)
    {
      static_cast<const T *>(object)->testInvariant ();
    }



    struct TestInvariant
    {
      /// availability: experts only
      /// description: this class will perform an invariant test as
      ///   the object goes in and out of scope.

      TestInvariant (TestInvariantFunction function,
		     const void *object);
      ~TestInvariant ();

    private:
      TestInvariantFunction m_function;
      const void *m_object;
    };



    /// effects: create the right invariant testing object
    /// returns: the tester
    template<class T> inline TestInvariant invariant_tester (T *object)
    {
      return TestInvariant (testInvariantVoid<T>, object);
    }
  }
}





#ifndef ROOTCORE_PACKAGE
#define ROOTCORE_PACKAGE 0
#endif

/// effects: perform a check
/// guarantee: strong
/// failures: !condition
#define RCU_CHECK(type,condition,message)				\
  (condition) ? (void) 0 : ::RCU::Check::fail (ROOTCORE_PACKAGE, __FILE__, __LINE__, ::RCU::Check::type, message)

/// effects: join together two tokens
#define RCU_CHECK_JOIN2(a,b)         RCU_CHECK_JOIN2_HIDDEN(a,b)
#define RCU_CHECK_JOIN2_HIDDEN(a,b)  a ## b



#define RCU_REQUIRE_SOFT(x)			\
  RCU_CHECK (require_soft,  x, #x)
#define RCU_REQUIRE2_SOFT(x,y)			\
  RCU_CHECK (require_soft,  x,  y)
#define RCU_REQUIRE0_SOFT(y)			\
  RCU_CHECK (require_soft,  0,  y)

#define RCU_PROVIDE_SOFT(x)			\
  RCU_CHECK (provide_soft,  x, #x)
#define RCU_PROVIDE2_SOFT(x,y)			\
  RCU_CHECK (provide_soft,  x,  y)
#define RCU_PROVIDE0_SOFT(y)			\
  RCU_CHECK (provide_soft,  0,  y)

#define RCU_ASSERT_SOFT(x)			\
  RCU_CHECK (assert_soft,   x, #x)
#define RCU_ASSERT2_SOFT(x,y)			\
  RCU_CHECK (assert_soft,   x,  y)
#define RCU_ASSERT0_SOFT(y)			\
  RCU_CHECK (assert_soft,   0,  y)



#define RCU_REQUIRE_NOIMP(x)			\
  RCU_CHECK (require_noimp,  x, #x)
#define RCU_REQUIRE2_NOIMP(x,y)			\
  RCU_CHECK (require_noimp,  x,  y)
#define RCU_REQUIRE0_NOIMP(y)			\
  RCU_CHECK (require_noimp,  0,  y)

#define RCU_PROVIDE_NOIMP(x)			\
  RCU_CHECK (provide_noimp,  x, #x)
#define RCU_PROVIDE2_NOIMP(x,y)			\
  RCU_CHECK (provide_noimp,  x,  y)
#define RCU_PROVIDE0_NOIMP(y)			\
  RCU_CHECK (provide_noimp,  0,  y)

#define RCU_ASSERT_NOIMP(x)			\
  RCU_CHECK (assert_noimp,   x, #x)
#define RCU_ASSERT2_NOIMP(x,y)			\
  RCU_CHECK (assert_noimp,   x,  y)
#define RCU_ASSERT0_NOIMP(y)			\
  RCU_CHECK (assert_noimp,   0,  y)



#ifndef NDEBUG

#define RCU_INVARIANT(x)			\
  RCU_CHECK (invariant, x, #x)
#define RCU_INVARIANT2(x,y)			\
  RCU_CHECK (invariant, x,  y)
#define RCU_INVARIANT0(y)			\
  RCU_CHECK (invariant, 0,  y)

#define RCU_REQUIRE(x)				\
  RCU_CHECK (require_hard,   x, #x)
#define RCU_REQUIRE2(x,y)			\
  RCU_CHECK (require_hard,   x,  y)
#define RCU_REQUIRE0(y)				\
  RCU_CHECK (require_hard,   0,  y)

#define RCU_PROVIDE(x)				\
  RCU_CHECK (provide_hard,   x, #x)
#define RCU_PROVIDE2(x,y)			\
  RCU_CHECK (provide_hard,   x,  y)
#define RCU_PROVIDE0(y)				\
  RCU_CHECK (provide_hard,   0,  y)

#define RCU_ASSERT(x)				\
  RCU_CHECK (assert_hard,    x, #x)
#define RCU_ASSERT2(x,y)			\
  RCU_CHECK (assert_hard,    x,  y)
#define RCU_ASSERT0(y)				\
  RCU_CHECK (assert_hard,    0,  y)

#define RCU_READ_INVARIANT(x)			\
  (x)->testInvariant ()
#define RCU_CHANGE_INVARIANT(x)						\
  ::RCU::Check::TestInvariant RCU_CHECK_JOIN2 (invariant, __LINE__) = ::RCU::Check::invariant_tester (x);
#define RCU_NEW_INVARIANT(x)			\
  (x)->testInvariant ()
#define RCU_DESTROY_INVARIANT(x)		\
  (x)->testInvariant ()



#else

#define RCU_INVARIANT(x)			\
  (void) 0
#define RCU_INVARIANT2(x,y)			\
  (void) 0
#define RCU_INVARIANT0(y)			\
  (void) 0
#define RCU_REQUIRE(x)				\
  (void) 0
#define RCU_REQUIRE2(x,y)			\
  (void) 0
#define RCU_REQUIRE0(y)				\
  (void) 0
#define RCU_PROVIDE(x)				\
  (void) 0
#define RCU_PROVIDE2(x,y)			\
  (void) 0
#define RCU_PROVIDE0(y)				\
  (void) 0
#define RCU_ASSERT(x)				\
  (void) 0
#define RCU_ASSERT2(x,y)			\
  (void) 0
#define RCU_ASSERT0(y)				\
  (void) 0
#define RCU_READ_INVARIANT(x)			\
  (void) 0
#define RCU_CHANGE_INVARIANT(x)			\
  (void) 0
#define RCU_NEW_INVARIANT(x)			\
  (void) 0
#define RCU_DESTROY_INVARIANT(x)		\
  (void) 0

#endif





#ifdef RCU_DEBUG_SLOW

#define RCU_REQUIRE_SLOW(x)			\
  RCU_REQUIRE(x)
#define RCU_REQUIRE2_SLOW(x,y)			\
  RCU_REQUIRE2(x,y)
#define RCU_PROVIDE_SLOW(x)			\
  RCU_PROVIDE(x)
#define RCU_PROVIDE2_SLOW(x,y)			\
  RCU_PROVIDE2(x,y)
#define RCU_ASSERT_SLOW(x)			\
  RCU_ASSERT(x)
#define RCU_ASSERT2_SLOW(x,y)			\
  RCU_ASSERT2(x,y)
#define RCU_READ_INVARIANT_SLOW(x)		\
  RCU_READ_INVARIANT (x)
#define RCU_CHANGE_INVARIANT_SLOW(x)		\
  RCU_CHANGE_INVARIANT (x)
#define RCU_NEW_INVARIANT_SLOW(x)		\
  RCU_NEW_INVARIANT (x)
#define RCU_DESTROY_INVARIANT_SLOW(x)		\
  RCU_DESTROY_INVARIANT (x)

#else

#define RCU_REQUIRE_SLOW(x)			\
  (void) 0
#define RCU_REQUIRE2_SLOW(x,y)			\
  (void) 0
#define RCU_PROVIDE_SLOW(x)			\
  (void) 0
#define RCU_PROVIDE2_SLOW(x,y)			\
  (void) 0
#define RCU_ASSERT_SLOW(x)			\
  (void) 0
#define RCU_ASSERT2_SLOW(x,y)			\
  (void) 0
#define RCU_READ_INVARIANT_SLOW(x)		\
  (void) 0
#define RCU_CHANGE_INVARIANT_SLOW(x)		\
  (void) 0
#define RCU_NEW_INVARIANT_SLOW(x)		\
  (void) 0
#define RCU_DESTROY_INVARIANT_SLOW(x)		\
  (void) 0

#endif


#endif
