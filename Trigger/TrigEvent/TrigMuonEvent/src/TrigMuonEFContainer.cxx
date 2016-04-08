/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     TrigMuonEFContainer.cxx
 Package : offline/Trigger/TrigEvent/TrigMuonEvent 
 
 AUTHORS:  Gabriella Cataldi
 CREATED:  November 2006 
 
 PURPOSE:  This is the Container Class for TrigMuonEF Objects.
********************************************************************/ 
 
 // INCLUDE HEADER FILES:
 
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include <iomanip>
#include <iostream>
 
 // print objects found in container
 void TrigMuonEFContainer::print()
 { 
 
   std::cout << "Number of Muon objects in container " 
             << this->size() << std::endl;
 
 }
 
