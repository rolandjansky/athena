/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArEndcapPresamplerCalculator.hh 
// Prepared 27-Dec-2002 Bill Seligman

// This is a separate "calculator" class for the endcap presampler.
// For more information about what a "calculator" class does, see the
// documentation: LArG4/doc/LArG4.ps.

#ifndef __LArEndcapPresamplerCalculator_H__
#define __LArEndcapPresamplerCalculator_H__

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include <stdexcept>
// Forward declarations.
class G4Step;
class LArG4BirksLaw;

namespace LArG4 {
  namespace EC {
    class PresamplerGeometry;
  }
}

class LArEndcapPresamplerCalculator : public LArVCalculator {
public:

  // Accessor for pointer to the singleton.
  static LArEndcapPresamplerCalculator* GetCalculator();

  ~LArEndcapPresamplerCalculator();

  /////////////////////////////////////////////
  // The interface for LArVCalculator.

  virtual G4float OOTcut() const { return m_OOTcut; }
  virtual void SetOutOfTimeCut(G4double o){ m_OOTcut = o; }

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

protected:
  // The constructor is protected according to the singleton design
  // pattern.
  LArEndcapPresamplerCalculator();

private:

  // Pointer to geometry routine.
  const LArG4::EC::PresamplerGeometry* m_geometry;

  // Store the out-of-time cut from the description:
  G4float m_OOTcut;

  LArG4BirksLaw *birksLaw;

  // The results of the Process calculation:
  //LArG4Identifier m_identifier;
  //G4double m_time;
  //G4double m_energy;
  std::vector<LArHitData> m_hdata;

  G4bool m_isInTime;

  LArEndcapPresamplerCalculator (const LArEndcapPresamplerCalculator&);
  LArEndcapPresamplerCalculator& operator= (const LArEndcapPresamplerCalculator&);
};

#endif
