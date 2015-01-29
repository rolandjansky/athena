/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4ParticleTable.hh"

// FIXME: this is just a temporary hack to allow genCong with the UserActionSvc. Please do not use this class anywhere in your code

class G4InitMute{
  
 public:
  G4InitMute(){
    
    G4ParticleTable::GetParticleTable()->SetReadiness();
    
  };

  ~G4InitMute(){};

};

class G4InitUnmute{
  
 public:
  G4InitUnmute(){
    
    G4ParticleTable::GetParticleTable()->SetReadiness(false);
    
  };

  ~G4InitUnmute(){};

};
