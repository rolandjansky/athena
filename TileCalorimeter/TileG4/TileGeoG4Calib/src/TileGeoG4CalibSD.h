/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4CalibSD.
// Calibration Sensitive Detector for TileCal simulation
//
// Author: Gia Khoriauli <gia@mail.cern.ch>
// May, 2005
//
// Major updates: December, 2005;
//                July, 2013 (Sergey);
//
//************************************************************

#ifndef TILEGEOG4CALIB_TILEGEOG4CALIBSD_H
#define TILEGEOG4CALIB_TILEGEOG4CALIBSD_H

#include "G4VSensitiveDetector.hh"

// Output collections
#include "StoreGate/WriteHandle.h"
#include "TileSimEvent/TileHitVector.h"
#include "CaloSimEvent/CaloCalibrationHit.h"

// Member variables
#include "CaloG4Sim/SimulationEnergies.h"

// Service handles
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ServiceHandle.h"

//    the lower line should be uncommented to produce special text or root file
//    with coordinates of all calibration hits
//    later the ROOT macro (test/testHits.C) can be used to produce plots
//#define HITSINFO    //added by Sergey

#ifdef HITSINFO
#include "GaudiKernel/ToolHandle.h"
#include "TileSimUtils/TileCalibHitNtuple.h"
#include "TileSimUtils/TileCalibHitCntNtup.h"
#define doHitsTXT  false
#define doHitsNTup true
#define IDcalc     0    //1-6, 0-all
#endif

class CaloCalibrationHitContainer;
class TileEscapedEnergyProcessing;
class G4Step;
class G4HCofThisEvent;
class Identifier;

class StoreGateSvc;
class CaloCell_ID;
class CaloDM_ID;

#ifdef HITSINFO
class TileCalibHitNtuple;
class TileCalibHitCntNtup;
#endif

class ITileCalculator;
class TileGeoG4LookupBuilder;
class TileGeoG4DMLookupBuilder;

class TileGeoG4Section;
class TileGeoG4Cell;
class TileGeoG4CalibSection;
class TileGeoG4PlateCell;
class TileGeoG4GirderCell;

struct TileHitData;

class G4TouchableHistory;
class G4VPhysicalVolume;

class EventInformation;

#include <string>
#include <vector>
typedef std::vector<double> E_4;

class TileGeoG4CalibSD: public G4VSensitiveDetector {
public:
  TileGeoG4CalibSD(const G4String& name, const std::vector<std::string>& m_outputCollectionNames,
                   ITileCalculator* tileCalculator, const ServiceHandle<StoreGateSvc> &detStore);
  ~TileGeoG4CalibSD();

  void InvokeUserRunAction();
  void InvokeUserEventAction();
  void InvokeUserTrackingAction();
  void InvokeUserSteppingAction();

  //SD MAIN METHODS FOR A STEP PROCESSING
  void Initialize(G4HCofThisEvent*) override final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  void EndOfAthenaEvent();

  //METHOD FOR CLASSIFYING STEP ENERGY AND THE METHODS,
  //WHICH CALCULATE IDENTIFIER FOR CELL OR DEAD MATERIAL
  //CALIBRATION HITS ON THE CURRENT STEP
  bool FindTileCalibSection();
  void CellIDCalculator();
  void ScintIDCalculator(TileHitData& hitData);
  void GirderCellIDCalculator();
  void PlateCellIDCalculator();
  void DefaultHitIDCalculator();
  void DefaultHitIDCalculatorTB(int sample=0, int region=0, int eta=0, int phi=0);

  //GET RESULT FOR THE STEP HIT ID AND CLASSIFYED ENERGIES
  Identifier GetCellIDOnStep();
  CaloG4::SimulationEnergies::ClassifyResult_t GetEnergiesOnStep();

  //DEBUG CELL AND DM CELL ID FIELDS
  void DebugCellIDFields();
  void DebugDMCellIDFields(int);

  //added by Sergey
  void CellNumCorrectToHit(int, int);
#ifdef HITSINFO
  void HitsInfoPrint (bool, int);
#endif

