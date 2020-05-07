/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFCalSamplingFraction.h"

// the first two come for free when using AthAlgorithm
//#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

// Event Info
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TTree.h"


// ID classes...
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/Identifier.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

// Hits and hit collections.
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "GeoAdaptors/GeoLArHit.h"
//#include "GeoAdaptors/GeoFCALHit.h" // not in 17.7.0
#include "GeoAdaptors/GeoCaloCalibHit.h"

// For Cryostat Positions
#include "LArG4RunControl/LArG4TBPosOptions.h"
// beam particle stuff
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/SimpleVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"

//17.3. changes
// no GeoAdapter/GeoFCalHit
// no LArSimevent/LArHit or LArHitContainer

//There is a LArHitContainer somewhere - LArHitFCAL

static const double mZ = 91.19 * CLHEP::GeV;
static const int MAX_PARTICLES = 20;

//using namespace Analysis;
//using namespace Rec;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

LArFCalSamplingFraction::LArFCalSamplingFraction(const std::string &name, ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_calibrationRun(false),
      cx1(0.0), cx2(0.0), cx3(0.0),
      cy1(0.0), cy2(0.0), cy3(0.0)
{
    declareProperty("Calibration", m_calibrationRun);
    /** switches to control the analysis through job options */

    //  declareProperty( "AnalysisTools",               m_analysisTools );
    //  declareProperty( "AnalysisSelectionTool",       m_analysisSelectionTool);
    //  declareProperty( "AnalysisPreparationTool",     m_analysisPreparationTool);
    //  declareProperty( "AnalysisOverlapCheckingTool", m_analysisOverlapCheckingTool);
    //  declareProperty( "AnalysisOverlapRemovalTool",  m_analysisOverlapRemovalTool);
    //  declareProperty( "TrigDecisionTool",            m_trigDec, "The tool to access TrigDecision");
    //
    //  declareProperty("ElectronContainer", m_electronContainerName="ElectronAODCollection");
    //  declareProperty("McParticleContainer", m_truthParticleContainerName = "SpclMC");
    //  declareProperty("MissingETObject",m_missingETObjectName="MET_RefFinal");
}


/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

LArFCalSamplingFraction::~LArFCalSamplingFraction() {}


////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate
/// get a handle on the analysis tools
/// book histograms

StatusCode LArFCalSamplingFraction::beginRun()
{
    //  ATH_MSG_ERROR("Initializing LArFCalSamplingFraction (before eventloop)");

    // retrieve trigger decision tool
    // needs to be done before the first run/event since a number of
    // BeginRun/BeginEvents are registered by dependent services
    StatusCode sc = StatusCode::SUCCESS;

    //  if ( m_doTrigger ) {
    //     sc = m_trigDec.retrieve();
    //     if ( sc.isFailure() )ATH_MSG_DEBUG("Can't get handle on TrigDecisionTool");
    //     else ATH_MSG_DEBUG("Got handle on TrigDecisionTool");
    //
    //  }
    //
    //  // Initialize the trigger passed counters
    //  // this can not be done before initialize, since the properties need to be set from job-options first
    //  std::vector<std::string>::const_iterator it;
    //  for(it = m_triggerChains.begin();it != m_triggerChains.end(); it++)
    //     m_triggersPassed[*it] = 0;

    return sc;
}

StatusCode LArFCalSamplingFraction::initialize()
{
    ATH_MSG_INFO("Initializing LArFCalSamplingFraction");

    /** get a handle on the NTuple and histogramming service */

    // sc = service("THistSvc", m_thistSvc);
    // if (sc.isFailure()) {
    //     ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
    //     return sc;
    // }

    ServiceHandle<ITHistSvc> histSvc("THistSvc", name());
    CHECK(histSvc.retrieve());
    ATH_MSG_DEBUG(" retrieved THistSvc");

    //StatusCode sc;
    // StoreGateSvc* detectorStore = 0;
    // sc = service("DetectorStore", detectorStore);
    //      sc = (detectorStore)->retrieve(posOptions,"LArG4TBPosOptions");
    const CaloIdManager *caloIdManager;
    ATH_CHECK(detStore()->retrieve(caloIdManager));

    // if ( !sc.isSuccess()  ||  caloIdManager == 0 ) {
    //     throw GaudiException("Could not get CaloIdManager",
    //                          "LArFCalTB_MC_CBNT_AA", StatusCode::FAILURE);
    // }

    // Use just for now for Calibration... change later to GeoCalibHit
    m_larFCalID = caloIdManager->getFCAL_ID();

    if (m_larFCalID == 0)
        throw GaudiException("Invalid LAr FCAL ID helper", "LArHitAnalysis", StatusCode::FAILURE);

    // Use for now... soon change to GeoCalibHit
    m_caloCellID = caloIdManager->getCaloCell_ID();

    if (m_caloCellID == 0)
        throw GaudiException("Invalid Calo Cell ID helper", "LArHitAnalysis", StatusCode::FAILURE);

    m_pdg_id = new std::vector<int>;  // particle id
    hit_x1 = new std::vector<double>; // hit positions of cells
    hit_y1 = new std::vector<double>;

    hit_x2 = new std::vector<double>;
    hit_y2 = new std::vector<double>;

    hit_x3 = new std::vector<double>;
    hit_y3 = new std::vector<double>;

    hit_ieta1 = new std::vector<double>; // hit indices of cells
    hit_iphi1 = new std::vector<double>;
    hit_ieta2 = new std::vector<double>;
    hit_iphi2 = new std::vector<double>;
    hit_ieta3 = new std::vector<double>;
    hit_iphi3 = new std::vector<double>;

    cell1_E = new std::vector<double>; // Energy in cells
    cell2_E = new std::vector<double>;
    cell3_E = new std::vector<double>;

    /** Now add branches and leaves to the AAN tree */
    // the TTree
    m_tree_AS = new TTree("tree_AS", "TTree of AnalysisSkleton");

    // First add Event info stuff
    m_tree_AS->Branch("Run", &m_runNumber, "Run/I");             // run number
    m_tree_AS->Branch("Event", &m_eventNumber, "Event/I");       // event number
    m_tree_AS->Branch("Time", &m_eventTime, "Time/I");           // time stamp
    m_tree_AS->Branch("LumiBlock", &m_lumiBlock, "LumiBlock/I"); // lumi block num
    m_tree_AS->Branch("BCID", &m_bCID, "BCID/I");                // bunch crossing ID
    m_tree_AS->Branch("Weight", &m_eventWeight, "Weight/D");     // weight

    // FCal stuff - sort this out - check whether branches need references
    m_tree_AS->Branch("EFCal", &m_totalFCalEnergy, "EFCal/D");
    m_tree_AS->Branch("NHitsFCal", &m_numHitsFCal, "NhitsFCal/I");

    m_tree_AS->Branch("Vertex_Eta", &vertex_eta, "Vertex_Eta/D");
    m_tree_AS->Branch("Vertex_Phi", &vertex_phi, "Vertex_Phi/D");

    m_tree_AS->Branch("Pt", &pt, "Pt/D");
    m_tree_AS->Branch("px", &px, "px/D");
    m_tree_AS->Branch("py", &py, "py/D");
    m_tree_AS->Branch("pz", &pz, "pz/D");
    m_tree_AS->Branch("E", &E, "E/D");

    m_tree_AS->Branch("VertexX", &vertx, "VertexX/D");
    m_tree_AS->Branch("VertexY", &verty, "VertexY/D");
    m_tree_AS->Branch("VertexZ", &vertz, "VertexZ/D");

    m_tree_AS->Branch("MC_CCX1", &x_mc_cc1, "MC_CCX1/D");
    m_tree_AS->Branch("MC_CCY1", &y_mc_cc1, "MC_CCY1/D");

    m_tree_AS->Branch("MC_CCX2", &x_mc_cc2, "MC_CCX2/D");
    m_tree_AS->Branch("MC_CCY2", &y_mc_cc2, "MC_CCY2/D");

    m_tree_AS->Branch("MC_CCX3", &x_mc_cc3, "MC_CCX3/D");
    m_tree_AS->Branch("MC_CCY3", &y_mc_cc3, "MC_CCY3/D");

    m_tree_AS->Branch("CCX1", &x_cc1, "CCX1/D");
    m_tree_AS->Branch("CCY1", &y_cc1, "CCY1/D");

    m_tree_AS->Branch("CCX2", &x_cc2, "CCX2/D");
    m_tree_AS->Branch("CCY2", &y_cc2, "CCY2/D");

    m_tree_AS->Branch("CCX3", &x_cc3, "CCX3/D");
    m_tree_AS->Branch("CCY3", &y_cc3, "CCY3/D");

    m_tree_AS->Branch("NCell_1", &NCell1, "NCell1_1/I");
    m_tree_AS->Branch("NCell_2", &NCell2, "NCell1_2/I");
    m_tree_AS->Branch("NCell_3", &NCell3, "NCell1_3/I");

    m_tree_AS->Branch("ParticleID", &m_pdg_id);

    m_tree_AS->Branch("Hit_X1", &hit_x1);
    m_tree_AS->Branch("Hit_Y1", &hit_y1);

    m_tree_AS->Branch("Hit_X2", &hit_x2);
    m_tree_AS->Branch("Hit_Y2", &hit_y2);

    m_tree_AS->Branch("Hit_X3", &hit_x3);
    m_tree_AS->Branch("Hit_Y3", &hit_y3);

    m_tree_AS->Branch("Hit_eta1", &hit_ieta1);
    m_tree_AS->Branch("Hit_phi1", &hit_iphi1);
    m_tree_AS->Branch("Hit_eta2", &hit_ieta2);
    m_tree_AS->Branch("Hit_phi2", &hit_iphi2);
    m_tree_AS->Branch("Hit_eta3", &hit_ieta3);
    m_tree_AS->Branch("Hit_phi3", &hit_iphi3);

    m_tree_AS->Branch("Cell1_E", &cell1_E);
    m_tree_AS->Branch("Cell2_E", &cell2_E);
    m_tree_AS->Branch("Cell3_E", &cell3_E);

    m_tree_AS->Branch("FCal1_E", &FCal1_SumE, "FCal1_E/D");
    m_tree_AS->Branch("FCal2_E", &FCal2_SumE, "FCal2_E/D");
    m_tree_AS->Branch("FCal3_E", &FCal3_SumE, "FCal3_E/D");

    if (m_calibrationRun) {
        m_caloDmID = caloIdManager->getDM_ID();

        if (m_caloDmID == 0)
            throw GaudiException("Invalid Calo DM ID helper", "LArFCalTB_MC_CBNT_AA", StatusCode::FAILURE);

        // Define the hit containers that we'll analyze in this program.
        // For now, initialize the pointers to the containers to NULL
        // (zero).
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


///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode LArFCalSamplingFraction::finalize()
{
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////
/// Clear - clear CBNT members

StatusCode LArFCalSamplingFraction::initEvent()
{
    /// For Athena-Aware NTuple

    //std::cout<<"MOOCOWY - starting event, clearing variables\n";

    vertx = 0; // x-position for vertex generated particle (beam)
    verty = 0;
    vertz = 0;

    vertex_eta = 0; // eta value of generated particle
    vertex_phi = 0;
    pt = 0;

    px = 0; // Momentum of generated particle
    py = 0;
    pz = 0;

    E = 0; // Energy of generated particle

    NCell1 = 0; // Number of cells hit
    NCell2 = 0;
    NCell3 = 0;

    x_mc_cc1 = 0; // Center of cluster in x (FCal1, extrapolated)
    y_mc_cc1 = 0;

    x_mc_cc2 = 0; // Center of cluster in x (FCal2, extrapolated)
    y_mc_cc2 = 0;

    x_mc_cc3 = 0; // Center of cluster in x (FCal3, extrapolated)
    y_mc_cc3 = 0;

    x_cc1 = 0; // Center of cluster in x (FCal1, c of g)
    y_cc1 = 0;

    x_cc2 = 0; // Center of cluster in x (FCal2, c of g)
    y_cc2 = 0;

    x_cc3 = 0; // Center of cluster in x (FCal3, c of g)
    y_cc3 = 0;

    m_pdg_id->clear(); // particle id
    hit_x1->clear();   // hit positions of cells
    hit_y1->clear();

    hit_x2->clear();
    hit_y2->clear();

    hit_x3->clear();
    hit_y3->clear();

    hit_ieta1->clear(); // hit indices of cells
    hit_iphi1->clear();
    hit_ieta2->clear();
    hit_iphi2->clear();
    hit_ieta3->clear();
    hit_iphi3->clear();

    cell1_E->clear(); // Energy in cells
    cell2_E->clear();
    cell3_E->clear();

    FCal1_SumE = 0; // Energy in individual FCal modules
    FCal2_SumE = 0;
    FCal3_SumE = 0;
    TCScint_E = 0;
    TCIron_E = 0;

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


/////////////////////////////////////////////////////////////////////////////////////////////////////
void LArFCalSamplingFraction::FCalHitCenter(const LArHitContainer *container)
{
    double max1 = 0.0;
    double max2 = 0.0;
    double max3 = 0.0;

    // this is screwing up.
    // there are entries in the hit collection
    // Either the iterator (hi) is bad (so that it can't be made to point to a valid LArHit)
    // or the conversion LArHit-> GeoFCALhit is bad.

    LArHitContainer::const_iterator hi = (*container).begin(), he = (*container).end();

    //  std::cout << "Beginning of first loop" << std::endl;
    while (hi != he) {
        // int size = container->size();
        // for (int i=0; i<m_numHitsFCal; i++){

        //    GeoFCALHit fcalhit(*((*container)[i]));

        // check the iterator. try a LArHit method on *hi
        // std::cout<<"MOOMOOCOWY  hit energy, cellid\n";
        // std::cout<<"energy = "<<(*hi)->energy()<<", ID = "<<(*hi)->cellID()<<"\n";

        //just use lar hit
        //getDetDescrElement()->is_lar_fcal()
        //getLayer()

        GeoLArHit fcalhit(**hi);

        // if (!(*hi))
        if ((!fcalhit) | (!fcalhit.getDetDescrElement()->is_lar_fcal())) {
            std::cout << "LArhit is not define" << std::endl;
            hi++;
            continue;
        }

        double energy = fcalhit.Energy();

        int module_index = fcalhit.getDetDescrElement()->getLayer();

        //    const FCALModule *module = fcalhit.getModule();
        //    FCALModule::Module module_index = module->getModuleIndex();
        //    const FCALTile *tile = fcalhit.getTile();

        // want x,y,module

        double x = fcalhit.getDetDescrElement()->x();
        double y = fcalhit.getDetDescrElement()->y();

        // Determine the center of the cluster for FCal1
        if (module_index == 1) {
            if (max1 < energy) {
                max1 = energy;
                cx1 = x;
                cy1 = y;
            }
        }

        // Determine the center of the cluster for FCal2
        if (module_index == 2) {
            // cell2_E = energy;
            if (max2 < energy) {
                max2 = energy;
                cx2 = x;
                cy2 = y;
            }
        }

        // Determine the center of the cluster for FCal3
        //std::cout << "if Module 3 " << std::endl;
        if (module_index == 3) {
            // cell3_E = energy;
            if (max3 < energy) {
                max3 = energy;
                cx3 = x;
                cy3 = y;
            }
        }

        hi++;

    } //End of hit loop;
}


void LArFCalSamplingFraction::FCalClusterCenter(const LArHitContainer *container)
{
    float xNumer1 = 0.0, xNumer2 = 0.0, xNumer3 = 0.0;
    float yNumer1 = 0.0, yNumer2 = 0.0, yNumer3 = 0.0;
    float Denom1 = 0.0, Denom2 = 0.0, Denom3 = 0.0;

    double subClusterSize = 30.0; // [mm]
    double thisCG_R = 0.0;
    //  LArHitContainer::const_iterator
    //    hi=(*container).begin(),he=(*container).end();
    //  while (hi!=he){
    //    GeoFCALHit fcalhit(**hi);

    //     int size = container->size();
    for (int i = 0; i < 1; i++) {

        GeoLArHit fcalhit(*((*container)[i]));

        // if (!fcalhit)
        // if (!(*hi))
        if ((!fcalhit) | (!fcalhit.getDetDescrElement()->is_lar_fcal())) {
            std::cout << "MOOCOWYCOWY: hit in LarHitContainer is not a GepFCalHit\n";
            continue;
        }

        double energy = fcalhit.Energy();
        int module_index = fcalhit.getDetDescrElement()->getLayer();

        //    const FCALModule *module = fcalhit.getModule();
        //    FCALModule::Module module_index = module->getModuleIndex();
        //    const FCALTile *tile = fcalhit.getTile();

        // want x,y,module

        double x = fcalhit.getDetDescrElement()->x();
        double y = fcalhit.getDetDescrElement()->y();

        /*
          std::cout << "module_index is " << module_index << std::endl;
          std::cout << "x is " << x << std::endl;
          std::cout << "y is " << y << std::endl;

          std::cout << "cx1 is " << cx1 << std::endl;
          std::cout << "cy1 is " << cy1 << std::endl;
        */
        // Determine Center of cluster of FCal1
        if (module_index == 1) {
            double x_subcheck1 = cx1 - x;
            double y_subcheck1 = cy1 - y;
            thisCG_R = sqrt(x_subcheck1 * x_subcheck1 + y_subcheck1 * y_subcheck1);

            if (thisCG_R <= subClusterSize) {
                // Calculate the Cluster Center;
                xNumer1 += x * energy;
                yNumer1 += y * energy;
                Denom1 += energy;
            }
        }

        // Determine Center of cluster of FCal2
        if (module_index == 2) {
            double x_subcheck2 = cx2 - x;
            double y_subcheck2 = cy2 - y;
            thisCG_R = sqrt(x_subcheck2 * x_subcheck2 + y_subcheck2 * y_subcheck2);

            if (thisCG_R <= subClusterSize) {
                // Calculate the Cluster Center;
                xNumer2 += x * energy;
                yNumer2 += y * energy;
                Denom2 += energy;
            }
        }

        // Determine Center of cluster of FCal3
        if (module_index == 3) {
            double x_subcheck3 = cx3 - x;
            double y_subcheck3 = cy3 - y;
            thisCG_R = sqrt(x_subcheck3 * x_subcheck3 + y_subcheck3 * y_subcheck3);

            if (thisCG_R <= subClusterSize) {
                // Calculate the Cluster Center;
                xNumer3 += x * energy;
                yNumer3 += y * energy;
                Denom3 += energy;
            }
        }

        //    hi++;
    } // End of Loop

    if (fabs(Denom1) > 0.0) {
        x_cc1 = xNumer1 / Denom1;
        y_cc1 = yNumer1 / Denom1;
    }

    if (fabs(Denom2) > 0.0) {
        x_cc2 = xNumer2 / Denom2;
        y_cc2 = yNumer2 / Denom2;
    }

    if (fabs(Denom3) > 0.0) {
        x_cc3 = xNumer3 / Denom3;
        y_cc3 = yNumer3 / Denom3;
    }
}


StatusCode LArFCalSamplingFraction::doCalib()
{
    StatusCode sc;
    MsgStream log(messageService(), name());

    log << MSG::VERBOSE
        << "TB2003 MC FCal Calibration Analysis started" << endreq;

    // Get the calibration hit containers (if any)
    for (m_calibHitMap_ptr_t i = m_calibHitMap.begin();
         i != m_calibHitMap.end();
         i++)
    {
        std::string name = (*i).first;
        const CaloCalibrationHitContainer *container = 0;
        sc = evtStore()->retrieve(container, name);

        if (sc.isFailure() || container == 0) {
            log << MSG::ERROR
                << "CaloCalibrationHit container was not found"
                << endreq;
            m_numHitsActive = 0;
            m_numHitsInactive = 0;
            m_numHitsDead = 0;
            (*i).second = 0;
            return StatusCode::FAILURE;
        }

        log << MSG::DEBUG
            << "CaloCalibrationHit container successfully retrieved."
            << endreq;

        (*i).second = container;
    }

    // Get the number of hits in each container. (The braces let
    // me re-use the name 'container'.)
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

    // Loop over all the hit containers, inspecting each hit within the
    // collection.
    for (m_calibHitMap_ptr_t i = m_calibHitMap.begin();
         i != m_calibHitMap.end();
         i++)
    {
        std::string name = (*i).first;
        const CaloCalibrationHitContainer *container = (*i).second;

        // Skip rest of loop if it's a null container.
        if (container == 0)
            continue;

        // For each hit in the container...
        CaloCalibrationHitContainer::const_iterator j;

        for (j = container->begin();
             j != container->end();
             j++)
        {
            // Reminder:
            // j = pointer to an entry in vector<CaloCalibrationHit*>
            // *j = an entry in vector<CaloCalibrationHit*>, that is, a CaloCalibrationHit*.
            const CaloCalibrationHit *hit = *j;
            Identifier identifier = hit->cellID();
            double energy = hit->energyTotal();

            // Accumulate energy sums.  Use the ID helpers to
            // determine in which detector the hit took place.

            m_totalCalibrationEnergy += energy;
            m_totalEmEnergy += hit->energyEM();
            m_totalNonEmEnergy += hit->energyNonEM();
            m_totalInvisibleEnergy += hit->energyInvisible();
            m_totalEscapedEnergy += hit->energyEscaped();

            if (name == "LArCalibrationHitActive")
                m_totalActiveEnergy += energy;

            if (name == "LArCalibrationHitInactive")
                m_totalInactiveEnergy += energy;

            if (name == "LArCalibrationHitDeadMaterial") {
                m_totalDeadMaterialEnergy += energy;

                if (m_caloDmID->is_lar(identifier) && (m_caloDmID->sampling(identifier) == 3) && (m_caloDmID->dmat(identifier) == 1))
                    m_FCalDead += energy;
            }

            // std::cout << "MOOCOWY:checking if this is an FCal hit\n"<<"Cell id: "<<identifier<<"\n";

            if (m_caloCellID->is_lar_fcal(identifier)) {
                //std::cout<<"MOOCOWY - is an fcalhit\n";
                FCalCalibAnalysis(name, hit);
            }
        } // for each calibration hit
    }     // for each calibration hit container

    // Execution completed.

    log << MSG::VERBOSE << "execute() completed successfully" << endreq;
    //  return StatusCode::SUCCESS;
    return sc;
}


//// Added by JPA, June 2005 for FCal Analysis with Calibration Hits on
void LArFCalSamplingFraction::FCalCalibAnalysis(const std::string name, const CaloCalibrationHit *CalibHit)
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
        m_totalFCal1CalibrationEnergy += energy;
        m_FCal1Em += CalibHit->energyEM();
        m_FCal1NonEm += CalibHit->energyNonEM();
        m_FCal1Invisible += CalibHit->energyInvisible();
        m_FCal1Escaped += CalibHit->energyEscaped();
    }

    if (m_larFCalID->module(id) == 2) {
        m_totalFCal2CalibrationEnergy += energy;
        m_FCal2Em += CalibHit->energyEM();
        m_FCal2NonEm += CalibHit->energyNonEM();
        m_FCal2Invisible += CalibHit->energyInvisible();
        m_FCal2Escaped += CalibHit->energyEscaped();
    }

    if (m_larFCalID->module(id) == 3) {
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


void LArFCalSamplingFraction::TruthImpactPosition(McEventCollection::const_iterator e)
{
    for (HepMC::GenEvent::particle_const_iterator p = (**e).particles_begin();
         p != (**e).particles_end(); p++)
    {
        // Call it "theParticle"
        const HepMC::GenParticle *theParticle = *p;

        // old GenParticles used HepLorentzVectors, now they use HepMC::FourVectors

        // Get the kinematic variables:
        //      HepLorentzVector momentum  = theParticle->momentum();
        // PT - HepMC->Hep Lorentz
        HepMC::FourVector HMCmom = theParticle->momentum();
        CLHEP::HepLorentzVector momentum(CLHEP::Hep3Vector(HMCmom.px(), HMCmom.py(), HMCmom.pz()), HMCmom.e());

        //      HepPoint3D       origin = theParticle->production_vertex()->point3d();
        HepMC::ThreeVector HMC3vec = theParticle->production_vertex()->point3d();
        HepGeom::Point3D<double> origin = HepGeom::Point3D<double>(HMC3vec.x(), HMC3vec.y(), HMC3vec.z());

        //      HepMC::GenVertex *decayVertex = theParticle->end_vertex();
        /*
        double           charge =
        theParticle->pdg_id() > 0 ?
        particleData->charge() : - particleData->charge();
             */
        // Put VEta and VPhi into the Ntuple
        vertex_phi = momentum.vect().phi();
        vertex_eta = -log(tan(momentum.vect().theta() / 2));

        if (!finite(vertex_eta))
            vertex_eta = 0;

        pt = momentum.vect().perp();

        px = momentum.px();
        py = momentum.py();
        pz = momentum.pz();

        E = momentum.e();

        vertx = theParticle->production_vertex()->point3d().x();
        verty = theParticle->production_vertex()->point3d().y();
        vertz = theParticle->production_vertex()->point3d().z();

        // Must get x-offset depending on TB position
        // The 90.0mm is from initial x-offset of fcal in cryostat
        // The -15.0mm is from initial y-offset of fcal in cryostat
        // 2nd number changes between different positions (these numbers
        // will be in data base soon

        std::string nickname;
        double xoffset = 0.0;
        //      LArG4TBPosOptions *posOptions = NULL;
        //
        //      StatusCode sc;
        //      StoreGateSvc* detectorStore = 0;
        //      sc = service("DetectorStore", detectorStore);
        //      sc = (detectorStore)->retrieve(posOptions,"LArG4TBPosOptions");
        //
        //      if (sc.isSuccess()) {
        //          nickname = posOptions->PositionNickname();
        //      }
        //      double sinangle = 0.0;
        //      if( nickname == "POSITION1")   {xoffset = 90.0 - 18.0; sinangle = 0.0154;} // [mm]
        //      if( nickname == "POSITION2")   {xoffset = 90.0 + 14.5; sinangle = 0.0218;}
        //      if( nickname == "POSITION3")   {xoffset = 90.0 + 47.0; sinangle = 0.0281;}
        //      if((nickname == "POSITION4L") ||
        //         (nickname == "POSITION4H")) {xoffset = 90.0 + 155.0; sinangle = 0.0520;}
        //
        //      // Extrapolate to FCal 1 face;
        //      const double yoffset = -15.0*mm;
        //      x_mc_cc1 = 0;
        //      y_mc_cc1 = 0;
        //      double x=0.0,y = 0.0;
        // change these for atlas
        double sinangle = 0;
        double yoffset = 0;
        const double z1 = -32668.5 * CLHEP::mm; // This is 4668.5 (z=0 to FCal1 Face) + 28000 (B9 to z=0)
        const double z2 = -33128.3 * CLHEP::mm; // This is 5128.3 (z=0 to FCal1 Face) + 28000 (B9 to z=0)
        const double z3 = -33602.8 * CLHEP::mm; // This is 5602.8 (z=0 to FCal1 Face) + 28000 (B9 to z=0)
        //PT
        double shift2 = sinangle * (5128.3 - 4668.5) * CLHEP::mm;
        double shift3 = sinangle * (5602.8 - 4668.5) * CLHEP::mm;
        // accounts for rotation of Fcal + cryostat.
        x_mc_cc1 = origin.x() + px * z1 / pz + xoffset;
        y_mc_cc1 = origin.y() + py * z1 / pz + yoffset;

        x_mc_cc2 = origin.x() + px * z2 / pz + shift2 + xoffset;
        y_mc_cc2 = origin.y() + py * z2 / pz + yoffset;

        x_mc_cc3 = origin.x() + px * z3 / pz + shift3 + xoffset;
        y_mc_cc3 = origin.y() + py * z3 / pz + yoffset;

    } // particles
}


////////////////////////////////////////////////////////////////////////////////////////////
StatusCode LArFCalSamplingFraction::doFCal()
{
    MsgStream log(messageService(), name());
    StatusCode sc;

    log << MSG::DEBUG
        << "LArFCalSamplingFraction parameters are: \n"
        << "Calibration: " << m_calibrationRun << "\n"
        << endreq;

    // ACCESSING EVENT INFORMATION
    // Get the basic event information (run number, event number).

    const EventInfo *eventInfo = 0;
    sc = evtStore()->retrieve(eventInfo);

    if (sc.isFailure()) {
        log << MSG::ERROR
            << "Could not fetch event information" << endreq;
        return sc;
    }

    const EventID *eventID = eventInfo->event_ID();
    m_runNumber = eventID->run_number();
    m_eventNumber = eventID->event_number();
    std::cout << "MOOCOWY: run " << m_runNumber << ", event " << m_eventNumber << "\n";

    // How to access MC Truth information:
    const McEventCollection *mcEventCollection;
    sc = evtStore()->retrieve(mcEventCollection, "TruthEvent");

    if (sc.isSuccess()) {
        // There can potentially be more than one MC event in the
        // collection.
        McEventCollection::const_iterator mcEvent;
        int numMcEvent = 0;

        for (mcEvent = mcEventCollection->begin();
             mcEvent != mcEventCollection->end();
             mcEvent++, numMcEvent++)
            TruthImpactPosition(mcEvent);
    } // retrieved MC event collection
    else {
        log << MSG::DEBUG
            << "Run " << m_runNumber
            << " Event " << m_eventNumber
            << " - Could not fetch MC event collection" << endreq;
    }

    // Accessing Hits
    // Regular hits
    std::string FCalContainerName = "LArHitFCAL";
    const LArHitContainer *container = 0;
    sc = evtStore()->retrieve(container, FCalContainerName);

    if (sc.isFailure() || container == 0) {
        log << MSG::ERROR
            << "LArHitFCAL container was not found"
            << endreq;
        m_numHitsFCal = 0;
        return StatusCode::FAILURE;
    }

    log << MSG::DEBUG
        << "LArHitFCAL container successfully retrieved."
        << endreq;

    // Get the number of hits in the LArHitFCAL container
    m_numHitsFCal = container->size();
    //  log << MSG::DEBUG
    std::cout << "NumHitsFCal = " << m_numHitsFCal << "\n";
    //      << endreq;

    if (m_numHitsFCal > 0) {
        // Calculate the center of gravity.
        FCalHitCenter(container);
        FCalClusterCenter(container);
    }

    // For each hit in the container...

    LArHitContainer::const_iterator j;

    // for ( j = container->begin();
    //       j != container->end();
    //       j++ )
    for (int i = 0; i < m_numHitsFCal; i++) {
        GeoLArHit fcalhit(*((*container)[i]));

        // Added by JPA to get particle id for each hit
        const McEventCollection *mcEventCollection;
        sc = evtStore()->retrieve(mcEventCollection, "TruthEvent");

        if (sc.isSuccess()) {
            // There can potentially be more than one MC event in the
            // collection.
            McEventCollection::const_iterator mcEvent;
            int numParticles = 0;

            for (mcEvent = mcEventCollection->begin();
                 mcEvent != mcEventCollection->end();
                 mcEvent++)
            {
                // There may be many particles per event
                for (HepMC::GenEvent::particle_const_iterator p = (**mcEvent).particles_begin();
                     p != (**mcEvent).particles_end(); p++)
                {
                    // Call it "theParticle"
                    const HepMC::GenParticle *theParticle = *p;
                    m_pdg_id->push_back(theParticle->pdg_id());
                    numParticles++;
                    // log << MSG::INFO << "The number of particles is " << numParticles << endreq;
                }
            }

            // log << MSG::INFO
            //     << "The number of particle is "
            //     << numParticles
            //     << endreq;
        } // retrieved MC event collection

        //End JPA particle id

        // j = pointer to an entry in vector<LArHit*>
        // *j = an entry in vector<LArHit*>, that is, a LArHit*.
        //      GeoFCALHit fcalhit(**j);
        if ((!fcalhit) | (!fcalhit.getDetDescrElement()->is_lar_fcal())) {
            log << MSG::ERROR
                << "GeoFCalHit is not defined"
                << endreq;
        }

        double energy = fcalhit.Energy();
        int module_index = fcalhit.getDetDescrElement()->getLayer();
        m_totalFCalEnergy += energy;

        if (module_index == 1) {
            FCal1_SumE += energy;
            FillCellInfo(fcalhit, cell1_E, hit_x1, hit_y1,
                         hit_ieta1, hit_iphi1, NCell1);
        }

        if (module_index == 2) {
            FCal2_SumE += energy;
            FillCellInfo(fcalhit, cell2_E, hit_x2, hit_y2,
                         hit_ieta2, hit_iphi2, NCell2);
        }

        if (module_index == 3) {
            FCal3_SumE += energy;
            FillCellInfo(fcalhit, cell3_E, hit_x3, hit_y3,
                         hit_ieta3, hit_iphi3, NCell3);
        }
    } // for each hit

    // Calibration hit analysis.
    if (m_calibrationRun)
        CHECK(doCalib());

    //  TCECollection* TCColl;
    //  sc = evtStore()->retrieve(TCColl, "TailCatcherEnergy");
    //  TCScint_E = (*TCColl)[0]->getScint();
    //  TCIron_E = (*TCColl)[0]->getAbsorber();
    log << MSG::VERBOSE << "doFCal() completed successfully" << endreq;

    //std::cout<<"MOCOWYCOWY: Doing the FCal stuff\n";

    return StatusCode::SUCCESS;
}


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
/// Execute - on event by event

StatusCode LArFCalSamplingFraction::execute()
{
    //
    m_eventNumber++;
    ATH_MSG_DEBUG(" in execute()");

    StatusCode sc;

    // initialize first before processing each event
    sc = initEvent();

    if (sc.isFailure())
        ATH_MSG_WARNING("initEvent failed. Continue");

    // initialize variables in ntuple

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
    ATH_MSG_DEBUG("in addEventInfo");

    // this code has been taken from AnalysisExamples/VFitZmmOnAOD
    // I have the actual EventNumber, but skipped the sequential count of event #
    //

    //get EventInfo for run and event number

    const EventInfo *eventInfo;
    StatusCode sc = evtStore()->retrieve(eventInfo);

    if (sc.isFailure()) {
        ATH_MSG_WARNING("Could not retrieve event info");
        return sc;
    }

    const EventID *myEventID = eventInfo->event_ID();
    //
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
