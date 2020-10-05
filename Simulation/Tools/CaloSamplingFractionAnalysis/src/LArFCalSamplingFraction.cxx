/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFCalSamplingFraction.h"

// C++ stdlib
#include <algorithm>
#include <functional>
#include <iostream>
#include <cmath>

// Root
#include "TTree.h"

// ATLAS Software
#include "GaudiKernel/IToolSvc.h"

// Event Info
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "GaudiKernel/ITHistSvc.h"

// ID classes
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "Identifier/Identifier.h"

// Hits and hit collections
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "GeoAdaptors/GeoLArHit.h"
#include "GeoAdaptors/GeoCaloCalibHit.h"

// For Cryostat Positions
#include "LArG4RunControl/LArG4TBPosOptions.h"

// Other useful tools for particle/event/beam info
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/SimpleVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"


///////////////////////////////////////////////////////////////////////////////
/// Constructor

LArFCalSamplingFraction::LArFCalSamplingFraction(const std::string &name, ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_calibrationRun(false),
      m_cx1(0.0), m_cx2(0.0), m_cx3(0.0),
      m_cy1(0.0), m_cy2(0.0), m_cy3(0.0)
{
    declareProperty("Calibration", m_calibrationRun);
}


///////////////////////////////////////////////////////////////////////////////
/// Destructor

LArFCalSamplingFraction::~LArFCalSamplingFraction() {}


///////////////////////////////////////////////////////////////////////////////
/// Initialize
///  - Get a handle on the analysis tools
///  - Set up output TTree

StatusCode LArFCalSamplingFraction::initialize()
{
    ATH_MSG_INFO("Initializing LArFCalSamplingFraction");

    // Get a handle on the NTuple and histogramming service
    ServiceHandle<ITHistSvc> histSvc("THistSvc", name());
    CHECK(histSvc.retrieve());
    ATH_MSG_DEBUG(" retrieved THistSvc");

    const CaloIdManager *caloIdManager;
    ATH_CHECK(detStore()->retrieve(caloIdManager));

    // Use just for now for Calibration... change later to GeoCalibHit
    m_larFCalID = caloIdManager->getFCAL_ID();

    if (m_larFCalID == 0)
        throw GaudiException("Invalid LAr FCAL ID helper", "LArHitAnalysis", StatusCode::FAILURE);

    // Use for now... soon change to GeoCalibHit
    m_caloCellID = caloIdManager->getCaloCell_ID();

    if (m_caloCellID == 0)
        throw GaudiException("Invalid Calo Cell ID helper", "LArHitAnalysis", StatusCode::FAILURE);

    m_pdg_id = new std::vector<int>; // particle id

    m_hit_x1 = new std::vector<double>; // hit positions of cells
    m_hit_y1 = new std::vector<double>;

    m_hit_x2 = new std::vector<double>;
    m_hit_y2 = new std::vector<double>;

    m_hit_x3 = new std::vector<double>;
    m_hit_y3 = new std::vector<double>;

    m_hit_ieta1 = new std::vector<double>; // hit indices of cells
    m_hit_iphi1 = new std::vector<double>;
    m_hit_ieta2 = new std::vector<double>;
    m_hit_iphi2 = new std::vector<double>;
    m_hit_ieta3 = new std::vector<double>;
    m_hit_iphi3 = new std::vector<double>;

    m_cell1_E = new std::vector<double>; // Energy in cells
    m_cell2_E = new std::vector<double>;
    m_cell3_E = new std::vector<double>;

    // Now add branches and leaves to the AAN tree
    m_tree_AS = new TTree("tree_AS", "TTree of AnalysisSkleton");

    // Event info
    m_tree_AS->Branch("Run", &m_runNumber, "Run/I");             // run number
    m_tree_AS->Branch("Event", &m_eventNumber, "Event/I");       // event number
    m_tree_AS->Branch("Time", &m_eventTime, "Time/I");           // time stamp
    m_tree_AS->Branch("LumiBlock", &m_lumiBlock, "LumiBlock/I"); // lumi block num
    m_tree_AS->Branch("BCID", &m_bCID, "BCID/I");                // bunch crossing ID
    m_tree_AS->Branch("Weight", &m_eventWeight, "Weight/D");     // weight

    // FCal-specific and other variables
    m_tree_AS->Branch("EFCal", &m_totalFCalEnergy, "EFCal/D");
    m_tree_AS->Branch("NHitsFCal", &m_numHitsFCal, "NhitsFCal/I");

    m_tree_AS->Branch("Vertex_Eta", &m_vertex_eta, "Vertex_Eta/D");
    m_tree_AS->Branch("Vertex_Phi", &m_vertex_phi, "Vertex_Phi/D");

    m_tree_AS->Branch("Pt", &m_pt, "Pt/D");
    m_tree_AS->Branch("px", &m_px, "px/D");
    m_tree_AS->Branch("py", &m_py, "py/D");
    m_tree_AS->Branch("pz", &m_pz, "pz/D");
    m_tree_AS->Branch("E", &m_E, "E/D");

    m_tree_AS->Branch("VertexX", &m_vertx, "VertexX/D");
    m_tree_AS->Branch("VertexY", &m_verty, "VertexY/D");
    m_tree_AS->Branch("VertexZ", &m_vertz, "VertexZ/D");

    m_tree_AS->Branch("MC_CCX1", &m_x_mc_cc1, "MC_CCX1/D");
    m_tree_AS->Branch("MC_CCY1", &m_y_mc_cc1, "MC_CCY1/D");

    m_tree_AS->Branch("MC_CCX2", &m_x_mc_cc2, "MC_CCX2/D");
    m_tree_AS->Branch("MC_CCY2", &m_y_mc_cc2, "MC_CCY2/D");

    m_tree_AS->Branch("MC_CCX3", &m_x_mc_cc3, "MC_CCX3/D");
    m_tree_AS->Branch("MC_CCY3", &m_y_mc_cc3, "MC_CCY3/D");

    m_tree_AS->Branch("CCX1", &m_x_cc1, "CCX1/D");
    m_tree_AS->Branch("CCY1", &m_y_cc1, "CCY1/D");

    m_tree_AS->Branch("CCX2", &m_x_cc2, "CCX2/D");
    m_tree_AS->Branch("CCY2", &m_y_cc2, "CCY2/D");

    m_tree_AS->Branch("CCX3", &m_x_cc3, "CCX3/D");
    m_tree_AS->Branch("CCY3", &m_y_cc3, "CCY3/D");

    m_tree_AS->Branch("NCell_1", &m_NCell1, "NCell1_1/I");
    m_tree_AS->Branch("NCell_2", &m_NCell2, "NCell1_2/I");
    m_tree_AS->Branch("NCell_3", &m_NCell3, "NCell1_3/I");

    m_tree_AS->Branch("ParticleID", &m_pdg_id);

    m_tree_AS->Branch("Hit_X1", &m_hit_x1);
    m_tree_AS->Branch("Hit_Y1", &m_hit_y1);

    m_tree_AS->Branch("Hit_X2", &m_hit_x2);
    m_tree_AS->Branch("Hit_Y2", &m_hit_y2);

    m_tree_AS->Branch("Hit_X3", &m_hit_x3);
    m_tree_AS->Branch("Hit_Y3", &m_hit_y3);

    m_tree_AS->Branch("Hit_eta1", &m_hit_ieta1);
    m_tree_AS->Branch("Hit_phi1", &m_hit_iphi1);
    m_tree_AS->Branch("Hit_eta2", &m_hit_ieta2);
    m_tree_AS->Branch("Hit_phi2", &m_hit_iphi2);
    m_tree_AS->Branch("Hit_eta3", &m_hit_ieta3);
    m_tree_AS->Branch("Hit_phi3", &m_hit_iphi3);

    m_tree_AS->Branch("Cell1_E", &m_cell1_E);
    m_tree_AS->Branch("Cell2_E", &m_cell2_E);
    m_tree_AS->Branch("Cell3_E", &m_cell3_E);

    m_tree_AS->Branch("FCal1_E", &m_FCal1_SumE, "FCal1_E/D");
    m_tree_AS->Branch("FCal2_E", &m_FCal2_SumE, "FCal2_E/D");
    m_tree_AS->Branch("FCal3_E", &m_FCal3_SumE, "FCal3_E/D");

    if (m_calibrationRun) {
        m_caloDmID = caloIdManager->getDM_ID();

        if (m_caloDmID == 0)
            throw GaudiException("Invalid Calo DM ID helper", "LArFCalTB_MC_CBNT_AA", StatusCode::FAILURE);

        // Define the hit containers that we'll analyze in this program.
        // For now, initialize the pointers to the containers to NULL (zero).
        m_calibHitMap["LArCalibrationHitActive"] = 0;
        m_calibHitMap["LArCalibrationHitInactive"] = 0;
        m_calibHitMap["LArCalibrationHitDeadMaterial"] = 0;

        m_tree_AS->Branch("Calib_TotalEnergy", &m_totalCalibrationEnergy, "Calib_TotalEnergy/D");
        m_tree_AS->Branch("Calib_TotalEm", &m_totalEmEnergy, "Calib_TotalEm/D");
        m_tree_AS->Branch("Calib_TotalNonEm", &m_totalNonEmEnergy, "Calib_TotalNonEm/D");
        m_tree_AS->Branch("Calib_TotalInvisible", &m_totalInvisibleEnergy, "Calib_TotalInvisible/D");
        m_tree_AS->Branch("Calib_TotalEscaped", &m_totalEscapedEnergy, "Calib_TotalEscaped/D");
        m_tree_AS->Branch("Calib_FCalEnergy", &m_totalFCalCalibrationEnergy, "Calib_FCalEnergy/D");
        m_tree_AS->Branch("Calib_TotalActive", &m_totalActiveEnergy, "Calib_TotalActive/D");
        m_tree_AS->Branch("Calib_TotalInactive", &m_totalInactiveEnergy, "Calib_TotalInactive/D");
        m_tree_AS->Branch("Calib_DeadEnergy", &m_totalDeadMaterialEnergy, "Calib_DeadEnergy/D");
        m_tree_AS->Branch("Calib_NHitsInactive", &m_numHitsInactive, "Calib_NHitsInactive/I");
        m_tree_AS->Branch("Calib_NHitsDead", &m_numHitsDead, "Calib_NHitsDead/I");
        m_tree_AS->Branch("Calib_FCal1Energy", &m_totalFCal1CalibrationEnergy, "Calib_FCal1Energy/D");
        m_tree_AS->Branch("Calib_FCal2Energy", &m_totalFCal2CalibrationEnergy, "Calib_FCal2Energy/D");
        m_tree_AS->Branch("Calib_FCal3Energy", &m_totalFCal3CalibrationEnergy, "Calib_FCal3Energy/D");

        m_tree_AS->Branch("Calib_FCalActive", &m_FCalActive, "Calib_FCalActive/D");
        m_tree_AS->Branch("Calib_FCalInactive", &m_FCalInactive, "Calib_FCalInactive/D");
        m_tree_AS->Branch("Calib_FCalDead", &m_FCalDead, "Calib_FCalDead/D");
        m_tree_AS->Branch("Calib_FCalEm", &m_FCalEm, "Calib_FCalEm/D");
        m_tree_AS->Branch("Calib_FCal1Em", &m_FCal1Em, "Calib_FCal1Em/D");
        m_tree_AS->Branch("Calib_FCal2Em", &m_FCal2Em, "Calib_FCal2Em/D");
        m_tree_AS->Branch("Calib_FCal3Em", &m_FCal3Em, "Calib_FCal3Em/D");
        m_tree_AS->Branch("Calib_FCalNonEm", &m_FCalNonEm, "Calib_FCalNonEm/D");
        m_tree_AS->Branch("Calib_FCal1NonEm", &m_FCal1NonEm, "Calib_FCal1NonEm/D");
        m_tree_AS->Branch("Calib_FCal2NonEm", &m_FCal2NonEm, "Calib_FCal2NonEm/D");
        m_tree_AS->Branch("Calib_FCal3NonEm", &m_FCal3NonEm, "Calib_FCal3NonEm/D");
        m_tree_AS->Branch("Calib_FCalInvisible", &m_FCalInvisible, "Calib_FCalInvisible/D");
        m_tree_AS->Branch("Calib_FCal1Invisible", &m_FCal1Invisible, "Calib_FCal1Invisible/D");
        m_tree_AS->Branch("Calib_FCal2Invisible", &m_FCal2Invisible, "Calib_FCal2Invisible/D");
        m_tree_AS->Branch("Calib_FCal3Invisible", &m_FCal3Invisible, "Calib_FCal3Invisible/D");
        m_tree_AS->Branch("Calib_FCalEscaped", &m_FCalEscaped, "Calib_FCalEscaped/D");
        m_tree_AS->Branch("Calib_FCal1Escaped", &m_FCal1Escaped, "Calib_FCal1Escaped/D");
        m_tree_AS->Branch("Calib_FCal2Escaped", &m_FCal2Escaped, "Calib_FCal2Escaped/D");
        m_tree_AS->Branch("Calib_FCal3Escaped", &m_FCal3Escaped, "Calib_FCal3Escaped/D");
        m_tree_AS->Branch("Calib_FCal1Active", &m_FCal1Active, "Calib_FCal1Active/D");
        m_tree_AS->Branch("Calib_FCal2Active", &m_FCal2Active, "Calib_FCal2Active/D");
        m_tree_AS->Branch("Calib_FCal3Active", &m_FCal3Active, "Calib_FCal3Active/D");
        m_tree_AS->Branch("Calib_FCal1Inactive", &m_FCal1Inactive, "Calib_FCal1Inactive/D");
        m_tree_AS->Branch("Calib_FCal2Inactive", &m_FCal2Inactive, "Calib_FCal2Inactive/D");
        m_tree_AS->Branch("Calib_FCal3Inactive", &m_FCal3Inactive, "Calib_FCal3Inactive/D");
        m_tree_AS->Branch("Calib_FCalActiveEm", &m_FCalActiveEm, "Calib_FCalActiveEm/D");
        m_tree_AS->Branch("Calib_FCalActiveNonEm", &m_FCalActiveNonEm, "Calib_FCalActiveNonEm/D");
        m_tree_AS->Branch("Calib_FCalActiveInvisible", &m_FCalActiveInvisible, "Calib_FCalActiveInvisible/D");
        m_tree_AS->Branch("Calib_FCalActiveEscaped", &m_FCalActiveEscaped, "Calib_FCalActiveEscaped/D");
    }

    CHECK(histSvc->regTree("/AANT/tree_AS", m_tree_AS));

    m_eventNumber = 0;

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
/// Finalize
///  - delete any memory allocation from the heap

StatusCode LArFCalSamplingFraction::finalize()
{
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
/// Init event
///  - clear CBNT members

StatusCode LArFCalSamplingFraction::initEvent()
{
    /// For Athena-Aware NTuple

    ATH_MSG_INFO("Initializing event, clearing variables");

    m_vertx = 0; // x-position for vertex generated particle (beam)
    m_verty = 0;
    m_vertz = 0;

    m_vertex_eta = 0; // eta value of generated particle
    m_vertex_phi = 0;

    m_pt = 0; // Momentum of generated particle
    m_px = 0;
    m_py = 0;
    m_pz = 0;

    m_E = 0; // Energy of generated particle

    m_NCell1 = 0; // Number of cells hit
    m_NCell2 = 0;
    m_NCell3 = 0;

    m_x_mc_cc1 = 0; // Center of cluster in x (FCal1, extrapolated)
    m_y_mc_cc1 = 0;

    m_x_mc_cc2 = 0; // Center of cluster in x (FCal2, extrapolated)
    m_y_mc_cc2 = 0;

    m_x_mc_cc3 = 0; // Center of cluster in x (FCal3, extrapolated)
    m_y_mc_cc3 = 0;

    m_x_cc1 = 0; // Center of cluster in x (FCal1, c of g)
    m_y_cc1 = 0;

    m_x_cc2 = 0; // Center of cluster in x (FCal2, c of g)
    m_y_cc2 = 0;

    m_x_cc3 = 0; // Center of cluster in x (FCal3, c of g)
    m_y_cc3 = 0;

    m_pdg_id->clear(); // particle id

    m_hit_x1->clear(); // hit positions of cells
    m_hit_y1->clear();

    m_hit_x2->clear();
    m_hit_y2->clear();

    m_hit_x3->clear();
    m_hit_y3->clear();

    m_hit_ieta1->clear(); // hit indices of cells
    m_hit_iphi1->clear();
    m_hit_ieta2->clear();
    m_hit_iphi2->clear();
    m_hit_ieta3->clear();
    m_hit_iphi3->clear();

    m_cell1_E->clear(); // Energy in cells
    m_cell2_E->clear();
    m_cell3_E->clear();

    m_FCal1_SumE = 0; // Energy in individual FCal modules
    m_FCal2_SumE = 0;
    m_FCal3_SumE = 0;
    m_TCScint_E = 0;
    m_TCIron_E = 0;

    m_totalFCalEnergy = 0;
    m_numHitsFCal = 0;

    m_totalCalibrationEnergy = 0; // Total energy

    // Physic Processes
    m_totalEmEnergy = 0;
    m_totalNonEmEnergy = 0;
    m_totalInvisibleEnergy = 0;
    m_totalEscapedEnergy = 0;

    // Energy deposited in different material categories
    m_totalActiveEnergy = 0;
    m_totalInactiveEnergy = 0;
    m_totalDeadMaterialEnergy = 0;

    // Number of hits in different material categories
    m_numHitsActive = 0;
    m_numHitsInactive = 0;
    m_numHitsDead = 0;

    // Total energy deposited in the different FCal Modules
    m_totalFCalCalibrationEnergy = 0; // Energy in all FCal
    m_totalFCal1CalibrationEnergy = 0;
    m_totalFCal2CalibrationEnergy = 0;
    m_totalFCal3CalibrationEnergy = 0;

    m_FCalActive = 0;
    m_FCalInactive = 0;
    m_FCalDead = 0;
    m_FCalActiveEm = 0;
    m_FCalActiveNonEm = 0;
    m_FCalActiveInvisible = 0;
    m_FCalActiveEscaped = 0;
    m_FCalEm = 0;
    m_FCal1Em = 0;
    m_FCal2Em = 0;
    m_FCal3Em = 0;
    m_FCalNonEm = 0;
    m_FCal1NonEm = 0;
    m_FCal2NonEm = 0;
    m_FCal3NonEm = 0;
    m_FCalInvisible = 0;
    m_FCal1Invisible = 0;
    m_FCal2Invisible = 0;
    m_FCal3Invisible = 0;
    m_FCalEscaped = 0;
    m_FCal1Escaped = 0;
    m_FCal2Escaped = 0;
    m_FCal3Escaped = 0;
    m_PhysTotE = 0;
    m_FCal1Active = 0;
    m_FCal2Active = 0;
    m_FCal3Active = 0;
    m_FCal1Inactive = 0;
    m_FCal2Inactive = 0;
    m_FCal3Inactive = 0;

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
/// Calculate FCal hit center

void LArFCalSamplingFraction::FCalHitCenter(const LArHitContainer *container)
{
    double max1 = 0.0;
    double max2 = 0.0;
    double max3 = 0.0;

    // Loop over hits in container
    for (LArHit* const& hit : *container) {

        GeoLArHit fcalhit(*hit);

        if ((!fcalhit) || (!fcalhit.getDetDescrElement()->is_lar_fcal())) {
            ATH_MSG_WARNING("Hit in LarHitContainer is not a GeoFCalHit");
            continue;
        }

        double energy = fcalhit.Energy();

        int module_index = fcalhit.getDetDescrElement()->getLayer();

        double x = fcalhit.getDetDescrElement()->x();
        double y = fcalhit.getDetDescrElement()->y();

        // Determine the center of the cluster for each FCal module
        if (module_index == 1) {
            // FCal1
            if (max1 < energy) {
                max1 = energy;
                m_cx1 = x;
                m_cy1 = y;
            }
        }
        else if (module_index == 2) {
            // FCal2
            if (max2 < energy) {
                max2 = energy;
                m_cx2 = x;
                m_cy2 = y;
            }
        }
        else if (module_index == 3) {
            // FCal3
            if (max3 < energy) {
                max3 = energy;
                m_cx3 = x;
                m_cy3 = y;
            }
        }

    } // End loop over hits in container
}


///////////////////////////////////////////////////////////////////////////////
/// Calculate FCal cluster center

void LArFCalSamplingFraction::FCalClusterCenter(const LArHitContainer *container)
{
    float xNumer1 = 0.0, xNumer2 = 0.0, xNumer3 = 0.0;
    float yNumer1 = 0.0, yNumer2 = 0.0, yNumer3 = 0.0;
    float Denom1 = 0.0, Denom2 = 0.0, Denom3 = 0.0;

    double subClusterSize = 30.0;  // [mm]
    double thisCG_R = 0.0;

    // Loop over hits in container
    for (LArHit* const& hit : *container) {

        GeoLArHit fcalhit(*hit);

        if ((!fcalhit) || (!fcalhit.getDetDescrElement()->is_lar_fcal())) {
            ATH_MSG_WARNING("Hit in LarHitContainer is not a GeoFCalHit");
            continue;
        }

        double energy = fcalhit.Energy();
        int module_index = fcalhit.getDetDescrElement()->getLayer();

        double x = fcalhit.getDetDescrElement()->x();
        double y = fcalhit.getDetDescrElement()->y();

        // Determine center of cluster
        if (module_index == 1) {
            // FCal1
            double x_subcheck1 = m_cx1 - x;
            double y_subcheck1 = m_cy1 - y;
            thisCG_R = sqrt(x_subcheck1 * x_subcheck1 + y_subcheck1 * y_subcheck1);

            if (thisCG_R <= subClusterSize) {
                xNumer1 += x * energy;
                yNumer1 += y * energy;
                Denom1 += energy;
            }
        }
        else if (module_index == 2) {
            // FCal2
            double x_subcheck2 = m_cx2 - x;
            double y_subcheck2 = m_cy2 - y;
            thisCG_R = sqrt(x_subcheck2 * x_subcheck2 + y_subcheck2 * y_subcheck2);

            if (thisCG_R <= subClusterSize) {
                xNumer2 += x * energy;
                yNumer2 += y * energy;
                Denom2 += energy;
            }
        }
        else if (module_index == 3) {
            // FCal3
            double x_subcheck3 = m_cx3 - x;
            double y_subcheck3 = m_cy3 - y;
            thisCG_R = sqrt(x_subcheck3 * x_subcheck3 + y_subcheck3 * y_subcheck3);

            if (thisCG_R <= subClusterSize) {
                xNumer3 += x * energy;
                yNumer3 += y * energy;
                Denom3 += energy;
            }
        }

    } // End loop over hits in container

    if (fabs(Denom1) > 0.0) {
        m_x_cc1 = xNumer1 / Denom1;
        m_y_cc1 = yNumer1 / Denom1;
    }

    if (fabs(Denom2) > 0.0) {
        m_x_cc2 = xNumer2 / Denom2;
        m_y_cc2 = yNumer2 / Denom2;
    }

    if (fabs(Denom3) > 0.0) {
        m_x_cc3 = xNumer3 / Denom3;
        m_y_cc3 = yNumer3 / Denom3;
    }
}


///////////////////////////////////////////////////////////////////////////////
/// Calibration hit analysis

StatusCode LArFCalSamplingFraction::doCalib()
{
    StatusCode sc;

    ATH_MSG_DEBUG("Starting FCal Calibration Analysis");

    // Get the calibration hit containers (if any)
    for (m_calibHitMap_ptr_t i = m_calibHitMap.begin();
         i != m_calibHitMap.end();
         i++)
    {
        std::string name = (*i).first;
        const CaloCalibrationHitContainer *container = 0;
        sc = evtStore()->retrieve(container, name);

        if (sc.isFailure() || container == 0) {
            ATH_MSG_ERROR("CaloCalibrationHit container was not found");
            m_numHitsActive = 0;
            m_numHitsInactive = 0;
            m_numHitsDead = 0;
            (*i).second = 0;

            return StatusCode::FAILURE;
        }

        ATH_MSG_DEBUG("CaloCalibrationHit container successfully retrieved");

        (*i).second = container;
    }

    // Get the number of hits in each container
    // (The braces let us re-use the name 'container')
    {
        const CaloCalibrationHitContainer *container = m_calibHitMap["LArCalibrationHitActive"];

        if (container != 0)
            m_numHitsActive = container->Size();
        else
            m_numHitsActive = 0;
    }
    {
        const CaloCalibrationHitContainer *container = m_calibHitMap["LArCalibrationHitInactive"];

        if (container != 0)
            m_numHitsInactive = container->Size();
        else
            m_numHitsInactive = 0;
    }
    {
        const CaloCalibrationHitContainer *container = m_calibHitMap["LArCalibrationHitDeadMaterial"];

        if (container != 0)
            m_numHitsDead = container->Size();
        else
            m_numHitsDead = 0;
    }

    // Loop over all the hit containers, inspecting each hit within the collection
    for (m_calibHitMap_ptr_t i = m_calibHitMap.begin();
         i != m_calibHitMap.end();
         i++)
    {
        std::string name = (*i).first;
        const CaloCalibrationHitContainer *container = (*i).second;

        // Skip rest of loop if it's a null container.
        if (container == 0)
            continue;

        // Loop over calibration hits in container
        for (CaloCalibrationHit* const& calibhit : *container) {

            Identifier identifier = calibhit->cellID();
            double energy = calibhit->energyTotal();

            // Accumulate energy sums. Use the ID helpers to determine in which
            // detector the hit took place.

            m_totalCalibrationEnergy += energy;
            m_totalEmEnergy += calibhit->energyEM();
            m_totalNonEmEnergy += calibhit->energyNonEM();
            m_totalInvisibleEnergy += calibhit->energyInvisible();
            m_totalEscapedEnergy += calibhit->energyEscaped();

            if (name == "LArCalibrationHitActive")
                m_totalActiveEnergy += energy;

            if (name == "LArCalibrationHitInactive")
                m_totalInactiveEnergy += energy;

            if (name == "LArCalibrationHitDeadMaterial") {
                m_totalDeadMaterialEnergy += energy;

                if (m_caloDmID->is_lar(identifier) && (m_caloDmID->sampling(identifier) == 3) && (m_caloDmID->dmat(identifier) == 1))
                    m_FCalDead += energy;
            }

            if (m_caloCellID->is_lar_fcal(identifier))
                FCalCalibAnalysis(name, calibhit);

        } // End loop over calibration hits in container
    } // End loop over containers

    // Execution completed

    ATH_MSG_DEBUG("doCalib() completed successfully");
    return sc;
}


///////////////////////////////////////////////////////////////////////////////
/// FCal Analysis with Calibration Hits on
/// Added by JPA, June 2005

void LArFCalSamplingFraction::FCalCalibAnalysis(const std::string& name, const CaloCalibrationHit *CalibHit)
{
    Identifier id = CalibHit->cellID();
    double energy = CalibHit->energyTotal();

    m_totalFCalCalibrationEnergy += energy;
    //// Added by JPA Feb. 2005
    m_FCalEm += CalibHit->energyEM();
    m_FCalNonEm += CalibHit->energyNonEM();
    m_FCalInvisible += CalibHit->energyInvisible();
    m_FCalEscaped += CalibHit->energyEscaped();

    if (m_larFCalID->module(id) == 1) {
        // FCal1
        m_totalFCal1CalibrationEnergy += energy;
        m_FCal1Em += CalibHit->energyEM();
        m_FCal1NonEm += CalibHit->energyNonEM();
        m_FCal1Invisible += CalibHit->energyInvisible();
        m_FCal1Escaped += CalibHit->energyEscaped();
    }
    else if (m_larFCalID->module(id) == 2) {
        // FCal2
        m_totalFCal2CalibrationEnergy += energy;
        m_FCal2Em += CalibHit->energyEM();
        m_FCal2NonEm += CalibHit->energyNonEM();
        m_FCal2Invisible += CalibHit->energyInvisible();
        m_FCal2Escaped += CalibHit->energyEscaped();
    }
    else if (m_larFCalID->module(id) == 3) {
        // FCal3
        m_totalFCal3CalibrationEnergy += energy;
        m_FCal3Em += CalibHit->energyEM();
        m_FCal3NonEm += CalibHit->energyNonEM();
        m_FCal3Invisible += CalibHit->energyInvisible();
        m_FCal3Escaped += CalibHit->energyEscaped();
    }

    if (name == "LArCalibrationHitActive") {
        m_FCalActive += energy;
        m_FCalActiveEm += CalibHit->energyEM();
        m_FCalActiveNonEm += CalibHit->energyNonEM();
        m_FCalActiveInvisible += CalibHit->energyInvisible();
        m_FCalActiveEscaped += CalibHit->energyEscaped();

        if (m_larFCalID->module(id) == 1)
            m_FCal1Active += energy;

        if (m_larFCalID->module(id) == 2)
            m_FCal2Active += energy;

        if (m_larFCalID->module(id) == 3)
            m_FCal3Active += energy;
    }

    if (name == "LArCalibrationHitInactive") {
        m_FCalInactive += energy;

        if (m_larFCalID->module(id) == 1)
            m_FCal1Inactive += energy;

        if (m_larFCalID->module(id) == 2)
            m_FCal2Inactive += energy;

        if (m_larFCalID->module(id) == 3)
            m_FCal3Inactive += energy;
    }
}


///////////////////////////////////////////////////////////////////////////////
/// Calculate truth impact position

void LArFCalSamplingFraction::TruthImpactPosition(McEventCollection::const_iterator e)
{
    for (auto theParticle: **e)
    {
        // Note: old GenParticles used HepLorentzVectors, now they use HepMC::FourVectors

        // Get the kinematic variables
        const HepMC::FourVector& HMCmom = theParticle->momentum();
        CLHEP::HepLorentzVector momentum(CLHEP::Hep3Vector(HMCmom.px(), HMCmom.py(), HMCmom.pz()), HMCmom.e());

        HepMC::FourVector HMC3vec(0.0,0.0,0.0,0.0);
        if (theParticle->production_vertex()) HMC3vec = theParticle->production_vertex()->position();
        HepGeom::Point3D<double> origin = HepGeom::Point3D<double>(HMC3vec.x(), HMC3vec.y(), HMC3vec.z());

        // Put VEta and VPhi into the Ntuple
        m_vertex_phi = momentum.vect().phi();
        m_vertex_eta = -log(tan(momentum.vect().theta() / 2));

        if (!finite(m_vertex_eta))
            m_vertex_eta = 0;

        m_pt = momentum.vect().perp();

        m_px = momentum.px();
        m_py = momentum.py();
        m_pz = momentum.pz();

        m_E = momentum.e();

        m_vertx = theParticle->production_vertex()->position().x();
        m_verty = theParticle->production_vertex()->position().y();
        m_vertz = theParticle->production_vertex()->position().z();

        // Must get x-offset depending on TB position. The 90.0 mm is from the
        // initial x-offset of FCal in cryostat. The -15.0 mm is from the
        // initial y-offset of FCal in cryostat. The second number changes
        // between different positions (these numbers will be in database soon)

        std::string nickname;
        double xoffset = 0;
        double sinangle = 0;
        double yoffset = 0;
        const double z1 = -32668.5 * CLHEP::mm; // This is 4668.5 (z=0 to FCal1 Face) + 28000 (B9 to z=0)
        const double z2 = -33128.3 * CLHEP::mm; // This is 5128.3 (z=0 to FCal1 Face) + 28000 (B9 to z=0)
        const double z3 = -33602.8 * CLHEP::mm; // This is 5602.8 (z=0 to FCal1 Face) + 28000 (B9 to z=0)

        double shift2 = sinangle * (5128.3 - 4668.5) * CLHEP::mm;
        double shift3 = sinangle * (5602.8 - 4668.5) * CLHEP::mm;

        // Accounts for rotation of Fcal + cryostat.
        m_x_mc_cc1 = origin.x() + m_px * z1 / m_pz + xoffset;
        m_y_mc_cc1 = origin.y() + m_py * z1 / m_pz + yoffset;

        m_x_mc_cc2 = origin.x() + m_px * z2 / m_pz + shift2 + xoffset;
        m_y_mc_cc2 = origin.y() + m_py * z2 / m_pz + yoffset;

        m_x_mc_cc3 = origin.x() + m_px * z3 / m_pz + shift3 + xoffset;
        m_y_mc_cc3 = origin.y() + m_py * z3 / m_pz + yoffset;

    } // particles
}


///////////////////////////////////////////////////////////////////////////////
/// The main FCal analysis method

StatusCode LArFCalSamplingFraction::doFCal()
{
    ATH_MSG_INFO("Starting main FCal analysis");

    ATH_MSG_DEBUG("LArFCalSamplingFraction parameters are:");
    ATH_MSG_DEBUG("Calibration: " << m_calibrationRun);

    StatusCode sc;

    // ACCESSING EVENT INFORMATION
    // Get the basic event information (run number, event number).

    const EventInfo *eventInfo = 0;
    sc = evtStore()->retrieve(eventInfo);

    if (sc.isFailure()) {
        ATH_MSG_ERROR("Could not fetch event information");
        return sc;
    }

    const EventID *eventID = eventInfo->event_ID();
    m_runNumber = eventID->run_number();
    m_eventNumber = eventID->event_number();

    ATH_MSG_INFO("Run " << m_runNumber << ", event " << m_eventNumber);

    // How to access MC Truth information:
    const McEventCollection *mcEventCollection;
    sc = evtStore()->retrieve(mcEventCollection, "TruthEvent");

    if (sc.isSuccess()) {
        // There can potentially be more than one MC event in the collection.
        McEventCollection::const_iterator mcEvent;
        int numMcEvent = 0;

        for (mcEvent = mcEventCollection->begin();
             mcEvent != mcEventCollection->end();
             mcEvent++, numMcEvent++)
            TruthImpactPosition(mcEvent);
    } // retrieved MC event collection
    else {
        ATH_MSG_DEBUG("Run " << m_runNumber << ", event " << m_eventNumber
                      << ": could not fetch MC event collection");
    }

    // Accessing Hits
    // Regular hits
    std::string FCalContainerName = "LArHitFCAL";
    const LArHitContainer *container = 0;
    sc = evtStore()->retrieve(container, FCalContainerName);

    if (sc.isFailure() || container == 0) {
        ATH_MSG_ERROR("LArHitFCAL container was not found");
        m_numHitsFCal = 0;
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("LArHitFCAL container successfully retrieved");

    // Get the number of hits in the LArHitFCAL container
    m_numHitsFCal = container->size();
    ATH_MSG_INFO("NumHitsFCal = " << m_numHitsFCal);

    if (m_numHitsFCal > 0) {
        // Calculate the center of gravity
        FCalHitCenter(container);
        FCalClusterCenter(container);
    }

    // Loop over hits in container
    for (LArHit* const& hit : *container) {

        GeoLArHit fcalhit(*hit);

        // Added by JPA to get particle id for each hit
        const McEventCollection *mcEventCollection;
        sc = evtStore()->retrieve(mcEventCollection, "TruthEvent");

        if (sc.isSuccess()) {
            // There can potentially be more than one MC event in the collection
            McEventCollection::const_iterator mcEvent;
            int numParticles = 0;

            for (mcEvent = mcEventCollection->begin();
                 mcEvent != mcEventCollection->end();
                 mcEvent++)
            {
                // There may be many particles per event
                for (auto theParticle: **mcEvent)
                {
                    m_pdg_id->push_back(theParticle->pdg_id());
                    numParticles++;
                }
            }
        } // retrieved MC event collection

        // End JPA particle id

        if ((!fcalhit) || (!fcalhit.getDetDescrElement()->is_lar_fcal())) {
            ATH_MSG_ERROR("GeoFCalHit is not defined");
        }

        double energy = fcalhit.Energy();
        int module_index = fcalhit.getDetDescrElement()->getLayer();
        m_totalFCalEnergy += energy;

        if (module_index == 1) {
            m_FCal1_SumE += energy;
            FillCellInfo(fcalhit, m_cell1_E, m_hit_x1, m_hit_y1, m_hit_ieta1, m_hit_iphi1, m_NCell1);
        }
        else if (module_index == 2) {
            m_FCal2_SumE += energy;
            FillCellInfo(fcalhit, m_cell2_E, m_hit_x2, m_hit_y2, m_hit_ieta2, m_hit_iphi2, m_NCell2);
        }
        else if (module_index == 3) {
            m_FCal3_SumE += energy;
            FillCellInfo(fcalhit, m_cell3_E, m_hit_x3, m_hit_y3, m_hit_ieta3, m_hit_iphi3, m_NCell3);
        }
    } // End loop over hits in container

    // Calibration hit analysis.
    if (m_calibrationRun)
        CHECK(doCalib());

    ATH_MSG_DEBUG("doFCal() completed successfully");

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
/// Fill FCal cell information

void LArFCalSamplingFraction::FillCellInfo(const GeoLArHit &fcalhit, std::vector<double> *cell_E,
                                           std::vector<double> *hit_x, std::vector<double> *hit_y,
                                           std::vector<double> *hit_ieta, std::vector<double> *hit_iphi,
                                           int &NCell)
{
    const double energy = fcalhit.Energy();

    //const FCALTile *tile = fcalhit.getTile();
    const double hitx = fcalhit.getDetDescrElement()->x(); //tile->getX();
    const double hity = fcalhit.getDetDescrElement()->y(); //tile->getY();
    const double hiteta = 7;                               //tile->getIndexI();
    const double hitphi = 7;                               //tile->getIndexJ();

    bool cellHit = true;

    if (NCell != 0) {
        for (int icell = 0; icell < NCell; icell++) {
            if ((hitx == hit_x->at(icell)) && (hity == hit_y->at(icell))) {
                cellHit = false;
                cell_E->at(icell) += energy;
                break;
            }
        }
    }

    if (cellHit) {
        cell_E->push_back(energy);
        hit_x->push_back(hitx);
        hit_y->push_back(hity);
        hit_ieta->push_back(hiteta);
        hit_iphi->push_back(hitphi);
        NCell += 1;
    }
}


//////////////////////////////////////////////////////////////////////////////////
/// Execute (event by event)

StatusCode LArFCalSamplingFraction::execute()
{
    ATH_MSG_DEBUG(" in execute()");

    m_eventNumber++;

    // Initialize first before processing each event
    StatusCode sc = initEvent();

    if (sc.isFailure())
        ATH_MSG_WARNING("initEvent failed. Continue");

    sc = doFCal();

    sc = addEventInfo();

    if (sc.isFailure()) {
        ATH_MSG_WARNING("Failure in getEventInfo() ");
        return StatusCode::SUCCESS;
    }

    m_tree_AS->Fill();

    return StatusCode::SUCCESS;
}


StatusCode LArFCalSamplingFraction::addEventInfo()
{
    ATH_MSG_DEBUG("in addEventInfo()");

    // This code has been taken from AnalysisExamples/VFitZmmOnAOD
    // I have the actual EventNumber, but skipped the sequential count of event #

    // Get EventInfo for run and event number
    const EventInfo *eventInfo;
    StatusCode sc = evtStore()->retrieve(eventInfo);

    if (sc.isFailure()) {
        ATH_MSG_WARNING("Could not retrieve event info");
        return sc;
    }

    const EventID *myEventID = eventInfo->event_ID();

    m_runNumber = myEventID->run_number();
    m_eventNumber = myEventID->event_number();
    ATH_MSG_DEBUG("event " << m_eventNumber);

    m_eventTime = myEventID->time_stamp();
    m_lumiBlock = myEventID->lumi_block();
    m_bCID = myEventID->bunch_crossing_id();

    const EventType *myEventType = eventInfo->event_type();

    if (myEventType != 0)
        m_eventWeight = myEventType->mc_event_weight();

    else
        m_eventWeight = -999;

    return StatusCode::SUCCESS;
}
