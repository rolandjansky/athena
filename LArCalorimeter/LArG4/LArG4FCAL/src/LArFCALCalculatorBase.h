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
#include "LArG4Code/LArCalculatorSvcImp.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"
#include "globals.hh"
#include <stdexcept>
class LArG4BirksLaw;
class FCALModule;

class LArFCALCalculatorBase : public LArCalculatorSvcImp
{
 public:
  // constructor
  LArFCALCalculatorBase(const std::string& name, ISvcLocator *pSvcLocator);
  StatusCode initialize();
  StatusCode finalize();

  /////////////////////////////////////////////

  virtual G4float OOTcut() const override final { return m_OOTcut; }

  virtual G4bool Process(const G4Step*, std::vector<LArHitData>&) const override final;

  // Check if the current hitTime is in-time
  virtual G4bool isInTime(G4double hitTime) const override final
  {
    return !(hitTime > m_OOTcut); //FIXME should we be checking the absolute value of hitTime here?
  }

  /////////////////////////////////////////////


 protected:

  bool m_doHV;

  FCAL_ChannelMap   *m_ChannelMap;
  const FCALModule  *m_posModule; // for hv access here...
  const FCALModule  *m_negModule; // for hv access here...
  // sampling
  G4int   m_FCalSampling;
  LArG4BirksLaw *m_birksLaw;

  LArFCALCalculatorBase (const LArFCALCalculatorBase&);
  LArFCALCalculatorBase operator= (const LArFCALCalculatorBase&);
};

#endif //  LArFCALCalculatorBase_H
