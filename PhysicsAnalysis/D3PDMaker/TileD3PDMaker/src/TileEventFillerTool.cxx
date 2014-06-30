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
  : D3PD::BlockFillerTool<EventInfo>(type,name,parent),
m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
m_muTool("LumiBlockMuTool"){
//=========================================================================
// CONSTRUCTOR
    declareProperty("TrigDecisionTool", m_trigDec);
    declareProperty("LumiBlockMuTool",  m_muTool);
    declareProperty("LevelOfDetails",   m_LevelOfDetails);
} // TILEMUONEVENTFILLERTOOL

//===============================================
StatusCode TileEventFillerTool::initialize(){
//===============================================
// RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES

    // CHECK WHETHER BLOCKFILLERTOOL HAS BEEN INITIALIZED
    CHECK( D3PD::BlockFillerTool<EventInfo>::initialize() );

    // OUTPUT MESSAGING SERVICE
    
    ATH_MSG_DEBUG(" NOW IN TILEMUONEVENTFILLERTOOL::INITIALIZE() ");

    // RETRIEVE STOREGATE SERVICE
    if(service("StoreGateSvc",m_storeGate).isFailure()){
        ATH_MSG_ERROR("STOREGATE SERVICE NOT FOUND!");
        return StatusCode::FAILURE;
    } // IF

    // RETRIEVE TRIGGER DECISION TOOL
    if(m_LevelOfDetails > 3){
        if(!m_trigDec){
            ATH_MSG_WARNING("TRIGGER DECISIONS ARE NOT LOADED PROPERLY.");
            return StatusCode::FAILURE;
        } // IF
        CHECK( m_trigDec.retrieve() );
        CHECK( m_muTool.retrieve() );
    } // IF

    // SET WHICH MISSING ENERGY VARIABLES SHOULD BE ADDED TO D3PD
    if(m_LevelOfDetails > 4){
        b_missET_RefFinal = true;
    } // IF
    if(m_LevelOfDetails > 5){
        b_missETCalo_Base0 = true;
        b_missETCalo_LocHadTopo = true;
        b_missETCalo_Topo = true;
    } // IF
    if(m_LevelOfDetails > 6){
        b_missET_Base0 = true;
        b_missET_CellOut_Eflow = true;
        b_missET_CellOut_Eflow_Muid = true;
        b_missET_CellOut_Eflow_Muons = true;
        b_missET_CellOut_em = true;
        b_missET_Cryo = true;
        b_missET_LocHadTopo = true;
        b_missET_Muid = true;
        b_missET_Muid_Spectro = true;
        b_missET_Muid_Track = true;
        b_missET_Muon = true;
        b_missET_MuonBoy = true;
        b_missET_MuonBoy_Spectro = true;
        b_missET_MuonBoy_Track = true;
        b_missET_MuonMuid = true;
        b_missET_MuonMuons = true;
        b_missET_Muons = true;
        b_missET_Muons_Spectro = true;
        b_missET_Muons_Track = true;
        b_missET_RefEle = true;
        b_missET_RefEle_em = true;
        b_missET_RefFinal_em = true;
        b_missET_RefGamma = true;
        b_missET_RefGamma_em = true;
        b_missET_RefJet = true;
        b_missET_RefJet_em = true;
        b_missET_RefMuon = true;
        b_missET_RefMuon_Muid = true;
        b_missET_RefMuon_Track = true;
        b_missET_RefMuon_Track_Muid = true;
        b_missET_RefMuon_Track_em = true;
        b_missET_RefMuon_em = true;
        b_missET_RefMuons = true;
        b_missET_RefMuons_Track = true;
        b_missET_RefTau = true;
        b_missET_RefTau_em = true;
        b_missET_SoftJets = true;
        b_missET_SoftJets_em = true;
        b_missET_Topo = true;
        b_missET_Track = true;
        b_missET_Truth = true;
        b_missET_Truth_PileUp = true;
        b_missETTruth_Truth = true;
        b_missETTruth_Truth_PileUp = true;
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

    // NUMBER OF MUONS
    if(m_LevelOfDetails > 1){
        CHECK( addVariable( "number_staco",   m_NStaco_mu, "", 0 ) );
        CHECK( addVariable( "number_muid",    m_NMuid_mu,  "", 0 ) );
    } // IF

    // ELABORATE EVENT INFORMATION
    if(m_LevelOfDetails > 2){
     
        CHECK( addVariable( "timestamp_ns",   m_timestamp_ns) );
      
        CHECK( addVariable( "bcid",           m_bcid) );
        CHECK( addVariable( "detmask0",       m_detmask0) );
        CHECK( addVariable( "detmask1",       m_detmask1) );
    } // IF

    // TRIGGER DECISIONS
    if(m_LevelOfDetails > 3){
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

    // BASIC MISSING ENERGY
    if(m_LevelOfDetails > 4){
        CHECK( addVariable( "MET_RefFinal_Ex",               m_MET_RefFinal_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefFinal_Ey",               m_MET_RefFinal_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefFinal_EtSum",            m_MET_RefFinal_EtSum, "", 0.) );
    } // IF

    // CALORIMETRIC MISSING ENERGY
    if(m_LevelOfDetails > 5){
        CHECK( addVariable( "MET_Calo_Base0_Ex",             m_MET_Calo_Base0_Ex, "", 0.) );
        CHECK( addVariable( "MET_Calo_Base0_Ey",             m_MET_Calo_Base0_Ey, "", 0.) );
        CHECK( addVariable( "MET_Calo_Base0_EtSum",          m_MET_Calo_Base0_EtSum, "", 0.) );

        CHECK( addVariable( "MET_Calo_LocHadTopo_Ex",        m_MET_Calo_LocHadTopo_Ex, "", 0.) );
        CHECK( addVariable( "MET_Calo_LocHadTopo_Ey",        m_MET_Calo_LocHadTopo_Ey, "", 0.) );
        CHECK( addVariable( "MET_Calo_LocHadTopo_EtSum",     m_MET_Calo_LocHadTopo_EtSum, "", 0.) );

        CHECK( addVariable( "MET_Calo_Topo_Ex",              m_MET_Calo_Topo_Ex, "", 0.) );
        CHECK( addVariable( "MET_Calo_Topo_Ey",              m_MET_Calo_Topo_Ey, "", 0.) );
        CHECK( addVariable( "MET_Calo_Topo_EtSum",           m_MET_Calo_Topo_EtSum, "", 0.) );
    } // IF

    // ELABORATE MISSING ENERGY
    if(m_LevelOfDetails > 6){
        CHECK( addVariable( "MET_Truth_Truth_Ex",            m_MET_Truth_Truth_Ex, "", 0.) );
        CHECK( addVariable( "MET_Truth_Truth_Ey",            m_MET_Truth_Truth_Ey, "", 0.) );
        CHECK( addVariable( "MET_Truth_Truth_EtSum",         m_MET_Truth_Truth_EtSum, "", 0.) );

        CHECK( addVariable( "MET_Truth_Truth_PileUp_Ex",     m_MET_Truth_Truth_PileUp_Ex, "", 0.) );
        CHECK( addVariable( "MET_Truth_Truth_PileUp_Ey",     m_MET_Truth_Truth_PileUp_Ey, "", 0.) );
        CHECK( addVariable( "MET_Truth_Truth_PileUp_EtSum",  m_MET_Truth_Truth_PileUp_EtSum, "", 0.) );

        CHECK( addVariable( "MET_Base0_Ex",                  m_MET_Base0_Ex, "", 0.) );
        CHECK( addVariable( "MET_Base0_Ey",                  m_MET_Base0_Ey, "", 0.) );
        CHECK( addVariable( "MET_Base0_EtSum",               m_MET_Base0_EtSum, "", 0.) );

        CHECK( addVariable( "MET_CellOut_Eflow_Ex",          m_MET_CellOut_Eflow_Ex, "", 0.) );
        CHECK( addVariable( "MET_CellOut_Eflow_Ey",          m_MET_CellOut_Eflow_Ey, "", 0.) );
        CHECK( addVariable( "MET_CellOut_Eflow_EtSum",       m_MET_CellOut_Eflow_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_CellOut_Eflow_Muid_Ex",     m_MET_CellOut_Eflow_Muid_Ex, "", 0.) );
        CHECK( addVariable( "MET_CellOut_Eflow_Muid_Ey",     m_MET_CellOut_Eflow_Muid_Ey, "", 0.) );
        CHECK( addVariable( "MET_CellOut_Eflow_Muid_EtSum",  m_MET_CellOut_Eflow_Muid_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_CellOut_Eflow_Muons_Ex",    m_MET_CellOut_Eflow_Muons_Ex, "", 0.) );
        CHECK( addVariable( "MET_CellOut_Eflow_Muons_Ey",    m_MET_CellOut_Eflow_Muons_Ey, "", 0.) );
        CHECK( addVariable( "MET_CellOut_Eflow_Muons_EtSum", m_MET_CellOut_Eflow_Muons_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_CellOut_em_Ex",             m_MET_CellOut_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_CellOut_em_Ey",             m_MET_CellOut_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_CellOut_em_EtSum",          m_MET_CellOut_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Cryo_Ex",                   m_MET_Cryo_Ex, "", 0.) );
        CHECK( addVariable( "MET_Cryo_Ey",                   m_MET_Cryo_Ey, "", 0.) );
        CHECK( addVariable( "MET_Cryo_EtSum",                m_MET_Cryo_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_LocHadTopo_Ex",             m_MET_LocHadTopo_Ex, "", 0.) );
        CHECK( addVariable( "MET_LocHadTopo_Ey",             m_MET_LocHadTopo_Ey, "", 0.) );
        CHECK( addVariable( "MET_LocHadTopo_EtSum",          m_MET_LocHadTopo_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Muid_Ex",                   m_MET_Muid_Ex, "", 0.) );
        CHECK( addVariable( "MET_Muid_Ey",                   m_MET_Muid_Ey, "", 0.) );
        CHECK( addVariable( "MET_Muid_EtSum",                m_MET_Muid_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Muid_Spectro_Ex",           m_MET_Muid_Spectro_Ex, "", 0.) );
        CHECK( addVariable( "MET_Muid_Spectro_Ey",           m_MET_Muid_Spectro_Ey, "", 0.) );
        CHECK( addVariable( "MET_Muid_Spectro_EtSum",        m_MET_Muid_Spectro_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Muid_Track_Ex",             m_MET_Muid_Track_Ex, "", 0.) );
        CHECK( addVariable( "MET_Muid_Track_Ey",             m_MET_Muid_Track_Ey, "", 0.) );
        CHECK( addVariable( "MET_Muid_Track_EtSum",          m_MET_Muid_Track_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Muon_Ex",                   m_MET_Muon_Ex, "", 0.) );
        CHECK( addVariable( "MET_Muon_Ey",                   m_MET_Muon_Ey, "", 0.) );
        CHECK( addVariable( "MET_Muon_EtSum",                m_MET_Muon_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_MuonBoy_Ex",                m_MET_MuonBoy_Ex, "", 0.) );
        CHECK( addVariable( "MET_MuonBoy_Ey",                m_MET_MuonBoy_Ey, "", 0.) );
        CHECK( addVariable( "MET_MuonBoy_EtSum",             m_MET_MuonBoy_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_MuonBoy_Spectro_Ex",        m_MET_MuonBoy_Spectro_Ex, "", 0.) );
        CHECK( addVariable( "MET_MuonBoy_Spectro_Ey",        m_MET_MuonBoy_Spectro_Ey, "", 0.) );
        CHECK( addVariable( "MET_MuonBoy_Spectro_EtSum",     m_MET_MuonBoy_Spectro_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_MuonBoy_Track_Ex",          m_MET_MuonBoy_Track_Ex, "", 0.) );
        CHECK( addVariable( "MET_MuonBoy_Track_Ey",          m_MET_MuonBoy_Track_Ey, "", 0.) );
        CHECK( addVariable( "MET_MuonBoy_Track_EtSum",       m_MET_MuonBoy_Track_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_MuonMuid_Ex",               m_MET_MuonMuid_Ex, "", 0.) );
        CHECK( addVariable( "MET_MuonMuid_Ey",               m_MET_MuonMuid_Ey, "", 0.) );
        CHECK( addVariable( "MET_MuonMuid_EtSum",            m_MET_MuonMuid_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_MuonMuons_Ex",              m_MET_MuonMuons_Ex, "", 0.) );
        CHECK( addVariable( "MET_MuonMuons_Ey",              m_MET_MuonMuons_Ey, "", 0.) );
        CHECK( addVariable( "MET_MuonMuons_EtSum",           m_MET_MuonMuons_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Muons_Ex",                  m_MET_Muons_Ex, "", 0.) );
        CHECK( addVariable( "MET_Muons_Ey",                  m_MET_Muons_Ey, "", 0.) );
        CHECK( addVariable( "MET_Muons_EtSum",               m_MET_Muons_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Muons_Spectro_Ex",          m_MET_Muons_Spectro_Ex, "", 0.) );
        CHECK( addVariable( "MET_Muons_Spectro_Ey",          m_MET_Muons_Spectro_Ey, "", 0.) );
        CHECK( addVariable( "MET_Muons_Spectro_EtSum",       m_MET_Muons_Spectro_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Muons_Track_Ex",            m_MET_Muons_Track_Ex, "", 0.) );
        CHECK( addVariable( "MET_Muons_Track_Ey",            m_MET_Muons_Track_Ey, "", 0.) );
        CHECK( addVariable( "MET_Muons_Track_EtSum",         m_MET_Muons_Track_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefEle_Ex",                 m_MET_RefEle_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefEle_Ey",                 m_MET_RefEle_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefEle_EtSum",              m_MET_RefEle_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefEle_em_Ex",              m_MET_RefEle_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefEle_em_Ey",              m_MET_RefEle_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefEle_em_EtSum",           m_MET_RefEle_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefFinal_em_Ex",            m_MET_RefFinal_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefFinal_em_Ey",            m_MET_RefFinal_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefFinal_em_EtSum",         m_MET_RefFinal_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefGamma_Ex",               m_MET_RefGamma_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefGamma_Ey",               m_MET_RefGamma_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefGamma_EtSum",            m_MET_RefGamma_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefGamma_em_Ex",            m_MET_RefGamma_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefGamma_em_Ey",            m_MET_RefGamma_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefGamma_em_EtSum",         m_MET_RefGamma_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefJet_Ex",                 m_MET_RefJet_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefJet_Ey",                 m_MET_RefJet_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefJet_EtSum",              m_MET_RefJet_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefJet_em_Ex",              m_MET_RefJet_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefJet_em_Ey",              m_MET_RefJet_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefJet_em_EtSum",           m_MET_RefJet_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefMuon_Ex",                m_MET_RefMuon_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Ey",                m_MET_RefMuon_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_EtSum",             m_MET_RefMuon_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefMuon_Muid_Ex",           m_MET_RefMuon_Muid_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Muid_Ey",           m_MET_RefMuon_Muid_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Muid_EtSum",        m_MET_RefMuon_Muid_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefMuon_Track_Ex",          m_MET_RefMuon_Track_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Track_Ey",          m_MET_RefMuon_Track_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Track_EtSum",       m_MET_RefMuon_Track_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefMuon_Track_Muid_Ex",     m_MET_RefMuon_Track_Muid_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Track_Muid_Ey",     m_MET_RefMuon_Track_Muid_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Track_Muid_EtSum",  m_MET_RefMuon_Track_Muid_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefMuon_Track_em_Ex",       m_MET_RefMuon_Track_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Track_em_Ey",       m_MET_RefMuon_Track_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_Track_em_EtSum",    m_MET_RefMuon_Track_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefMuon_em_Ex",             m_MET_RefMuon_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_em_Ey",             m_MET_RefMuon_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefMuon_em_EtSum",          m_MET_RefMuon_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefMuons_Ex",               m_MET_RefMuons_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefMuons_Ey",               m_MET_RefMuons_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefMuons_EtSum",            m_MET_RefMuons_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefMuons_Track_Ex",         m_MET_RefMuons_Track_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefMuons_Track_Ey",         m_MET_RefMuons_Track_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefMuons_Track_EtSum",      m_MET_RefMuons_Track_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefTau_Ex",                 m_MET_RefTau_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefTau_Ey",                 m_MET_RefTau_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefTau_EtSum",              m_MET_RefTau_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_RefTau_em_Ex",              m_MET_RefTau_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_RefTau_em_Ey",              m_MET_RefTau_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_RefTau_em_EtSum",           m_MET_RefTau_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_SoftJets_Ex",               m_MET_SoftJets_Ex, "", 0.) );
        CHECK( addVariable( "MET_SoftJets_Ey",               m_MET_SoftJets_Ey, "", 0.) );
        CHECK( addVariable( "MET_SoftJets_EtSum",            m_MET_SoftJets_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_SoftJets_em_Ex",            m_MET_SoftJets_em_Ex, "", 0.) );
        CHECK( addVariable( "MET_SoftJets_em_Ey",            m_MET_SoftJets_em_Ey, "", 0.) );
        CHECK( addVariable( "MET_SoftJets_em_EtSum",         m_MET_SoftJets_em_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Topo_Ex",                   m_MET_Topo_Ex, "", 0.) );
        CHECK( addVariable( "MET_Topo_Ey",                   m_MET_Topo_Ey, "", 0.) );
        CHECK( addVariable( "MET_Topo_EtSum",                m_MET_Topo_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Track_Ex",                  m_MET_Track_Ex, "", 0.) );
        CHECK( addVariable( "MET_Track_Ey",                  m_MET_Track_Ey, "", 0.) );
        CHECK( addVariable( "MET_Track_EtSum",               m_MET_Track_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Truth_Ex",                  m_MET_Truth_Ex, "", 0.) );
        CHECK( addVariable( "MET_Truth_Ey",                  m_MET_Truth_Ey, "", 0.) );
        CHECK( addVariable( "MET_Truth_EtSum",               m_MET_Truth_EtSum, "", 0.) );
         
        CHECK( addVariable( "MET_Truth_PileUp_Ex",           m_MET_Truth_PileUp_Ex, "", 0.) );
        CHECK( addVariable( "MET_Truth_PileUp_Ey",           m_MET_Truth_PileUp_Ey, "", 0.) );
        CHECK( addVariable( "MET_Truth_PileUp_EtSum",        m_MET_Truth_PileUp_EtSum, "", 0.) );
    } // IF

    // VERTEXING AND PILEUP
    if(m_LevelOfDetails > 7){
        CHECK( addVariable( "nVtx",           m_nVtx,      "", 0 ) );
    
    } // IF

    // INVARIANT MASS
    if(m_LevelOfDetails > 8){
        CHECK( addVariable( "mass_muid",      m_mass_muid,          "", -99.) );
        CHECK( addVariable( "mass_staco",     m_mass_staco,         "", -99.) );
    } // IF

    return StatusCode::SUCCESS;
} // TILEMUONEVENTFILLERTOOL::BOOK

//===========================================================
StatusCode TileEventFillerTool::fill(const EventInfo& p){
//===========================================================
// FILLS D3PD TREE EVENT-BY-EVENT

    // OUTPUT MESSAGING SERVICE
    
    ATH_MSG_DEBUG(" in TileEventFillerTool::fill ");

    // GENERAL EVENT IDENTIFICATION INFORMATION
    if(m_LevelOfDetails > 0){
        // EVENT ID
        const EventID& id = *p.event_ID();
        *m_run_number     = id.run_number();
        *m_event_number   = id.event_number();
        *m_timestamp      = id.time_stamp();
	*m_is_simulation  = p.event_type()->test( EventType::IS_SIMULATION );
        *m_is_calibration = p.event_type()->test( EventType::IS_CALIBRATION );
        *m_is_testbeam    = p.event_type()->test( EventType::IS_TESTBEAM );
	*m_lbn          = id.lumi_block();
	 *m_actualInteractionPerCrossing  = m_muTool->actualInteractionsPerCrossing();
        *m_averageInteractionPerCrossing = m_muTool->averageInteractionsPerCrossing(); 
    } // IF

    // ELABORATE EVENT INFORMATION
    if(m_LevelOfDetails > 2){
        // STORE EVENT TYPE
       
        const EventID& id = *p.event_ID();
        *m_timestamp_ns = id.time_stamp_ns_offset();
       
        *m_bcid         = id.bunch_crossing_id();
        *m_detmask0     = id.detector_mask0();
        *m_detmask1     = id.detector_mask1();
    } // IF

    if(m_LevelOfDetails > 3){
        // ACCESS TRIGGER INFORMATION
        const std::vector< TriggerInfo::StreamTag > stags = p.trigger_info()->streamTags();
        //std::vector< TriggerInfo::StreamTag >::const_iterator itr = stags.begin();
        //std::vector< TriggerInfo::StreamTag >::const_iterator end = stags.end();

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
    } // IF

    //======================//
    // STORE MISSING ENERGY //
    //======================//
    // MISSING ENERGY POINTER RETRIEVAL: EACH TIME IT IS CHECKED WHETHER THE MISSING ENERGY VARIABLE
    // COULD BE RETRIEVED FROM THE STOREGATE. THEN THE TRANSVERSE ENERGIES IN X AND Y AND THE SUMMED
    // MISSING ENERGY ARE STORED.
    if(m_LevelOfDetails > 4){
        if(b_missET_RefFinal && m_storeGate->retrieve(m_missET_RefFinal,"MET_RefFinal").isFailure())
            b_missET_RefFinal = false;

        if(m_missET_RefFinal && b_missET_RefFinal){
            *m_MET_RefFinal_Ex    = m_missET_RefFinal ? m_missET_RefFinal->etx() : 0.;
            *m_MET_RefFinal_Ey    = m_missET_RefFinal ? m_missET_RefFinal->ety() : 0.;
            *m_MET_RefFinal_EtSum = m_missET_RefFinal ? m_missET_RefFinal->sumet() : 0.;
        } // IF
    } // IF

    if(m_LevelOfDetails > 5){
        if(b_missETCalo_Base0 && m_storeGate->retrieve(m_missETCalo_Base0,"MET_Base0").isFailure())
            b_missETCalo_Base0 = false;
        if(b_missETCalo_LocHadTopo && m_storeGate->retrieve(m_missETCalo_LocHadTopo,"MET_LocHadTopo").isFailure())
                b_missETCalo_LocHadTopo = false;
        if(b_missETCalo_Topo && m_storeGate->retrieve(m_missETCalo_Topo,"MET_Topo").isFailure())
            b_missETCalo_Topo = false;

        if(m_missETCalo_Base0 && b_missETCalo_Base0){
            *m_MET_Calo_Base0_Ex = m_missETCalo_Base0 ? m_missETCalo_Base0->etx() : 0.;
            *m_MET_Calo_Base0_Ey = m_missETCalo_Base0 ? m_missETCalo_Base0->ety() : 0.;
            *m_MET_Calo_Base0_EtSum = m_missETCalo_Base0 ? m_missETCalo_Base0->sumet() : 0.;
        } if(m_missETCalo_LocHadTopo && b_missETCalo_LocHadTopo){
            *m_MET_Calo_LocHadTopo_Ex = m_missETCalo_LocHadTopo ? m_missETCalo_LocHadTopo->etx() : 0.;
            *m_MET_Calo_LocHadTopo_Ey = m_missETCalo_LocHadTopo ? m_missETCalo_LocHadTopo->ety() : 0.;
            *m_MET_Calo_LocHadTopo_EtSum = m_missETCalo_LocHadTopo ? m_missETCalo_LocHadTopo->sumet() : 0.;
        } if(m_missETCalo_Topo && b_missETCalo_Topo){
            *m_MET_Calo_Topo_Ex = m_missETCalo_Topo ? m_missETCalo_Topo->etx() : 0.;
            *m_MET_Calo_Topo_Ey = m_missETCalo_Topo ? m_missETCalo_Topo->ety() : 0.;
            *m_MET_Calo_Topo_EtSum = m_missETCalo_Topo ? m_missETCalo_Topo->sumet() : 0.;
        } // IF
    } // IF

    if(m_LevelOfDetails > 6){
        if(b_missET_Base0 && m_storeGate->retrieve(m_missET_Base0,"MET_Base0").isFailure())
            b_missET_Base0 = false;
        if(b_missET_CellOut_Eflow && m_storeGate->retrieve(m_missET_CellOut_Eflow,"MET_CellOut_Eflow").isFailure())
            b_missET_CellOut_Eflow = false;
        if(b_missET_CellOut_Eflow_Muid && m_storeGate->retrieve(m_missET_CellOut_Eflow_Muid,"MET_CellOut_Eflow_Muid").isFailure())
            b_missET_CellOut_Eflow_Muid = false;
        if(b_missET_CellOut_Eflow_Muons && m_storeGate->retrieve(m_missET_CellOut_Eflow_Muons,"MET_CellOut_Eflow_Muons").isFailure())
            b_missET_CellOut_Eflow_Muons = false;
        if(b_missET_CellOut_em && m_storeGate->retrieve(m_missET_CellOut_em,"MET_CellOut_em").isFailure())
            b_missET_CellOut_em = false;
        if(b_missET_Cryo && m_storeGate->retrieve(m_missET_Cryo,"MET_Cryo").isFailure())
            b_missET_Cryo = false;
        if(b_missET_LocHadTopo && m_storeGate->retrieve(m_missET_LocHadTopo,"MET_LocHadTopo").isFailure())
            b_missET_LocHadTopo = false;
        if(b_missET_Muid && m_storeGate->retrieve(m_missET_Muid,"MET_Muid").isFailure())
            b_missET_Muid = false;
        if(b_missET_Muid_Spectro && m_storeGate->retrieve(m_missET_Muid_Spectro,"MET_Muid_Spectro").isFailure())
            b_missET_Muid_Spectro = false;
        if(b_missET_Muid_Track && m_storeGate->retrieve(m_missET_Muid_Track,"MET_Muid_Track").isFailure())
            b_missET_Muid_Track = false;
        if(b_missET_Muon && m_storeGate->retrieve(m_missET_Muon,"MET_Muon").isFailure())
            b_missET_Muon = false;
        if(b_missET_MuonBoy && m_storeGate->retrieve(m_missET_MuonBoy,"MET_MuonBoy").isFailure())
            b_missET_MuonBoy = false;
        if(b_missET_MuonBoy_Spectro && m_storeGate->retrieve(m_missET_MuonBoy_Spectro,"MET_MuonBoy_Spectro").isFailure())
            b_missET_MuonBoy_Spectro = false;
        if(b_missET_MuonBoy_Track && m_storeGate->retrieve(m_missET_MuonBoy_Track,"MET_MuonBoy_Track").isFailure())
            b_missET_MuonBoy_Track = false;
        if(b_missET_MuonMuid && m_storeGate->retrieve(m_missET_MuonMuid,"MET_MuonMuid").isFailure())
            b_missET_MuonMuid = false;
        if(b_missET_MuonMuons && m_storeGate->retrieve(m_missET_MuonMuons,"MET_MuonMuons").isFailure())
            b_missET_MuonMuons = false;
        if(b_missET_Muons && m_storeGate->retrieve(m_missET_Muons,"MET_Muons").isFailure())
            b_missET_Muons = false;
        if(b_missET_Muons_Spectro && m_storeGate->retrieve(m_missET_Muons_Spectro,"MET_Muons_Spectro").isFailure())
            b_missET_Muons_Spectro = false;
        if(b_missET_Muons_Track && m_storeGate->retrieve(m_missET_Muons_Track,"MET_Muons_Track").isFailure())
            b_missET_Muons_Track = false;
        if(b_missET_RefEle && m_storeGate->retrieve(m_missET_RefEle,"MET_RefEle").isFailure())
            b_missET_RefEle = false;
        if(b_missET_RefEle_em && m_storeGate->retrieve(m_missET_RefEle_em,"MET_RefEle_em").isFailure())
            b_missET_RefEle_em = false;
        if(b_missET_RefFinal_em && m_storeGate->retrieve(m_missET_RefFinal_em,"MET_RefFinal_em").isFailure())
            b_missET_RefFinal_em = false;
        if(b_missET_RefGamma && m_storeGate->retrieve(m_missET_RefGamma,"MET_RefGamma").isFailure())
            b_missET_RefGamma = false;
        if(b_missET_RefGamma_em && m_storeGate->retrieve(m_missET_RefGamma_em,"MET_RefGamma_em").isFailure())
            b_missET_RefGamma_em = false;
        if(b_missET_RefJet && m_storeGate->retrieve(m_missET_RefJet,"MET_RefJet").isFailure())
            b_missET_RefJet = false;
        if(b_missET_RefJet_em && m_storeGate->retrieve(m_missET_RefJet_em,"MET_RefJet_em").isFailure())
            b_missET_RefJet_em = false;
        if(b_missET_RefMuon && m_storeGate->retrieve(m_missET_RefMuon,"MET_RefMuon").isFailure())
            b_missET_RefMuon = false;
        if(b_missET_RefMuon_Muid && m_storeGate->retrieve(m_missET_RefMuon_Muid,"MET_RefMuon_Muid").isFailure())
            b_missET_RefMuon_Muid = false;
        if(b_missET_RefMuon_Track && m_storeGate->retrieve(m_missET_RefMuon_Track,"MET_RefMuon_Track").isFailure())
            b_missET_RefMuon_Track = false;
        if(b_missET_RefMuon_Track_Muid && m_storeGate->retrieve(m_missET_RefMuon_Track_Muid,"MET_RefMuon_Track_Muid").isFailure())
            b_missET_RefMuon_Track_Muid = false;
        if(b_missET_RefMuon_Track_em && m_storeGate->retrieve(m_missET_RefMuon_Track_em,"MET_RefMuon_Track_em").isFailure())
            b_missET_RefMuon_Track_em = false;
        if(b_missET_RefMuon_em && m_storeGate->retrieve(m_missET_RefMuon_em,"MET_RefMuon_em").isFailure())
            b_missET_RefMuon_em = false;
        if(b_missET_RefMuons && m_storeGate->retrieve(m_missET_RefMuons,"MET_RefMuons").isFailure())
            b_missET_RefMuons = false;
        if(b_missET_RefMuons_Track && m_storeGate->retrieve(m_missET_RefMuons_Track,"MET_RefMuons_Track").isFailure())
            b_missET_RefMuons_Track = false;
        if(b_missET_RefTau && m_storeGate->retrieve(m_missET_RefTau,"MET_RefTau").isFailure())
            b_missET_RefTau = false;
        if(b_missET_RefTau_em && m_storeGate->retrieve(m_missET_RefTau_em,"MET_RefTau_em").isFailure())
            b_missET_RefTau_em = false;
        if(b_missET_SoftJets && m_storeGate->retrieve(m_missET_SoftJets,"MET_SoftJets").isFailure())
            b_missET_SoftJets = false;
        if(b_missET_SoftJets_em && m_storeGate->retrieve(m_missET_SoftJets_em,"MET_SoftJets_em").isFailure())
            b_missET_SoftJets_em = false;
        if(b_missET_Topo && m_storeGate->retrieve(m_missET_Topo,"MET_Topo").isFailure())
            b_missET_Topo = false;
        if(b_missET_Track && m_storeGate->retrieve(m_missET_Track,"MET_Track").isFailure())
            b_missET_Track = false;
        if(b_missET_Truth && m_storeGate->retrieve(m_missET_Truth,"MET_Truth").isFailure())
            b_missET_Truth = false;
        if(b_missET_Truth_PileUp && m_storeGate->retrieve(m_missET_Truth_PileUp,"MET_Truth_PileUp").isFailure())
            b_missET_Truth_PileUp = false;
        if(b_missETTruth_Truth && m_storeGate->retrieve(m_missETTruth_Truth,"MET_Truth").isFailure())
            b_missETTruth_Truth = false;
        if(b_missETTruth_Truth_PileUp && m_storeGate->retrieve(m_missETTruth_Truth_PileUp,"MET_Truth_PileUp").isFailure())
            b_missETTruth_Truth_PileUp = false;

        if(m_missETTruth_Truth && b_missETTruth_Truth){
            *m_MET_Truth_Truth_Ex = m_missETTruth_Truth ? m_missETTruth_Truth->etx() : 0.;
            *m_MET_Truth_Truth_Ey = m_missETTruth_Truth ? m_missETTruth_Truth->ety() : 0.;
            *m_MET_Truth_Truth_EtSum = m_missETTruth_Truth ? m_missETTruth_Truth->sumet() : 0.;
        } if(m_missETTruth_Truth_PileUp && b_missETTruth_Truth_PileUp){
            *m_MET_Truth_Truth_PileUp_Ex = m_missETTruth_Truth_PileUp ? m_missETTruth_Truth_PileUp->etx() : 0.;
            *m_MET_Truth_Truth_PileUp_Ey = m_missETTruth_Truth_PileUp ? m_missETTruth_Truth_PileUp->ety() : 0.;
            *m_MET_Truth_Truth_PileUp_EtSum = m_missETTruth_Truth_PileUp ? m_missETTruth_Truth_PileUp->sumet() : 0.;
        } if(m_missET_Base0 && b_missET_Base0){
            *m_MET_Base0_Ex = m_missET_Base0 ? m_missET_Base0->etx() : 0.;
            *m_MET_Base0_Ey = m_missET_Base0 ? m_missET_Base0->ety() : 0.;
            *m_MET_Base0_EtSum = m_missET_Base0 ? m_missET_Base0->sumet() : 0.;
        } if(m_missET_CellOut_Eflow && b_missET_CellOut_Eflow){
            *m_MET_CellOut_Eflow_Ex = m_missET_CellOut_Eflow ? m_missET_CellOut_Eflow->etx() : 0.;
            *m_MET_CellOut_Eflow_Ey = m_missET_CellOut_Eflow ? m_missET_CellOut_Eflow->ety() : 0.;
            *m_MET_CellOut_Eflow_EtSum = m_missET_CellOut_Eflow ? m_missET_CellOut_Eflow->sumet() : 0.;
        } if(m_missET_CellOut_Eflow_Muid && b_missET_CellOut_Eflow_Muid){
            *m_MET_CellOut_Eflow_Muid_Ex = m_missET_CellOut_Eflow_Muid ? m_missET_CellOut_Eflow_Muid->etx() : 0.;
            *m_MET_CellOut_Eflow_Muid_Ey = m_missET_CellOut_Eflow_Muid ? m_missET_CellOut_Eflow_Muid->ety() : 0.;
            *m_MET_CellOut_Eflow_Muid_EtSum = m_missET_CellOut_Eflow_Muid ? m_missET_CellOut_Eflow_Muid->sumet() : 0.;
        } if(m_missET_CellOut_Eflow_Muons && b_missET_CellOut_Eflow_Muons){
            *m_MET_CellOut_Eflow_Muons_Ex = m_missET_CellOut_Eflow_Muons ? m_missET_CellOut_Eflow_Muons->etx() : 0.;
            *m_MET_CellOut_Eflow_Muons_Ey = m_missET_CellOut_Eflow_Muons ? m_missET_CellOut_Eflow_Muons->ety() : 0.;
            *m_MET_CellOut_Eflow_Muons_EtSum = m_missET_CellOut_Eflow_Muons ? m_missET_CellOut_Eflow_Muons->sumet() : 0.;
        } if(m_missET_CellOut_em && b_missET_CellOut_em){
            *m_MET_CellOut_em_Ex = m_missET_CellOut_em ? m_missET_CellOut_em->etx() : 0.;
            *m_MET_CellOut_em_Ey = m_missET_CellOut_em ? m_missET_CellOut_em->ety() : 0.;
            *m_MET_CellOut_em_EtSum = m_missET_CellOut_em ? m_missET_CellOut_em->sumet() : 0.;
        } if(m_missET_Cryo && b_missET_Cryo){
            *m_MET_Cryo_Ex = m_missET_Cryo ? m_missET_Cryo->etx() : 0.;
            *m_MET_Cryo_Ey = m_missET_Cryo ? m_missET_Cryo->ety() : 0.;
            *m_MET_Cryo_EtSum = m_missET_Cryo ? m_missET_Cryo->sumet() : 0.;
        } if(m_missET_LocHadTopo && b_missET_LocHadTopo){
            *m_MET_LocHadTopo_Ex = m_missET_LocHadTopo ? m_missET_LocHadTopo->etx() : 0.;
            *m_MET_LocHadTopo_Ey = m_missET_LocHadTopo ? m_missET_LocHadTopo->ety() : 0.;
            *m_MET_LocHadTopo_EtSum = m_missET_LocHadTopo ? m_missET_LocHadTopo->sumet() : 0.;
        } if(m_missET_Muid && b_missET_Muid){
            *m_MET_Muid_Ex = m_missET_Muid ? m_missET_Muid->etx() : 0.;
            *m_MET_Muid_Ey = m_missET_Muid ? m_missET_Muid->ety() : 0.;
            *m_MET_Muid_EtSum = m_missET_Muid ? m_missET_Muid->sumet() : 0.;
        } if(m_missET_Muid_Spectro && b_missET_Muid_Spectro){
            *m_MET_Muid_Spectro_Ex = m_missET_Muid_Spectro ? m_missET_Muid_Spectro->etx() : 0.;
            *m_MET_Muid_Spectro_Ey = m_missET_Muid_Spectro ? m_missET_Muid_Spectro->ety() : 0.;
            *m_MET_Muid_Spectro_EtSum = m_missET_Muid_Spectro ? m_missET_Muid_Spectro->sumet() : 0.;
        } if(m_missET_Muid_Track && b_missET_Muid_Track){
            *m_MET_Muid_Track_Ex = m_missET_Muid_Track ? m_missET_Muid_Track->etx() : 0.;
            *m_MET_Muid_Track_Ey = m_missET_Muid_Track ? m_missET_Muid_Track->ety() : 0.;
            *m_MET_Muid_Track_EtSum = m_missET_Muid_Track ? m_missET_Muid_Track->sumet() : 0.;
        } if(m_missET_Muon && b_missET_Muon){
            *m_MET_Muon_Ex = m_missET_Muon ? m_missET_Muon->etx() : 0.;
            *m_MET_Muon_Ey = m_missET_Muon ? m_missET_Muon->ety() : 0.;
            *m_MET_Muon_EtSum = m_missET_Muon ? m_missET_Muon->sumet() : 0.;
        } if(m_missET_MuonBoy && b_missET_MuonBoy){
            *m_MET_MuonBoy_Ex = m_missET_MuonBoy ? m_missET_MuonBoy->etx() : 0.;
            *m_MET_MuonBoy_Ey = m_missET_MuonBoy ? m_missET_MuonBoy->ety() : 0.;
            *m_MET_MuonBoy_EtSum = m_missET_MuonBoy ? m_missET_MuonBoy->sumet() : 0.;
        } if(m_missET_MuonBoy_Spectro && b_missET_MuonBoy_Spectro){
            *m_MET_MuonBoy_Spectro_Ex = m_missET_MuonBoy_Spectro ? m_missET_MuonBoy_Spectro->etx() : 0.;
            *m_MET_MuonBoy_Spectro_Ey = m_missET_MuonBoy_Spectro ? m_missET_MuonBoy_Spectro->ety() : 0.;
            *m_MET_MuonBoy_Spectro_EtSum = m_missET_MuonBoy_Spectro ? m_missET_MuonBoy_Spectro->sumet() : 0.;
        } if(m_missET_MuonBoy_Track && b_missET_MuonBoy_Track){
            *m_MET_MuonBoy_Track_Ex = m_missET_MuonBoy_Track ? m_missET_MuonBoy_Track->etx() : 0.;
            *m_MET_MuonBoy_Track_Ey = m_missET_MuonBoy_Track ? m_missET_MuonBoy_Track->ety() : 0.;
            *m_MET_MuonBoy_Track_EtSum = m_missET_MuonBoy_Track ? m_missET_MuonBoy_Track->sumet() : 0.;
        } if(m_missET_MuonMuid && b_missET_MuonMuid){
            *m_MET_MuonMuid_Ex = m_missET_MuonMuid ? m_missET_MuonMuid->etx() : 0.;
            *m_MET_MuonMuid_Ey = m_missET_MuonMuid ? m_missET_MuonMuid->ety() : 0.;
            *m_MET_MuonMuid_EtSum = m_missET_MuonMuid ? m_missET_MuonMuid->sumet() : 0.;
        } if(m_missET_MuonMuons && b_missET_MuonMuons){
            *m_MET_MuonMuons_Ex = m_missET_MuonMuons ? m_missET_MuonMuons->etx() : 0.;
            *m_MET_MuonMuons_Ey = m_missET_MuonMuons ? m_missET_MuonMuons->ety() : 0.;
            *m_MET_MuonMuons_EtSum = m_missET_MuonMuons ? m_missET_MuonMuons->sumet() : 0.;
        } if(m_missET_Muons && b_missET_Muons){
            *m_MET_Muons_Ex = m_missET_Muons ? m_missET_Muons->etx() : 0.;
            *m_MET_Muons_Ey = m_missET_Muons ? m_missET_Muons->ety() : 0.;
            *m_MET_Muons_EtSum = m_missET_Muons ? m_missET_Muons->sumet() : 0.;
        } if(m_missET_Muons_Spectro && b_missET_Muons_Spectro){
            *m_MET_Muons_Spectro_Ex = m_missET_Muons_Spectro ? m_missET_Muons_Spectro->etx() : 0.;
            *m_MET_Muons_Spectro_Ey = m_missET_Muons_Spectro ? m_missET_Muons_Spectro->ety() : 0.;
            *m_MET_Muons_Spectro_EtSum = m_missET_Muons_Spectro ? m_missET_Muons_Spectro->sumet() : 0.;
        } if(m_missET_Muons_Track && b_missET_Muons_Track){
            *m_MET_Muons_Track_Ex = m_missET_Muons_Track ? m_missET_Muons_Track->etx() : 0.;
            *m_MET_Muons_Track_Ey = m_missET_Muons_Track ? m_missET_Muons_Track->ety() : 0.;
            *m_MET_Muons_Track_EtSum = m_missET_Muons_Track ? m_missET_Muons_Track->sumet() : 0.;
        } if(m_missET_RefEle && b_missET_RefEle){
            *m_MET_RefEle_Ex = m_missET_RefEle ? m_missET_RefEle->etx() : 0.;
            *m_MET_RefEle_Ey = m_missET_RefEle ? m_missET_RefEle->ety() : 0.;
            *m_MET_RefEle_EtSum = m_missET_RefEle ? m_missET_RefEle->sumet() : 0.;
        } if(m_missET_RefEle_em && b_missET_RefEle_em){
            *m_MET_RefEle_em_Ex = m_missET_RefEle_em ? m_missET_RefEle_em->etx() : 0.;
            *m_MET_RefEle_em_Ey = m_missET_RefEle_em ? m_missET_RefEle_em->ety() : 0.;
            *m_MET_RefEle_em_EtSum = m_missET_RefEle_em ? m_missET_RefEle_em->sumet() : 0.;
        } if(m_missET_RefFinal_em && b_missET_RefFinal_em){
            *m_MET_RefFinal_em_Ex = m_missET_RefFinal_em ? m_missET_RefFinal_em->etx() : 0.;
            *m_MET_RefFinal_em_Ey = m_missET_RefFinal_em ? m_missET_RefFinal_em->ety() : 0.;
            *m_MET_RefFinal_em_EtSum = m_missET_RefFinal_em ? m_missET_RefFinal_em->sumet() : 0.;
        } if(m_missET_RefGamma && b_missET_RefGamma){
            *m_MET_RefGamma_Ex = m_missET_RefGamma ? m_missET_RefGamma->etx() : 0.;
            *m_MET_RefGamma_Ey = m_missET_RefGamma ? m_missET_RefGamma->ety() : 0.;
            *m_MET_RefGamma_EtSum = m_missET_RefGamma ? m_missET_RefGamma->sumet() : 0.;
        } if(m_missET_RefGamma_em && b_missET_RefGamma_em){
            *m_MET_RefGamma_em_Ex = m_missET_RefGamma_em ? m_missET_RefGamma_em->etx() : 0.;
            *m_MET_RefGamma_em_Ey = m_missET_RefGamma_em ? m_missET_RefGamma_em->ety() : 0.;
            *m_MET_RefGamma_em_EtSum = m_missET_RefGamma_em ? m_missET_RefGamma_em->sumet() : 0.;
        } if(m_missET_RefJet && b_missET_RefJet){
            *m_MET_RefJet_Ex = m_missET_RefJet ? m_missET_RefJet->etx() : 0.;
            *m_MET_RefJet_Ey = m_missET_RefJet ? m_missET_RefJet->ety() : 0.;
            *m_MET_RefJet_EtSum = m_missET_RefJet ? m_missET_RefJet->sumet() : 0.;
        } if(m_missET_RefJet_em && b_missET_RefJet_em){
            *m_MET_RefJet_em_Ex = m_missET_RefJet_em ? m_missET_RefJet_em->etx() : 0.;
            *m_MET_RefJet_em_Ey = m_missET_RefJet_em ? m_missET_RefJet_em->ety() : 0.;
            *m_MET_RefJet_em_EtSum = m_missET_RefJet_em ? m_missET_RefJet_em->sumet() : 0.;
        } if(m_missET_RefMuon && b_missET_RefMuon){
            *m_MET_RefMuon_Ex = m_missET_RefMuon ? m_missET_RefMuon->etx() : 0.;
            *m_MET_RefMuon_Ey = m_missET_RefMuon ? m_missET_RefMuon->ety() : 0.;
            *m_MET_RefMuon_EtSum = m_missET_RefMuon ? m_missET_RefMuon->sumet() : 0.;
        } if(m_missET_RefMuon_Muid && b_missET_RefMuon_Muid){
            *m_MET_RefMuon_Muid_Ex = m_missET_RefMuon_Muid ? m_missET_RefMuon_Muid->etx() : 0.;
            *m_MET_RefMuon_Muid_Ey = m_missET_RefMuon_Muid ? m_missET_RefMuon_Muid->ety() : 0.;
            *m_MET_RefMuon_Muid_EtSum = m_missET_RefMuon_Muid ? m_missET_RefMuon_Muid->sumet() : 0.;
        } if(m_missET_RefMuon_Track && b_missET_RefMuon_Track){
            *m_MET_RefMuon_Track_Ex = m_missET_RefMuon_Track ? m_missET_RefMuon_Track->etx() : 0.;
            *m_MET_RefMuon_Track_Ey = m_missET_RefMuon_Track ? m_missET_RefMuon_Track->ety() : 0.;
            *m_MET_RefMuon_Track_EtSum = m_missET_RefMuon_Track ? m_missET_RefMuon_Track->sumet() : 0.;
        } if(m_missET_RefMuon_Track_Muid && b_missET_RefMuon_Track_Muid){
            *m_MET_RefMuon_Track_Muid_Ex = m_missET_RefMuon_Track_Muid ? m_missET_RefMuon_Track_Muid->etx() : 0.;
            *m_MET_RefMuon_Track_Muid_Ey = m_missET_RefMuon_Track_Muid ? m_missET_RefMuon_Track_Muid->ety() : 0.;
            *m_MET_RefMuon_Track_Muid_EtSum = m_missET_RefMuon_Track_Muid ? m_missET_RefMuon_Track_Muid->sumet() : 0.;
        } if(m_missET_RefMuon_Track_em && b_missET_RefMuon_Track_em){
            *m_MET_RefMuon_Track_em_Ex = m_missET_RefMuon_Track_em ? m_missET_RefMuon_Track_em->etx() : 0.;
            *m_MET_RefMuon_Track_em_Ey = m_missET_RefMuon_Track_em ? m_missET_RefMuon_Track_em->ety() : 0.;
            *m_MET_RefMuon_Track_em_EtSum = m_missET_RefMuon_Track_em ? m_missET_RefMuon_Track_em->sumet() : 0.;
        } if(m_missET_RefMuon_em && b_missET_RefMuon_em){
            *m_MET_RefMuon_em_Ex = m_missET_RefMuon_em ? m_missET_RefMuon_em->etx() : 0.;
            *m_MET_RefMuon_em_Ey = m_missET_RefMuon_em ? m_missET_RefMuon_em->ety() : 0.;
            *m_MET_RefMuon_em_EtSum = m_missET_RefMuon_em ? m_missET_RefMuon_em->sumet() : 0.;
        } if(m_missET_RefMuons && b_missET_RefMuons){
            *m_MET_RefMuons_Ex = m_missET_RefMuons ? m_missET_RefMuons->etx() : 0.;
            *m_MET_RefMuons_Ey = m_missET_RefMuons ? m_missET_RefMuons->ety() : 0.;
            *m_MET_RefMuons_EtSum = m_missET_RefMuons ? m_missET_RefMuons->sumet() : 0.;
        } if(m_missET_RefMuons_Track && b_missET_RefMuons_Track){
            *m_MET_RefMuons_Track_Ex = m_missET_RefMuons_Track ? m_missET_RefMuons_Track->etx() : 0.;
            *m_MET_RefMuons_Track_Ey = m_missET_RefMuons_Track ? m_missET_RefMuons_Track->ety() : 0.;
            *m_MET_RefMuons_Track_EtSum = m_missET_RefMuons_Track ? m_missET_RefMuons_Track->sumet() : 0.;
        } if(m_missET_RefTau && b_missET_RefTau){
            *m_MET_RefTau_Ex = m_missET_RefTau ? m_missET_RefTau->etx() : 0.;
            *m_MET_RefTau_Ey = m_missET_RefTau ? m_missET_RefTau->ety() : 0.;
            *m_MET_RefTau_EtSum = m_missET_RefTau ? m_missET_RefTau->sumet() : 0.;
        } if(m_missET_RefTau_em && b_missET_RefTau_em){
            *m_MET_RefTau_em_Ex = m_missET_RefTau_em ? m_missET_RefTau_em->etx() : 0.;
            *m_MET_RefTau_em_Ey = m_missET_RefTau_em ? m_missET_RefTau_em->ety() : 0.;
            *m_MET_RefTau_em_EtSum = m_missET_RefTau_em ? m_missET_RefTau_em->sumet() : 0.;
        } if(m_missET_SoftJets && b_missET_SoftJets){
            *m_MET_SoftJets_Ex = m_missET_SoftJets ? m_missET_SoftJets->etx() : 0.;
            *m_MET_SoftJets_Ey = m_missET_SoftJets ? m_missET_SoftJets->ety() : 0.;
            *m_MET_SoftJets_EtSum = m_missET_SoftJets ? m_missET_SoftJets->sumet() : 0.;
        } if(m_missET_SoftJets_em && b_missET_SoftJets_em){
            *m_MET_SoftJets_em_Ex = m_missET_SoftJets_em ? m_missET_SoftJets_em->etx() : 0.;
            *m_MET_SoftJets_em_Ey = m_missET_SoftJets_em ? m_missET_SoftJets_em->ety() : 0.;
            *m_MET_SoftJets_em_EtSum = m_missET_SoftJets_em ? m_missET_SoftJets_em->sumet() : 0.;
        } if(m_missET_Topo && b_missET_Topo){
            *m_MET_Topo_Ex = m_missET_Topo ? m_missET_Topo->etx() : 0.;
            *m_MET_Topo_Ey = m_missET_Topo ? m_missET_Topo->ety() : 0.;
            *m_MET_Topo_EtSum = m_missET_Topo ? m_missET_Topo->sumet() : 0.;
        } if(m_missET_Track && b_missET_Track){
            *m_MET_Track_Ex = m_missET_Track ? m_missET_Track->etx() : 0.;
            *m_MET_Track_Ey = m_missET_Track ? m_missET_Track->ety() : 0.;
            *m_MET_Track_EtSum = m_missET_Track ? m_missET_Track->sumet() : 0.;
        } if(m_missET_Truth && b_missET_Truth){
            *m_MET_Truth_Ex = m_missET_Truth ? m_missET_Truth->etx() : 0.;
            *m_MET_Truth_Ey = m_missET_Truth ? m_missET_Truth->ety() : 0.;
            *m_MET_Truth_EtSum = m_missET_Truth ? m_missET_Truth->sumet() : 0.;
        } if(m_missET_Truth_PileUp && b_missET_Truth_PileUp){
            *m_MET_Truth_PileUp_Ex = m_missET_Truth_PileUp ? m_missET_Truth_PileUp->etx() : 0.;
            *m_MET_Truth_PileUp_Ey = m_missET_Truth_PileUp ? m_missET_Truth_PileUp->ety() : 0.;
            *m_MET_Truth_PileUp_EtSum = m_missET_Truth_PileUp ? m_missET_Truth_PileUp->sumet() : 0.;
        } // IF
    } // IF

    if(m_LevelOfDetails > 7){
        // PILEUP INFORMATION
       

        // STORE NUMBER OF VERTICES
        // RETRIEVE PRIMARY VERTICES
        if(m_storeGate->retrieve( m_pVertex, "VxPrimaryCandidate").isFailure()){
            ATH_MSG_WARNING("No VxPrimaryCandidate container found in TES");
        } // IF
        *m_nVtx           = m_pVertex ? m_pVertex->size() : -1;
    } // IF

    if(m_LevelOfDetails > 8){
        // CALCULATE INVARIANT MASS IN THIS EVENT 
        // USING MISSING ENERGY AND MUON MOMENTUM
        // GET HANDLE ON MUON CONTAINER
        typedef Analysis::MuonContainer MUONCONTAINER;
        const MUONCONTAINER* muid_muons;
        const MUONCONTAINER* stac_muons;

        CHECK( m_storeGate->retrieve( muid_muons, "MuidMuonCollection" ) );
        CHECK( m_storeGate->retrieve( stac_muons, "StacoMuonCollection" ) );

        // DETERMINE NUMBER OF MUONS RETRIEVED FROM STOREGATE
        if(muid_muons) *m_NMuid_mu  = muid_muons->size();
        if(stac_muons) *m_NStaco_mu = stac_muons->size();

        // GET ITERATORS FOR EACH MUON TYPE
        std::vector<MUONCONTAINER::const_iterator> start;
        start.push_back(muid_muons->begin());
        start.push_back(stac_muons->begin());
        std::vector<MUONCONTAINER::const_iterator> stop;
        stop.push_back(muid_muons->end());
        stop.push_back(stac_muons->end());

        // LOOP OVER MUON TYPES (STACO/MUID)
        for(unsigned int i=0;i<start.size();++i){
            MUONCONTAINER::const_iterator f_mu = start[i];
            MUONCONTAINER::const_iterator l_mu = stop[i];
            std::vector<TLorentzVector> v_tracks;
            std::vector<TLorentzVector> v_muons;
            int muon_counter(0);

            // LOOP OVER MUONS
            for( ; f_mu != l_mu ; ++f_mu ){
                const Analysis::Muon* muon = ( *f_mu );

                // STORE MUON THE LORENTZ VECTOR
                v_tracks.push_back(TLorentzVector());
                v_tracks.at(muon_counter).SetPtEtaPhiE(muon->pt(),muon->eta(),muon->phi(),muon->p());
                v_muons.push_back(TLorentzVector(v_tracks.at(muon_counter).Px(),
                                                 v_tracks.at(muon_counter).Py(),0.,
                                                 v_tracks.at(muon_counter).Pt()));
                ++muon_counter;
            } // FOR (MUONS)

            // MISSING ENERGY
            if(m_missET_RefFinal){
                v_muons.push_back(TLorentzVector(*m_MET_RefFinal_Ex,*m_MET_RefFinal_Ey,0.,
                                                 sqrt((*m_MET_RefFinal_Ex)*(*m_MET_RefFinal_Ex)+
                                                      (*m_MET_RefFinal_Ey)*(*m_MET_RefFinal_Ey)) ));
            } // IF

            TLorentzVector sum;
            for(uint j=0; j<v_muons.size(); ++j){
                sum += v_muons.at(j);
            } // FOR (MUONS & MISSING ENERGY)
            if(i==0) *m_mass_muid  = sum.M();
            if(i==1) *m_mass_staco = sum.M();
        } // FOR (MUON TYPES)
    } // IF

    return StatusCode::SUCCESS;
} // TILEMUONEVENTFILLERTOOL::FILL
} // NAMESPACE D3PD
