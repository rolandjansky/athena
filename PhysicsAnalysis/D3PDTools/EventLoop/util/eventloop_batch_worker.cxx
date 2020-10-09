/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <EventLoop/Worker.h>
#include <xAODRootAccess/Init.h>
#include <AsgMessaging/MessageCheck.h>

int main (int argc, char **argv)
{
  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  ANA_CHECK (xAOD::Init ());

  if (argc != 3)
  {
    ANA_MSG_ERROR ("invalid number of arguments");
    return -1;
  }

  unsigned job_id = 0;
  {
    std::istringstream str (argv[1]);
    if (!(str >> job_id))
    {
      ANA_MSG_ERROR ("failed to parse job_id: " << job_id);
      return -1;

    }
  }

  auto confFile = argv[2];

  EL::Worker worker;
  ANA_CHECK (worker.batchExecute (job_id, confFile));
  return 0;
}
