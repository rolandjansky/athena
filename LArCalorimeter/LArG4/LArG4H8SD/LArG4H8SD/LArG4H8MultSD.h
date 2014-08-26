/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H8MultSD_H__
#define __LArG4H8MultSD_H__

#include "LArG4SD/LArG4MultSD.h"
#include <vector>

// Forward declarations.
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class IMessageSvc;

class LArG4H8MultSD : public LArG4MultSD
{
public:
  LArG4H8MultSD(G4String name);
  virtual ~LArG4H8MultSD();
private:
};

#endif // __LArG4H8MultSD_H__
