/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CINT__
#include <EventLoop/BackgroundWorker.h>
#endif

void background_worker (int socket)
{
  EL::BackgroundWorker::execute (socket);
}