  //RESET ALL CLASS MEMBER ID FIELDS, NEW & DEFAULT HIT FLAGS,
  //SECTION & SIDE FLAGS, DEPOSITED ENERGIES IN SCINTILLATOR/PMTs
  // ! THESE METHODS ARE NEEDED AT THE BEGGINING OF EACH STEP !
  void ResetCellIDFields();
  void ResetDMCellIDFields();
  void ResetSectSideFlags();

protected:
  Identifier DM_ID_Maker(int det, int type, int sample, int region, int tower, int module);

  //THIS METHOD IS USED BY IDENTIFIER CALCULATOR METHODS FOR FINDING TILE SECTION
  //WHERE THE CURRENT STEP IS AND FOR DETERMINING SOME RELATED IDENTIFIER FIELD
  //VALUES. ALSO, DETRMINES SOME FLAGS USED IN A FURTHER PROCESSING OF THE STEP
  bool AreClassifiedEnergiesAllZero();
  void SetEscapedEnergy(double escapedEnergy);
  double GetVisibleEnergy();
  double GetInvisibleEnergy();

  void EnergiesSimpleCounter();
  void DebugEnergies();

private:
  TileGeoG4CalibSD(const TileGeoG4CalibSD&);
  TileGeoG4CalibSD& operator=(const TileGeoG4CalibSD&);

  //CALIBRATION HIT CONTAINERS
  SG::WriteHandle<CaloCalibrationHitContainer> m_tileActiveCellCalibHits;
  SG::WriteHandle<CaloCalibrationHitContainer> m_tileInactiveCellCalibHits;
  SG::WriteHandle<CaloCalibrationHitContainer> m_tileDeadMaterialCalibHits;
  SG::WriteHandle<TileHitVector> m_tileHits;

  // Handles for later use
  const CaloCell_ID* m_caloCell_ID;
  const CaloDM_ID* m_caloDM_ID;

  ServiceHandle<IRDBAccessSvc> m_rdbSvc;
  ServiceHandle<IGeoModelSvc> m_geoModSvc;

  //FLAGS FROM JOBOPTIONS
  bool m_tileTB;
  bool m_doCalibHitParticleID;

#ifdef HITSINFO
  ToolHandle<TileCalibHitNtuple> m_ntuple;
  ToolHandle<TileCalibHitCntNtup> m_ntupleCnt;
#endif

  //ORDINARY AND CALIBRATION LOOKUP BUILDERS
  ITileCalculator* m_calc;
  TileGeoG4LookupBuilder* m_lookup;
  TileGeoG4DMLookupBuilder* m_lookupDM;

  //CALIBRATION ENERGY CLASSIFYER
  CaloG4::SimulationEnergies* m_simEn;
  TileEscapedEnergyProcessing* m_tile_eep;

  //ENERGY AND POSITION AT CURRENT STEP
  G4Step* m_aStep;

  //FOR SIMPLE ENERGY DEBUGGING
  double m_E_tot;
  double m_E_em;
  double m_E_nonem;
  double m_E_invisible;
  double m_E_escaped;

  //STEP TOUCHABLE HISTORY AND VOLUME
  G4TouchableHistory* m_stepTouchable;
  G4VPhysicalVolume* m_stepPhysVol;
  G4String m_nameVol;

  //CALIBRATION SECTION & CELLS
  TileGeoG4CalibSection* m_cSection;
  TileGeoG4Cell* m_cell;
  TileGeoG4PlateCell* m_pCell;
  TileGeoG4GirderCell* m_gCell;

  //CLASSIFIED ENERGY VECTOR
  CaloG4::SimulationEnergies::ClassifyResult_t m_result;

  //IDENTIFIER FOR CALIBRATION HIT
  Identifier m_id;

  //CELL ACTIVE || CELL INACTIVE || DM
  int m_calibHitType;

  //REAL CELL ID VALUES
  int m_subCalo;
  int m_detector;
  int m_side;
  int m_module;
  int m_tower;
  int m_sample;

  //DM CELL ID VALUES
  int m_dm_subDet;
  int m_dm_type;
  int m_dm_sample;
  int m_dm_region;
  int m_dm_neta;
  int m_dm_nphi;

