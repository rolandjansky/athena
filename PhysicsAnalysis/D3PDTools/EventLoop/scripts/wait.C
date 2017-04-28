/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void wait (const char *location)
{
  // do the actual wait
  EL::Driver::wait (location);
}

void wait (const char *location, unsigned time)
{
  // do the actual wait
  EL::Driver::wait (location, time);
}
