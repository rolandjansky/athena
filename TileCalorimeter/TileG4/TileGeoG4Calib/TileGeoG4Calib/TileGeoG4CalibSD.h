/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef TileGeoG4CalibSD_H
#define TileGeoG4CalibSD_H

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

#include "TileSimEvent/TileHitVector.h"
#include "CaloG4Sim/SimulationEnergies.h"
#include "CaloSimEvent/CaloCalibrationHit.h"

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

using namespace FADS;

class CaloCalibrationHitContainer;
class TileEscapedEnergyProcessing ;
class G4Step;
class G4HCofThisEvent;
class Identifier;
class CaloCell_ID;
class CaloDM_ID;
class IMessageSvc;
class IRDBAccessSvc;
class IGeoModelSvc;
class MsgStream;

#ifdef HITSINFO
class TileCalibHitNtuple;
class TileCalibHitCntNtup;
#endif

class TileGeoG4SDCalc;
class TileGeoG4LookupBuilder;
class TileGeoG4DMLookupBuilder;

class TileGeoG4Section     ;
class TileGeoG4Cell        ;
class TileGeoG4CalibSection;
class TileGeoG4PlateCell   ;
class TileGeoG4GirderCell  ;

class G4TouchableHistory   ;
class G4VPhysicalVolume    ;

class EventInformation;

#include <vector>
typedef std::vector<double> E_4 ;


class TileGeoG4CalibSD : public FadsSensitiveDetector
{
public:
    TileGeoG4CalibSD                  (G4String);
    ~TileGeoG4CalibSD                 ();

    void InvokeUserRunAction          ();
    void InvokeUserEventAction        ();
    void InvokeUserTrackingAction     ();
    void InvokeUserSteppingAction     ();

    //SD MAIN METHODS FOR A STEP PROCESSING
    void   Initialize                 (G4HCofThisEvent*);
    G4bool ProcessHits                (G4Step*, G4TouchableHistory*);
    void   EndOfEvent                 (G4HCofThisEvent*);

    //METHOD FOR CLASSIFYING STEP ENERGY AND THE METHODS,
    //WHICH CALCULATE IDENTIFIER FOR CELL OR DEAD MATERIAL
    //CALIBRATION HITS ON THE CURRENT STEP
    void       ClassifyEnergy         ();
    bool       FindTileCalibSection   ();
    Identifier CellIDCalculator       ();
    Identifier ScintIDCalculator      ();
    Identifier GirderCellIDCalculator ();
    Identifier PlateCellIDCalculator  ();
    Identifier DefaultHitIDCalculator ();

    //GET RESULT FOR THE STEP HIT ID AND CLASSIFYED ENERGIES
    Identifier GetCellIDOnStep                                        ();
    CaloG4::SimulationEnergies::ClassifyResult_t  GetEnergiesOnStep   ();

    //DEBUG CELL AND DM CELL ID FIELDS
    void DebugCellIDFields            ();
    void DebugDMCellIDFields          (int);

    //added by Sergey
    void CellNumCorrectToHit          (int, int);
#ifdef HITSINFO
    void HitsInfoPrint                (bool, int);
#endif

    //RESET ALL CLASS MEMBER ID FIELDS, NEW & DEFAULT HIT FLAGS,
    //SECTION & SIDE FLAGS, DEPOSITED ENERGIES IN SCINTILLATOR/PMTs
    // ! THESE METHODS ARE NEEDED AT THE BEGGINING OF EACH STEP !
    void ResetCellIDFields            ();
    void ResetDMCellIDFields          ();
    void ResetSectSideFlags           ();

protected:
    Identifier DM_ID_Maker            (int det   , int type , int sample,
                                       int region, int tower, int module);

    //THIS METHOD IS USED BY IDENTIFIER CALCULATOR METHODS FOR FINDING TILE SECTION
    //WHERE THE CURRENT STEP IS AND FOR DETERMINING SOME RELATED IDENTIFIER FIELD
    //VALUES. ALSO, DETRMINES SOME FLAGS USED IN A FURTHER PROCESSING OF THE STEP
    bool AreClassifiedEnergiesAllZero ();
    void SetEscapedEnergy             (double escapedEnergy);

    void EnergiesSimpleCounter        ();
    void DebugEnergies                ();

private:
    TileGeoG4CalibSD (const TileGeoG4CalibSD&);
    TileGeoG4CalibSD& operator= (const TileGeoG4CalibSD&);

    bool m_debug;
    bool m_verbose;

    //ATHENA AND OTHER SERVICES
    IMessageSvc*   m_msgSvc   ;
    IRDBAccessSvc* m_rdbSvc   ;
    IGeoModelSvc*  m_geoModSvc;
    MsgStream*     m_log;

    //CALIBRATION HIT CONTAINERS
    CaloCalibrationHitContainer* tileActiveCellCalibHits;
    CaloCalibrationHitContainer* tileInactiveCellCalibHits;
    CaloCalibrationHitContainer* tileDeadMaterialCalibHits;

#ifdef HITSINFO
    ToolHandle<TileCalibHitNtuple>  m_ntuple;
    ToolHandle<TileCalibHitCntNtup> m_ntupleCnt;
#endif

