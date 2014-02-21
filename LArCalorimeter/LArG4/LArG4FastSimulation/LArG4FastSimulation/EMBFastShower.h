/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBFastShower_h
#define EMBFastShower_h

#include <string>

#include "LArG4FastSimulation/LArFastShower.h"

class LArHitMaker;
class ParticleBounds;
class StoreGateSvc;
class ILArG4ShowerLibSvc;
class IFastSimDedicatedSD;

class EMBFastShower: public LArFastShower
{
 public:

  EMBFastShower(G4String);                    //!< Default constructor
  virtual ~EMBFastShower() {}

  virtual G4bool ForcedAccept(const G4FastTrack &); //!< If it returns true, the particle will be parameterized without further checks
  virtual G4bool ForcedDeny  (const G4FastTrack &); //!< If it returns true, the particle will be returned to G4 without further checks
  
 private: 

  ILArG4FastSimSvc* fastSimSvc();                    //!< Accessor for particle energy bounds


};

#endif

