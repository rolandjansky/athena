/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIM_LARFCS_STEPINFOSD_H
#define ISF_FASTCALOSIM_LARFCS_STEPINFOSD_H

// Base class header
#include "FCS_StepInfoSD.h"

#include "LArG4Code/LArG4Identifier.h"

// Forward declarations
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;
class CaloDetDescrManager;

class G4Step;
class G4TouchableHistory;
class ILArCalculatorSvc;

/// @class FCS_StepInfoSD
/// @brief Common sensitive detector class for LAr systems.
///
/// This SD implementation saves the standard LArHits.
/// See LArG4CalibSD for an SD that handles calibration hits.
///
class LArFCS_StepInfoSD : public FCS_StepInfoSD
{
public:

  /// Constructor
  LArFCS_StepInfoSD(G4String a_name, const FCS_Param::Config& config);

  /// Destructor
  virtual ~LArFCS_StepInfoSD();

  /// Main processing method
  G4bool ProcessHits(G4Step* a_step, G4TouchableHistory*) override;

private:
  /// Keep a map instead of trying to keep the full vector.
  /// At the end of the event we'll push the map back into the
  /// FCS_StepInfoCollection in StoreGate.
  virtual void update_map(const CLHEP::Hep3Vector & l_vec, const Identifier & l_cell, double l_energy, double l_time, bool l_valid, int l_detector) override final;

  /// Helper function for making "real" identifiers from LArG4Identifiers
  Identifier ConvertID(const LArG4Identifier& a_ident) const;

  /// Member variable - the calculator we'll use
  ILArCalculatorSvc * m_calculator;
};

#endif // ISF_FASTCALOSIM_LARFCS_STEPINFOSD_H