    //HIT CONTAINER EXPORTER TO STOREGATE
    AthenaHitsCollectionHelper   m_hitCollHelp;

    //ORDINARY AND CALIBRATION LOOKUP BUILDERS
    TileGeoG4SDCalc*             m_calc    ;
    TileGeoG4LookupBuilder*      m_lookup  ;
    TileGeoG4DMLookupBuilder*    m_lookupDM;

    //CALIBRATION ENERGY CLASSIFYER
    CaloG4::SimulationEnergies*  m_simEn   ;
    TileEscapedEnergyProcessing* m_tile_eep;

    //ENERGY AND POSITION AT CURRENT STEP
    G4Step* aStep;

    //ID HELPERS
    const CaloCell_ID* m_caloCell_ID;
    const CaloDM_ID*   m_caloDM_ID  ;

    //FOR SIMPLE ENERGY DEBUGGING
    double E_tot      ;
    double E_em       ;
    double E_nonem    ;
    double E_invisible;
    double E_escaped  ;

    //FOR ENERGY DEBUGGING DIRECTLY FROM
    //CALIBHITS AT THE END OF EVENT
    double E0;
    double E1;
    double E2;
    double E3;

    //STEP TOUCHABLE HISTORY AND VOLUME
    G4TouchableHistory* _StepTouchable;
    G4VPhysicalVolume*  _StepPhysVol  ;

    //CALIBRATION SECTION & CELLS
    TileGeoG4CalibSection* _c_section;
    TileGeoG4Cell*         _cell     ;
    TileGeoG4PlateCell*    _p_cell   ;
    TileGeoG4GirderCell*   _g_cell   ;

    //CLASSIFIED ENERGY VECTOR
    CaloG4::SimulationEnergies::ClassifyResult_t  _result;

    //IDENTIFIERS FOR CALIBRATION AND ORDINARY HITS
    Identifier  _id,  _id_pmt_up,  _id_pmt_down;

    //CELL ACTIVE || CELL INACTIVE || DM
    int _calibhit_type;

    //REAL CELL ID VALUES
    int _subCalo ;
    int _detector;
    int _side    ;
    int _module  ;
    int _tower   ;
    int _sample  ;

    //DM CELL ID VALUES
    int dm_subDet ;
    int dm_type   ;
    int dm_sample ;
    int dm_region ;
    int dm_neta   ;
    int dm_nphi   ;

    //GLOBAL HIT POSITION -- added by Sergey
    double xGlobal   ;
    double yGlobal   ;
    double zGlobal   ;
    double phiGlobal ;
    int    _cellNum  ;
#ifdef HITSINFO
    int nEvent;
    int det_side;
#endif

    //FLAG FOR DEFAULT HIT
    bool _DefaultHit;

    /** @brief set to true to apply Birks' law for a given hit
        this flag is set to false for DM hits and true for normal hits */
    bool _doBirkFlag;

    /** @brief set to true if hit is in extended barrel */
    bool _is_extended;

    /** @brief set to true if hit is in negative side */
    bool _is_negative;

    /** @brief set to true if we are running testbeam configuration */
    bool _is_ctb ;

    /** @brief set to true if DM hit in end/front plate is added to a real cell (taken from DB) */
    bool _plateToCell;

    /** @brief set to true if DM hit in front plate is added to a real cell 
        (currently equal to _plateToCell) */
    bool AddToCell;

    /** @brief set to true if DM hit in absorber at outer radius is added to girder
        (currently inverse of  _plateToCell) */
    bool AddToGirder;

    /** variable used for producing calibration hits signed with primary particle ID */
    EventInformation* m_event_info;

    class LessHit
    {
        public:
        bool operator() ( CaloCalibrationHit* const& p, CaloCalibrationHit* const& q ) const
        {
            return p->Less(q);
        }
    };

    typedef std::set< CaloCalibrationHit*, LessHit >  m_calibrationHits_t;
    typedef m_calibrationHits_t::iterator             m_calibrationHits_ptr_t;
    m_calibrationHits_t                               m_activeCalibrationHits;
    m_calibrationHits_t                               m_inactiveCalibrationHits;
    m_calibrationHits_t                               m_deadCalibrationHits;

}; //CLASS TileGeoG4CalibSD


//////////////////
//inline methods//
//////////////////
inline void TileGeoG4CalibSD::InvokeUserRunAction()      {;}
inline void TileGeoG4CalibSD::InvokeUserEventAction()    {;}
inline void TileGeoG4CalibSD::InvokeUserTrackingAction() {;}
inline void TileGeoG4CalibSD::InvokeUserSteppingAction() {;}

inline Identifier TileGeoG4CalibSD::GetCellIDOnStep()    { return _id;}
inline CaloG4::SimulationEnergies::ClassifyResult_t      TileGeoG4CalibSD::GetEnergiesOnStep() { return _result;}

inline void TileGeoG4CalibSD::ResetCellIDFields()        { _detector=_side=_module=_tower=_sample=-999; }
inline void TileGeoG4CalibSD::ResetDMCellIDFields()      { dm_type=dm_sample=dm_region=dm_neta=dm_nphi=-999;}
inline void TileGeoG4CalibSD::ResetSectSideFlags()       { _is_extended=_is_negative=false; }


#endif

