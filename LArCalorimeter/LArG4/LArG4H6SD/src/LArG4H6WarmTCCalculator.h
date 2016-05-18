/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H6WarmTCCalculator_H__
#define __LArG4H6WarmTCCalculator_H__

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include <stdexcept>
enum WTCComp {
   WTC_X,       // X - coordinate scintilator
   WTC_Y,       // Y - coordinate scintilator
   WTC_ABS      // Absorbers
};

// Forward declarations.
class G4Step;

class LArG4H6WarmTCCalculator : public LArVCalculator {
public:

  LArG4H6WarmTCCalculator(WTCComp icomp = WTC_ABS);


  /////////////////////////////////////////////
  // The interface for LArVCalculator.

  virtual G4float OOTcut() const { return m_OOTcut; }
  virtual void SetOutOfTimeCut(G4double c) { m_OOTcut = c; }

  virtual G4bool Process(const G4Step* a_step);
  virtual G4bool Process(const G4Step*, std::vector<LArHitData>&);
  G4int address() const { return m_addr;}
  virtual const LArG4Identifier& identifier(int i=0) const {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_identifier;
  }

  virtual G4double time(int i=0) const {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_time;
  }
  virtual G4double energy(int i=0) const {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_energy;
  };
  virtual G4bool isInTime(int i=0) const    {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return     m_isInTime;
  }
  virtual G4bool isOutOfTime(int i=0) const {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return ( ! m_isInTime );
  }
  bool isX() const { return m_isX; }
  bool isABS() const { return m_isABS; }


private:

  float m_OOTcut;
  LArG4Identifier m_identifier;
  int m_addr;

  double m_time;
  double m_energy;
  bool m_isInTime;
  bool m_isX;
  bool m_isABS;

};

#endif
