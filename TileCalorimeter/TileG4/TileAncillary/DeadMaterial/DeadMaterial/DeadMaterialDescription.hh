/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for DeadMaterial
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************


#ifndef DeadMaterialDescription_H 
#define DeadMaterialDescription_H

#include "TileG4DetDescr/DetectorDescription.h"

class DeadMaterialDescription: public FADS::DetectorDescription 
{
public:  
  double zLength;       
  double yLength;          
  double xLength;          

  DeadMaterialDescription(std::string n);
};

#endif

