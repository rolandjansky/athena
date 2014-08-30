/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     TrigMuonEFInfoTrackContainer.cxx
 Package : offline/Trigger/TrigEvent/TrigMuonEvent

 AUTHORS:  Sergio Grancagnolo, Alexander Oh
 CREATED:  April 2008, Feb 2009

 PURPOSE:  This is the Container Class for TrigMuonEFInfoTrack Objects.
********************************************************************/

 // INCLUDE HEADER FILES:

#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#ifdef HAVE_NEW_IOSTREAMS
#include <iomanip>
#include <iostream>
#else
#include <iomanip.h>
#include <iostream.h>
#endif

// print objects found in container
void TrigMuonEFInfoTrackContainer::print()
{

  std::cout << "Number of Muon Track objects in container "
	    << this->size() << std::endl;

}
