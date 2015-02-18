/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHECWheelCalculator.hh 

// Revision history:

// Jan-2008: (M.Fincke)  To be used for new Module-Geometry 

#ifndef __LArG4HECWheelCalculator_H__
#define __LArG4HECWheelCalculator_H__

#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include <stdexcept>

// Forward declarations.

class G4Step;
class HECGeometry;
class IMessageSvc;
class LArG4BirksLaw;
class HECDetectorManager;

namespace LArG4 {
  namespace HEC {
     class HECGeometry;
  }
}


class LArHECWheelCalculator : public LArVCalculator {
public:

     // Accessor for pointer to the singleton.
  static LArHECWheelCalculator* GetCalculator();


  /////////////////////////////////////////////
  // The interface for LArVCalculator.

  virtual G4float OOTcut() const { return m_OOTcut; }
  virtual void SetOutOfTimeCut(G4double c) { m_OOTcut = c; }

  virtual G4bool Process(const G4Step* a_step);
  
  virtual const LArG4Identifier& identifier() const { return m_identifier; }
  virtual const LArG4Identifier& identifier(int i) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_identifier; 
  }

  virtual G4double time() const { return m_time; } 
  virtual G4double time(int i) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_time; 
  }
  virtual G4double energy() const { return m_energy; }
  virtual G4double energy(int i) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_energy; 
  };
  virtual G4bool isInTime() const    { return     m_isInTime; }
  virtual G4bool isInTime(int i) const    { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return     m_isInTime; 
  }
  virtual G4bool isOutOfTime() const { return ( ! m_isInTime ); } 
  virtual G4bool isOutOfTime(int i) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return ( ! m_isInTime ); 
  }

  ~LArHECWheelCalculator();

protected:
  // The constructor is protected according to the singleton design
  // pattern.
  LArHECWheelCalculator();

private:
  static LArHECWheelCalculator* m_instance;

  LArG4::HEC::HECGeometry *m_Geometry;
  const HECDetectorManager *m_DetectorManager;

  IMessageSvc *m_msgSvc;

  LArG4Identifier m_identifier;

  G4double m_time;
  G4double m_energy;
  G4float  m_OOTcut;
  G4bool   m_isInTime;
  LArG4BirksLaw *m_birksLaw;

  LArHECWheelCalculator (const LArHECWheelCalculator&);
  LArHECWheelCalculator& operator= (const LArHECWheelCalculator&);
};

#endif
