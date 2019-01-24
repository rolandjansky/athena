/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <EventLoopGrid/GridWorker.h>
#include <TROOT.h>
#include <iostream>
#include <string>
#include <xAODRootAccess/Init.h>
#include <AsgTools/MessageCheck.h>

int main (int argc, char **argv)
{
  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  ANA_CHECK (xAOD::Init ());

  if (argc != 2)
  {
    ANA_MSG_ERROR ("invalid number of arguments");
    return -1;
  }

  std::string sampleName = argv[1];

  EL::GridWorker::execute (sampleName);
}
