/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LAR_FCAL_SAMPLING_FRACTION_H
#define LAR_FCAL_SAMPLING_FRACTION_H

// C++ stdlib
#include <string>

// ATLAS Software
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "GeoAdaptors/GeoLArHit.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "StoreGate/StoreGateSvc.h"

class JetCollection;
class ISvcLocator;
class StoreGateSvc;
class LArFCAL_ID;
class CaloDM_ID;
class CaloCell_ID;
class LArHitContainer;
class CaloCalibrationHit;
class CaloCalibrationHitContainer;
class CaloDetDescrManager;
class TCECollection;
class TCEnergies;


class LArFCalSamplingFraction : public AthAlgorithm
{
public:
    LArFCalSamplingFraction(const std::string &name, ISvcLocator *pSvcLocator);
    ~LArFCalSamplingFraction();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute();
    virtual StatusCode initEvent();
    StatusCode doFCal();
    void TruthImpactPosition(McEventCollection::const_iterator);

    void FCalCalibAnalysis(const std::string name, const CaloCalibrationHit *CalibHit);
    void FCalClusterCenter(const LArHitContainer *container);
    void FCalHitCenter(const LArHitContainer *container);
    void FillCellInfo(const GeoLArHit &fcalhit, std::vector<double> *cell_E,
                      std::vector<double> *hit_x, std::vector<double> *hit_y,
                      std::vector<double> *hit_ieta, std::vector<double> *hit_iphi,
                      int &NCell);
    StatusCode doCalib();

private:
    /** methods called by execute() */

    // To add event info to new ntuple (used to go by default in CollectionTree)
    StatusCode addEventInfo();

private:
    /** Athena-Aware Ntuple (AAN) variables - branches of the AAN TTree */
    TTree *m_tree_AS;

    unsigned int m_runNumber;
    unsigned int m_eventNumber;
    unsigned int m_eventTime;
    unsigned int m_lumiBlock;
    unsigned int m_bCID;
    double m_eventWeight;

    /* ----- FCal-related variables ----- */
    double m_vertx; // x-position for vertex generated particle (beam)
    double m_verty;
    double m_vertz;

    double m_vertex_eta; // eta value of generated particle
    double m_vertex_phi;

    double m_pt; // Momentum of generated particle
    double m_px;
    double m_py;
    double m_pz;

    double m_E; // Energy of generated particle

    int m_NCell1; // Number of cells hit
    int m_NCell2;
    int m_NCell3;

    double m_x_mc_cc1; // Center of cluster in x (FCal1, extrapolated)
    double m_y_mc_cc1;

    double m_x_mc_cc2; // Center of cluster in x (FCal2, extrapolated)
    double m_y_mc_cc2;

    double m_x_mc_cc3; // Center of cluster in x (FCal3, extrapolated)
    double m_y_mc_cc3;

    double m_x_cc1; // Center of cluster in x (FCal1, c of g)
    double m_y_cc1;

    double m_x_cc2; // Center of cluster in x (FCal2, c of g)
    double m_y_cc2;

    double m_x_cc3; // Center of cluster in x (FCal3, c of g)
    double m_y_cc3;

    std::vector<int> *m_pdg_id;  // particle id

    std::vector<double> *m_hit_x1; // hit positions of cells
    std::vector<double> *m_hit_y1;

    std::vector<double> *m_hit_x2;
    std::vector<double> *m_hit_y2;

    std::vector<double> *m_hit_x3;
    std::vector<double> *m_hit_y3;

    std::vector<double> *m_hit_ieta1; // hit indices of cells
    std::vector<double> *m_hit_iphi1;
    std::vector<double> *m_hit_ieta2;
    std::vector<double> *m_hit_iphi2;
    std::vector<double> *m_hit_ieta3;
    std::vector<double> *m_hit_iphi3;

    std::vector<double> *m_cell1_E; // Energy in cells
    std::vector<double> *m_cell2_E;
    std::vector<double> *m_cell3_E;

    double m_FCal1_SumE; // Energy in individual FCal modules
    double m_FCal2_SumE;
    double m_FCal3_SumE;
    double m_TCScint_E;
    double m_TCIron_E;

    double m_totalFCalEnergy;
    int m_numHitsFCal;

    /* ----- Calibration Hits Variables ----- */
    double m_totalCalibrationEnergy; // Total energy

    // Physic processes
    double m_totalEmEnergy;
    double m_totalNonEmEnergy;
    double m_totalInvisibleEnergy;
    double m_totalEscapedEnergy;

    // Energy deposited in different material categories
    double m_totalActiveEnergy;
    double m_totalInactiveEnergy;
    double m_totalDeadMaterialEnergy;

    // Number of hits in different material categories
    int m_numHitsActive;
    int m_numHitsInactive;
    int m_numHitsDead;

    // Total energy deposited in the different FCal Modules
    double m_totalFCalCalibrationEnergy; // Energy in all FCal
    double m_totalFCal1CalibrationEnergy;
    double m_totalFCal2CalibrationEnergy;
    double m_totalFCal3CalibrationEnergy;

    double m_FCalActive;
    double m_FCalInactive;
    double m_FCalDead;
    double m_FCalActiveEm;
    double m_FCalActiveNonEm;
    double m_FCalActiveInvisible;
    double m_FCalActiveEscaped;
    double m_FCalEm;
    double m_FCal1Em;
    double m_FCal2Em;
    double m_FCal3Em;
    double m_FCalNonEm;
    double m_FCal1NonEm;
    double m_FCal2NonEm;
    double m_FCal3NonEm;
    double m_FCalInvisible;
    double m_FCal1Invisible;
    double m_FCal2Invisible;
    double m_FCal3Invisible;
    double m_FCalEscaped;
    double m_FCal1Escaped;
    double m_FCal2Escaped;
    double m_FCal3Escaped;
    double m_PhysTotE;
    double m_FCal1Active;
    double m_FCal2Active;
    double m_FCal3Active;
    double m_FCal1Inactive;
    double m_FCal2Inactive;
    double m_FCal3Inactive;

    const CaloDM_ID *m_caloDmID;
    const CaloCell_ID *m_caloCellID;
    const LArFCAL_ID *m_larFCalID;

    // For LAr analysis: Do we have any calibration hits at all?
    bool m_calibrationRun;

    // In order to write some clever loops, store all the hit containers
    // of a given type in a map.
    typedef std::map<std::string, const CaloCalibrationHitContainer *> m_calibHitMap_t;
    typedef m_calibHitMap_t::iterator m_calibHitMap_ptr_t;
    m_calibHitMap_t m_calibHitMap;

    // Variables needed for cluster centers
    double m_cx1, m_cx2, m_cx3;
    double m_cy1, m_cy2, m_cy3;
};

#endif // LAR_FCAL_SAMPLING_FRACTION_H
