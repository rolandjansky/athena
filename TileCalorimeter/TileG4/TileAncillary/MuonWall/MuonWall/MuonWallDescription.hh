/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for MuonWall
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************


#ifndef MuonWallDescription_H 
#define MuonWallDescription_H

#include "TileG4DetDescr/DetectorDescription.h"

class MuonWallDescription: public FADS::DetectorDescription 
{
public:  
  double zLength;       
  double yLength;          
  double xLength;          

  MuonWallDescription(std::string n);
};

#endif

