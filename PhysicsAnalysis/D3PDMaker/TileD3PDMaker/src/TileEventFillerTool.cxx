/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileEventFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Date: September 2012
 *
 */

/// =========================== BEGIN OF COMMENTS ================================
/// Implementation of TileEventFillerTool, which is responsible for 
/// retrieving EventInfo and feeding the D3PD with event-by-event information.
/// Level of details can be set as follows:
/// - 0 NO INFORMATION
/// - 1 RUN AND EVENT NUMBERS, TIMESTAMP
/// - 2 NUMBER OF STACO/MUID MUONS
/// - 3 ELABORATE EVENT INFORMATION
/// - 4 TRIGGER DECISIONS
/// - 5 BASIC MISSING ENERGY
/// - 6 CALORIMETRIC MISSING ENERGY
/// - 7 ELABORATE MISSING ENERGY
/// - 8 INVARIANT MASS

// INCLUDE HEADER FILE
#include "TileEventFillerTool.h"


// ADD TILEMUONTRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{
//=========================================================================
TileEventFillerTool::TileEventFillerTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : D3PD::BlockFillerTool<xAOD::EventInfo>(type,name,parent)
{
//=========================================================================
// CONSTRUCTOR
  declareProperty("LevelOfDetails", m_LevelOfDetails);
  declareProperty("METContainer", m_metContainer = "MET_Reference_AntiKt4LCTopo"); // 'MET_RefFinal' in r19

  m_storeGate =	0;
  m_trigDec = 0;
  m_pVertex = 0;
        // PILE-UP
  m_actualInteractionPerCrossing = 0;
  m_averageInteractionPerCrossing = 0;

  // EVENT TYPE
  m_is_simulation = 0;
  m_is_calibration = 0;
  m_is_testbeam = 0;

  // VERTEXING
  m_nVtx = 0;
 
   // EVENT FILTERS
    m_EF_2mu10 = 0;
    m_EF_2mu10_loose = 0;
    m_EF_2mu4 = 0;
    m_EF_2mu6 = 0;
    m_EF_3mu4 = 0;
    m_EF_3mu6 = 0;
    m_EF_4mu4 = 0;
    m_EF_mu10 = 0;
    m_EF_mu10_loose = 0;
    m_EF_mu10i_loose = 0;
    m_EF_mu13 = 0;
    m_EF_mu15 = 0;
    m_EF_mu15i = 0;
    m_EF_mu15i_medium = 0;
    m_EF_mu18 = 0;
    m_EF_mu18_medium = 0;
    m_EF_mu20 = 0;
    m_EF_mu20_medium = 0;
    m_EF_mu20i = 0;
    m_EF_mu20i_medium = 0;
    m_EF_mu22 = 0;
    m_EF_mu22_medium = 0;
    m_EF_mu24_medium = 0;
    m_EF_mu24_tight = 0;
    m_EF_mu30_medium = 0;
    m_EF_mu30_tight = 0;
    m_EF_mu4 = 0;
    m_EF_mu4_mu6 = 0;
    m_EF_mu40_slow = 0;
    m_EF_mu6 = 0;

   // LEVEL 1 TRIGGERS
    m_L1_MU0_EMPTY = 0;
    m_L1_MU0 = 0;
    m_L1_MU4 = 0;
    m_L1_MU6 = 0;
    m_L1_MU10 = 0;
    m_L1_MU11 = 0;
    m_L1_MU15 = 0;
    m_L1_MU20 = 0;
    m_L1_MU40 = 0;
    m_L1_MU0_COMM = 0;
    m_L1_2MU0 = 0;
    m_L1_2MU4 = 0;
    m_L1_2MU6 = 0;
    m_L1_2MU10 = 0;
    m_L1_2MU11 = 0;
    m_L1_2MU15 = 0;
    m_L1_2MU20 = 0;
    m_L1_3MU0 = 0;
    m_L1_3MU4 = 0;
    m_L1_3MU6 = 0;
    m_L1_4MU0 = 0;


   // EVENT FLAGS
   m_run_number = 0;
   m_event_number = 0;
   m_timestamp = 0;
   m_timestamp_ns = 0;
   m_lbn = 0;
   m_bcid = 0;
   m_detmask0 = 0;
   m_detmask1 = 0;

   // MISSING ENERGY VARIABLES
   m_isMissingEtAvailable = true;

   m_MET_Ref_FinalClus_Ex = 0;
   m_MET_Ref_FinalClus_Ey = 0;
   m_MET_Ref_FinalClus_EtSum = 0;

   m_MET_Ref_FinalTrk_Ex = 0;
   m_MET_Ref_FinalTrk_Ey = 0;
   m_MET_Ref_FinalTrk_EtSum = 0;

   //size of Muons container
   m_N_mu = 0;


} // TILEMUONEVENTFILLERTOOL

