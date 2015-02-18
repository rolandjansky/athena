/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4HECLocalCalculator.hh 

// Revision history:

// 17-Feb-2006 : Pavol Strizenec

#ifndef __LArG4HECLocalCalculator_H__
#define __LArG4HECLocalCalculator_H__

//#include "globals.hh"
//#include "G4ThreeVector.hh"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include <stdexcept>
#include "CLHEP/Units/SystemOfUnits.h"


// Forward declarations.
class G4Step;
class LocalGeometry;
class IMessageSvc;
class LArG4BirksLaw;

namespace LArG4 {
  namespace HEC {
     class LocalGeometry;
  }
}


class LArHECLocalCalculator : public LArVCalculator {
public:

     // Accessor for pointer to the singleton.
  static LArHECLocalCalculator* GetCalculator();


  /////////////////////////////////////////////
  // The interface for LArVCalculator.

  virtual G4float OOTcut() const { return m_OOTcut; }
  virtual void SetOutOfTimeCut(G4double c) { m_OOTcut = c; }

  virtual G4bool Process(const G4Step* a_step){ return Process(a_step,0, 4.*CLHEP::mm);}
  virtual G4bool Process(const G4Step* a_step, int depthadd, double deadzone);
  
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
  G4bool isX() const { return m_isX; }
  void   SetX(bool x);  

protected:
  // The constructor is protected according to the singleton design
  // pattern.
  LArHECLocalCalculator();
  ~LArHECLocalCalculator();

private:
  static LArHECLocalCalculator* m_instance;

  LArG4::HEC::LocalGeometry *m_Geometry;

  IMessageSvc *m_msgSvc;

  LArG4Identifier m_identifier;

  G4double m_time;
  G4double m_energy;
  G4float m_OOTcut;
  G4bool m_isInTime;

  LArG4BirksLaw *m_birksLaw;
  static G4bool m_isX;

  LArHECLocalCalculator (const LArHECLocalCalculator&);
  LArHECLocalCalculator operator= (const LArHECLocalCalculator&);
};

#endif
