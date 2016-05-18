/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4H6COLDTCMod0Calculator
// Prepared 8-March-2004: Mohsen Khakzad
// Updated for GeoModelize simulations 27-May-2007: P. Strizenec

// Defines constants specific to a single FCAL module.

#ifndef LArG4H6COLDTCMod0Calculator_H
#define LArG4H6COLDTCMod0Calculator_H

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArVG4DetectorParameters.h"


#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4ios.hh"

#include "globals.hh"

#include "LArG4H6COLDTCMod0ChannelMap.h"
#include <stdexcept>

class LArG4H6COLDTCMod0Calculator: public LArVCalculator
{
public:

  static LArG4H6COLDTCMod0Calculator* GetInstance();

  // destructor
  virtual ~LArG4H6COLDTCMod0Calculator() { };

  //
  virtual G4float OOTcut() const { return m_OOTcut; }
  virtual void SetOutOfTimeCut(G4double c) { m_OOTcut = c; }

  virtual G4bool Process(const G4Step*);
  virtual G4bool Process(const G4Step*, std::vector<LArHitData>&);
  virtual const LArG4Identifier& identifier() const {return m_identifier;}
  virtual const LArG4Identifier& identifier(int i=0) const {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_identifier;
  }

  virtual G4double time() const      {return m_time;}
  virtual G4double time(int i=0) const      {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_time;
  }
  virtual G4double energy() const    {return m_energy;}
  virtual G4double energy(int i=0) const    {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return m_energy;
  }
  virtual G4bool isInTime() const    {return     m_isInTime;}
  virtual G4bool isInTime(int i=0) const    {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return     m_isInTime;
  }
  virtual G4bool isOutOfTime() const { return ( ! m_isInTime );}
  virtual G4bool isOutOfTime(int i=0) const {
    if (i!=0) throw std::range_error("Multiple hits not yet implemented");
    return ( ! m_isInTime );
  }

  // access module parameters  ---> bulk absorber = Cu
  virtual G4double GetModulePhiStart() const
    { return m_phiModuleStart; }
  virtual G4double GetModulePhiEnd() const
    { return m_phiModuleEnd; }

  // access active medium ---> readout gap = LAr
  virtual G4double GetActiveDepth() const
    { return m_fullActiveDepth; }
  virtual G4double GetActiveInnerRadius() const
    { return m_innerActiveRadius; }
  virtual G4double GetActiveOuterRadius() const
    { return m_outerActiveRadius; }

protected:

  LArG4H6COLDTCMod0Calculator();

private:

  static LArG4H6COLDTCMod0Calculator* m_instance;
  // private datamember handling the hit
  G4float m_OOTcut;

  G4double m_time;
  G4double m_energy;
  G4bool   m_isInTime;
  LArG4Identifier m_identifier;
  G4int m_FCalSampling;

  // geometry of ColdTC: overall
  G4double m_phiModuleStart;
  G4double m_phiModuleEnd;
  G4double m_fullModuleDepth;

  // geometry of ColdTC: active argon
  G4double m_fullActiveDepth;
  G4double m_innerActiveRadius;
  G4double m_outerActiveRadius;
  G4double m_areaActive;

  // Channel map
  LArG4H6COLDTCMod0ChannelMap m_channelMap;
};
#endif