//===============================================
StatusCode TileEventFillerTool::initialize(){
//===============================================
// RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES

    // CHECK WHETHER BLOCKFILLERTOOL HAS BEEN INITIALIZED
    CHECK( D3PD::BlockFillerTool<xAOD::EventInfo>::initialize() );

    // OUTPUT MESSAGING SERVICE
    
    ATH_MSG_DEBUG(" NOW IN TILEMUONEVENTFILLERTOOL::INITIALIZE() ");

    // RETRIEVE STOREGATE SERVICE
    if(service("StoreGateSvc",m_storeGate).isFailure()){
        ATH_MSG_ERROR("STOREGATE SERVICE NOT FOUND!");
        return StatusCode::FAILURE;
    } // IF



    // RETRIEVE TRIGGER DECISION TOOL
    if(m_LevelOfDetails > 4){
        if( ! m_storeGate->retrieve( m_trigDec , "xTrigDecision").isSuccess() ){
          ATH_MSG_WARNING("TRIGGER DECISIONS ARE NOT LOADED PROPERLY.");
          return StatusCode::FAILURE;
        }
    } // IF

    return StatusCode::SUCCESS;
} // TILEMUONEVENTFILLERTOOL::INITIALIZE

//=========================================
StatusCode TileEventFillerTool::book(){
//=========================================
// ADDS VARIABLES TO THE D3PD TREE

    // OUTPUT MESSAGING SERVICE
    
    ATH_MSG_DEBUG(" NOW IN TileEventFillerTool::book() ");

    // EVENT FLAGS AND VARIABLES
    if(m_LevelOfDetails > 0){
        CHECK( addVariable( "run_number",     m_run_number) );
        CHECK( addVariable( "event_number",   m_event_number) );
        CHECK( addVariable( "timestamp",      m_timestamp) );
 
        CHECK( addVariable( "actualMu",       m_actualInteractionPerCrossing, "", 0 ) );
        CHECK( addVariable( "averageMu",      m_averageInteractionPerCrossing, "", 0 ) ); 
	CHECK( addVariable( "is_simulation",  m_is_simulation,  "", false ) );
        CHECK( addVariable( "is_calibration", m_is_calibration, "", false ) );
        CHECK( addVariable( "is_testbeam",    m_is_testbeam,    "", false ) );
     
        CHECK( addVariable( "lbn",            m_lbn) );
   
    } // IF

    // ELABORATE EVENT INFORMATION
    if(m_LevelOfDetails > 1){
        CHECK( addVariable( "timestamp_ns",   m_timestamp_ns) );
        CHECK( addVariable( "bcid",           m_bcid) );
        CHECK( addVariable( "detmask0",       m_detmask0) );
        CHECK( addVariable( "detmask1",       m_detmask1) );
    } // IF

    // NUMBER OF MUONS
    if(m_LevelOfDetails > 2){
        CHECK( addVariable( "muon_n",   m_N_mu, "", 0 ) );
    } // IF

    // BASIC MISSING ENERGY
    if(m_LevelOfDetails > 3){
        CHECK( addVariable( "MET_Ref_FinalClus_Ex",               m_MET_Ref_FinalClus_Ex, "", 0.) );
        CHECK( addVariable( "MET_Ref_FinalClus_Ey",               m_MET_Ref_FinalClus_Ey, "", 0.) );
        CHECK( addVariable( "MET_Ref_FinalClus_EtSum",            m_MET_Ref_FinalClus_EtSum, "", 0.) );

        CHECK( addVariable( "MET_Ref_FinalTrk_Ex",               m_MET_Ref_FinalTrk_Ex, "", 0.) );
        CHECK( addVariable( "MET_Ref_FinalTrk_Ey",               m_MET_Ref_FinalTrk_Ey, "", 0.) );
        CHECK( addVariable( "MET_Ref_FinalTrk_EtSum",            m_MET_Ref_FinalTrk_EtSum, "", 0.) );

    } // IF

    // TRIGGER DECISIONS
    if(m_LevelOfDetails > 4){
        // HIGH LEVEL TRIGGERS (HLT): EVENT FILTERS (EF)
        CHECK( addVariable ("EF_2mu10",       m_EF_2mu10,       "", false) );
        CHECK( addVariable ("EF_2mu10_loose", m_EF_2mu10_loose, "", false) );
        CHECK( addVariable ("EF_2mu4",        m_EF_2mu4,        "", false) );
        CHECK( addVariable ("EF_2mu6",        m_EF_2mu6,        "", false) );
        CHECK( addVariable ("EF_3mu4",        m_EF_3mu4,        "", false) );
        CHECK( addVariable ("EF_3mu6",        m_EF_3mu6,        "", false) );
        CHECK( addVariable ("EF_4mu4",        m_EF_4mu4,        "", false) );
        CHECK( addVariable ("EF_mu10",        m_EF_mu10,        "", false) );
        CHECK( addVariable ("EF_mu10_loose",  m_EF_mu10_loose,  "", false) );
        CHECK( addVariable ("EF_mu10i_loose", m_EF_mu10i_loose, "", false) );
        CHECK( addVariable ("EF_mu13",        m_EF_mu13,        "", false) );
        CHECK( addVariable ("EF_mu15",        m_EF_mu15,        "", false) );
        CHECK( addVariable ("EF_mu15i",       m_EF_mu15i,       "", false) );
        CHECK( addVariable ("EF_mu15i_medium",m_EF_mu15i_medium,"", false) );
        CHECK( addVariable ("EF_mu18",        m_EF_mu18,        "", false) );
        CHECK( addVariable ("EF_mu18_medium", m_EF_mu18_medium, "", false) );
        CHECK( addVariable ("EF_mu20",        m_EF_mu20,        "", false) );
        CHECK( addVariable ("EF_mu20_medium", m_EF_mu20_medium, "", false) );
        CHECK( addVariable ("EF_mu20i",       m_EF_mu20i,       "", false) );
        CHECK( addVariable ("EF_mu20i_medium",m_EF_mu20i_medium,"", false) );
        CHECK( addVariable ("EF_mu22",        m_EF_mu22,        "", false) );
        CHECK( addVariable ("EF_mu22_medium", m_EF_mu22_medium, "", false) );
        CHECK( addVariable ("EF_mu24_medium", m_EF_mu24_medium, "", false) );
        CHECK( addVariable ("EF_mu24_tight",  m_EF_mu24_tight,  "", false) );
        CHECK( addVariable ("EF_mu30_medium", m_EF_mu30_medium, "", false) );
        CHECK( addVariable ("EF_mu30_tight",  m_EF_mu30_tight,  "", false) );
        CHECK( addVariable ("EF_mu4",         m_EF_mu4,         "", false) );
        CHECK( addVariable ("EF_mu4_mu6",     m_EF_mu4_mu6,     "", false) );
        CHECK( addVariable ("EF_mu40_slow",   m_EF_mu40_slow,   "", false) );
        CHECK( addVariable ("EF_mu6",         m_EF_mu6,         "", false) );

        // LEVEL 1 TRIGGERS
        CHECK( addVariable ("L1_MU0_EMPTY", m_L1_MU0_EMPTY      ,"", false) );
        CHECK( addVariable ("L1_MU0",       m_L1_MU0            ,"", false) );
        CHECK( addVariable ("L1_MU4",       m_L1_MU4            ,"", false) );
        CHECK( addVariable ("L1_MU6",       m_L1_MU6            ,"", false) );
        CHECK( addVariable ("L1_MU10",      m_L1_MU10           ,"", false) );
        CHECK( addVariable ("L1_MU11",      m_L1_MU11           ,"", false) );
        CHECK( addVariable ("L1_MU15",      m_L1_MU15           ,"", false) );
        CHECK( addVariable ("L1_MU20",      m_L1_MU20           ,"", false) );
        CHECK( addVariable ("L1_MU40",      m_L1_MU40           ,"", false) );
        CHECK( addVariable ("L1_MU0_COMM",  m_L1_MU0_COMM       ,"", false) );
        CHECK( addVariable ("L1_2MU0",      m_L1_2MU0           ,"", false) );
        CHECK( addVariable ("L1_2MU4",      m_L1_2MU4           ,"", false) );
        CHECK( addVariable ("L1_2MU6",      m_L1_2MU6           ,"", false) );
        CHECK( addVariable ("L1_2MU10",     m_L1_2MU10          ,"", false) );
        CHECK( addVariable ("L1_2MU11",     m_L1_2MU11          ,"", false) );
        CHECK( addVariable ("L1_2MU15",     m_L1_2MU15          ,"", false) );
        CHECK( addVariable ("L1_2MU20",     m_L1_2MU20          ,"", false) );
        CHECK( addVariable ("L1_3MU0",      m_L1_3MU0           ,"", false) );
        CHECK( addVariable ("L1_3MU4",      m_L1_3MU4           ,"", false) );
        CHECK( addVariable ("L1_3MU6",      m_L1_3MU6           ,"", false) );
        CHECK( addVariable ("L1_4MU0",      m_L1_4MU0           ,"", false) );
    } // IF

    // VERTEXING AND PILEUP
    if(m_LevelOfDetails > 5){
        CHECK( addVariable( "nVtx",           m_nVtx,      "", 0 ) );
    } // IF

    return StatusCode::SUCCESS;
} // TILEMUONEVENTFILLERTOOL::BOOK

