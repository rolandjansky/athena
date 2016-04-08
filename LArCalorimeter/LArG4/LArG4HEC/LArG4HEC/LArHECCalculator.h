/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHECCalculator

// Revision history:

// 31-May-2002 : Karapetian Gaiane

// 13-Jan-2004 WGS: Now uses LArG4::HEC::Geometry for the Identifier calculations.

#ifndef LARG4HEC_LARHECCALCULATOR_H
#define LARG4HEC_LARHECCALCULATOR_H

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include <stdexcept>
// Forward declarations.
class G4Step;
namespace LArG4 {
  namespace HEC {
    class Geometry;
  }
}

class LArHECCalculator : public LArVCalculator {
public:

  // Accessor for pointer to the singleton.
  static LArHECCalculator* GetCalculator();

  // "zShift" is the z-distance (cm) that the EM endcap is shifted
  // (due to cabling, etc.)
  G4float zShift() const { return m_zShift; }

  /////////////////////////////////////////////
  // The interface for LArVCalculator.

  virtual G4float OOTcut() const { return m_OOTcut; }

  virtual G4bool Process(const G4Step* a_step){return Process(a_step, m_hdata);}
  virtual G4bool Process(const G4Step* a_step, std::vector<LArHitData>& hdata);

  virtual const LArG4Identifier& identifier(int i=0) const { 
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    if(m_hdata.size()<1) throw std::range_error("No hit yet");
    return  m_hdata[0].id; 
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

  G4bool GetDoChargeCollection() const { return m_chargeCollection; } 
  void SetDoChargeCollection(G4bool f) { m_chargeCollection = f; }

protected:
  // The constructor is protected according to the singleton design
  // pattern.
  LArHECCalculator();

private:
  static LArHECCalculator* m_instance;

  G4float m_zShift;
  G4float m_OOTcut;
//  LArG4Identifier m_identifier;

//  G4double m_time;
//  G4double m_energy;
  std::vector<LArHitData> m_hdata;

  G4bool m_isInTime;

  // Class for calculating the identifier.
  LArG4::HEC::Geometry* m_geometry;

  // Flag to determine if we invoke charge-collection processing.
  G4bool m_chargeCollection;
};

#endif // not LARG4HEC_LARHECCALCULATOR_H
