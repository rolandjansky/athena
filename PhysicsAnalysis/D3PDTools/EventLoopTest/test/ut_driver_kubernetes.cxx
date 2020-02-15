/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <EventLoop/Global.h>

#include <EventLoop/Job.h>
#include <EventLoop/KubernetesDriver.h>
#include <EventLoopTest/UnitTest.h>

//
// main program
//

using namespace EL;

int main ()
{
  KubernetesDriver driver;
  driver.options()->setString (Job::optDockerImage, "krumnack/el_docker_test");
  driver.options()->setString (Job::optBatchSetupFile, "EventLoopTest/kubernetes_test_setup.yml");
  driver.options()->setString (Job::optBatchConfigFile, "EventLoopTest/kubernetes_test_job.yml");
  driver.shellInit = "source setup.sh";
  UnitTest ut ("local_docker");
  ut.cleanup = false;
  ut.location = "$HOME/unit-test";
  return ut.run (driver);
}
