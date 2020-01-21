/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/Global.h>

#include <EventLoop/Job.h>
#include <EventLoop/LocalDriver.h>
#include <EventLoopTest/UnitTest.h>

//
// main program
//

using namespace EL;

int main ()
{
  LocalDriver driver;
  driver.options()->setString (Job::optDockerImage, "el_docker_test");
  driver.shellInit = "source setup.sh";
  driver.options()->setString (Job::optDockerOptions, "-v $HOME/cache/local:$HOME/cache/local");
  UnitTest ut ("local_docker");
  ut.cleanup = false;
  ut.location = "$HOME/DockerDriverTest";
  return ut.run (driver);
}