  //GLOBAL HIT POSITION -- added by Sergey
  double m_xGlobal;
  double m_yGlobal;
  double m_zGlobal;
  double m_phiGlobal;
  int m_cellNum;
#ifdef HITSINFO
  int m_nEvent;
  int m_det_side;
#endif

  //FLAG FOR DEFAULT HIT
  bool m_defaultHit;

  /** @brief set to true if hit is in extended barrel */
  bool m_isExtended;

  /** @brief set to true if hit is in negative side */
  bool m_isNegative;

  /** @brief set to true if DM hit in end/front plate is added to a real cell (taken from DB or from JO) */
  bool m_plateToCell;

  /** @brief set to true if DM hit in front plate is added to a real cell 
      (currently equal to _plateToCell) */
  bool m_addToCell;

  /** @brief set to true if DM hit in absorber at outer radius is added to girder
      (currently inverse of  _plateToCell) */
  bool m_addToGirder;

  /** variable used for producing calibration hits signed with primary particle ID */
  EventInformation* m_event_info;

  class LessHit {
  public:
    bool operator()(CaloCalibrationHit* const & p, CaloCalibrationHit* const & q) const {
      return p->Less(q);
    }
  };

  typedef std::set<CaloCalibrationHit*, LessHit> m_calibrationHits_t;
  typedef m_calibrationHits_t::iterator m_calibrationHits_ptr_t;
  m_calibrationHits_t m_activeCalibrationHits;
  m_calibrationHits_t m_inactiveCalibrationHits;
  m_calibrationHits_t m_deadCalibrationHits;

};
//CLASS TileGeoG4CalibSD

//////////////////
//inline methods//
//////////////////
inline void TileGeoG4CalibSD::InvokeUserRunAction() { ;}
inline void TileGeoG4CalibSD::InvokeUserEventAction() {;}
inline void TileGeoG4CalibSD::InvokeUserTrackingAction() {;}
inline void TileGeoG4CalibSD::InvokeUserSteppingAction() {;}

inline Identifier TileGeoG4CalibSD::GetCellIDOnStep() {
  return m_id;
}

inline CaloG4::SimulationEnergies::ClassifyResult_t TileGeoG4CalibSD::GetEnergiesOnStep() {
  return m_result;
}

inline void TileGeoG4CalibSD::ResetCellIDFields() {
  m_detector = m_side = m_module = m_tower = m_sample = -999;
}
inline void TileGeoG4CalibSD::ResetDMCellIDFields() {
  m_dm_type = m_dm_sample = m_dm_region = m_dm_neta = m_dm_nphi = -999;
}
inline void TileGeoG4CalibSD::ResetSectSideFlags() {
  m_isExtended = m_isNegative = false;
}

inline bool TileGeoG4CalibSD::AreClassifiedEnergiesAllZero() {
  //check any kind of classified energies. if they all are zero
  //then nothing to do and go to the next Step
  return ((m_result.energy[CaloG4::SimulationEnergies::kEm] == 0.) && (m_result.energy[CaloG4::SimulationEnergies::kNonEm] == 0.)
          && (m_result.energy[CaloG4::SimulationEnergies::kInvisible0] == 0.)
          && (m_result.energy[CaloG4::SimulationEnergies::kEscaped] == 0.));
}

inline void TileGeoG4CalibSD::SetEscapedEnergy(double escapedEnergy) {
  m_result.energy[CaloG4::SimulationEnergies::kEm] = 0.;
  m_result.energy[CaloG4::SimulationEnergies::kNonEm] = 0.;
  m_result.energy[CaloG4::SimulationEnergies::kInvisible0] = 0.;
  m_result.energy[CaloG4::SimulationEnergies::kEscaped] = escapedEnergy;
}

inline double TileGeoG4CalibSD::GetVisibleEnergy()
{
  return ( m_result.energy[CaloG4::SimulationEnergies::kEm]
         + m_result.energy[CaloG4::SimulationEnergies::kNonEm]
         + m_result.energy[CaloG4::SimulationEnergies::kEscaped]);
}

inline double TileGeoG4CalibSD::GetInvisibleEnergy()
{
  return m_result.energy[CaloG4::SimulationEnergies::kInvisible0];
}

#endif // TILEGEOG4CALIB_TILEGEOG4CALIBSD_H

