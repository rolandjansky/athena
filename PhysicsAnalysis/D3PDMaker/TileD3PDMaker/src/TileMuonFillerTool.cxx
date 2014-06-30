/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileMuonFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Date: September 2012
 *
 */

// INCLUDE HEADER FILE
#include "TileMuonFillerTool.h"

// CALORIMETER INCLUDES
#include "TileEvent/TileCell.h"
#include "CaloEvent/CaloCellContainer.h"

// MONTE CARLO INCLUDES
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

// ADD TILEMUONTRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{
//=========================================================================
TileMuonFillerTool::TileMuonFillerTool (const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : D3PD::BlockFillerTool<Analysis::Muon> (type, name, parent),
  m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"),
  m_trackToVertexTool("Reco::TrackToVertex"){
//=========================================================================
// CONSTRUCTOR

    book().ignore(); // AVOID COVERITY WARNINGS 

    // TRACK EXTRAPOLATOR TOOL
    declareProperty("ExtrapolateInCaloTool", m_inCalo);
    declareProperty("TrackToVertexTool",     m_trackToVertexTool, "tool to map track to vertex");
    declareProperty("CellsName",             m_calocellContainerName="SelectedCells" );
    declareProperty("TruthInfo",             m_MCeventCollection="TruthEvent");
    declareProperty("DefaultValue",          m_defaultValue=0, "Default value to which to set unfilled variables.");
    declareProperty("LevelOfDetails",        m_LevelOfDetails = 1);
    declareProperty("TryStandAloneMuons",    m_standalone = "no");
    declareProperty("TrackType",             m_trackType = 0);
} // TileMuonFillerTool::TileMuonFillerTool

//==========================================
StatusCode TileMuonFillerTool::initialize(){
//==========================================

    ATH_MSG_DEBUG("in TileMuonFillerTool::initialize");
    CHECK( D3PD::BlockFillerTool<Analysis::Muon>::initialize() ); 

    // CHECK WHETHER STOREGATE CAN BE RETRIEVED
    StatusCode sc = StatusCode::SUCCESS; 

    sc = service("StoreGateSvc",m_storeGate);
    if(sc.isFailure()) {
        ATH_MSG_ERROR("StoreGate service not found !");
        return StatusCode::FAILURE;
    } // IF

    // MONTE CARLO EVENT COLLECTION
    truthRetrieved = m_storeGate->retrieve(m_MCtruth,m_MCeventCollection);
    if(truthRetrieved.isFailure()) if(m_LevelOfDetails > 6) ATH_MSG_ERROR("NO TRUTH INFORMATION");

    // RETRIEVE CALORIMETER EXTRAPOLATION TOOLS
    if(m_LevelOfDetails > 3){
        if(m_inCalo.retrieve().isFailure() ) {
            ATH_MSG_ERROR(" Cannot retrieve extrapolator in calorimeter properly. ");
        } // IF
    } // IF

    if(m_LevelOfDetails > 3){
        if(m_trackToVertexTool.retrieve().isFailure() ) {
            ATH_MSG_ERROR(" Cannot retrieve vertex tool properly. ");
        } // IF
    } // IF

    // INITIALISE THE CALOCELL FILTER ALGORITHM
    ATH_MSG_INFO(" NOW IN TileMuonFillerTool::initialize() ");

    return StatusCode::SUCCESS;
  } // TileMuonFillerTool::initialize

//====================================
StatusCode TileMuonFillerTool::book(){
//====================================

    // MUON HITS IN SUBDETECTORS
    if(m_LevelOfDetails > 2){
        // HIT NUMBERS IN SUBDETECTORS
        CHECK( addVariable("numberOfBLayerHits",               m_numberOfBLayerHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfPixelHits",                m_numberOfPixelHits,                "", m_defaultValue));
        CHECK( addVariable("numberOfSCTHits",                  m_numberOfSCTHits,                  "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHits",                  m_numberOfTRTHits,                  "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHighThresholdHits",     m_numberOfTRTHighThresholdHits,     "", m_defaultValue));
        CHECK( addVariable("numberOfBLayerSharedHits",         m_numberOfBLayerSharedHits,         "", m_defaultValue));
        CHECK( addVariable("numberOfPixelSharedHits",          m_numberOfPixelSharedHits,          "", m_defaultValue));
        CHECK( addVariable("numberOfPixelHoles",               m_numberOfPixelHoles,               "", m_defaultValue));
        CHECK( addVariable("numberOfSCTSharedHits",            m_numberOfSCTSharedHits,            "", m_defaultValue));
        CHECK( addVariable("numberOfSCTHoles",                 m_numberOfSCTHoles,                 "", m_defaultValue));
        CHECK( addVariable("numberOfTRTOutliers",              m_numberOfTRTOutliers,              "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHighThresholdOutliers", m_numberOfTRTHighThresholdOutliers, "", m_defaultValue));
        CHECK( addVariable("numberOfMDTHits",                  m_numberOfMDTHits,                  "", m_defaultValue));
        CHECK( addVariable("numberOfMDTHoles",                 m_numberOfMDTHoles,                 "", m_defaultValue));
        CHECK( addVariable("numberOfCSCEtaHits",               m_numberOfCSCEtaHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfCSCEtaHoles",              m_numberOfCSCEtaHoles,              "", m_defaultValue));
        CHECK( addVariable("numberOfCSCPhiHits",               m_numberOfCSCPhiHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfCSCPhiHoles",              m_numberOfCSCPhiHoles,              "", m_defaultValue));
        CHECK( addVariable("numberOfRPCEtaHits",               m_numberOfRPCEtaHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfRPCEtaHoles",              m_numberOfRPCEtaHoles,              "", m_defaultValue));
        CHECK( addVariable("numberOfRPCPhiHits",               m_numberOfRPCPhiHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfRPCPhiHoles",              m_numberOfRPCPhiHoles,              "", m_defaultValue));
        CHECK( addVariable("numberOfTGCEtaHits",               m_numberOfTGCEtaHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfTGCEtaHoles",              m_numberOfTGCEtaHoles,              "", m_defaultValue));
        CHECK( addVariable("numberOfTGCPhiHits",               m_numberOfTGCPhiHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfTGCPhiHoles",              m_numberOfTGCPhiHoles,              "", m_defaultValue));
        CHECK( addVariable("numberOfGangedPixels",             m_numberOfGangedPixels,             "", m_defaultValue));
        CHECK( addVariable("numberOfOutliersOnTrack",          m_numberOfOutliersOnTrack,          "", m_defaultValue));
        CHECK( addVariable("numberOfTrackSummaryTypes",        m_numberOfTrackSummaryTypes,        "", m_defaultValue));

        // ETA/PHI COORDINATES IN DIFFERENT TILE LAYERS
        // TILE1: LBA  (SAMPLING 12), EBA (SAMPLING 18)
        // TILE2: LBBC (SAMPLING 13), EBB (SAMPLING 19), GAP/CRACK SCINTILLATORS (SAMPLING 17), C10 (SAMPLING 15)
        // TILE3: LBD  (SAMPLING 14), EBD (SAMPLING 20), D4 CELLS (SAMPLING 16)
//         CHECK( addVariable("ps_eta",                           m_psEta,                            "", -9999.));
//         CHECK( addVariable("ps_phi",                           m_psPhi,                            "", -9999.));
//         CHECK( addVariable("ps_x",                             m_psX,                              "", -9999.));
//         CHECK( addVariable("ps_y",                             m_psY,                              "", -9999.));
//         CHECK( addVariable("ps_z",                             m_psZ,                              "", -9999.));
//         CHECK( addVariable("em1_eta",                          m_em1Eta,                           "", -9999.));
//         CHECK( addVariable("em1_phi",                          m_em1Phi,                           "", -9999.));
//         CHECK( addVariable("em1_x",                            m_em1X,                             "", -9999.));
//         CHECK( addVariable("em1_y",                            m_em1Y,                             "", -9999.));
//         CHECK( addVariable("em1_z",                            m_em1Z,                             "", -9999.));
//         CHECK( addVariable("em2_eta",                          m_em2Eta,                           "", -9999.));
//         CHECK( addVariable("em2_phi",                          m_em2Phi,                           "", -9999.));
//         CHECK( addVariable("em2_x",                            m_em2X,                             "", -9999.));
//         CHECK( addVariable("em2_y",                            m_em2Y,                             "", -9999.));
//         CHECK( addVariable("em2_z",                            m_em2Z,                             "", -9999.));
//         CHECK( addVariable("em3_eta",                          m_em3Eta,                           "", -9999.));
//         CHECK( addVariable("em3_phi",                          m_em3Phi,                           "", -9999.));
//         CHECK( addVariable("em3_x",                            m_em3X,                             "", -9999.));
//         CHECK( addVariable("em3_y",                            m_em3Y,                             "", -9999.));
//         CHECK( addVariable("em3_z",                            m_em3Z,                             "", -9999.));
//         CHECK( addVariable("tile1_eta",                        m_tile1Eta,                         "", -9999.));
//         CHECK( addVariable("tile1_phi",                        m_tile1Phi,                         "", -9999.));
//         CHECK( addVariable("tile1_x",                          m_tile1X,                           "", -9999.));
//         CHECK( addVariable("tile1_y",                          m_tile1Y,                           "", -9999.));
//         CHECK( addVariable("tile1_z",                          m_tile1Z,                           "", -9999.));
//         CHECK( addVariable("tile2_eta",                        m_tile2Eta,                         "", -9999.));
//         CHECK( addVariable("tile2_phi",                        m_tile2Phi,                         "", -9999.));
//         CHECK( addVariable("tile2_x",                          m_tile2X,                           "", -9999.));
//         CHECK( addVariable("tile2_y",                          m_tile2Y,                           "", -9999.));
//         CHECK( addVariable("tile2_z",                          m_tile2Z,                           "", -9999.));
//         CHECK( addVariable("tile3_eta",                        m_tile3Eta,                         "", -9999.));
//         CHECK( addVariable("tile3_phi",                        m_tile3Phi,                         "", -9999.));
//         CHECK( addVariable("tile3_x",                          m_tile3X,                           "", -9999.));
//         CHECK( addVariable("tile3_y",                          m_tile3Y,                           "", -9999.));
//         CHECK( addVariable("tile3_z",                          m_tile3Z,                           "", -9999.));
//         CHECK( addVariable("hec0_eta",                         m_hec0Eta,                          "", -9999.));
//         CHECK( addVariable("hec0_phi",                         m_hec0Phi,                          "", -9999.));
//         CHECK( addVariable("hec0_x",                           m_hec0X,                            "", -9999.));
//         CHECK( addVariable("hec0_y",                           m_hec0Y,                            "", -9999.));
//         CHECK( addVariable("hec0_z",                           m_hec0Z,                            "", -9999.));
//         CHECK( addVariable("hec1_eta",                         m_hec1Eta,                          "", -9999.));
//         CHECK( addVariable("hec1_phi",                         m_hec1Phi,                          "", -9999.));
//         CHECK( addVariable("hec1_x",                           m_hec1X,                            "", -9999.));
//         CHECK( addVariable("hec1_y",                           m_hec1Y,                            "", -9999.));
//         CHECK( addVariable("hec1_z",                           m_hec1Z,                            "", -9999.));
//         CHECK( addVariable("hec2_eta",                         m_hec2Eta,                          "", -9999.));
//         CHECK( addVariable("hec2_phi",                         m_hec2Phi,                          "", -9999.));
//         CHECK( addVariable("hec2_x",                           m_hec2X,                            "", -9999.));
//         CHECK( addVariable("hec2_y",                           m_hec2Y,                            "", -9999.));
//         CHECK( addVariable("hec2_z",                           m_hec2Z,                            "", -9999.));
//         CHECK( addVariable("hec3_eta",                         m_hec3Eta,                          "", -9999.));
//         CHECK( addVariable("hec3_phi",                         m_hec3Phi,                          "", -9999.));
//         CHECK( addVariable("hec3_x",                           m_hec3X,                            "", -9999.));
//         CHECK( addVariable("hec3_y",                           m_hec3Y,                            "", -9999.));
//         CHECK( addVariable("hec3_z",                           m_hec3Z,                            "", -9999.));

        CHECK( addVariable("PreSamplerB_X",                  m_PreSamplerB_X,                    "", -9999.));
        CHECK( addVariable("PreSamplerB_Y",                  m_PreSamplerB_Y,                    "", -9999.));
        CHECK( addVariable("PreSamplerB_Z",                  m_PreSamplerB_Z,                    "", -9999.));
        CHECK( addVariable("PreSamplerB_Eta",                m_PreSamplerB_Eta,                  "", -9999.));
        CHECK( addVariable("PreSamplerB_Phi",                m_PreSamplerB_Phi,                  "", -9999.));
        CHECK( addVariable("EMB1_X",                         m_EMB1_X,                           "", -9999.));
        CHECK( addVariable("EMB1_Y",                         m_EMB1_Y,                           "", -9999.));
        CHECK( addVariable("EMB1_Z",                         m_EMB1_Z,                           "", -9999.));
        CHECK( addVariable("EMB1_Eta",                       m_EMB1_Eta,                         "", -9999.));
        CHECK( addVariable("EMB1_Phi",                       m_EMB1_Phi,                         "", -9999.));
        CHECK( addVariable("EMB2_X",                         m_EMB2_X,                           "", -9999.));
        CHECK( addVariable("EMB2_Y",                         m_EMB2_Y,                           "", -9999.));
        CHECK( addVariable("EMB2_Z",                         m_EMB2_Z,                           "", -9999.));
        CHECK( addVariable("EMB2_Eta",                       m_EMB2_Eta,                         "", -9999.));
        CHECK( addVariable("EMB2_Phi",                       m_EMB2_Phi,                         "", -9999.));
        CHECK( addVariable("EMB3_X",                         m_EMB3_X,                           "", -9999.));
        CHECK( addVariable("EMB3_Y",                         m_EMB3_Y,                           "", -9999.));
        CHECK( addVariable("EMB3_Z",                         m_EMB3_Z,                           "", -9999.));
        CHECK( addVariable("EMB3_Eta",                       m_EMB3_Eta,                         "", -9999.));
        CHECK( addVariable("EMB3_Phi",                       m_EMB3_Phi,                         "", -9999.));
        CHECK( addVariable("PreSamplerE_X",                  m_PreSamplerE_X,                    "", -9999.));
        CHECK( addVariable("PreSamplerE_Y",                  m_PreSamplerE_Y,                    "", -9999.));
        CHECK( addVariable("PreSamplerE_Z",                  m_PreSamplerE_Z,                    "", -9999.));
        CHECK( addVariable("PreSamplerE_Eta",                m_PreSamplerE_Eta,                  "", -9999.));
        CHECK( addVariable("PreSamplerE_Phi",                m_PreSamplerE_Phi,                  "", -9999.));
        CHECK( addVariable("EME1_X",                         m_EME1_X,                           "", -9999.));
        CHECK( addVariable("EME1_Y",                         m_EME1_Y,                           "", -9999.));
        CHECK( addVariable("EME1_Z",                         m_EME1_Z,                           "", -9999.));
        CHECK( addVariable("EME1_Eta",                       m_EME1_Eta,                         "", -9999.));
        CHECK( addVariable("EME1_Phi",                       m_EME1_Phi,                         "", -9999.));
        CHECK( addVariable("EME2_X",                         m_EME2_X,                           "", -9999.));
        CHECK( addVariable("EME2_Y",                         m_EME2_Y,                           "", -9999.));
        CHECK( addVariable("EME2_Z",                         m_EME2_Z,                           "", -9999.));
        CHECK( addVariable("EME2_Eta",                       m_EME2_Eta,                         "", -9999.));
        CHECK( addVariable("EME2_Phi",                       m_EME2_Phi,                         "", -9999.));
        CHECK( addVariable("EME3_X",                         m_EME3_X,                           "", -9999.));
        CHECK( addVariable("EME3_Y",                         m_EME3_Y,                           "", -9999.));
        CHECK( addVariable("EME3_Z",                         m_EME3_Z,                           "", -9999.));
        CHECK( addVariable("EME3_Eta",                       m_EME3_Eta,                         "", -9999.));
        CHECK( addVariable("EME3_Phi",                       m_EME3_Phi,                         "", -9999.));
        CHECK( addVariable("HEC0_X",                         m_HEC0_X,                           "", -9999.));
        CHECK( addVariable("HEC0_Y",                         m_HEC0_Y,                           "", -9999.));
        CHECK( addVariable("HEC0_Z",                         m_HEC0_Z,                           "", -9999.));
        CHECK( addVariable("HEC0_Eta",                       m_HEC0_Eta,                         "", -9999.));
        CHECK( addVariable("HEC0_Phi",                       m_HEC0_Phi,                         "", -9999.));
        CHECK( addVariable("HEC1_X",                         m_HEC1_X,                           "", -9999.));
        CHECK( addVariable("HEC1_Y",                         m_HEC1_Y,                           "", -9999.));
        CHECK( addVariable("HEC1_Z",                         m_HEC1_Z,                           "", -9999.));
        CHECK( addVariable("HEC1_Eta",                       m_HEC1_Eta,                         "", -9999.));
        CHECK( addVariable("HEC1_Phi",                       m_HEC1_Phi,                         "", -9999.));
        CHECK( addVariable("HEC2_X",                         m_HEC2_X,                           "", -9999.));
        CHECK( addVariable("HEC2_Y",                         m_HEC2_Y,                           "", -9999.));
        CHECK( addVariable("HEC2_Z",                         m_HEC2_Z,                           "", -9999.));
        CHECK( addVariable("HEC2_Eta",                       m_HEC2_Eta,                         "", -9999.));
        CHECK( addVariable("HEC2_Phi",                       m_HEC2_Phi,                         "", -9999.));
        CHECK( addVariable("HEC3_X",                         m_HEC3_X,                           "", -9999.));
        CHECK( addVariable("HEC3_Y",                         m_HEC3_Y,                           "", -9999.));
        CHECK( addVariable("HEC3_Z",                         m_HEC3_Z,                           "", -9999.));
        CHECK( addVariable("HEC3_Eta",                       m_HEC3_Eta,                         "", -9999.));
        CHECK( addVariable("HEC3_Phi",                       m_HEC3_Phi,                         "", -9999.));
        CHECK( addVariable("TileBar0_X",                     m_TileBar0_X,                       "", -9999.));
        CHECK( addVariable("TileBar0_Y",                     m_TileBar0_Y,                       "", -9999.));
        CHECK( addVariable("TileBar0_Z",                     m_TileBar0_Z,                       "", -9999.));
        CHECK( addVariable("TileBar0_Eta",                   m_TileBar0_Eta,                     "", -9999.));
        CHECK( addVariable("TileBar0_Phi",                   m_TileBar0_Phi,                     "", -9999.));
        CHECK( addVariable("TileBar1_X",                     m_TileBar1_X,                       "", -9999.));
        CHECK( addVariable("TileBar1_Y",                     m_TileBar1_Y,                       "", -9999.));
        CHECK( addVariable("TileBar1_Z",                     m_TileBar1_Z,                       "", -9999.));
        CHECK( addVariable("TileBar1_Eta",                   m_TileBar1_Eta,                     "", -9999.));
        CHECK( addVariable("TileBar1_Phi",                   m_TileBar1_Phi,                     "", -9999.));
        CHECK( addVariable("TileBar2_X",                     m_TileBar2_X,                       "", -9999.));
        CHECK( addVariable("TileBar2_Y",                     m_TileBar2_Y,                       "", -9999.));
        CHECK( addVariable("TileBar2_Z",                     m_TileBar2_Z,                       "", -9999.));
        CHECK( addVariable("TileBar2_Eta",                   m_TileBar2_Eta,                     "", -9999.));
        CHECK( addVariable("TileBar2_Phi",                   m_TileBar2_Phi,                     "", -9999.));
        CHECK( addVariable("TileGap1_X",                     m_TileGap1_X,                       "", -9999.));
        CHECK( addVariable("TileGap1_Y",                     m_TileGap1_Y,                       "", -9999.));
        CHECK( addVariable("TileGap1_Z",                     m_TileGap1_Z,                       "", -9999.));
        CHECK( addVariable("TileGap1_Eta",                   m_TileGap1_Eta,                     "", -9999.));
        CHECK( addVariable("TileGap1_Phi",                   m_TileGap1_Phi,                     "", -9999.));
        CHECK( addVariable("TileGap2_X",                     m_TileGap2_X,                       "", -9999.));
        CHECK( addVariable("TileGap2_Y",                     m_TileGap2_Y,                       "", -9999.));
        CHECK( addVariable("TileGap2_Z",                     m_TileGap2_Z,                       "", -9999.));
        CHECK( addVariable("TileGap2_Eta",                   m_TileGap2_Eta,                     "", -9999.));
        CHECK( addVariable("TileGap2_Phi",                   m_TileGap2_Phi,                     "", -9999.));
        CHECK( addVariable("TileGap3_X",                     m_TileGap3_X,                       "", -9999.));
        CHECK( addVariable("TileGap3_Y",                     m_TileGap3_Y,                       "", -9999.));
        CHECK( addVariable("TileGap3_Z",                     m_TileGap3_Z,                       "", -9999.));
        CHECK( addVariable("TileGap3_Eta",                   m_TileGap3_Eta,                     "", -9999.));
        CHECK( addVariable("TileGap3_Phi",                   m_TileGap3_Phi,                     "", -9999.));
        CHECK( addVariable("TileExt0_X",                     m_TileExt0_X,                       "", -9999.));
        CHECK( addVariable("TileExt0_Y",                     m_TileExt0_Y,                       "", -9999.));
        CHECK( addVariable("TileExt0_Z",                     m_TileExt0_Z,                       "", -9999.));
        CHECK( addVariable("TileExt0_Eta",                   m_TileExt0_Eta,                     "", -9999.));
        CHECK( addVariable("TileExt0_Phi",                   m_TileExt0_Phi,                     "", -9999.));
        CHECK( addVariable("TileExt1_X",                     m_TileExt1_X,                       "", -9999.));
        CHECK( addVariable("TileExt1_Y",                     m_TileExt1_Y,                       "", -9999.));
        CHECK( addVariable("TileExt1_Z",                     m_TileExt1_Z,                       "", -9999.));
        CHECK( addVariable("TileExt1_Eta",                   m_TileExt1_Eta,                     "", -9999.));
        CHECK( addVariable("TileExt1_Phi",                   m_TileExt1_Phi,                     "", -9999.));
        CHECK( addVariable("TileExt2_X",                     m_TileExt2_X,                       "", -9999.));
        CHECK( addVariable("TileExt2_Y",                     m_TileExt2_Y,                       "", -9999.));
        CHECK( addVariable("TileExt2_Z",                     m_TileExt2_Z,                       "", -9999.));
        CHECK( addVariable("TileExt2_Eta",                   m_TileExt2_Eta,                     "", -9999.));
        CHECK( addVariable("TileExt2_Phi",                   m_TileExt2_Phi,                     "", -9999.));
    } // IF

    CHECK (book2());

    return StatusCode::SUCCESS;
} // TileMuonFillerTool::book


StatusCode TileMuonFillerTool::book2()
{

    // MUON TRACK PARAMETERS
    if(m_LevelOfDetails > 1){
        CHECK( addVariable("type",                             m_mutype,                           "", m_defaultValue));
        CHECK( addVariable("pT",                               m_pT,                               "", m_defaultValue));
        CHECK( addVariable("p",                                m_p,                                "", m_defaultValue));
        CHECK( addVariable("charge",                           m_charge,                           "", m_defaultValue));
        CHECK( addVariable("eta",                              m_eta,                              "", m_defaultValue));
        CHECK( addVariable("phi",                              m_phi,                              "", m_defaultValue));
        CHECK( addVariable("beta",                             m_beta,                             "", m_defaultValue));
    } // IF

    if(m_LevelOfDetails > 3){
        CHECK( addVariable("nucone10",                         m_nucone10,                         "", m_defaultValue));
        CHECK( addVariable("nucone20",                         m_nucone20,                         "", m_defaultValue));
        CHECK( addVariable("nucone30",                         m_nucone30,                         "", m_defaultValue));
        CHECK( addVariable("nucone40",                         m_nucone40,                         "", m_defaultValue));
        CHECK( addVariable("etcone10",                         m_etcone10,                         "", m_defaultValue));
        CHECK( addVariable("etcone20",                         m_etcone20,                         "", m_defaultValue));
        CHECK( addVariable("etcone30",                         m_etcone30,                         "", m_defaultValue));
        CHECK( addVariable("etcone40",                         m_etcone40,                         "", m_defaultValue));
        CHECK( addVariable("ptcone10",                         m_ptcone10,                         "", m_defaultValue));
        CHECK( addVariable("ptcone20",                         m_ptcone20,                         "", m_defaultValue));
        CHECK( addVariable("ptcone30",                         m_ptcone30,                         "", m_defaultValue));
        CHECK( addVariable("ptcone40",                         m_ptcone40,                         "", m_defaultValue));

        // ENERGY ISOLATION VARIABLES
        CHECK( addVariable("E_Tile_cone05",                    m_Tile_cone05,                      "", m_defaultValue));
        CHECK( addVariable("E_Tile_cone10",                    m_Tile_cone10,                      "", m_defaultValue));
        CHECK( addVariable("E_Tile_cone20",                    m_Tile_cone20,                      "", m_defaultValue));
        CHECK( addVariable("E_Tile_cone30",                    m_Tile_cone30,                      "", m_defaultValue));
        CHECK( addVariable("E_Tile_cone40",                    m_Tile_cone40,                      "", m_defaultValue));

        CHECK( addVariable("E_LAr_cone05",                     m_LAr_cone05,                       "", m_defaultValue));
        CHECK( addVariable("E_LAr_cone10",                     m_LAr_cone10,                       "", m_defaultValue));
        CHECK( addVariable("E_LAr_cone20",                     m_LAr_cone20,                       "", m_defaultValue));
        CHECK( addVariable("E_LAr_cone30",                     m_LAr_cone30,                       "", m_defaultValue));
        CHECK( addVariable("E_LAr_cone40",                     m_LAr_cone40,                       "", m_defaultValue));

        CHECK( addVariable("E_Tile_iso_cone05",                m_Tile_iso_cone05,                  "", m_defaultValue));
        CHECK( addVariable("E_Tile_iso_cone10",                m_Tile_iso_cone10,                  "", m_defaultValue));
        CHECK( addVariable("E_Tile_iso_cone20",                m_Tile_iso_cone20,                  "", m_defaultValue));
        CHECK( addVariable("E_Tile_iso_cone30",                m_Tile_iso_cone30,                  "", m_defaultValue));
        CHECK( addVariable("E_Tile_iso_cone40",                m_Tile_iso_cone40,                  "", m_defaultValue));

        CHECK( addVariable("E_LAr_iso_cone05",                 m_LAr_iso_cone05,                   "", m_defaultValue));
        CHECK( addVariable("E_LAr_iso_cone10",                 m_LAr_iso_cone10,                   "", m_defaultValue));
        CHECK( addVariable("E_LAr_iso_cone20",                 m_LAr_iso_cone20,                   "", m_defaultValue));
        CHECK( addVariable("E_LAr_iso_cone30",                 m_LAr_iso_cone30,                   "", m_defaultValue));
        CHECK( addVariable("E_LAr_iso_cone40",                 m_LAr_iso_cone40,                   "", m_defaultValue));

    } // IF

    if(m_LevelOfDetails > 3){
        CHECK( addVariable("vtxX",                             m_vtxX,                             "", m_defaultValue));
        CHECK( addVariable("vtxY",                             m_vtxY,                             "", m_defaultValue));
        CHECK( addVariable("vtxZ",                             m_vtxZ,                             "", m_defaultValue));
        CHECK( addVariable("d0",                               m_d0,                               "", m_defaultValue));
        CHECK( addVariable("z0",                               m_z0,                               "", m_defaultValue));
        CHECK( addVariable("phi0",                             m_phi0,                             "", m_defaultValue));
        CHECK( addVariable("theta",                            m_theta,                            "", m_defaultValue));
        CHECK( addVariable("qOverP",                           m_qOverP,                           "", m_defaultValue));
    } // IF

    if(m_LevelOfDetails > 5){
        CHECK( addVariable("matchChi2",                         m_matchChi2,                         "", m_defaultValue));
        CHECK( addVariable("matchChi2OverDoF",                  m_matchChi2OverDoF,                  "", m_defaultValue));
        CHECK( addVariable("matchNumberDoF",                    m_matchNumberDoF,                    "", m_defaultValue));
        CHECK( addVariable("fitChi2",                           m_fitChi2,                           "", m_defaultValue));
        CHECK( addVariable("fitNumberDoF",                      m_fitNumberDoF,                      "", m_defaultValue));
        CHECK( addVariable("fitChi2OverDoF",                    m_fitChi2OverDoF,                    "", m_defaultValue));
        CHECK( addVariable("bestMatch",                         m_bestMatch,                         "", m_defaultValue));
        CHECK( addVariable("isCorrected",                       m_isCorrected,                       "", m_defaultValue));
        CHECK( addVariable("hasCombinedMuon",                   m_hasCombinedMuon,                   "", m_defaultValue));
        CHECK( addVariable("hasInDetTrackParticle",             m_hasInDetTrackParticle,             "", m_defaultValue));
        CHECK( addVariable("hasMuonExtrapolatedTrackParticle",  m_hasMuonExtrapolatedTrackParticle,  "", m_defaultValue));
        CHECK( addVariable("hasInnerExtrapolatedTrackParticle", m_hasInnerExtrapolatedTrackParticle, "", m_defaultValue));
        CHECK( addVariable("hasCombinedMuonTrackParticle",      m_hasCombinedMuonTrackParticle,      "", m_defaultValue));
        CHECK( addVariable("hasCluster",                        m_hasCluster,                        "", m_defaultValue));
        CHECK( addVariable("isLoose",                           m_isLoose,                           "", m_defaultValue));
        CHECK( addVariable("isMedium",                          m_isMedium,                          "", m_defaultValue));
        CHECK( addVariable("isTight",                           m_isTight,                           "", m_defaultValue));
    } // IF

    if(m_LevelOfDetails > 6){
        if(truthRetrieved.isSuccess() && m_MCtruth){
            CHECK( addVariable("truth_deltaR",                 m_truth_deltaR,                     "", m_defaultValue));
            CHECK( addVariable("truth_vtxX",                   m_truth_vtxX,                       "", m_defaultValue));
            CHECK( addVariable("truth_vtxY",                   m_truth_vtxY,                       "", m_defaultValue));
            CHECK( addVariable("truth_vtxZ",                   m_truth_vtxZ,                       "", m_defaultValue));
            CHECK( addVariable("truth_eta",                    m_truth_eta,                        "", m_defaultValue));
            CHECK( addVariable("truth_phi",                    m_truth_phi,                        "", m_defaultValue));
            CHECK( addVariable("truth_p",                      m_truth_p,                          "", m_defaultValue));
            CHECK( addVariable("truth_pT",                     m_truth_pT,                         "", m_defaultValue));
            CHECK( addVariable("truth_charge",                 m_truth_charge,                     "", m_defaultValue));
            CHECK( addVariable("truth_mass",                   m_truth_mass,                       "", m_defaultValue));
        } // IF
    } // IF

    return StatusCode::SUCCESS;
}


//===========================================================
StatusCode TileMuonFillerTool::fill(const Analysis::Muon& p){
//===========================================================

    ATH_MSG_DEBUG("in TileMuonFillerTool::fill");

    // RETRIEVE APPROPRIATE POINTERS
    const Analysis::Muon* muonPointer       = &p;
    *m_mutype = m_trackType; 

    // GET TRK::TRACK AND REC::TRACKPARTICLE
    const Trk::Track* muon_trk           = muonPointer->combinedMuonTrkTrack();
    const Rec::TrackParticle* muon_track = muonPointer->combinedMuonTrackParticle();
    switch(m_trackType){
        case 0:  muon_trk   = muonPointer->combinedMuonTrkTrack(); 
                 muon_track = muonPointer->combinedMuonTrackParticle();
                 break;
        case 1:  muon_trk   = muonPointer->inDetTrkTrack();
                 muon_track = muonPointer->inDetTrackParticle();
                 break;
        case 2:  muon_trk   = muonPointer->muonSpectrometerTrkTrack();  
                 muon_track = muonPointer->muonSpectrometerTrackParticle();
                 break;
        case 3:  muon_trk   = muonPointer->muonExtrapolatedTrkTrack();  
                 muon_track = muonPointer->muonExtrapolatedTrackParticle();
                 break;
        case 4:  muon_trk   = muonPointer->innerExtrapolatedTrkTrack(); 
                 muon_track = muonPointer->innerExtrapolatedTrackParticle();
                 break;
        default: muon_trk   = muonPointer->combinedMuonTrkTrack();      
                 muon_track = muonPointer->combinedMuonTrackParticle();
                 break;
    } // SWITCH

    if( (!muon_trk||!muon_track) ){ return StatusCode::SUCCESS; }

    // STORE ISOLATION ENERGIES
    if(m_LevelOfDetails > 3){
        *m_nucone10                         = muonPointer->parameter(MuonParameters::nucone10);
        *m_nucone20                         = muonPointer->parameter(MuonParameters::nucone20);
        *m_nucone30                         = muonPointer->parameter(MuonParameters::nucone30);
        *m_nucone40                         = muonPointer->parameter(MuonParameters::nucone40);
        *m_etcone10                         = muonPointer->parameter(MuonParameters::etcone10);
        *m_etcone20                         = muonPointer->parameter(MuonParameters::etcone20);
        *m_etcone30                         = muonPointer->parameter(MuonParameters::etcone30);
        *m_etcone40                         = muonPointer->parameter(MuonParameters::etcone40);
        *m_ptcone10                         = muonPointer->parameter(MuonParameters::ptcone10);
        *m_ptcone20                         = muonPointer->parameter(MuonParameters::ptcone20);
        *m_ptcone30                         = muonPointer->parameter(MuonParameters::ptcone30);
        *m_ptcone40                         = muonPointer->parameter(MuonParameters::ptcone40);

        if(muon_trk){
            *m_LAr_iso_cone05 = m_inCalo->getIsolationEnergy(muon_trk,0.05,em1,em3,false);
            *m_LAr_iso_cone10 = m_inCalo->getIsolationEnergy(muon_trk,0.10,em1,em3,false);
            *m_LAr_iso_cone20 = m_inCalo->getIsolationEnergy(muon_trk,0.20,em1,em3,false);
            *m_LAr_iso_cone30 = m_inCalo->getIsolationEnergy(muon_trk,0.30,em1,em3,false);
            *m_LAr_iso_cone40 = m_inCalo->getIsolationEnergy(muon_trk,0.40,em1,em3,false);

            *m_Tile_iso_cone05 = m_inCalo->getIsolationEnergy(muon_trk,0.05,tile1,tile3,false);
            *m_Tile_iso_cone10 = m_inCalo->getIsolationEnergy(muon_trk,0.10,tile1,tile3,false);
            *m_Tile_iso_cone20 = m_inCalo->getIsolationEnergy(muon_trk,0.20,tile1,tile3,false);
            *m_Tile_iso_cone30 = m_inCalo->getIsolationEnergy(muon_trk,0.30,tile1,tile3,false);
            *m_Tile_iso_cone40 = m_inCalo->getIsolationEnergy(muon_trk,0.40,tile1,tile3,false);

            *m_LAr_cone05 = m_inCalo->getMeasuredEnergy(muon_trk,0.05,em1,em3,false,true);
            *m_LAr_cone10 = m_inCalo->getMeasuredEnergy(muon_trk,0.10,em1,em3,false,true);
            *m_LAr_cone20 = m_inCalo->getMeasuredEnergy(muon_trk,0.20,em1,em3,false,true);
            *m_LAr_cone30 = m_inCalo->getMeasuredEnergy(muon_trk,0.30,em1,em3,false,true);
            *m_LAr_cone40 = m_inCalo->getMeasuredEnergy(muon_trk,0.40,em1,em3,false,true);

            *m_Tile_cone05 = m_inCalo->getMeasuredEnergy(muon_trk,0.05,tile1,tile3,false,true);
            *m_Tile_cone10 = m_inCalo->getMeasuredEnergy(muon_trk,0.10,tile1,tile3,false,true);
            *m_Tile_cone20 = m_inCalo->getMeasuredEnergy(muon_trk,0.20,tile1,tile3,false,true);
            *m_Tile_cone30 = m_inCalo->getMeasuredEnergy(muon_trk,0.30,tile1,tile3,false,true);
            *m_Tile_cone40 = m_inCalo->getMeasuredEnergy(muon_trk,0.40,tile1,tile3,false,true);
        } // IF
    } // IF

    if(m_LevelOfDetails > 2){
        // NUMBER OF HITS IN SUBDETECTORS
        *m_numberOfBLayerHits               = muonPointer->numberOfBLayerHits();
        *m_numberOfPixelHits                = muonPointer->numberOfPixelHits();
        *m_numberOfSCTHits                  = muonPointer->numberOfSCTHits();
        *m_numberOfTRTHits                  = muonPointer->numberOfTRTHits();
        *m_numberOfTRTHighThresholdHits     = muonPointer->numberOfTRTHighThresholdHits();
        *m_numberOfBLayerSharedHits         = muonPointer->numberOfBLayerSharedHits();
        *m_numberOfPixelSharedHits          = muonPointer->numberOfPixelSharedHits();
        *m_numberOfPixelHoles               = muonPointer->numberOfPixelHoles();
        *m_numberOfSCTSharedHits            = muonPointer->numberOfSCTSharedHits();
        *m_numberOfSCTHoles                 = muonPointer->numberOfSCTHoles();
        *m_numberOfTRTOutliers              = muonPointer->numberOfTRTOutliers();
        *m_numberOfTRTHighThresholdOutliers = muonPointer->numberOfTRTHighThresholdOutliers();
        *m_numberOfMDTHits                  = muonPointer->numberOfMDTHits();
        *m_numberOfMDTHoles                 = muonPointer->numberOfMDTHoles();
        *m_numberOfCSCEtaHits               = muonPointer->numberOfCSCEtaHits();
        *m_numberOfCSCEtaHoles              = muonPointer->numberOfCSCEtaHoles();
        *m_numberOfCSCPhiHits               = muonPointer->numberOfCSCPhiHits();
        *m_numberOfCSCPhiHoles              = muonPointer->numberOfCSCPhiHoles();
        *m_numberOfRPCEtaHits               = muonPointer->numberOfRPCEtaHits();
        *m_numberOfRPCEtaHoles              = muonPointer->numberOfRPCEtaHoles();
        *m_numberOfRPCPhiHits               = muonPointer->numberOfRPCPhiHits();
        *m_numberOfRPCPhiHoles              = muonPointer->numberOfRPCPhiHoles();
        *m_numberOfTGCEtaHits               = muonPointer->numberOfTGCEtaHits();
        *m_numberOfTGCEtaHoles              = muonPointer->numberOfTGCEtaHoles();
        *m_numberOfTGCPhiHits               = muonPointer->numberOfTGCPhiHits();
        *m_numberOfTGCPhiHoles              = muonPointer->numberOfTGCPhiHoles();
        *m_numberOfGangedPixels             = muonPointer->numberOfGangedPixels();
        *m_numberOfOutliersOnTrack          = muonPointer->numberOfOutliersOnTrack();
        *m_numberOfTrackSummaryTypes        = muonPointer->numberOfTrackSummaryTypes();

        if(muon_trk){
            // GET ALL EXTRAPOLATIONS
            std::vector< double > coordinates_PreSamplerB = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,0);
            std::vector< double > coordinates_EMB1        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,1);
            std::vector< double > coordinates_EMB2        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,2);
            std::vector< double > coordinates_EMB3        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,3);
            std::vector< double > coordinates_PreSamplerE = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,4);
            std::vector< double > coordinates_EME1        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,5);
            std::vector< double > coordinates_EME2        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,6);
            std::vector< double > coordinates_EME3        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,7);
            std::vector< double > coordinates_HEC0        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,8);
            std::vector< double > coordinates_HEC1        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,9);
            std::vector< double > coordinates_HEC2        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,10);
            std::vector< double > coordinates_HEC3        = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,11);
            std::vector< double > coordinates_TileBar0    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,12);
            std::vector< double > coordinates_TileBar1    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,13);
            std::vector< double > coordinates_TileBar2    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,14);
            std::vector< double > coordinates_TileGap1    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,15);
            std::vector< double > coordinates_TileGap2    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,16);
            std::vector< double > coordinates_TileGap3    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,17);
            std::vector< double > coordinates_TileExt0    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,18);
            std::vector< double > coordinates_TileExt1    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,19);
            std::vector< double > coordinates_TileExt2    = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,20);

            // FILL VARIABLES
            if(coordinates_PreSamplerB.size()==5){
                *m_PreSamplerB_X   = coordinates_PreSamplerB[0];
                *m_PreSamplerB_Y   = coordinates_PreSamplerB[1];
                *m_PreSamplerB_Z   = coordinates_PreSamplerB[2];
                *m_PreSamplerB_Eta = coordinates_PreSamplerB[3];
                *m_PreSamplerB_Phi = coordinates_PreSamplerB[4];
            } // IF
            if(coordinates_EMB1.size()==5){
                *m_EMB1_X   = coordinates_EMB1[0];
                *m_EMB1_Y   = coordinates_EMB1[1];
                *m_EMB1_Z   = coordinates_EMB1[2];
                *m_EMB1_Eta = coordinates_EMB1[3];
                *m_EMB1_Phi = coordinates_EMB1[4];
            } // IF
            if(coordinates_EMB2.size()==5){
                *m_EMB2_X   = coordinates_EMB2[0];
                *m_EMB2_Y   = coordinates_EMB2[1];
                *m_EMB2_Z   = coordinates_EMB2[2];
                *m_EMB2_Eta = coordinates_EMB2[3];
                *m_EMB2_Phi = coordinates_EMB2[4];
            } // IF
            if(coordinates_EMB3.size()==5){
                *m_EMB3_X   = coordinates_EMB3[0];
                *m_EMB3_Y   = coordinates_EMB3[1];
                *m_EMB3_Z   = coordinates_EMB3[2];
                *m_EMB3_Eta = coordinates_EMB3[3];
                *m_EMB3_Phi = coordinates_EMB3[4];
            } // IF
            if(coordinates_PreSamplerE.size()==5){
                *m_PreSamplerE_X   = coordinates_PreSamplerE[0];
                *m_PreSamplerE_Y   = coordinates_PreSamplerE[1];
                *m_PreSamplerE_Z   = coordinates_PreSamplerE[2];
                *m_PreSamplerE_Eta = coordinates_PreSamplerE[3];
                *m_PreSamplerE_Phi = coordinates_PreSamplerE[4];
            } // IF
            if(coordinates_EME1.size()==5){
                *m_EME1_X   = coordinates_EME1[0];
                *m_EME1_Y   = coordinates_EME1[1];
                *m_EME1_Z   = coordinates_EME1[2];
                *m_EME1_Eta = coordinates_EME1[3];
                *m_EME1_Phi = coordinates_EME1[4];
            } // IF
            if(coordinates_EME2.size()==5){
                *m_EME2_X   = coordinates_EME2[0];
                *m_EME2_Y   = coordinates_EME2[1];
                *m_EME2_Z   = coordinates_EME2[2];
                *m_EME2_Eta = coordinates_EME2[3];
                *m_EME2_Phi = coordinates_EME2[4];
            } // IF
            if(coordinates_EME3.size()==5){
                *m_EME3_X   = coordinates_EME3[0];
                *m_EME3_Y   = coordinates_EME3[1];
                *m_EME3_Z   = coordinates_EME3[2];
                *m_EME3_Eta = coordinates_EME3[3];
                *m_EME3_Phi = coordinates_EME3[4];
            } // IF
            if(coordinates_HEC0.size()==5){
                *m_HEC0_X   = coordinates_HEC0[0];
                *m_HEC0_Y   = coordinates_HEC0[1];
                *m_HEC0_Z   = coordinates_HEC0[2];
                *m_HEC0_Eta = coordinates_HEC0[3];
                *m_HEC0_Phi = coordinates_HEC0[4];
            } // IF
            if(coordinates_HEC1.size()==5){
                *m_HEC1_X   = coordinates_HEC1[0];
                *m_HEC1_Y   = coordinates_HEC1[1];
                *m_HEC1_Z   = coordinates_HEC1[2];
                *m_HEC1_Eta = coordinates_HEC1[3];
                *m_HEC1_Phi = coordinates_HEC1[4];
            } // IF
            if(coordinates_HEC2.size()==5){
                *m_HEC2_X   = coordinates_HEC2[0];
                *m_HEC2_Y   = coordinates_HEC2[1];
                *m_HEC2_Z   = coordinates_HEC2[2];
                *m_HEC2_Eta = coordinates_HEC2[3];
                *m_HEC2_Phi = coordinates_HEC2[4];
            } // IF
            if(coordinates_HEC3.size()==5){
                *m_HEC3_X   = coordinates_HEC3[0];
                *m_HEC3_Y   = coordinates_HEC3[1];
                *m_HEC3_Z   = coordinates_HEC3[2];
                *m_HEC3_Eta = coordinates_HEC3[3];
                *m_HEC3_Phi = coordinates_HEC3[4];
            } // IF
            if(coordinates_TileBar0.size()==5){
                *m_TileBar0_X   = coordinates_TileBar0[0];
                *m_TileBar0_Y   = coordinates_TileBar0[1];
                *m_TileBar0_Z   = coordinates_TileBar0[2];
                *m_TileBar0_Eta = coordinates_TileBar0[3];
                *m_TileBar0_Phi = coordinates_TileBar0[4];
            } // IF
            if(coordinates_TileBar1.size()==5){
                *m_TileBar1_X   = coordinates_TileBar1[0];
                *m_TileBar1_Y   = coordinates_TileBar1[1];
                *m_TileBar1_Z   = coordinates_TileBar1[2];
                *m_TileBar1_Eta = coordinates_TileBar1[3];
                *m_TileBar1_Phi = coordinates_TileBar1[4];
            } // IF
            if(coordinates_TileBar2.size()==5){
                *m_TileBar2_X   = coordinates_TileBar2[0];
                *m_TileBar2_Y   = coordinates_TileBar2[1];
                *m_TileBar2_Z   = coordinates_TileBar2[2];
                *m_TileBar2_Eta = coordinates_TileBar2[3];
                *m_TileBar2_Phi = coordinates_TileBar2[4];
            } // IF
            if(coordinates_TileGap1.size()==5){
                *m_TileGap1_X   = coordinates_TileGap1[0];
                *m_TileGap1_Y   = coordinates_TileGap1[1];
                *m_TileGap1_Z   = coordinates_TileGap1[2];
                *m_TileGap1_Eta = coordinates_TileGap1[3];
                *m_TileGap1_Phi = coordinates_TileGap1[4];
            } // IF
            if(coordinates_TileGap2.size()==5){
                *m_TileGap2_X   = coordinates_TileGap2[0];
                *m_TileGap2_Y   = coordinates_TileGap2[1];
                *m_TileGap2_Z   = coordinates_TileGap2[2];
                *m_TileGap2_Eta = coordinates_TileGap2[3];
                *m_TileGap2_Phi = coordinates_TileGap2[4];
            } // IF
            if(coordinates_TileGap3.size()==5){
                *m_TileGap3_X   = coordinates_TileGap3[0];
                *m_TileGap3_Y   = coordinates_TileGap3[1];
                *m_TileGap3_Z   = coordinates_TileGap3[2];
                *m_TileGap3_Eta = coordinates_TileGap3[3];
                *m_TileGap3_Phi = coordinates_TileGap3[4];
            } // IF
            if(coordinates_TileExt0.size()==5){
                *m_TileExt0_X   = coordinates_TileExt0[0];
                *m_TileExt0_Y   = coordinates_TileExt0[1];
                *m_TileExt0_Z   = coordinates_TileExt0[2];
                *m_TileExt0_Eta = coordinates_TileExt0[3];
                *m_TileExt0_Phi = coordinates_TileExt0[4];
            } // IF
            if(coordinates_TileExt1.size()==5){
                *m_TileExt1_X   = coordinates_TileExt1[0];
                *m_TileExt1_Y   = coordinates_TileExt1[1];
                *m_TileExt1_Z   = coordinates_TileExt1[2];
                *m_TileExt1_Eta = coordinates_TileExt1[3];
                *m_TileExt1_Phi = coordinates_TileExt1[4];
            } // IF
            if(coordinates_TileExt2.size()==5){
                *m_TileExt2_X   = coordinates_TileExt2[0];
                *m_TileExt2_Y   = coordinates_TileExt2[1];
                *m_TileExt2_Z   = coordinates_TileExt2[2];
                *m_TileExt2_Eta = coordinates_TileExt2[3];
                *m_TileExt2_Phi = coordinates_TileExt2[4];
            } // IF
        } // IF
    } // IF

    // MUON TRACK PARAMETERS
    if(m_LevelOfDetails > 1){
        *m_pT                               = muonPointer->pt();
        *m_p                                = muonPointer->p();
        *m_charge                           = muonPointer->charge();
        *m_eta                              = muonPointer->eta();
        *m_phi                              = muonPointer->phi();
        *m_beta                             = muonPointer->beta();
    } // IF

    // MUON QUALITY INFORMATION
    if(m_LevelOfDetails > 5){
        *m_matchChi2                        = muonPointer->matchChi2();
        *m_matchChi2OverDoF                 = muonPointer->matchChi2OverDoF();
        *m_matchNumberDoF                   = muonPointer->matchNumberDoF();
        *m_fitChi2                          = muonPointer->fitChi2();
        *m_fitNumberDoF                     = muonPointer->fitNumberDoF();
        *m_fitChi2OverDoF                   = muonPointer->fitChi2OverDoF();
        *m_bestMatch                        = muonPointer->bestMatch();
        *m_isCorrected                      = muonPointer->isCorrected();
        *m_hasCombinedMuon                  = muonPointer->hasCombinedMuon();
        *m_hasInDetTrackParticle            = muonPointer->hasInDetTrackParticle();
        *m_hasMuonExtrapolatedTrackParticle = muonPointer->hasMuonExtrapolatedTrackParticle();
        *m_hasInnerExtrapolatedTrackParticle= muonPointer->hasInnerExtrapolatedTrackParticle();
        *m_hasCombinedMuonTrackParticle     = muonPointer->hasCombinedMuonTrackParticle();
        *m_hasCluster                       = muonPointer->hasCluster();
        *m_isLoose                          = muonPointer->isLoose();
        *m_isMedium                         = muonPointer->isMedium();
        *m_isTight                          = muonPointer->isTight();
    } // IF

    if(m_LevelOfDetails > 3){
        if(m_trackToVertexTool){
            *m_vtxX                         = m_trackToVertexTool->perigeeAtVertex(*muon_track)->parameters()[Trk::x];
            *m_vtxY                         = m_trackToVertexTool->perigeeAtVertex(*muon_track)->parameters()[Trk::y];
            *m_vtxZ                         = m_trackToVertexTool->perigeeAtVertex(*muon_track)->parameters()[Trk::z];
            *m_d0                           = m_trackToVertexTool->perigeeAtVertex(*muon_track)->parameters()[Trk::d0];
            *m_z0                           = m_trackToVertexTool->perigeeAtVertex(*muon_track)->parameters()[Trk::z0];
            *m_phi0                         = m_trackToVertexTool->perigeeAtVertex(*muon_track)->parameters()[Trk::phi0];
            *m_theta                        = m_trackToVertexTool->perigeeAtVertex(*muon_track)->parameters()[Trk::theta];
            *m_qOverP                       = m_trackToVertexTool->perigeeAtVertex(*muon_track)->parameters()[Trk::qOverP];
        } // IF
    } // IF

    if(m_LevelOfDetails > 7){
    // STORE MONTE CARLO TRUTH INFORMATION IF AVAILABLE
        if(truthRetrieved.isSuccess() && m_MCtruth){
            McEventCollection::const_iterator MCitr          = m_MCtruth->begin();
            HepMC::GenEvent::particle_const_iterator particle_itr  = (*MCitr)->particles_begin();
            HepMC::GenEvent::particle_const_iterator particle_eitr = (*MCitr)->particles_end();
            for(;particle_itr!=particle_eitr;++particle_eitr){
                HepMC::GenParticle* particle = *particle_itr;
                if(abs(particle->pdg_id()) != 13) continue;
                float deltaEta = fabs(*m_eta - particle->momentum().eta());
                float deltaPhi = *m_phi - particle->momentum().phi();
                deltaPhi = fabs(deltaPhi > M_PI ? deltaPhi - 2.*M_PI : ( deltaPhi < -M_PI ? deltaPhi + 2.*M_PI : deltaPhi ));
                *m_truth_deltaR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
                *m_truth_vtxX   = particle->production_vertex()->position().x();
                *m_truth_vtxY   = particle->production_vertex()->position().y();
                *m_truth_vtxZ   = particle->production_vertex()->position().z();
                *m_truth_eta    = particle->momentum().eta();
                *m_truth_phi    = particle->momentum().phi();
                *m_truth_p      = sqrt(particle->momentum().perp() * particle->momentum().perp() + 
                                       particle->momentum().z()    * particle->momentum().z());
                *m_truth_pT     = particle->momentum().perp();
                *m_truth_charge = particle->pdg_id();
            } // FOR
        } // IF
    } // IF

    return StatusCode::SUCCESS;
} // TileMuonFillerTool::fill

} // namespace D3PD
