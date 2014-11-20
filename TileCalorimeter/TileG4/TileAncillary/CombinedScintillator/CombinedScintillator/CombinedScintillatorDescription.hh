/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for Scintilator between LAr & Tile
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************


#ifndef CombinedScintillatorDescription_H 
#define CombinedScintillatorDescription_H

#include "TileG4DetDescr/DetectorDescription.h"

class CombinedScintillatorDescription: public FADS::DetectorDescription 
{
public:  
  double rMin;       
  double rMax;          
  double dzSci;
  double phiPos;
  double phiNeg;          

  CombinedScintillatorDescription(std::string n);
};

#endif

