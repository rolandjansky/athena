/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SIMPLESD_H
#define LARG4SIMPLESD_H

#include "G4VSensitiveDetector.hh"

#include "LArG4Code/LArG4Identifier.h"

#include "LArSimEvent/LArHit.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <set>

// Forward declarations
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;

class ILArCalculatorSvc;
class LArHitContainer;

class StoreGateSvc;


/// @class LArG4SimpleSD
/// @brief Common sensitive detector class for LAr systems.
///
/// This SD implementation saves the standard LArHits.
/// See LArG4CalibSD for an SD that handles calibration hits.
///
class LArG4SimpleSD : public G4VSensitiveDetector
{
public:

  enum LArHitTimeBins
  {
      HitTimeBinDefault = 0,
      HitTimeBinUniform = 1
  };

  /// Constructor
  LArG4SimpleSD(G4String a_name, ILArCalculatorSvc* calc,
                const std::string& type="Default",
                const float width=2.5*CLHEP::ns);

  /// Alternative constructor, particularly for fast simulations.
  LArG4SimpleSD(G4String a_name, StoreGateSvc* detStore);

  /// Destructor
  virtual ~LArG4SimpleSD();

  /// Main processing method
  G4bool ProcessHits(G4Step* a_step, G4TouchableHistory*) override;

  /// First method translates to this - also for fast sims
  G4bool SimpleHit( LArG4Identifier lar_id , G4double time , G4double energy );

  /// End of athena event processing
  void EndOfAthenaEvent( LArHitContainer* hitContnainer );

  /// Sets the ID helper pointers
  void setupHelpers( const LArEM_ID* EM ,
                     const LArFCAL_ID* FCAL ,
                     const LArHEC_ID* HEC ,
                     const LArMiniFCAL_ID* mini ) {
    m_larEmID = EM;
    m_larFcalID = FCAL;
    m_larHecID = HEC;
    m_larMiniFcalID = mini;
  }

  /// Helper function for making "real" identifiers from LArG4Identifiers
  Identifier ConvertID(const LArG4Identifier& a_ident) const;

protected:
  /// Helper method for time info
  G4int getTimeBin(G4double time) const;

  /// Member variable - the calculator we'll use
  ILArCalculatorSvc * m_calculator;

  /// Count the number of invalid hits.
  G4int m_numberInvalidHits;

  // We need two types containers for hits:

  // The set defined below is used to tell us if we've already had a
  // hit in a cell.  We store these hits in a set, so we can quickly
  // search it.  Note the use of a custom definition of a "less"
  // function for the set, so we're not just comparing hit pointers.

  class LessHit {
  public:
    bool operator() ( LArHit* const& p, LArHit* const& q ) const
    {
      return p->Less(q);
    }
  };

  typedef std::set< LArHit*, LessHit >  hits_t;

  // The hits are grouped into time bins, with the width of a bin
  // determined by a user parameter.  This map is used to associate a
  // time bin with its corresponding set of hits.

  typedef std::map < G4int, hits_t* >   timeBins_t;

  // Two types of LAr hit time binning
  // 1. 'Default'
  //
  //     All negative times to the bin 0
  //     0 <= time < 10 by 2.5ns
  //     10 <= time < 50 by 10ns
  //     50 <= time < 100 by 25ns
  //     All others to the bin 14
  //
  //
  // 2. 'Uniform'
  //
  // Old style time binning by 2.5ns
  LArG4SimpleSD::LArHitTimeBins m_timeBinType;

  /// Width of the time bins for summing hits - for the uniform binning
  G4float m_timeBinWidth;

  /// The map of hit sets binned in time
  timeBins_t m_timeBins;

  /// Pointers to the identifier helpers
  const LArEM_ID*       m_larEmID;
  const LArFCAL_ID*     m_larFcalID;
  const LArHEC_ID*      m_larHecID;
  const LArMiniFCAL_ID* m_larMiniFcalID;

};

#endif