//===========================================================
StatusCode TileEventFillerTool::fill(const xAOD::EventInfo& p){
//===========================================================
// FILLS D3PD TREE EVENT-BY-EVENT

    // OUTPUT MESSAGING SERVICE
    
    ATH_MSG_DEBUG(" in TileEventFillerTool::fill ");

    // GENERAL EVENT IDENTIFICATION INFORMATION
    if(m_LevelOfDetails > 0){
        // EVENT ID
        *m_run_number     = p.runNumber();
        *m_event_number   = p.eventNumber();
        *m_timestamp      = p.timeStamp();
	*m_is_simulation  = p.eventType( xAOD::EventInfo::IS_SIMULATION );
        *m_is_calibration = p.eventType( xAOD::EventInfo::IS_CALIBRATION );
        *m_is_testbeam    = p.eventType( xAOD::EventInfo::IS_TESTBEAM );
	*m_lbn            = p.lumiBlock();
        *m_actualInteractionPerCrossing  = p.actualInteractionsPerCrossing(); 
        *m_averageInteractionPerCrossing = p.averageInteractionsPerCrossing(); 
    } // IF

    // ELABORATE EVENT INFORMATION
    if(m_LevelOfDetails > 1){
        *m_timestamp_ns = p.timeStampNSOffset();
        *m_bcid         = p.bcid();
        *m_detmask0     = p.detectorMask0();
        *m_detmask1     = p.detectorMask1();
    } // IF

    // NUMBER OF MUONS
    if(m_LevelOfDetails > 2){
         const xAOD::MuonContainer *MuonsCont = 0;
         CHECK( m_storeGate->retrieve( MuonsCont, "Muons" ) );
         *m_N_mu        = MuonsCont->size();
    } // IF

    //======================//
    // STORE MISSING ENERGY //
    //======================//
    // MISSING ENERGY POINTER RETRIEVAL: EACH TIME IT IS CHECKED WHETHER THE MISSING ENERGY VARIABLE
    // COULD BE RETRIEVED FROM THE STOREGATE. THEN THE TRANSVERSE ENERGIES IN X AND Y AND THE SUMMED
    // MISSING ENERGY ARE STORED.
    if(m_LevelOfDetails > 3 && m_isMissingEtAvailable){
      // SET WHICH MISSING ENERGY VARIABLES SHOULD BE ADDED TO D3PD
      const xAOD::MissingETContainer* missingEtContainer; 
      if(!m_storeGate->retrieve(missingEtContainer, m_metContainer).isFailure()) {

        const MissingET* finalClus = (*missingEtContainer)["FinalClus"]; 
        if (!finalClus) {
          ATH_MSG_WARNING(  "No total MissingET object found in container with name FinalClus"); 
        } else {
          *m_MET_Ref_FinalClus_Ex    = finalClus->mpx();
          *m_MET_Ref_FinalClus_Ey    = finalClus->mpy();
          *m_MET_Ref_FinalClus_EtSum = finalClus->sumet();
        }

        const MissingET* finalTrk = (*missingEtContainer)["FinalTrk"]; 
        if (!finalTrk) {
          ATH_MSG_WARNING(  "No total MissingET object found in container with name FinalTrk"); 
        } else {
          *m_MET_Ref_FinalTrk_Ex    = finalTrk->mpx();
          *m_MET_Ref_FinalTrk_Ey    = finalTrk->mpy();
          *m_MET_Ref_FinalTrk_EtSum = finalTrk->sumet();
        }
        
      } else {
        m_isMissingEtAvailable = false;
        ATH_MSG_ERROR("COULD NOT RETRIVE MET!");       
      }

    } // IF



    if(m_LevelOfDetails > 4){
        // ACCESS TRIGGER INFORMATION
        const std::vector< xAOD::EventInfo::StreamTag > stags = p.streamTags();
        //std::vector< xAOD::EventInfo::StreamTag >::const_iterator itr = stags.begin();
        //std::vector< xAOD::EventInfo::StreamTag >::const_iterator end = stags.end();

/*      //THIS NEEDS TO MODIFY TO WORK ON xAODs
        // CHECK TRIGGER DECISIONS
        *m_EF_2mu10       = m_trigDec->isPassed("EF_2mu10");
        *m_EF_2mu10_loose = m_trigDec->isPassed("EF_2mu10_loose");
        *m_EF_2mu4        = m_trigDec->isPassed("EF_2mu4");
        *m_EF_2mu6        = m_trigDec->isPassed("EF_2mu6");
        *m_EF_3mu4        = m_trigDec->isPassed("EF_3mu4");
        *m_EF_3mu6        = m_trigDec->isPassed("EF_3mu6");
        *m_EF_4mu4        = m_trigDec->isPassed("EF_4mu4");
        *m_EF_mu10        = m_trigDec->isPassed("EF_mu10");
        *m_EF_mu10_loose  = m_trigDec->isPassed("EF_mu10_loose");
        *m_EF_mu10i_loose = m_trigDec->isPassed("EF_mu10i_loose");
        *m_EF_mu13        = m_trigDec->isPassed("EF_mu13");
        *m_EF_mu15        = m_trigDec->isPassed("EF_mu15");
        *m_EF_mu15i       = m_trigDec->isPassed("EF_mu15i");
        *m_EF_mu15i_medium= m_trigDec->isPassed("EF_mu15i_medium");
        *m_EF_mu18        = m_trigDec->isPassed("EF_mu18");
        *m_EF_mu18_medium = m_trigDec->isPassed("EF_mu18_medium");
        *m_EF_mu20        = m_trigDec->isPassed("EF_mu20");
        *m_EF_mu20_medium = m_trigDec->isPassed("EF_mu20_medium");
        *m_EF_mu20i       = m_trigDec->isPassed("EF_mu20i");
        *m_EF_mu20i_medium= m_trigDec->isPassed("EF_mu20i_medium");
        *m_EF_mu22        = m_trigDec->isPassed("EF_mu22");
        *m_EF_mu22_medium = m_trigDec->isPassed("EF_mu22_medium");
        *m_EF_mu24_medium = m_trigDec->isPassed("EF_mu24_medium");
        *m_EF_mu24_tight  = m_trigDec->isPassed("EF_mu24_tight");
        *m_EF_mu30_medium = m_trigDec->isPassed("EF_mu30_medium");
        *m_EF_mu30_tight  = m_trigDec->isPassed("EF_mu30_tight");
        *m_EF_mu4         = m_trigDec->isPassed("EF_mu4");
        *m_EF_mu4_mu6     = m_trigDec->isPassed("EF_mu4_mu6");
        *m_EF_mu40_slow   = m_trigDec->isPassed("EF_mu40_slow");
        *m_EF_mu6         = m_trigDec->isPassed("EF_mu6");

        // LEVEL 1 TRIGGERS
        *m_L1_MU0_EMPTY   = m_trigDec->isPassed("L1_MU0_EMPTY");
        *m_L1_MU0         = m_trigDec->isPassed("L1_MU0");
        *m_L1_MU4         = m_trigDec->isPassed("L1_MU4");
        *m_L1_MU6         = m_trigDec->isPassed("L1_MU6");
        *m_L1_MU10        = m_trigDec->isPassed("L1_MU10");
        *m_L1_MU11        = m_trigDec->isPassed("L1_MU11");
        *m_L1_MU15        = m_trigDec->isPassed("L1_MU15");
        *m_L1_MU20        = m_trigDec->isPassed("L1_MU20");
        *m_L1_MU40        = m_trigDec->isPassed("L1_MU40");
        *m_L1_MU0_COMM    = m_trigDec->isPassed("L1_MU0_COMM");
        *m_L1_2MU0        = m_trigDec->isPassed("L1_2MU0");
        *m_L1_2MU4        = m_trigDec->isPassed("L1_2MU4");
        *m_L1_2MU6        = m_trigDec->isPassed("L1_2MU6");
        *m_L1_2MU10       = m_trigDec->isPassed("L1_2MU10");
        *m_L1_2MU11       = m_trigDec->isPassed("L1_2MU11");
        *m_L1_2MU15       = m_trigDec->isPassed("L1_2MU15");
        *m_L1_2MU20       = m_trigDec->isPassed("L1_2MU20");
        *m_L1_3MU0        = m_trigDec->isPassed("L1_3MU0");
        *m_L1_3MU4        = m_trigDec->isPassed("L1_3MU4");
        *m_L1_3MU6        = m_trigDec->isPassed("L1_3MU6");
        *m_L1_4MU0        = m_trigDec->isPassed("L1_4MU0");
*/
    } // IF

    if(m_LevelOfDetails > 5){
        // STORE NUMBER OF VERTICES
        // RETRIEVE PRIMARY VERTICES
        if(m_storeGate->retrieve( m_pVertex, "VxPrimaryCandidate").isFailure()){
            ATH_MSG_WARNING("No VxPrimaryCandidate container found in TES");
        } // IF
        *m_nVtx           = m_pVertex ? m_pVertex->size() : -1;
    } // IF

    return StatusCode::SUCCESS;
} // TILEMUONEVENTFILLERTOOL::FILL
} // NAMESPACE D3PD
