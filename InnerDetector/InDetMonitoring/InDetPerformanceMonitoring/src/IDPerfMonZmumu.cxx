
//==================================================================================

//==================================================================================
// Include files...
//==================================================================================

// This files header
#include "InDetPerformanceMonitoring/IDPerfMonZmumu.h"
// Standard headers
#include <string>
#include "TTree.h"
#include "TLorentzVector.h"

#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include "InDetPerformanceMonitoring/ZmumuEvent.h"

//#include "TrkFitterInterfaces/ITrackFitter.h"
#include "egammaInterfaces/IegammaTrkRefitterTool.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
//#include "xAODTruth/xAODTruthHelpers.h"
//For extrapolation

// ATLAS headers
#include "GaudiKernel/IInterface.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/TrackTruthKey.h"

/** needed for IP resolution studies **/
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

//==================================================================================
// Public Methods
//==================================================================================
IDPerfMonZmumu::IDPerfMonZmumu(const std::string& name,
  ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_isMC(false),
  m_doRefit(false),
  m_TrackRefitter1(""),
  m_TrackRefitter2(""),
  m_trackToVertexTool("Reco::TrackToVertex"),
  m_triggerDecision("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_selTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool"),
  m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator"),
  m_beamSpotSvc("BeamCondSvc",name),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_validationMode(true),
  m_defaultTreeName("Default_InDetTrackParticle"),
  m_IDTreeName("ID_InDetTrackParticle"),
  m_refit1TreeName("Refit1_SiAndTRT"),
  m_refit2TreeName("Refit2_SiOnly"),
  m_truthTreeName("TruthParams"),
  //  m_meStacoTreeName("MEStacoParams"),  //not existent in xAOD anymore
  m_combTreeName("CombinedTrackParticle"),
  m_combMuidTreeName("CombMuidParams"),
  m_ValidationTreeDescription("Small Tree for Zmumu fits"),
  m_defaultTreeFolder("/ZmumuValidation/default"),
  m_IDTreeFolder("/ZmumuValidation/ID"),
  m_refit1TreeFolder("/ZmumuValidation/refit1"),
  m_refit2TreeFolder("/ZmumuValidation/refit2"),
  m_truthTreeFolder("/ZmumuValidation/truth"),
  //  m_meStacoTreeFolder("/ZmumuValidation/mestaco"),
  m_combTreeFolder("/ZmumuValidation/comb"),
  m_defaultTree(0),
  m_IDTree(0),
  m_refit1Tree(0),
  m_refit2Tree(0),
  m_truthTree(0),
  //  m_meStacoTree(0),   // not existent in xAOD anymore
  m_combTree(0),
  m_combMuidTree(0),
  m_doRemoval(true),
  m_doDebug(false)

{
  // Properties that are set from the python scripts.

  declareProperty("UseTrigger"    ,    m_UseTrigger     = false);
  declareProperty("doIsoSelection",    m_doIsoSelection = true );
  declareProperty("doIPSelection",     m_doIPSelection = true );
  declareProperty("isMC",              m_isMC = false);
  declareProperty("doRefit",           m_doRefit = false);
  declareProperty("doIPextrToPV"       , m_doIP = false);
  declareProperty("BeamCondSvc", m_beamSpotSvc);
  declareProperty ( "Extrapolator", m_extrapolator );
  declareProperty("MassWindowLow",     m_MassWindowLow = 60.0, "Lower cut in mu+mu- invariant mass" );
  declareProperty("MassWindowHigh",    m_MassWindowHigh = 120.0, "Upper cut in mu+mu- invariant mass" );
  declareProperty("OpeningAngle",      m_OpeningAngleCut = 0.2, "Opening angle between the two muons (in radians)");
  declareProperty("OutputTracksName",  m_outputTracksName = "ZmumuTracks");
  declareProperty("PtLeadingMuon",     m_LeadingMuonPtCut = 20., "Pt cut on the leading muon");
  declareProperty("PtSecondMuon",      m_SecondMuonPtCut = 15., "Pt cut on the second muon");
  declareProperty("ReFitterTool1",     m_TrackRefitter1, "ToolHandle for track fitter implementation");
  declareProperty("ReFitterTool2",     m_TrackRefitter2, "ToolHandle for track fitter implementation");
  declareProperty("TrackToVertexTool", m_trackToVertexTool);
  declareProperty("TrackTruthName",    m_truthName="TrackTruthCollection");
  declareProperty("TrackParticleName", m_trackParticleName="InDetTrackParticles");
  declareProperty("triggerChainName",  m_sTriggerChainName               );
  declareProperty("ValidationMode",    m_validationMode);
  declareProperty("xAODTruthLinkVector",m_truthLinkVecName="xAODTruthLinks");
  declareProperty("Z0Gap",             m_Z0GapCut = 5.0, "maximum gap between the z0 of both muons (in mm)");
  declareProperty( "TrackSelectionTool", m_selTool );
  declareProperty("UseTrackSelectionTool",   m_useTrackSelectionTool = false);
  declareProperty("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);


  declareProperty("defaultTreeFolder", m_defaultTreeFolder );
  declareProperty("IDTreeFolder", m_IDTreeFolder );
  
  declareProperty("refit1TreeFolder", m_refit1TreeFolder );
  declareProperty("refit2TreeFolder", m_refit2TreeFolder );
  declareProperty("truthTreeFolder", m_truthTreeFolder );
  declareProperty("combTreeFolder", m_combTreeFolder );
  declareProperty("UnbiasVertex", m_doRemoval);
  
  declareProperty("DefaultTree", m_defaultTreeName );
  declareProperty("IDTree", m_IDTreeName );
  declareProperty("RefitTree",   m_combTreeName );
  declareProperty("Refit1Tree",  m_refit1TreeName );
  declareProperty("Refit2Tree",  m_refit2TreeName );
  
  return;
}



IDPerfMonZmumu::~IDPerfMonZmumu()
{}



//==================================================================================
StatusCode IDPerfMonZmumu::initialize()
{


  // Setup the services
  ISvcLocator* pxServiceLocator = serviceLocator();
  if ( pxServiceLocator != NULL ) {
      StatusCode xSC = PerfMonServices::InitialiseServices( pxServiceLocator );
      if ( xSC == !StatusCode::SUCCESS )
	{
          ATH_MSG_FATAL("Problem Initializing PerfMonServices");
	  //return PARENT::initialize();
	}
  }

  // Retrieve fitter
  if (m_TrackRefitter1.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter1 );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_TrackRefitter1 );
  }

  // Retrieve the second fitter
  if (m_TrackRefitter2.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter2 );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_TrackRefitter2 );
  }


  if (m_trackToVertexTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_trackToVertexTool );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_trackToVertexTool );
  }

  if(m_useTrackSelectionTool){
    ATH_CHECK( m_selTool.retrieve() );
  }

  if (m_doIP) {
    ATH_MSG_DEBUG("Retrieving tool (trackToVertexIPEstimator)");
    ATH_CHECK (m_trackToVertexIPEstimator.retrieve());
  }

  ATH_CHECK( m_beamSpotSvc.retrieve());
  ATH_CHECK( m_extrapolator.retrieve());
  
  m_xZmm.setDebugMode(false);

  m_LooseT2VAssociationTool = std::make_unique<CP::LooseTrackVertexAssociationTool>("Loose");
  

  if( m_defaultTree == 0){

    //m_defaultTree = new TTree((m_trackParticleName+m_defaultTreeName).c_str(), m_ValidationTreeDescription.c_str());
    m_defaultTree = new TTree((m_defaultTreeName).c_str(), m_ValidationTreeDescription.c_str());

    m_defaultTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_defaultTree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_defaultTree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

    m_defaultTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_defaultTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_defaultTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_defaultTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_defaultTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_defaultTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_defaultTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");
    m_defaultTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_defaultTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_defaultTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_defaultTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_defaultTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_defaultTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_defaultTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
    
    if(m_doIP){

      m_defaultTree->Branch("Negative_d0_PV",      &m_negative_d0_PV   ,  "Negative_d0_PV/D");
      m_defaultTree->Branch("Negative_z0_PV",      &m_negative_z0_PV   ,  "Negative_z0_PV/D");
      m_defaultTree->Branch("Positive_z0_PV",      &m_positive_z0_PV,  "Positive_z0_PV/D");
      m_defaultTree->Branch("Positive_d0_PV",      &m_positive_d0_PV,  "Positive_d0_PV/D");

      m_defaultTree->Branch("Negative_d0_PVerr",   &m_negative_d0_PVerr,  "Negative_d0_PVerr/D");
      m_defaultTree->Branch("Negative_z0_PVerr",   &m_negative_z0_PVerr,  "Negative_z0_PVerr/D");
      m_defaultTree->Branch("Positive_z0_PVerr",   &m_positive_z0_PVerr,  "Positive_z0_PVerr/D");
      m_defaultTree->Branch("Positive_d0_PVerr",   &m_positive_d0_PVerr,  "Positive_d0_PVerr/D");
    }
  }
  
  
  if( m_IDTree == 0){
      
    m_IDTree = new TTree((m_IDTreeName).c_str(), m_ValidationTreeDescription.c_str());
    
    m_IDTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_IDTree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_IDTree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

    m_IDTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_IDTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_IDTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_IDTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_IDTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_IDTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_IDTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");
    m_IDTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_IDTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_IDTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_IDTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_IDTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_IDTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_IDTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
    
    if(m_doIP){
      
      m_IDTree->Branch("Negative_d0_PV",      &m_negative_d0_PV   ,  "Negative_d0_PV/D");
      m_IDTree->Branch("Negative_z0_PV",      &m_negative_z0_PV   ,  "Negative_z0_PV/D");
      m_IDTree->Branch("Positive_z0_PV",      &m_positive_z0_PV,  "Positive_z0_PV/D");
      m_IDTree->Branch("Positive_d0_PV",      &m_positive_d0_PV,  "Positive_d0_PV/D");
      
      m_IDTree->Branch("Negative_d0_PVerr",   &m_negative_d0_PVerr,  "Negative_d0_PVerr/D");
      m_IDTree->Branch("Negative_z0_PVerr",   &m_negative_z0_PVerr,  "Negative_z0_PVerr/D");
      m_IDTree->Branch("Positive_z0_PVerr",   &m_positive_z0_PVerr,  "Positive_z0_PVerr/D");
      m_IDTree->Branch("Positive_d0_PVerr",   &m_positive_d0_PVerr,  "Positive_d0_PVerr/D");
    }
    

    
  }
  
  if( m_doRefit && m_refit1Tree == 0){
    
    // m_refit1Tree = new TTree((m_trackParticleName+m_refit1TreeName).c_str(), m_ValidationTreeDescription.c_str());
    m_refit1Tree = new TTree((m_refit1TreeName).c_str(), m_ValidationTreeDescription.c_str()); // Salva --> attempt to remove the lon InDetTrackParticles from the tree name 
    
    m_refit1Tree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_refit1Tree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_refit1Tree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");
    m_refit1Tree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

    m_refit1Tree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_refit1Tree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_refit1Tree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_refit1Tree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_refit1Tree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_refit1Tree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_refit1Tree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");


    m_refit1Tree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_refit1Tree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_refit1Tree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_refit1Tree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_refit1Tree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_refit1Tree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_refit1Tree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");

    if(m_doIP){

      m_refit1Tree->Branch("Negative_d0_PV",      &m_negative_d0_PV   ,  "Negative_d0_PV/D");
      m_refit1Tree->Branch("Negative_z0_PV",      &m_negative_z0_PV   ,  "Negative_z0_PV/D");
      m_refit1Tree->Branch("Positive_z0_PV",      &m_positive_z0_PV,     "Positive_z0_PV/D");
      m_refit1Tree->Branch("Positive_d0_PV",      &m_positive_d0_PV,     "Positive_d0_PV/D");
      
      m_refit1Tree->Branch("Negative_d0_PVerr",      &m_negative_d0_PVerr   ,  "Negative_d0_PVerr/D");
      m_refit1Tree->Branch("Negative_z0_PVerr",      &m_negative_z0_PVerr   ,  "Negative_z0_PVerr/D");
      m_refit1Tree->Branch("Positive_z0_PVerr",     &m_positive_z0_PVerr,     "Positive_z0_PVerr/D");
      m_refit1Tree->Branch("Positive_d0_PVerr",     &m_positive_d0_PVerr,     "Positive_d0_PVerr/D");
    }
    

  }

  if( m_doRefit && m_refit2Tree == 0){

    //m_refit2Tree = new TTree((m_trackParticleName+m_refit2TreeName).c_str(), m_ValidationTreeDescription.c_str());
    m_refit2Tree = new TTree((m_refit2TreeName).c_str(), m_ValidationTreeDescription.c_str());

    m_refit2Tree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_refit2Tree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_refit2Tree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");
    m_refit2Tree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

    m_refit2Tree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_refit2Tree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_refit2Tree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_refit2Tree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_refit2Tree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_refit2Tree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_refit2Tree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");



    m_refit2Tree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_refit2Tree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_refit2Tree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_refit2Tree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_refit2Tree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_refit2Tree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_refit2Tree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");


    if(m_doIP){
      m_refit2Tree->Branch("Negative_d0_PV",      &m_negative_d0_PV   ,  "Negative_d0_PV/D");
      m_refit2Tree->Branch("Negative_z0_PV",      &m_negative_z0_PV   ,  "Negative_z0_PV/D");
      m_refit2Tree->Branch("Positive_d0_PV",      &m_positive_d0_PV   ,  "Positive_d0_PV/D");
      m_refit2Tree->Branch("Positive_z0_PV",      &m_positive_z0_PV   ,  "Positive_z0_PV/D");
      
      
      m_refit2Tree->Branch("Negative_z0_PVerr",  &m_negative_z0_PVerr,  "Negative_z0_PVerr/D");
      m_refit2Tree->Branch("Negative_d0_PVerr",  &m_negative_d0_PVerr,  "Negative_d0_PVerr/D");
      m_refit2Tree->Branch("Positive_z0_PVerr",  &m_positive_z0_PVerr,  "Positive_z0_PVerr/D");
      m_refit2Tree->Branch("Positive_d0_PVerr",  &m_positive_d0_PVerr,  "Positive_d0_PVerr/D");
    }



    
  }
  
  //  if( m_meStacoTree == 0){

  //m_meStacoTree = new TTree(m_meStacoTreeName.c_str(), m_ValidationTreeDescription.c_str());

  //m_meStacoTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
  //m_meStacoTree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
  //m_meStacoTree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");
  //m_meStacoTree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

  //m_meStacoTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
  //m_meStacoTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
  //m_meStacoTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
  //m_meStacoTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
  //m_meStacoTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");

  //m_meStacoTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
  //m_meStacoTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
  //m_meStacoTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
  //m_meStacoTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
  //m_meStacoTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
  //}

  if( m_combTree == 0){

    //m_combTree = new TTree((m_trackParticleName+m_combTreeName).c_str(), m_ValidationTreeDescription.c_str());
    m_combTree = new TTree((m_combTreeName).c_str(), m_ValidationTreeDescription.c_str());

    m_combTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_combTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_combTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");
    m_combTree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

    m_combTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_combTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_combTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_combTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_combTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_combTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_combTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");


    m_combTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_combTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_combTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_combTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_combTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_combTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_combTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
    
    if(m_doIP){

      m_combTree->Branch("Negative_z0_PV",  &m_negative_z0_PV,  "Negative_z0_PV/D");
      m_combTree->Branch("Negative_d0_PV",  &m_negative_d0_PV,  "Negative_d0_PV/D");
      m_combTree->Branch("Positive_z0_PV",  &m_positive_z0_PV,  "Positive_z0_PV/D");
      m_combTree->Branch("Positive_d0_PV",  &m_positive_d0_PV,  "Positive_d0_PV/D");

      m_combTree->Branch("Negative_z0_PVerr",  &m_negative_z0_PVerr,  "Negative_z0_PVerr/D");
      m_combTree->Branch("Negative_d0_PVerr",  &m_negative_d0_PVerr,  "Negative_d0_PVerr/D");
      m_combTree->Branch("Positive_z0_PVerr",  &m_positive_z0_PVerr,  "Positive_z0_PVerr/D");
      m_combTree->Branch("Positive_d0_PVerr",  &m_positive_d0_PVerr,  "Positive_d0_PVerr/D");
    }
  }

  
  if( m_doRefit && m_combMuidTree == 0){

    m_combMuidTree = new TTree(m_combMuidTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_combMuidTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_combMuidTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_combMuidTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");
    m_combMuidTree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");
    
    m_combMuidTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_combMuidTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_combMuidTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_combMuidTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_combMuidTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_combMuidTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_combMuidTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");



    m_combMuidTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_combMuidTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_combMuidTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_combMuidTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_combMuidTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_combMuidTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_combMuidTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
  }

  if( m_isMC && m_truthTree == 0){

    m_truthTree = new TTree(m_truthTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_truthTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_truthTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_truthTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");
    m_truthTree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

    m_truthTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_truthTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_truthTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_truthTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_truthTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_truthTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_truthTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");

    m_truthTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_truthTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_truthTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_truthTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_truthTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_truthTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_truthTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
  }
  
  // now register the Trees
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()){
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !");
    m_validationMode = false;
  }
  
  if ((tHistSvc->regTree(m_defaultTreeFolder, m_defaultTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_defaultTree; m_defaultTree = 0;
    m_validationMode = false;
  }
  
  if ((tHistSvc->regTree(m_IDTreeFolder, m_IDTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_IDTree; m_IDTree = 0;
    m_validationMode = false;
  }

  if ((tHistSvc->regTree(m_combTreeFolder, m_combTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_combTree; m_combTree = 0;
    m_validationMode = false;
  }

  if( m_doRefit ){

    if ((tHistSvc->regTree(m_refit1TreeFolder, m_refit1Tree)).isFailure() ) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_refit1Tree; m_refit1Tree = 0;
      m_validationMode = false;
    }
    if ((tHistSvc->regTree(m_refit2TreeFolder, m_refit2Tree)).isFailure() ) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_refit2Tree; m_refit2Tree = 0;
      m_validationMode = false;
    }

    if ((tHistSvc->regTree(m_combMuidTreeFolder, m_combMuidTree)).isFailure() ) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_combMuidTree; m_combMuidTree = 0;
      m_validationMode = false;
    }

  }
  if (m_isMC) {
  if ((tHistSvc->regTree(m_truthTreeFolder, m_truthTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_truthTree; m_truthTree = 0;
    m_validationMode = false;
  }
  }

  return StatusCode::SUCCESS;
}


//==================================================================================
void IDPerfMonZmumu::RegisterHistograms()
{
  return;
}


//==================================================================================
StatusCode IDPerfMonZmumu::execute()
{

  ATH_MSG_DEBUG("Retrieving event info.");
  const EventInfo * eventInfo;
  if (evtStore()->retrieve(eventInfo).isFailure())
    ATH_MSG_ERROR("Could not retrieve event info.");
  else
  {
    m_runNumber = eventInfo->event_ID()->run_number();
    m_evtNumber = eventInfo->event_ID()->event_number();
    m_lumi_block = eventInfo->event_ID()->lumi_block();
    ATH_MSG_DEBUG(" Execute() starting on --> Run: " << m_runNumber << "  event: " << m_evtNumber);
  }

  //
  //Fill Staco muon parameters only
  m_xZmm.setContainer(PerfMonServices::MUON_COLLECTION);
  m_xZmm.doIsoSelection(m_doIsoSelection);
  m_xZmm.doIPSelection(m_doIPSelection);
  m_xZmm.SetMassWindowLow(m_MassWindowLow);
  m_xZmm.SetMassWindowHigh(m_MassWindowHigh);
  m_xZmm.SetLeadingMuonPtCut(m_LeadingMuonPtCut);
  m_xZmm.SetSecondMuonPtCut(m_SecondMuonPtCut);
  m_xZmm.SetOpeningAngleCut(m_OpeningAngleCut);
  m_xZmm.SetZ0GapCut(m_Z0GapCut);

  // check if the muon-pair passed the resonance selection cuts:
  if(!m_xZmm.Reco()){
    //failed reconstruction
    ATH_MSG_DEBUG("Failed dimuon reconstruction. m_xZmm.Reco() returned FALSE (actually returning SUCCESS)");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO("Sucessfull dimuon reconstruction. Invariant mass = " << m_xZmm.GetInvMass() << " GeV ");

  // reached this point there is a good mu+mu- pair that stisfies all selection cuts


  if (m_UseTrigger)
    {
      bool isTriggerPassed = CheckTriggerStatusAndPrescale ();
      if (isTriggerPassed) {
	ATH_MSG_INFO("Trigger passed -> accept event");
      }
      else{
	ATH_MSG_INFO("Trigger Failed -> reject event --> leave event");
	return StatusCode::SUCCESS;
      }
    }


  // std::cout << " ** IDPerfMonZmumu ** extracting muon_pos and muon_neg... " << std::endl;
  const xAOD::Muon* muon_pos = m_xZmm.getCombMuon(m_xZmm.getPosMuon(ZmumuEvent::CB));
  const xAOD::Muon* muon_neg = m_xZmm.getCombMuon(m_xZmm.getNegMuon(ZmumuEvent::CB));

  //std::cout << "  ** IDPerfMonZmumu ** muon_pos(CB) = " << muon_pos << std::endl;
  //std::cout << "                       muon_neg(CB) = " << muon_neg << std::endl;
  const xAOD::TrackParticle* p1_comb; const xAOD::Vertex* p1_comb_v;
  const xAOD::TrackParticle* p2_comb; const xAOD::Vertex* p2_comb_v;
  
  
  //To protect against failures of the estimation
  bool success_pos =false;
  bool success_neg =false;

  
  if (muon_pos && muon_neg) { // if both combined muons exist and were retrieved
    
    if (m_trackParticleName.find("InDetTrackParticles") != std::string::npos)
      {
	ATH_MSG_INFO("Selecting InDetTrackParticles");
	p1_comb = muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	p1_comb_v = p1_comb->vertex();
	p2_comb = muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	p2_comb_v = p2_comb->vertex();
	
	if (!p1_comb || !p2_comb)
	  {
	    ATH_MSG_WARNING("Requested InnerDetectorTrackParticles but not present in the input file. Exiting event..");
	    return StatusCode::SUCCESS;
	  }
      }
    else {
      ATH_MSG_INFO("Selecting CombinedTrackParticles");
      // std::cout << "  ** IDPerfMonZmumu ** retrieving combined muons from  muon_pos and muon_neg (ZmumuEvent::CB) " << std::endl;
      p1_comb = muon_pos->trackParticle(xAOD::Muon::CombinedTrackParticle);
      p1_comb_v = muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) ? (muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle))->vertex() : p1_comb->vertex();  // I'm afraid CB track has always NULL vertex
      p2_comb = muon_neg->trackParticle(xAOD::Muon::CombinedTrackParticle);
      p2_comb_v = muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) ? (muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle))->vertex() : p2_comb->vertex();  // I'm afraid CB track has always NULL vertex
      
      if (!p1_comb || !p2_comb){
	ATH_MSG_WARNING("Requested CombinedTrackParticles but not present in the input file. Exiting event");
	return StatusCode::SUCCESS;
      }
      if (m_doDebug) std::cout << " -- Salva (start) -- p1_comb and p2_comb not null :)" << std::endl; 

      
      //This is hiding things under the rug
      // in case the combined muons couldn't be retrieved try with the ID part
      // std::cout << "  ** IDPerfMonZmumu ** retrieving ID muons with (ZmumuEvent::ID) " << std::endl;
      //p1_comb = m_xZmm.getIDTrack(m_xZmm.getPosMuon(ZmumuEvent::ID));
      //p1_comb_v = p1_comb->vertex();
      //p2_comb = m_xZmm.getIDTrack(m_xZmm.getNegMuon(ZmumuEvent::ID));
      //p2_comb_v = p2_comb->vertex();
      
    }
  }
  else {
    ATH_MSG_WARNING("Could not find pos/neg muons");
    return StatusCode::SUCCESS;
  }
  if ( m_xZmm.EventPassed() ) {
    ATH_MSG_DEBUG("Accepted muon pair. Going to fill combined Staco parameters ");
    //fill Combined Staco parameters
    if (!muon_pos || !muon_neg) {
      ATH_MSG_WARNING("CB Staco Muons missing!");
    } 
    else {
      //       FillRecParameters(muon_pos->combinedMuonTrackParticle()->originalTrack(), muon_pos->combinedMuonTrackParticle()->charge());
      //       FillRecParameters(muon_neg->combinedMuonTrackParticle()->originalTrack(), muon_neg->combinedMuonTrackParticle()->charge());
      
      if (m_useTrackSelectionTool) {
	if ( !m_selTool->accept(muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)) ||
	     !m_selTool->accept(muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)) ) {
	  ATH_MSG_DEBUG("Exiting because the muons don't pass the TrackSelection");
	  return StatusCode::SUCCESS;
	}	
      }
      
      
      ATH_MSG_DEBUG("######### >> before fill rec with default << #########");
      
      //success_pos = FillRecParametersTP(muon_pos->trackParticle(xAOD::Muon::CombinedTrackParticle), muon_pos->trackParticle(xAOD::Muon::CombinedTrackParticle)->charge());
      //success_neg = FillRecParametersTP(muon_neg->trackParticle(xAOD::Muon::CombinedTrackParticle), muon_neg->trackParticle(xAOD::Muon::CombinedTrackParticle)->charge());
      
      success_pos = FillRecParametersTP(muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->charge(),muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->vertex());
      success_neg = FillRecParametersTP(muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->charge(),muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->vertex());
      if (success_pos && success_neg) m_IDTree->Fill();

      
      success_pos = FillRecParametersTP(p1_comb, muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParametersTP(p2_comb, muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);
      if (success_pos && success_neg) m_defaultTree->Fill();
      
      // combined muons
      success_pos = FillRecParameters(p1_comb->track(),muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParameters(p2_comb->track(),muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);
      
      if (success_pos && success_neg) m_combTree->Fill();
    }
  }

  // no good muon pair found, leave and continue
  if ( !m_xZmm.EventPassed() ) {
    //failed cuts, continue to next event
    ATH_MSG_DEBUG ("No good muon pair found. Leaving Execute(). Run: " << m_runNumber << "  event: " << m_evtNumber);
    return StatusCode::SUCCESS;
  }

  
  //fill truth first in case no truth match found
  if (m_isMC) {
    if (FillTruthParameters(p1_comb).isFailure()){
      ATH_MSG_WARNING("Failed to fill truth parameters - skipping event");
      return StatusCode::SUCCESS;
    }
    if (FillTruthParameters(p2_comb).isFailure()){
      ATH_MSG_WARNING("Failed to fill truth parameters - skipping event");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("fill truthTree with parameters : ");
    ATH_MSG_DEBUG("######   (negative)   ########## ");
    ATH_MSG_DEBUG("Negative px: " << m_negative_px << "\n"
		  << "Negative py: " << m_negative_py << "\n"
		  << "Negative pz: " << m_negative_pz << "\n"
		  << "Negative d0: " << m_negative_d0 << "\n"
		  << "Negative z0: " << m_negative_z0 << "\n");
    
    
    ATH_MSG_DEBUG("######   (positive)   ########## ");
    ATH_MSG_DEBUG( "Positive px: " << m_positive_px << "\n"
		   << "Positive py: " << m_positive_py << "\n"
		   << "Positive pz: " << m_positive_pz << "\n"
		   << "Positive d0: " << m_positive_d0 << "\n"
		   << "Positive z0: " << m_positive_z0 << "\n");
    
    m_truthTree->Fill();
  } // if (m_isMC)


  // changed refitting to combinedparticles since run II DESDM_ZMUMU did not store InDetTrackParticles
  if (!p1_comb->track() || !p2_comb->track()) {
     ATH_MSG_WARNING("Track missing!  p1_comb->track() or p2_comb->track()  ** Skipping Event Run: " << m_runNumber << "  event: " << m_evtNumber);
     return StatusCode::SUCCESS;
  }


  if( m_doRefit ){
    ATH_MSG_DEBUG("Going to build TrackCollections: muonTrks, muonTrksRefit1 and muonTrksRefit2");
    TrackCollection* muonTrks  = new TrackCollection(SG::OWN_ELEMENTS);
    TrackCollection* muonTrksRefit1  = new TrackCollection(SG::OWN_ELEMENTS);
    TrackCollection* muonTrksRefit2  = new TrackCollection(SG::OWN_ELEMENTS);


    ATH_MSG_DEBUG(" Going to refit and store the track parameters ");
    Trk::Track* defaultMuonTrk1 = 0;
    Trk::Track* defaultMuonTrk2 = 0;
    Trk::Track* refit1MuonTrk1 = 0;
    Trk::Track* refit2MuonTrk1 = 0;
    Trk::Track* refit1MuonTrk2 = 0;
    Trk::Track* refit2MuonTrk2 = 0;

    const xAOD::Electron* egam = 0;

    StatusCode fitStatus;
    //save default and refit track parameters
    if( p1_comb->track() ) {
      defaultMuonTrk1 = new Trk::Track(*p1_comb->track());

      fitStatus = m_TrackRefitter1->refitTrack( p1_comb->track(),egam );
      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG("Track Refit1 Failed. Skipping Event");
	return StatusCode::SUCCESS;
      } else {
	refit1MuonTrk1 = m_TrackRefitter1->refittedTrack();
	muonTrksRefit1->push_back(refit1MuonTrk1);
	ATH_MSG_DEBUG("Successfully refitted (1) track");
      }


      fitStatus = m_TrackRefitter2->refitTrack( p1_comb->track(),egam );
      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG("Track Refit2 Failed. Skipping Event");
	return StatusCode::SUCCESS;
      } else {
	refit2MuonTrk1 = m_TrackRefitter2->refittedTrack();
	muonTrksRefit2->push_back(refit2MuonTrk1);
	ATH_MSG_DEBUG("Successfully refitted (2) track");
      }

    }

    if( p2_comb->track() ) {
      defaultMuonTrk2 = new Trk::Track(*p2_comb->track());

      fitStatus = m_TrackRefitter1->refitTrack( p2_comb->track(),egam );
      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG("Track Refit1 Failed. Skipping Event");
	return StatusCode::SUCCESS;
      } else {
	refit1MuonTrk2 = m_TrackRefitter1->refittedTrack();
	muonTrksRefit1->push_back(refit1MuonTrk2);
	ATH_MSG_DEBUG("Successfully refitted (1) track");
      }


      fitStatus = m_TrackRefitter2->refitTrack( p2_comb->track(),egam );
      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG("Track Refit2 Failed. Skipping Event");
	return StatusCode::SUCCESS;
      } else {
	refit2MuonTrk2 = m_TrackRefitter2->refittedTrack();
	muonTrksRefit2->push_back(refit2MuonTrk2);
	ATH_MSG_DEBUG("Successfully refitted (2) track");
      }
    }

    //save tracks to storegrate	/
    muonTrks->push_back(defaultMuonTrk1);
    muonTrks->push_back(defaultMuonTrk2);

    StatusCode sc = evtStore()->record(muonTrks, m_outputTracksName);
    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName << endreq;
    }
    else{
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrks->size() << " " << m_outputTracksName <<" into StoreGate" << endreq;
    }

    sc = evtStore()->record(muonTrksRefit1, m_outputTracksName + "Refit1");
    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName + "Refit1" << endreq;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrksRefit1->size() << " " << m_outputTracksName  + "Refit1" <<" into StoreGate" << endreq;
    }

    sc = evtStore()->record(muonTrksRefit2, m_outputTracksName + "Refit2");
    if (sc.isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName +"Refit2" << endreq;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrksRefit2->size() << " " << m_outputTracksName + "Refit2" <<" into StoreGate" << endreq;
    }



    /* (SH) always fill the InDetTrackParticles on the default tree using the trackparticles directly
    //fill default ID parameters
    if (muonTrks->size()<2){
    ATH_MSG_WARNING("Default muon tracks are missing!");
    }else{
    ATH_MSG_INFO("-- >> going to fill default ");
    FillRecParameters(defaultMuonTrk1, p1_comb->charge());
    FillRecParameters(defaultMuonTrk2, p2_comb->charge());
    m_defaultTree->Fill();
    }

    */

    
    //fill refit1 ID parameters
    if (muonTrksRefit1->size()<2) {
      ATH_MSG_WARNING("Refit1 muon tracks are missing!");
    }
    else{
  
      ATH_MSG_DEBUG("-- >> going to fill refit1params << -- ");
      success_pos = FillRecParameters(refit1MuonTrk1, muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParameters(refit1MuonTrk2, muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);


      ATH_MSG_DEBUG("fill refit1Tree with parameters : ");
      ATH_MSG_DEBUG("######   (negative)   ########## ");
      ATH_MSG_DEBUG("Negative px: " << m_negative_px << "\n"
		    << "Negative py: " << m_negative_py << "\n"
		    << "Negative pz: " << m_negative_pz << "\n"
		    << "Negative d0: " << m_negative_d0 << "\n"
		    << "Negative z0: " << m_negative_z0 << "\n");

      ATH_MSG_DEBUG("######   (positive)   ########## ");
      ATH_MSG_DEBUG( "Positive px: " << m_positive_px << "\n"
		     << "Positive py: " << m_positive_py << "\n"
		     << "Positive pz: " << m_positive_pz << "\n"
		     << "Positive d0: " << m_positive_d0 << "\n"
		     << "Positive z0: " << m_positive_z0 << "\n");
      if (success_pos && success_neg)
	m_refit1Tree->Fill();
    }
    //fill refit2 ID parameters

    if (muonTrksRefit2->size()<2) {
      ATH_MSG_WARNING("Refit2 muon tracks are missing!");
    }else{

      ATH_MSG_DEBUG("-- >> going to fill refit2params << --");
      success_pos = FillRecParameters(refit2MuonTrk1, muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParameters(refit2MuonTrk2, muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);
      
      if (success_pos && success_neg)
	m_refit2Tree->Fill();

    }

    if (!muon_pos || !muon_neg) {
      ATH_MSG_WARNING("CB Muons missing!");
    } 
    else {

      ATH_MSG_DEBUG("-- >> going to fill combined muons params << --");
      success_pos = FillRecParameters(p1_comb->track(), muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParameters(p2_comb->track(), muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);
      if (success_pos && success_neg) m_combMuidTree->Fill();
    }

    ATH_MSG_DEBUG(" Execute() completed for Run: " << m_runNumber << "  event: " << m_evtNumber);
  }

  if (m_doDebug) std::cout<< " -- Salva --  event completed -- Run: " << m_runNumber << "  event: " << m_evtNumber << std::endl;
  return StatusCode::SUCCESS;
}

//==================================================================================
bool IDPerfMonZmumu::FillRecParametersTP(const xAOD::TrackParticle* trackp, const xAOD::TrackParticle* trackp_for_unbias,double charge, const xAOD::Vertex* vertex)
{
  if (!trackp){
    //ATH_MSG_WARNING("Empty Trackparticle. Skipping.");
    return false;
  }

  if (m_doRemoval && !trackp_for_unbias){
    return false;
  }
    
  
  //   auto trkPerigee = &trackp->perigeeParameters();
  //  if(m_CorrectMaps) {
   //xAOD::TrackParticle* newTrack = nullptr;
   //if ( m_trackBiasingTool->correctedCopy( *trackp, newTrack ) == CP::CorrectionCode::Error) {
   //    }
   //    auto trkPerigee = &newTrack->perigeeParameters();
   //  }
  double px = 0;
  double py = 0;
  double pz = 0;
  double d0 = 0;
  double z0 = 0;
  double d0res = 0;
  double z0res = 0;
  double PVd0res = 0;
  double PVz0res = 0;
  double PVd0 = 0;
  double PVz0 = 0;

  px = trackp->p4().Px();
  py = trackp->p4().Py();
  pz = trackp->p4().Pz();
  d0 = trackp->d0();
  z0 = trackp->z0();
  //d0res = -99;   //Absolutely need to get the covariance matrix
  d0res = trackp->definingParametersCovMatrix()(0,0);
  //z0res = -99;   //Absolutely need to get the covariance matrix
  z0res = trackp->definingParametersCovMatrix()(1,1);
  
  
  if(m_doIP){
    const Trk::ImpactParametersAndSigma* iPandSigma(NULL);
    ATH_MSG_INFO("using the trackToVertexIPEstimator");
    
    //Calling the estimate(trackp,newtrackp,vertex,doRemoval)
    //The first track is used to unbias the vertex, the second to get the estrapolation
    iPandSigma = m_trackToVertexIPEstimator->estimate(trackp_for_unbias,trackp,vertex,m_doRemoval);
    
    if( iPandSigma==0 ){
      ATH_MSG_WARNING ("FillRecParametersTP::trackToVertexIPEstimator failed !");
      return false;
    }
    else{
      ATH_MSG_DEBUG("FillRecParametersTP::trackToVertexIPEstimator success !");
      PVd0 = iPandSigma->IPd0;
      PVd0res = iPandSigma->PVsigmad0;
      d0res = iPandSigma->sigmad0;
      PVz0 = iPandSigma->IPz0;
      PVz0res = iPandSigma->PVsigmaz0;
      z0res = iPandSigma->sigmaz0;
    }
  }

  if (charge == 1) {
    m_positive_px = px;
    m_positive_py = py;
    m_positive_pz = pz;
    m_positive_z0 = z0;
    m_positive_z0_err = z0res;
    m_positive_d0 = d0;
    m_positive_d0_err = d0res;
    if(m_doIP){
      m_positive_z0_PV = PVz0;
      m_positive_d0_PV = PVd0;
      m_positive_z0_PVerr = PVz0res;
      m_positive_d0_PVerr = PVd0res;
      ATH_MSG_DEBUG("(Filled charge == 1 ) (reco)-> px : "<< px <<" py: "<<py <<" pz: "<<pz <<" d0: "<<m_positive_d0 << " d0res : "<< d0res << " PVd0res : "<< PVd0res <<" z0: "<< m_positive_z0 << " z0res : " << z0res <<  " PVz0res : "<< PVz0res );
    }

  } else  if (charge == -1) {
    m_negative_px = px;
    m_negative_py = py;
    m_negative_pz = pz;
    m_negative_z0 = z0;
    m_negative_z0_err = z0res;
    m_negative_d0 = d0;
    m_negative_d0_err = d0res;
    if(m_doIP){
      m_negative_z0_PV = PVz0;
      m_negative_d0_PV = PVd0;
      m_negative_z0_PVerr = PVz0res;
      m_negative_d0_PVerr = PVd0res;
      ATH_MSG_DEBUG("(Filled charge == -1 ) (reco)-> px : "<< px <<" py: "<<py <<" pz: "<<pz <<" d0: "<<m_negative_d0 << " d0res : "<< d0res << " PVd0res : "<< PVd0res <<" z0: "<< m_negative_z0 << " z0res : " << z0res <<  " PVz0res : "<< PVz0res );
    }

  }

  return true;
}


//==================================================================================
bool IDPerfMonZmumu::FillRecParameters(const Trk::Track* track, const xAOD::TrackParticle* trackp_for_unbias, double charge,const xAOD::Vertex* vertex)
{

  if (!track){
    //ATH_MSG_WARNING("Empty Track. Skipping.");
    return false;
  }

  if (m_doRemoval && !trackp_for_unbias) {
    return false;
  }

  const Trk::Perigee* trkPerigee = track->perigeeParameters();
  const Trk::Perigee* trk_for_unbiasPerigee = &(trackp_for_unbias->perigeeParameters());

  double px = 0;
  double py = 0;
  double pz = 0;
  double d0 = 0;
  double z0 = 0;
  
  double d0_err = 999.;
  double z0_err = 999.;
  
  double PVd0 = 0;
  double PVz0 = 0;
  double PVd0res = 0;
  double PVz0res = 0;
  
  

  if(trkPerigee){
    double qOverP   = trkPerigee->parameters()[Trk::qOverP];
    if (qOverP) {
      px = trkPerigee->momentum().x();
      py = trkPerigee->momentum().y();
      pz = trkPerigee->momentum().z();
      d0 = trkPerigee->parameters()[Trk::d0];
      z0 = trkPerigee->parameters()[Trk::z0];
      d0_err = Amg::error(*trkPerigee->covariance(),Trk::d0);
      z0_err = Amg::error(*trkPerigee->covariance(),Trk::z0);
    }
    //    std::cout << " -- trkPerigee -- q: " << (int) charge << "  px: " << px << "  py: " << py << "  pz: " << pz << "  d0: "<< d0 << "   z0: "<< z0 << " d0_err: " << d0_err << std::endl;
    //    delete trkPerigee;
  }

  //const Trk::AtaStraightLine*  atBL =   dynamic_cast<const Trk::AtaStraightLine*>(m_trackToVertexTool->trackAtBeamline( *track ));
  auto atBL = m_trackToVertexTool->trackAtBeamline( *track );

  if (atBL){
    // std::cout << "################## >>> does not fail atBL extrapolation" << std::endl;
    double qOverP   = atBL->parameters()[Trk::qOverP];
    if(qOverP){
      px = atBL->momentum().x();
      py = atBL->momentum().y();
      pz = atBL->momentum().z();
      d0 = atBL->parameters()[Trk::d0];
      z0 = atBL->parameters()[Trk::z0];
      //      z0_err = Amg::error(*trkPerigee->covariance(),Trk::z0);  //->Why not?
      //      d0_err = Amg::error(*trkPerigee->covariance(),Trk::d0);  //->Why not?
    } 
    //    std::cout << " -- atBL -- px: " << px << "  py: " << py << "  pz: " << pz << "  d0: "<< d0 << "   z0: "<< z0 << std::endl;
    delete atBL;
  }
  else 
    {
      ATH_MSG_WARNING("FillRecParameters::Failed extrapolation to the BeamLine");
    }
  
  if(m_doIP && vertex && track->perigeeParameters()){ //I assume that the vertex is the same of the original track
    const Trk::ImpactParametersAndSigma* iPandSigma(NULL);
    iPandSigma = m_trackToVertexIPEstimator->estimate(trk_for_unbiasPerigee,trkPerigee,vertex,m_doRemoval);
    if( iPandSigma==0 ){
      ATH_MSG_WARNING ("FillRecParameters::trackToVertexIPEstimator failed !");
      return false;
    }
    else{
      ATH_MSG_DEBUG("FillRecParameters::trackToVertexIPEstimator success !");
      PVd0 = iPandSigma->IPd0;
      PVd0res = iPandSigma->PVsigmad0;
      PVz0 = iPandSigma->IPz0;
      PVz0res = iPandSigma->PVsigmaz0;
    }
  }
    
  if (charge == 1) {
    m_positive_px = px;
    m_positive_py = py;
    m_positive_pz = pz;
    m_positive_z0 = z0;
    m_positive_d0 = d0;
    m_positive_d0_err = d0_err;
    m_positive_z0_err = z0_err;
    if(m_doIP){
      m_positive_z0_PV    = PVz0;
      m_positive_d0_PV    = PVd0;
      m_positive_z0_PVerr = PVz0res;
      m_positive_d0_PVerr = PVd0res;
    }
    ATH_MSG_DEBUG("(Filled charge == 1 ) (reco)-> px : "<< px <<" py: "<<py <<" pz: "<<pz <<" d0: "<<d0<<" z0: "<< z0);
  }
  if (charge == -1) {
    m_negative_px = px;
    m_negative_py = py;
    m_negative_pz = pz;
    m_negative_z0 = z0;
    m_negative_d0 = d0;
    m_negative_d0_err = d0_err;
    m_negative_z0_err = z0_err;
    if(m_doIP){
      m_negative_z0_PV = PVz0;
      m_negative_d0_PV = PVd0;
      m_negative_z0_PVerr = PVz0res;
      m_negative_d0_PVerr = PVd0res;
    }
    ATH_MSG_DEBUG("(Filled charge == -1 ) (reco)-> px : "<< px <<" py: "<<py <<" pz: "<<pz <<" d0: "<<d0<<" z0: "<<z0 );
  }

  return true;
}

//==================================================================================
StatusCode IDPerfMonZmumu::FillTruthParameters(const xAOD::TrackParticle* trackParticle)
{
  if (!trackParticle ){//|| !trackParticle->vertex()){
    ATH_MSG_WARNING("Empty Trackparticle. Skipping.");
    return StatusCode::FAILURE;
  }
  float result(std::numeric_limits<float>::quiet_NaN());
  if (trackParticle->isAvailable<float>("truthMatchProbability")){
    result = trackParticle->auxdata<float>("truthMatchProbability" );
    std::cout <<"DEBUG :: TruthMatchProbablity of trackpartile : > " << result<< std::endl;
  }

  const xAOD::TruthParticle* particle = getTruthParticle( *trackParticle );
  double charge = 0;
  if (!particle) {
    ATH_MSG_DEBUG(" Failure while retrieving the truth particle");
    return StatusCode::FAILURE;
  }
  if(particle->pdgId() == 13) charge = -1.;
  else if(particle->pdgId() == -13) charge = 1.;
  if( particle->isNeutral() ) return StatusCode::FAILURE;
  const Amg::Vector3D momentum(particle->px(), particle->py(), particle->pz());
  
  const xAOD::TruthVertex * ptruthVertex(0);
  ptruthVertex=particle->prodVtx();
  if (!ptruthVertex){
    ATH_MSG_DEBUG("A production vertex pointer was retrieved, but it is NULL");
    return StatusCode::FAILURE;
  }

  const auto xPos=ptruthVertex->x();
  const auto yPos=ptruthVertex->y();
  const auto z_truth=ptruthVertex->z();

  const Amg::Vector3D position(xPos, yPos, z_truth);
  //  const float prodR_truth = std::sqrt(xPos*xPos+ yPos*yPos);
  //delete ptruthVertex;ptruthVertex=0;
  const Trk::CurvilinearParameters cParameters(position, momentum, charge);

  Trk::PerigeeSurface persf( m_beamSpotSvc->beamPos() );

  const Trk::TrackParameters* tP = m_extrapolator->extrapolate(cParameters,persf, Trk::anyDirection, false);


  double px = 0;
  double py = 0;
  double pz = 0;
  double d0 = 0;
  double d0res = 0;
  double PVd0res = 0;
  double z0 = 0;
  double z0res = 0;
  double PVz0res = 0;


  double d0recoPos =  m_positive_d0;
  double z0recoPos =  m_positive_z0;
  double d0recoNeg =  m_negative_d0;
  double z0recoNeg =  m_negative_z0;

  ATH_MSG_DEBUG("reco IPs (pos): > d0 : "<<d0recoPos << " z0: " << z0recoPos << " trackp z0 : " << trackParticle->z0() << " trackp d0 : " << trackParticle->d0());
  ATH_MSG_DEBUG("reco IPs (neg): > d0 : "<<d0recoNeg << " z0: " << z0recoNeg << " trackp z0 : " << trackParticle->z0() <<" trackp d0 : " << trackParticle->d0() );

  if (tP){
    double qOverP_truth = tP->parameters()[Trk::qOverP];
    if( qOverP_truth ){

      px = tP->momentum().x();
      py = tP->momentum().y();
      pz = tP->momentum().z();
      d0 = tP->parameters()[Trk::d0];
      z0 = tP->parameters()[Trk::z0];

      ATH_MSG_DEBUG("cand perig HEP particle (truth) px : "<< tP->momentum().x());
      ATH_MSG_DEBUG("cand perig HEP particle (truth) py : "<< tP->momentum().y());
      ATH_MSG_DEBUG("cand perig HEP particle (truth) pz : "<< tP->momentum().z());
      ATH_MSG_DEBUG("cand perig HEP particle (truth) d0 : "<< tP->parameters()[Trk::d0]);
      ATH_MSG_DEBUG("cand perig HEP particle (truth) z0 : "<< tP->parameters()[Trk::z0]);
      delete tP;tP=0;
    }
  }


  if (charge == 1) {
    m_positive_px = px;
    m_positive_py = py;
    m_positive_pz = pz;
    m_positive_z0 = z0recoPos -z0;
    //    m_positive_z0 = z0;

    m_positive_z0_err = z0res;
    m_positive_z0_PVerr = PVz0res;
    m_positive_d0 = d0recoPos -d0;
    //    m_positive_d0 = d0;

    m_positive_d0_err = d0res;
    m_positive_d0_PVerr = PVd0res;
    //    m_px = px;
    //    m_py = py;
    //    m_pz = pz;
    //    m_z0 = z0;
    //    m_d0 = d0;
    //    m_d0_rec_truth = d0recoPos -d0;
    //    m_z0_rec_truth = z0recoPos -z0;
    ATH_MSG_DEBUG("(Filled charge == 1 ) (truth)-> px : "<< m_positive_px <<" py: "<<m_positive_py <<" pz: "<<m_positive_pz <<" d0: "<<m_positive_d0 << " z0: "<< m_positive_z0);

  } else  if (charge == -1) {
    m_negative_px = px;
    m_negative_py = py;
    m_negative_pz = pz;
    m_negative_z0 = z0recoNeg-z0;
    //    m_negative_z0 = z0;

    m_negative_z0_err = z0res;
    m_negative_z0_PVerr = PVz0res;
    m_negative_d0 = d0recoNeg-d0;
    //    m_negative_d0 = d0;

    m_negative_d0_err = d0res;
    m_negative_d0_PVerr = PVd0res;

    //    m_px = px;
    //    m_py = py;
    //    m_pz = pz;
    //    m_z0 = z0;
    //    m_d0 = d0;
    //    m_d0_rec_truth = d0recoNeg -d0;
    //    m_z0_rec_truth = z0recoNeg -z0;
    ATH_MSG_DEBUG("(Filled charge == -1 ) (truth)-> px : "<< m_negative_px <<" py: "<<m_negative_py <<" pz: "<<m_negative_pz <<" d0: "<<m_negative_d0 << " z0: "<< m_negative_z0);
  }
  return StatusCode::SUCCESS;
}


//==================================================================================
const xAOD::TruthParticle* IDPerfMonZmumu::getTruthParticle( const xAOD::IParticle& p ) {


  /// A convenience type declaration
  typedef ElementLink< xAOD::TruthParticleContainer > Link_t;

  /// A static accessor for the information
  static SG::AuxElement::ConstAccessor< Link_t > acc( "truthParticleLink" );

  // Check if such a link exists on the object:
  if( ! acc.isAvailable( p ) ) {
    return 0;
  }

  // Get the link:
  const Link_t& link = acc( p );

  // Check if the link is valid:
  if( ! link.isValid() ) {
    return 0;
  }

  // Everything has passed, let's return the pointer:
  return *link;
}

//==================================================================================
StatusCode IDPerfMonZmumu::finalize()
{
  return StatusCode::SUCCESS;
}

//==================================================================================
StatusCode IDPerfMonZmumu::CheckTriggerStatusAndPrescale ()
{
  m_triggerPrescale = 1; // default value

  // check trigger status
  if(m_triggerDecision.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_triggerDecision << " turn it off");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("retrieved tool: " << m_triggerDecision );
  }

  float thisEventTriggerPrescale = 999999.9;
  float thisHLTTriggerPrescale = 999999.9;
  bool thisHLTIsPassed = false;

  // HLT triggers
  const std::vector<std::string> myHLTtriglist = m_triggerDecision->getChainGroup("HLT_mu.*, HLT_2mu.*")->getListOfTriggers();
  for (int i=0; i < (int) myHLTtriglist.size(); i++) {
    if ( m_triggerDecision->isPassed(myHLTtriglist.at(i)) ) {
      // std::cout << "HLT trigger = " << i << " out of " << myHLTtriglist.size() << " --> " << myHLTtriglist.at(i) <<  "  isPassed? " << m_triggerDecision->isPassed(myHLTtriglist.at(i)) << "  prescale: " << m_triggerDecision->getChainGroup(myHLTtriglist.at(i))->getPrescale() << std::endl;
      ATH_MSG_DEBUG("HLT trigger = " << i << " out of " << myHLTtriglist.size() << " --> " << myHLTtriglist.at(i) <<  "  isPassed? " << m_triggerDecision->isPassed(myHLTtriglist.at(i)) << "  prescale: " << m_triggerDecision->getChainGroup(myHLTtriglist.at(i))->getPrescale() );
      if (m_triggerDecision->getChainGroup(myHLTtriglist.at(i))->getPrescale() - (int) m_triggerDecision->getChainGroup(myHLTtriglist.at(i))->getPrescale() >0.1) std::cout << " -- SALVA -- fractional " << std::endl;
    }
    if ( m_triggerDecision->isPassed(myHLTtriglist.at(i)) ) { // trigger is passed
      thisHLTIsPassed = true;
      float thisprescale = m_triggerDecision->getChainGroup(myHLTtriglist.at(i))->getPrescale();
      if (thisprescale < thisHLTTriggerPrescale) {
	thisHLTTriggerPrescale = thisprescale;
      }
    }
  }
  if (thisHLTTriggerPrescale < 1) thisHLTTriggerPrescale = 1;
  ATH_MSG_DEBUG("Event HLT trigger prescale = " << thisHLTTriggerPrescale);

  //if (thisL1IsPassed && thisHLTIsPassed) {
  if (thisHLTIsPassed) {
    ATH_MSG_DEBUG("Trigger passed -> accept event");
  }
  else{
    ATH_MSG_DEBUG("Trigger Failed -> reject event");
    return StatusCode::FAILURE;
  }

  // event prescale
  thisEventTriggerPrescale = thisHLTTriggerPrescale;
  ATH_MSG_INFO("Event trigger prescale = " << thisEventTriggerPrescale);

  m_triggerPrescale = thisEventTriggerPrescale;

  return StatusCode::SUCCESS;
}


//==================================================================================
const xAOD::Vertex* IDPerfMonZmumu::GetDiMuonVertex(const xAOD::TrackParticle* muon1, const xAOD::TrackParticle* muon2) {
  
  const xAOD::VertexContainer* vxContainer(0);
  const xAOD::Vertex* myVtx(0);
  vxContainer = PerfMonServices::getContainer<xAOD::VertexContainer> (PerfMonServices::VTX_COLLECTION);
  if (!vxContainer) { 
    return myVtx;
  }
  
  for (int ivtx=0; ivtx< (int) vxContainer->size();ivtx++)  {
    myVtx = (*vxContainer)[ivtx];
    if ((myVtx->vertexType() == xAOD::VxType::PriVtx)) {
      if (m_LooseT2VAssociationTool->isCompatible(*muon1,*myVtx) && (m_LooseT2VAssociationTool->isCompatible(*muon2,*myVtx)))
	return myVtx;
    }
  }
  return myVtx;
}
