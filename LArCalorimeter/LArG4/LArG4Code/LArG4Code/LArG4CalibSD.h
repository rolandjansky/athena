/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4CALIBSD_H
#define LARG4CALIBSD_H

#include "G4VSensitiveDetector.hh"

#include "LArG4Code/LArG4Identifier.h"
#include "CaloSimEvent/CaloCalibrationHit.h"

#include <set>
#include <vector>

// Forward declarations
class G4Step;

class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;
class CaloDM_ID;
class AtlasDetectorID;

class ILArCalibCalculatorSvc; 

class CaloCalibrationHitContainer;


/// @class LArG4CalibSD
/// @brief A specialized SD class for saving LAr calibration hits.
///
/// This SD implementation saves CaloCalibrationHit objects.
/// See LArG4SimpleSD for an SD that handles the standard LArHits.
///
class LArG4CalibSD : public G4VSensitiveDetector
{
public:

  /// Constructor
  LArG4CalibSD(G4String a_name, ILArCalibCalculatorSvc* calc, bool doPID=false);

  /// Destructor
  virtual ~LArG4CalibSD();

  /// Main processing method
  G4bool ProcessHits(G4Step* a_step,G4TouchableHistory*) override;

  /// End of athena event processing
  void EndOfAthenaEvent( CaloCalibrationHitContainer* hitContainer, CaloCalibrationHitContainer* deadHitContainer=nullptr );

  /// Sets the ID helper pointers
  void setupHelpers( const LArEM_ID* EM ,
                     const LArFCAL_ID* FCAL ,
                     const LArHEC_ID* HEC ,
                     const LArMiniFCAL_ID* mini ,
                     const CaloDM_ID* caloDm ) {
    m_larEmID = EM;
    m_larFcalID = FCAL;
    m_larHecID = HEC;
    m_larMiniFcalID = mini;
    m_caloDmID = caloDm;
  }

  void addDetectorHelper( const AtlasDetectorID* id_helper) { m_id_helper=id_helper; }

  /// For other classes that need to call into us...
  G4bool SpecialHit(G4Step* a_step, const std::vector<G4double>& a_energies);

protected:

  // We need two types containers for hits:

  // The set defined below is used to tell us if we've already had a
  // hit in a cell.  We store these hits in a set, so we can quickly
  // search it.  Note the use of a custom definition of a "less"
  // function for the set, so we're not just comparing hit pointers.

  class LessHit {
  public:
    bool operator() ( CaloCalibrationHit* const& p, CaloCalibrationHit* const& q ) const
    {
      return p->Less(q);
    }
  };

  typedef std::set< CaloCalibrationHit*, LessHit >  m_calibrationHits_t;
  /// Constructs the calibration hit and saves it to the set
  G4bool SimpleHit( const LArG4Identifier& a_ident, const std::vector<double>& energies, m_calibrationHits_t& calibrationHits );

  /// Member variable - the calculator we'll use
  ILArCalibCalculatorSvc * m_calculator;

  /// Count the number of invalid hits.
  G4int m_numberInvalidHits;

  /// Are we set up to run with PID hits?
  G4bool m_doPID;

  /// The actual set of calibration hits
  m_calibrationHits_t m_calibrationHits;

  /// The actual set of dead material calibration hits
  m_calibrationHits_t m_deadCalibrationHits;

  /// Helper function for making "real" identifiers from LArG4Identifiers
  Identifier ConvertID(const LArG4Identifier& a_ident) const;

  /// Pointers to the identifier helpers
  const LArEM_ID*       m_larEmID;
  const LArFCAL_ID*     m_larFcalID;
  const LArHEC_ID*      m_larHecID;
  const LArMiniFCAL_ID* m_larMiniFcalID;
  const CaloDM_ID*      m_caloDmID;
  const AtlasDetectorID* m_id_helper;
};

#endif
