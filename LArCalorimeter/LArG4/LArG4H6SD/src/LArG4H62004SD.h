/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LARG4H62004SD_H
#define LARG4H6SD_LARG4H62004SD_H

// Base class
#include "LArG4Code/LArG4SimpleSD.h"

// Forward declarations.
class ILArCalculatorSvc;
class G4Step;
class G4TouchableHistory;


class LArG4H62004SD : public LArG4SimpleSD
{
public:
  LArG4H62004SD(G4String name, ILArCalculatorSvc* calc, const std::string& type, const float width);
  virtual ~LArG4H62004SD() {}

  // The required functions for all sensitive detectors:
  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist);

};

#endif // LARG4H6SD_LARG4H62004SD_H
