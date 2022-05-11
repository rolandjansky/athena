/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "../share/RunPileUpOverlayReuseToys.C"
#include "TRint.h"

int main( int argc, char* argv[] ) 
{
  RunPileUpOverlayReuseToys();
  
  TRint *theApp = new TRint("Rint", &argc, argv);

  // and enter the event loop...
  theApp->Run();

  delete theApp;

  return 0;
}
