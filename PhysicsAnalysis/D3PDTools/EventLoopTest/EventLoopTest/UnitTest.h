/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_UNIT_TEST_HH
#define EVENT_LOOP_UNIT_TEST_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines functions for running unit test for different
/// architectures.  The interface provided in this module is intended
/// for experts only.  The module is considered to be in the pre-alpha
/// stage.



#include <EventLoopTest/Global.h>

#include <string>
#include <EventLoop/Global.h>
#include <SampleHandler/SampleHandler.h>

namespace EL
{
  struct UnitTest
  {
    /// description: the name of the unit test
    std::string name;

    /// description: the base path for the data files
    std::string base_path;

    /// \brief whether to scan for number of events
    bool scanNEvents {false};

    /// \brief whether to use the inputs from the grid dataset
    bool gridInput {false};

    /// description: whether we clean up the submit directory
    ///   afterwards
    bool cleanup;

    /// description: whether to test creating output n-tuples
    bool testOutput;

    /// description: the location for the unit test
    std::string location;

    /// description: the output disk
    SH::DiskOutput *outputDisk;

    /// \brief whether to test the fileExecute method
    bool testFileExecute;


    /// effects: standard constructor
    /// guarantee: storng
    /// failures: out of memory II
    UnitTest (const std::string& val_name, std::string base_path = "");


    /// effects: perform a unit test with the given driver
    /// returns: EXIT_SUCCESS on success, EXIT_FAILURE on failure
    /// guarantee: basic
    /// failures: unit test failures
    /// requires: !sample.empty()
    int run (const Driver& driver) const;
  };
}

#endif
