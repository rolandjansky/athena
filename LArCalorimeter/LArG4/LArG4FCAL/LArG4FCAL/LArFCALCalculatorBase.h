/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArFCALCalculator
// 20-Feb-2003 Bill Seligman (from Peter Loch)

// A base class for the calculators required for the individual
// modules.

#ifndef LArFCALCalculatorBase_H
#define LArFCALCalculatorBase_H

//-----------------------------------------------------------------------------
//
//             Forward Calorimeter Construction in GEANT4
//
//-----------------------------------------------------------------------------
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"
#include "globals.hh" 
#include <stdexcept>
class LArG4BirksLaw;
class FCALModule;

class LArFCALCalculatorBase : public LArVCalculator
{
 public:
  // constructor
  LArFCALCalculatorBase();
  // destructor
  virtual ~LArFCALCalculatorBase();

  /////////////////////////////////////////////
  // The interface for LArVCalculator.

  virtual G4float OOTcut() const { return m_OOTcut; }

  virtual G4bool Process(const G4Step* a_step){return  Process(a_step, m_hdata);}
  virtual G4bool Process(const G4Step*, std::vector<LArHitData>&); 

  virtual const LArG4Identifier& identifier(int i=0) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    if(m_hdata.size()<1) throw std::range_error("No hit yet");
    return m_hdata[0].id; 
  }

  virtual G4double time(int i=0) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    if(m_hdata.size()<1) throw std::range_error("No hit yet");
    return m_hdata[0].time;  
  }
  virtual G4double energy(int i=0) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    if(m_hdata.size()<1) throw std::range_error("No hit yet");
    return m_hdata[0].energy; 
  };
  virtual G4bool isInTime(int i=0) const    { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return     m_isInTime; 
  }
  virtual G4bool isOutOfTime(int i=0) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return ( ! m_isInTime ); 
  }
  /////////////////////////////////////////////


 protected:

  // Store the endcap shift and out-of-time cut from the description:
  G4float m_OOTcut;

  // The results of the Process calculation:
  //LArG4Identifier m_identifier;
  //G4double m_time;
  //G4double m_energy;
  std::vector<LArHitData> m_hdata;

  G4bool m_isInTime;

  FCAL_ChannelMap   *m_ChannelMap;
  const FCALModule  *m_posModule; // for hv access here...
  const FCALModule  *m_negModule; // for hv access here...
  // sampling
  G4int   m_FCalSampling;
  LArG4BirksLaw *birksLaw;

  LArFCALCalculatorBase (const LArFCALCalculatorBase&);
  LArFCALCalculatorBase operator= (const LArFCALCalculatorBase&);
};

#endif //  LArFCALCalculatorBase_H


