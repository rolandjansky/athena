/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////// 
// class TileEscapedEnergyProcessing
//
// Author: Gia Khoriauli  <gia@mail.cern.ch>
// May, 2005
//
// This manages processing of those particles, which were created 
// in Tile region and escaped from World later
//
///////////////////////////////////////////////////////////////////

#ifndef TileG4_TileEscapedEnergyProcessing_H
#define TileG4_TileEscapedEnergyProcessing_H

// The SimulationEnergies class provides a common procedure for
// categorizing the energy deposited in a given G4Step.  However,
// special processing is required for escaped energy.

// The issue is that, if a particle's energy is lost by escaping the
// simulation, you don't want to record that energy in the volume it
// escapes; you want to record that energy in the volume in which the
// particle was created.  Neutrinos are a good example of this.

// In effect, the SimulationEnergies class has to issue an "interrupt"
// to some other volume than the current G4Step, telling that other
// volume to accumulate the energy of the escaped particle.

// This class contains the processing to handle the escaped energy
// processing for volumes in the Tile portion of the simulation.

#include "CaloG4Sim/VEscapedEnergyProcessing.h"

#include "G4TouchableHandle.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class G4Step;
class IMessageSvc;
class MsgStream;

class TileEscapedEnergyProcessing : public CaloG4::VEscapedEnergyProcessing
{
  public:
    TileEscapedEnergyProcessing(IMessageSvc* m_msgSvc);
    virtual ~TileEscapedEnergyProcessing();

    // Method: The G4TouchableHandle to the volume in which "point" is
    // located; the value of "point" itself in case additional
    // processing is necessary, and the amount of escaped energy.
    virtual G4bool Process( G4TouchableHandle& handle, G4ThreeVector& point, G4double energy );

    //After escaped particle is detected and its 'mother' step is created
    //this method will retrieve the respective SD for the volume where escaped
    //particle was created and invokes SD::ProcessHits() - method.		
    G4bool Call_SD_ProcessHits( G4Step* aStep, G4String &SDname);

    void   SetEscapedFlag(bool);
    void   SetEnergy5(double);
    void   SetEscapedEnergy(double);

    bool   GetEscapedFlag();
    double GetEnergy5();
    double GetEscapedEnergy();


private:

    // takes the flag that particle has escaped,
    // the 'mother' step of escaped particle and
    // amount of that energy, wich can't be calculated
    // by Simulationenergies
    bool   m_escaped;
    double m_escapedEnergy;
    double m_energy5;
    MsgStream * m_log;

};

inline void   TileEscapedEnergyProcessing::SetEscapedFlag(bool escaped)           {m_escaped=escaped;}
inline void   TileEscapedEnergyProcessing::SetEnergy5(double ene5)                {m_energy5=ene5;}
inline void   TileEscapedEnergyProcessing::SetEscapedEnergy(double escapedEnergy) {m_escapedEnergy=escapedEnergy;}

inline bool   TileEscapedEnergyProcessing::GetEscapedFlag()   {return m_escaped;}
inline double TileEscapedEnergyProcessing::GetEnergy5()       {return m_energy5;}
inline double TileEscapedEnergyProcessing::GetEscapedEnergy() {return m_escapedEnergy;}



#endif // TileG4_TileEscapedEnergyProcessing_H

