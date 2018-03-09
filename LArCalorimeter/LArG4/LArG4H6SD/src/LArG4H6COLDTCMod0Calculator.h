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
#include "LArG4Code/LArCalculatorSvcImp.h"

#include "globals.hh"

#include "LArG4H6COLDTCMod0ChannelMap.h"
//#include <stdexcept>

class LArG4H6COLDTCMod0Calculator : public LArCalculatorSvcImp
{
public:

  LArG4H6COLDTCMod0Calculator(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override final;
  // destructor
  virtual ~LArG4H6COLDTCMod0Calculator() { };

  //
  virtual G4float OOTcut() const override final { return m_OOTcut; }
  // virtual void SetOutOfTimeCut(G4double c) { m_OOTcut = c; } //FIXME public, but not part of interface

  virtual G4bool Process(const G4Step*, std::vector<LArHitData>&) const override final;

  virtual G4bool isInTime(G4double hitTime) const override final
  {
    return !(hitTime > m_OOTcut);
  }

  // // access module parameters  ---> bulk absorber = Cu
  // virtual G4double GetModulePhiStart() const //FIXME public, but not part of interface
  //   { return m_phiModuleStart; }
  // virtual G4double GetModulePhiEnd() const //FIXME public, but not part of interface
  //   { return m_phiModuleEnd; }

  // // access active medium ---> readout gap = LAr
  // virtual G4double GetActiveDepth() const //FIXME public, but not part of interface
  //   { return m_fullActiveDepth; }
  // virtual G4double GetActiveInnerRadius() const //FIXME public, but not part of interface
  //   { return m_innerActiveRadius; }
  // virtual G4double GetActiveOuterRadius() const //FIXME public, but not part of interface
  //   { return m_outerActiveRadius; }

private:

  // private datamember handling the hit
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
