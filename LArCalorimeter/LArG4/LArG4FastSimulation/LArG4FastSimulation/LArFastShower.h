/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArFastShower_h
#define LArFastShower_h

#include <string>
#include <fstream>
#include <map>

#include "G4FastSimulation/FastSimModel.h"

class LArHitMaker;
class StoreGateSvc;
class ILArG4ShowerLibSvc;
class ILArG4FastSimSvc;
class IFastSimDedicatedSD;

namespace HepMC {
	class GenEvent;
}

class LArFastShower: public FastSimModel
{
 public:

  LArFastShower(G4String);                    //!< Default constructor

  virtual ~LArFastShower();                           //!< Default destructor
  
  /** Determines the applicability of the fast sim model to this particle type
  Called once for each track picked up by Geant as it enters a region with the fast sim assigned to it.
  Currently returns false for all but photons and electrons.*/
  G4bool IsApplicable(const G4ParticleDefinition&);

  /** Determines the applicability of the fast sim model to this particular track.
  Checks that geometric location, energy, and particle type are within bounds.  Also checks for
  containment of the particle's shower within a specific detector region.*/
  virtual G4bool ModelTrigger(const G4FastTrack &);

  virtual G4bool ForcedAccept(const G4FastTrack &) //!< If it returns true, the particle will be parameterized without further checks
  {return false;}
  virtual G4bool ForcedDeny  (const G4FastTrack &) //!< If it returns true, the particle will be returned to G4 without further checks
  {return false;}

  /** Assigns the track to the appropriate method for application of the fast simulation.
  Only called if ModelTrigger returns true.*/
  void DoIt(const G4FastTrack&, G4FastStep&);

 protected: 

  ILArG4FastSimSvc* m_fastSimSvc;                     //!< The particle bounds for this particular particle
  virtual ILArG4FastSimSvc* fastSimSvc()=0;         //!< Accessor for the above

  IFastSimDedicatedSD *m_FastSimDedicatedSD;               //!< Shower library sensitive detector for this shower

  ILArG4ShowerLibSvc* showerLibSvc( );             //!< Accessor for the shower library service
  ILArG4ShowerLibSvc* m_showerLibSvc;            //!< Pointer to the shower library service

  // Method to kill a particle and deposit its energy using exponential decay function
  void KillParticle(const G4FastTrack&, G4FastStep&);    //!< Simple kill function for low energy particles
  void UseShowerLib(const G4FastTrack&, G4FastStep&);    //!< Function for the application of shower library

  virtual G4bool CheckContainment(const G4FastTrack &fastTrack); //!< Function to check the containment of a shower within a regular detector region

  HepMC::GenEvent * GetGenEvent(const G4FastTrack &fastTrack);

 private:
  int m_eventNum;
  std::map<int,bool> m_applicableMap;

};

#endif

