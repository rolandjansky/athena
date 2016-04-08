/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MiniFCALCalculator.hh 

// Revision history:

// Aug-2008: M.Fincke  First implementation for SLHC MiniFCal

#ifndef LARG4MINIFCAL_MINIFCALCALCULATOR_H
#define LARG4MINIFCAL_MINIFCALCALCULATOR_H

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include <stdexcept>

// Forward declarations.
class G4Step;
class MiniFCALAssignIdentifier;
class IMessageSvc;
namespace LArG4 {
  namespace MiniFCAL {
     class MiniFCALAssignIdentifier;
  }
}


class MiniFCALCalculator : public LArVCalculator {
public:

     // Accessor for pointer to the singleton.
  static MiniFCALCalculator* GetCalculator();


  /////////////////////////////////////////////
  // The interface for LArVCalculator.

  virtual G4float OOTcut() const { return m_OOTcut; }
  virtual void SetOutOfTimeCut(G4double c) { m_OOTcut = c; }


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
  virtual G4bool isOutOfTime(int i) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return ( !m_isInTime ); 
  }


protected:
  // The constructor is protected according to the singleton design
  // pattern.
  MiniFCALCalculator();

private:
  static MiniFCALCalculator* m_instance;

  LArG4::MiniFCAL::MiniFCALAssignIdentifier *m_Geometry;

  //LArG4Identifier m_identifier;
  //G4double m_time;
  //G4double m_energy;
  std::vector<LArHitData> m_hdata;

  G4float  m_OOTcut;
  G4bool   m_isInTime;

};

#endif // LARG4MINIFCAL_MINIFCALCALCULATOR_H

