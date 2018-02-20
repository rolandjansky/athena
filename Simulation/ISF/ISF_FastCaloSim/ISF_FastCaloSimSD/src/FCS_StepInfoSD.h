/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIM_FCS_STEPINFOSD_H
#define ISF_FASTCALOSIM_FCS_STEPINFOSD_H

#include "G4VSensitiveDetector.hh"

#include "CaloIdentifier/CaloCell_ID.h" // For CaloCell_ID::CaloSample
#include "LArG4Code/LArG4Identifier.h"

#include "LArSimEvent/LArHit.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "ISF_FastCaloSimEvent/FCS_StepInfoCollection.h"

#include <map>
#include <vector>

// Forward declarations
class G4Step;
class G4TouchableHistory;

class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;
class CaloDetDescrManager;

class ILArCalculatorSvc;
class ITileCalculator;
class LArHitContainer;

class StoreGateSvc;

namespace FCS_Param {

  struct Config
  {
    /** Helper to keep the same verbosity everywhere */
    int verboseLevel=0;
    bool shift_lar_subhit=true;
    bool shorten_lar_step=false;

    // Merging properties
    double            m_maxRadius=25.;                //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
    double            m_maxRadiusFine=1.;             //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
    double            m_maxRadiusLAr=25.;             //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
    double            m_maxRadiusHEC=100.;             //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
    double            m_maxRadiusFCAL=100.;            //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
    double            m_maxRadiusTile=100.;            //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink

    double            m_maxTime=25.;
    double            m_maxTimeLAr=25.;
    double            m_maxTimeHEC=25.;
    double            m_maxTimeFCAL=25.;
    double            m_maxTimeTile=25.;

    // Optimised merging scheme
    double            m_maxEtaPS=1.;
    double            m_maxPhiPS=5.;
    double            m_maxrPS=0.;

    double            m_maxEtaEM1=1.;
    double            m_maxPhiEM1=5.;
    double            m_maxrEM1=15.;

    double            m_maxEtaEM2=1.;
    double            m_maxPhiEM2=5.;
    double            m_maxrEM2=60.;

    double            m_maxEtaEM3=1.;
    double            m_maxPhiEM3=5.;
    double            m_maxrEM3=8.;

    ILArCalculatorSvc *m_LArCalculator=nullptr;
    ITileCalculator *m_TileCalculator=nullptr;
  };

}

/// @class FCS_StepInfoSD
/// @brief Common sensitive detector class for LAr systems.
///
/// This SD implementation saves the standard LArHits.
/// See LArG4CalibSD for an SD that handles calibration hits.
///
class FCS_StepInfoSD : public G4VSensitiveDetector
{
public:

  /// Constructor
  FCS_StepInfoSD(G4String a_name, const FCS_Param::Config& config);

  /// Destructor
  virtual ~FCS_StepInfoSD();

  /// Main processing method
  G4bool ProcessHits(G4Step* a_step, G4TouchableHistory*) override;

  /// End of athena event processing
  void EndOfAthenaEvent( ISF_FCS_Parametrization::FCS_StepInfoCollection* hitContnainer );

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

protected:
  /// Keep a map instead of trying to keep the full vector.
  /// At the end of the event we'll push the map back into the
  /// FCS_StepInfoCollection in StoreGate.
  virtual void update_map(const CLHEP::Hep3Vector & l_vec, const Identifier & l_cell, double l_energy, double l_time, bool l_valid, int l_detector);
  FCS_Param::Config m_config;
  /// Pointers to the identifier helpers
  const LArEM_ID*       m_larEmID;
  const LArFCAL_ID*     m_larFcalID;
  const LArHEC_ID*      m_larHecID;
  const LArMiniFCAL_ID* m_larMiniFcalID;
  const CaloDetDescrManager *m_calo_dd_man;
  std::map< Identifier , std::vector< ISF_FCS_Parametrization::FCS_StepInfo* >* > m_hit_map;

private:
  ///
  double getMaxTime(const CaloCell_ID::CaloSample& layer) const;
  ///
  double getMaxRadius(const CaloCell_ID::CaloSample& layer) const;
  ///
  double getMaxDeltaR(const CaloCell_ID::CaloSample& layer) const;
  ///
  double getMaxDeltaEta(const CaloCell_ID::CaloSample& layer) const;
  ///
  double getMaxDeltaPhi(const CaloCell_ID::CaloSample& layer) const;

};

#endif // ISF_FASTCALOSIM_FCS_STEPINFOSD_H
