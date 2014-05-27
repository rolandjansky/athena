#ifndef ROOT_CORE_UTILS_UNIT_TEST_DIR_HH
#define ROOT_CORE_UTILS_UNIT_TEST_DIR_HH

//          Copyright Nils Krumnack 2011 - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a class that manages a temporary directory for
/// unit tests.  The interface provided in this module is intended for
/// experts only.  The module is considered to be in the pre-alpha
/// stage.



#include <RootCoreUtils/Global.h>

#include <string>

namespace RCU
{
  class UnitTestDir
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory
    /// failures: directory creation errors
  public:
    UnitTestDir (const std::string& package, const std::string& name);


    /// rationale: I'm making these private to avoid copying
  private:
    UnitTestDir (const UnitTestDir&);
    UnitTestDir& operator = (const UnitTestDir&);


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~UnitTestDir ();


    /// description: the path to the directory
    /// guarantee: no-fail
  public:
    const std::string& path () const;


    /// description: whether we clean up on completion
    /// guarantee: no-fail
  public:
    bool cleanup () const;
    void cleanup (bool val_cleanup);



    //
    // private interface
    //

    /// description: members directly corresponding to accessors
  private:
    std::string m_path;
  private:
    bool m_cleanup;
  };
}

#endif
