/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileMBTSFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * Description: CaloCell filler.
 *
 * Created in September 2012
 */

// INCLUDE HEADER
#include "TileMBTSFillerTool.h"

// ADD TILEMUONCELLFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{

//=======================================================================================================================
TileMBTSFillerTool::TileMBTSFillerTool(const std::string& type,const std::string& name,const IInterface* parent):
BlockFillerTool<TileCell> (type, name, parent),
m_inCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//=======================================================================================================================

    // TRACK EXTRAPOLATOR TOOL
    declareProperty("ExtrapolateInCaloTool", m_inCalo);
    declareProperty("LevelOfDetails",        m_LevelOfDetails = 1);
    declareProperty("TryStandAloneMuons",    m_standalone = "no");
    declareProperty("FilterConfiguration",   m_filterConfiguration = 0);
    declareProperty("TrackType",             m_trackType = 0);

    book().ignore(); // AVOID COVERITY WARNINGS

} // TileMBTSFillerTool::TileMBTSFillerTool

//==============================================
StatusCode TileMBTSFillerTool::initialize(){
//==============================================

    ATH_MSG_DEBUG("in TileMBTSFillerTool::initialize()");

    IToolSvc* toolsvc;
    CHECK(service("ToolSvc",toolsvc));

    // RETRIEVE CALORIMETER EXTRAPOLATOR TOOLS
    CHECK(m_inCalo.retrieve());

    // RETRIEVE STOREGATE AND SERVICES
    CHECK(service("StoreGateSvc",m_storeGate));
    CHECK(service("DetectorStore",m_detStore));
    if(m_LevelOfDetails > 2){
        CHECK(detStore()->retrieve(m_tileID));
        CHECK(detStore()->retrieve(m_tileHWID));
    } // IF

    return StatusCode::SUCCESS;

} // TileMBTSFillerTool::initialize

//============================================
StatusCode TileMBTSFillerTool::finalize(){
//============================================

    ATH_MSG_DEBUG("in TileMBTSFillerTool::finalize()");
    return StatusCode::SUCCESS;

} // TileMBTSFillerTool::finalize

//========================================
StatusCode TileMBTSFillerTool::book(){
//========================================

    if(m_LevelOfDetails > 1){
        // CELL ENERGY AND ETA/PHI
        CHECK( addVariable ("E",                     m_cellEnergy));
        CHECK( addVariable ("Et",                    m_cellEt));
        CHECK( addVariable ("eta",                   m_cellEta));
        CHECK( addVariable ("phi",                   m_cellPhi));
    } // IF

    if(m_LevelOfDetails > 2){
        // GLOBAL CELL POSITIONING AND QUALITY
        CHECK( addVariable ("sinTh",                 m_cellSinTh));
        CHECK( addVariable ("cosTh",                 m_cellCosTh));
        CHECK( addVariable ("cotTh",                 m_cellCotTh));
        CHECK( addVariable ("x",                     m_cellX));
        CHECK( addVariable ("y",                     m_cellY));
        CHECK( addVariable ("z",                     m_cellZ));
        CHECK( addVariable ("gain",                  m_cellGain));
        CHECK( addVariable ("badCell",               m_badCell));
        CHECK( addVariable ("samplingID",            m_samplingID));
        CHECK( addVariable ("time",                  m_time));
        CHECK( addVariable ("quality",               m_quality));

        // CELL DIMENSIONS
        CHECK( addVariable ("subcalo",               m_subcalo));
        CHECK( addVariable ("R",                     m_cellR));
        CHECK( addVariable ("dR",                    m_celldR));
        CHECK( addVariable ("dx",                    m_celldX));
        CHECK( addVariable ("dy",                    m_celldY));
        CHECK( addVariable ("dz",                    m_celldZ));
        CHECK( addVariable ("V",                     m_cellV));
        CHECK( addVariable ("dPhi",                  m_celldPhi));
        CHECK( addVariable ("dEta",                  m_celldEta));

        // TILECAL CELL POSITIONING
        CHECK( addVariable ("side",                  m_side     ,     "",-2));
        CHECK( addVariable ("section",               m_section  ,     "",-2));
        CHECK( addVariable ("module",                m_module   ,     "",-2));
        CHECK( addVariable ("tower",                 m_tower    ,     "",-2));
        CHECK( addVariable ("sample",                m_sample   ,     "",-2));
        CHECK( addVariable ("eta_index",             m_eta_index,     "",-2));
    } // IF

    if(m_LevelOfDetails > 4){
        // PMT INFORMATION
        CHECK( addVariable ("PMT1_energy",           m_pmt1Energy,    "",0.));
        CHECK( addVariable ("PMT1_time",             m_pmt1Time,      "",0.));
        CHECK( addVariable ("PMT1_quality",          m_pmt1Quality,   "",0));
        CHECK( addVariable ("PMT1_qbit",             m_pmt1Qbit,      "",0));
        CHECK( addVariable ("PMT1_bad",              m_pmt1Bad,       "",false));
        CHECK( addVariable ("PMT1_gain",             m_pmt1Gain,      "",0));

        CHECK( addVariable ("PMT2_energy",           m_pmt2Energy,    "",0.));
        CHECK( addVariable ("PMT2_time",             m_pmt2Time,      "",0.));
        CHECK( addVariable ("PMT2_quality",          m_pmt2Quality,   "",0));
        CHECK( addVariable ("PMT2_qbit",             m_pmt2Qbit,      "",0));
        CHECK( addVariable ("PMT2_bad",              m_pmt2Bad,       "",false));
        CHECK( addVariable ("PMT2_gain",             m_pmt2Gain,      "",0));
    } // IF

    if(m_LevelOfDetails > 7){
        // PATH LENGTH AND CELL RESPONSE (STACO)
//         CHECK( addVariable ("mu_N",               m_muN,   "", 0));
        CHECK( addVariable ("mu_dx",              m_dx_mu));
        CHECK( addVariable ("mu_dEdx",            m_celldEdx_mu));
//         CHECK( addVariable ("mu_type",            m_mutype));
        CHECK( addVariable ("mu_x",               m_muonX));
        CHECK( addVariable ("mu_y",               m_muonY));
        CHECK( addVariable ("mu_z",               m_muonZ));
        CHECK( addVariable ("mu_phi",             m_muonPhi));
        CHECK( addVariable ("mu_eta",             m_muonEta));
        CHECK( addVariable ("to_mu_dx",           m_to_mu_dx));
        CHECK( addVariable ("to_mu_dy",           m_to_mu_dy));
        CHECK( addVariable ("to_mu_dz",           m_to_mu_dz));
        CHECK( addVariable ("to_mu_dPhi",         m_to_mu_dPhi));
        CHECK( addVariable ("to_mu_dEta",         m_to_mu_dEta));
    } // IF

    return StatusCode::SUCCESS;
} // TileMBTSFillerTool::book


