/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LAR_FCAL_SAMPLING_FRACTION_H
#define LAR_FCAL_SAMPLING_FRACTION_H

// C++
#include <string>

// ROOT
#include "TH1.h"

// ATLAS Software
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GeneratorObjects/McEventCollection.h"
//#include "AnalysisTools/AnalysisTools.h"
//#include "GeoAdaptors/GeoFCALHit.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
//#include "UserAnalysisUtils/UserAnalysisSelectionTool.h"
//#include "UserAnalysisUtils/UserAnalysisPreparationTool.h"
//#include "UserAnalysisUtils/UserAnalysisOverlapCheckingTool.h"
//#include "UserAnalysisUtils/UserAnalysisOverlapRemovalTool.h"

//#include "TrigDecisionTool/TrigDecisionTool.h"

//#include "TLorentzVector.h"
//#include "CLHEP/Vector/LorentzVector.h"
#include "GeoAdaptors/GeoLArHit.h"

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

//using namespace Trig;
//namespace Trig {
//  class ChainGroup;
//}

// upgrade to inherit from AthAlgorithm

class LArFCalSamplingFraction : public AthAlgorithm
{
public:
    LArFCalSamplingFraction(const std::string &name, ISvcLocator *pSvcLocator);
    ~LArFCalSamplingFraction();

    virtual StatusCode beginRun();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute();
    virtual StatusCode initEvent();
    StatusCode doFCal();
    void TruthImpactPosition(McEventCollection::const_iterator);

    void FCalCalibAnalysis(const std::string name, const CaloCalibrationHit *CalibHit);
    void FCalClusterCenter(const LArHitContainer *container);
    void FCalHitCenter(const LArHitContainer *container);
    void FillCellInfo(const GeoLArHit &fcalhit, std::vector<double> *cell_E, std::vector<double> *hit_x,
                      std::vector<double> *hit_y, std::vector<double> *hit_ieta, std::vector<double> *hit_iphi,
                      int &NCell);
    StatusCode doCalib();

private:
    /** methods called by execute() */

    // to add event info to new ntuple (used to go by default in CollectionTree)
    StatusCode addEventInfo();

private:
    /** get a handle to the tool helper */
    //   ToolHandle<AnalysisTools> m_analysisTools;

    /** get a handle on the user tool for pre-selection and overlap removal */
    //   ToolHandle<UserAnalysisSelectionTool>       m_analysisSelectionTool;
    //   ToolHandle<UserAnalysisPreparationTool>     m_analysisPreparationTool;
    //   ToolHandle<UserAnalysisOverlapCheckingTool> m_analysisOverlapCheckingTool;
    //   ToolHandle<UserAnalysisOverlapRemovalTool>  m_analysisOverlapRemovalTool;

    /** tool to access the trigger decision */
    //   ToolHandle<Trig::TrigDecisionTool> m_trigDec;

    /** a handle on the Hist/TTree registration service */
    ITHistSvc *m_thistSvc;

    /** a handle on Store Gate for access to the Event Store */
    StoreGateSvc *m_storeGate;

    /** the key of the Electron Container to retrieve from the AOD */
    //   std::string m_electronContainerName;

    /** name of the AOD truth particle container to retrieve from StoreGate */
    //   std::string m_truthParticleContainerName;

    /** key to get missing ET information */
    //   std::string m_missingETObjectName;

    /// The missing ET object
    //   const MissingET * m_pMissing;
    //   double m_pxMiss;
    //   double m_pyMiss;
    //   double m_ptMiss;

    /** Athena-Aware Ntuple (AAN) variables - branches of the AAN TTree */

    // stuff for new ntuple
    // The standard AANT, CollectionTree, is bare bones
    TTree *m_tree_AS;

    unsigned int m_runNumber;
    unsigned int m_eventNumber;
    unsigned int m_eventTime;
    unsigned int m_lumiBlock;
    unsigned int m_bCID;
    double m_eventWeight;
    unsigned int m_statusElement;


    // FCal-related variables
    double vertx; // x-position for vertex generated particle (beam)
    double verty;
    double vertz;

    double vertex_eta; // eta value of generated particle
    double vertex_phi;
    double pt;

    double px; // Momentum of generated particle
    double py;
    double pz;

    double E; // Energy of generated particle

    int NCell1; // Number of cells hit
    int NCell2;
    int NCell3;

    double x_mc_cc1; // Center of cluster in x (FCal1, extrapolated)
    double y_mc_cc1;

    double x_mc_cc2; // Center of cluster in x (FCal2, extrapolated)
    double y_mc_cc2;

    double x_mc_cc3; // Center of cluster in x (FCal3, extrapolated)
    double y_mc_cc3;

    double x_cc1; // Center of cluster in x (FCal1, c of g)
    double y_cc1;

    double x_cc2; // Center of cluster in x (FCal2, c of g)
    double y_cc2;

    double x_cc3; // Center of cluster in x (FCal3, c of g)
    double y_cc3;

    std::vector<int> *m_pdg_id;  // particle id
    std::vector<double> *hit_x1; // hit positions of cells
    std::vector<double> *hit_y1;

    std::vector<double> *hit_x2;
    std::vector<double> *hit_y2;

    std::vector<double> *hit_x3;
    std::vector<double> *hit_y3;

    std::vector<double> *hit_ieta1; // hit indices of cells
    std::vector<double> *hit_iphi1;
    std::vector<double> *hit_ieta2;
    std::vector<double> *hit_iphi2;
    std::vector<double> *hit_ieta3;
    std::vector<double> *hit_iphi3;

    std::vector<double> *cell1_E; // Energy in cells
    std::vector<double> *cell2_E;
    std::vector<double> *cell3_E;

    double FCal1_SumE; // Energy in individual FCal modules
    double FCal2_SumE;
    double FCal3_SumE;
    double TCScint_E;
    double TCIron_E;

    double m_totalFCalEnergy;
    int m_numHitsFCal;

    //--------------------- Calibration Hits Variables -----------------------//
    //------------------------------------------------------------------------//

    double m_totalCalibrationEnergy; // Total energy

    // Physic Processes
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
    double cx1, cx2, cx3;
    double cy1, cy2, cy3;
};

#endif // LAR_FCAL_SAMPLING_FRACTION_H
