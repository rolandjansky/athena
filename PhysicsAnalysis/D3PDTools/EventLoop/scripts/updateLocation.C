/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void updateLocation (const char *location)
{
  // load the libraries for all packages
  gROOT->ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C");

  // do the actual updateLocation
  EL::Driver::updateLocation (location);
}
