/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for LAr Crate in tile Gap
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// November 18, 2004
//
//************************************************************


#ifndef LArCrateDescription_H 
#define LArCrateDescription_H

#include "TileG4DetDescr/DetectorDescription.h"

class LArCrateDescription: public FADS::DetectorDescription 
{
public:  
  double zLength;       
  double yLength;          
  double xLength;          
  
  LArCrateDescription(std::string n);
};

#endif