//==========================================================
StatusCode TileMBTSFillerTool::fill (const TileCell& p){
//==========================================================
    const CaloDetDescrElement *dde = p.caloDDE();

    ATH_MSG_DEBUG("in TileMBTSFillerTool::fill()");

    if(m_LevelOfDetails > 1){
        // DEPOSITED ENERGY
        *m_cellEnergy                  = p.energy();
        *m_cellEt                      = p.et();

        // CELL POSITION
        *m_cellEta                     = p.eta();
        *m_cellPhi                     = p.phi();
    } // IF

    if(m_LevelOfDetails > 2){
        *m_cellSinTh                   = p.sinTh();
        *m_cellCosTh                   = p.cosTh();
        *m_cellCotTh                   = p.cotTh();

        // CELL DIMENSIONS
        *m_cellX                       = dde->x();
        *m_cellY                       = dde->y();
        *m_cellZ                       = dde->z();
        *m_cellR                       = dde->r();
        *m_celldX                      = dde->dx();
        *m_celldY                      = dde->dy();
        *m_celldZ                      = dde->dz();
        *m_celldR                      = dde->dr();
        *m_cellV                       = dde->volume();
        *m_celldPhi                    = dde->dphi();
        *m_celldEta                    = dde->deta();

        // CELL STATUS INFORMATION
        *m_cellGain                    = p.gain();
        *m_badCell                     = p.badcell();
        *m_time                        = p.time();
        *m_quality                     = p.provenance();

        *m_samplingID                  = dde->getSampling();
        *m_subcalo                     = dde->getSubCalo();

        // RAW CHANNEL INFORMATION
        if(m_LevelOfDetails > 1){
            *m_side                    = m_tileID->side((&p)->ID());
            *m_section                 = m_tileID->section((&p)->ID());
            *m_module                  = m_tileID->module((&p)->ID());
            *m_tower                   = m_tileID->tower((&p)->ID());
            *m_sample                  = m_tileID->sample((&p)->ID());
            *m_eta_index               = m_inCalo->retrieveIndex(*m_samplingID,*m_cellEta);

            if(m_LevelOfDetails > 4){
                *m_pmt1Energy          = (&p)->ene1();
                *m_pmt2Energy          = (&p)->ene2();
                *m_pmt1Time            = (&p)->time1();
                *m_pmt2Time            = (&p)->time2();
                *m_pmt1Quality         = (&p)->qual1();
                *m_pmt2Quality         = (&p)->qual2();
                *m_pmt1Qbit            = (&p)->qbit1();
                *m_pmt2Qbit            = (&p)->qbit2();
                *m_pmt1Bad             = (&p)->badch1();
                *m_pmt2Bad             = (&p)->badch2();
                *m_pmt1Gain            = (&p)->gain1();
                *m_pmt2Gain            = (&p)->gain2();
            } // IF
        } // ELSE
    } // IF

    if(m_LevelOfDetails > 3 && m_filterConfiguration == 0 ){
        // RETRIEVE APPROPRIATE POINTERS
        // GET HANDLE ON MUON CONTAINER
        typedef Analysis::MuonContainer MUONCONTAINER;
        const MUONCONTAINER* stac_muons;

        // STACO MUONS
        CHECK( m_storeGate->retrieve( stac_muons, "StacoMuonCollection" ) );
        std::vector<MUONCONTAINER::const_iterator> start;
        std::vector<MUONCONTAINER::const_iterator> stop;
        start.push_back(stac_muons->begin());
        stop. push_back(stac_muons->end());

        for(unsigned int i=0;i<start.size();++i){
            MUONCONTAINER::const_iterator f_mu = start[i];
            MUONCONTAINER::const_iterator l_mu = stop[i];
            for( ; f_mu != l_mu ; ++f_mu ){
                const Analysis::Muon* muonPointer = *f_mu;
                const Trk::Track* muon_trk = NULL;
                switch(m_trackType){
                    case 0:  muon_trk = muonPointer->combinedMuonTrkTrack();      break;
                    case 1:  muon_trk = muonPointer->inDetTrkTrack();             break;
                    case 2:  muon_trk = muonPointer->muonSpectrometerTrkTrack();  break;
                    case 3:  muon_trk = muonPointer->muonExtrapolatedTrkTrack();  break;
                    case 4:  muon_trk = muonPointer->innerExtrapolatedTrkTrack(); break;
                    default: muon_trk = muonPointer->combinedMuonTrkTrack();      break;
                } // SWITCH
                if(!muon_trk) continue;

                std::vector<double> coordinates = m_inCalo->getXYZEtaPhiInCellSampling(muon_trk,dde->getSampling());
                if(m_LevelOfDetails > 7 && 
                   m_inCalo->getLayer(&p) != -1 && 
                   muon_trk && 
                   coordinates.size() == 5
                  ){
                    // CALCULATE PATH LENGTHS USING EXTRAPOLATOR TOOL
                    float path = m_inCalo->getCBNTPathInsideCell(muon_trk,&p);
                    switch(i){
                        case 0:
                            m_muonX  ->push_back(coordinates[0]);
                            m_muonY  ->push_back(coordinates[1]);
                            m_muonZ  ->push_back(coordinates[2]);
                            m_muonPhi->push_back(coordinates[3]);
                            m_muonEta->push_back(coordinates[4]);

                            m_to_mu_dx  ->push_back(*m_cellX   - coordinates[0]);
                            m_to_mu_dy  ->push_back(*m_cellY   - coordinates[1]);
                            m_to_mu_dz  ->push_back(*m_cellZ   - coordinates[2]);
                            m_to_mu_dEta->push_back(*m_cellEta - coordinates[3]);
                            m_to_mu_dPhi->push_back(phidiff(*m_cellPhi,coordinates[4]));

                            m_dx_mu->push_back(path);
                            m_celldEdx_mu->push_back(path > 0 ? p.energy() / path : -1.);
                            break;
                        default: break;
                    } // SWITCH
                } // IF
            } // FOR
        } // FOR (MUON TYPES)
    } // IF

    return StatusCode::SUCCESS;
    } // TileMBTSFillerTool::fill

double TileMBTSFillerTool::phidiff(double phi1, double phi2){
     // FEW CONSTANTS
     double phi_min = -2.*asin(1.);
     double twopi   = 4.*asin(1.);
     double phi_max = phi_min + twopi;

     // COMPUTE PHI DIFFERENCE
     double ph1     = phi1 < phi_min ? phi1 + twopi : ( phi1 > phi_max ? phi1 - twopi : phi1 );
     double ph2     = phi2 < phi_min ? phi2 + twopi : ( phi2 > phi_max ? phi2 - twopi : phi2 );
     double res     = ph1-ph2;
     return           res  < phi_min ? res + twopi  : ( res  > phi_max ?  res - twopi : res );
} // TrackTools::phidiff

} // namespace D3PD
