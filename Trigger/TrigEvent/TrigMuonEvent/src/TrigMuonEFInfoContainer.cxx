/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     TrigMuonEFInfoContainer.cxx
 Package : offline/Trigger/TrigEvent/TrigMuonEvent 
 
 AUTHORS:  Sergio Grancagnolo
 CREATED:  April 2008
 
 PURPOSE:  This is the Container Class for TrigMuonEFInfo Objects.
********************************************************************/ 
 
 // INCLUDE HEADER FILES:
 
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#ifdef HAVE_NEW_IOSTREAMS
#include <iomanip>
#include <iostream>
#else
#include <iomanip.h>
#include <iostream.h>
#endif
 
// print objects found in container
void TrigMuonEFInfoContainer::print()
{
 
  std::cout << "Number of Muon objects in container " 
	    << this->size() << std::endl;

}
