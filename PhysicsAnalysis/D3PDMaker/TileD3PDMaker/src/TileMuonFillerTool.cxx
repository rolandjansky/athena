/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileMuonFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Date: September 2012
 *
 * Archil Durglishvili <Archil.Durglishvili@cern.ch>
 * Date: October 2014
 */

// INCLUDE HEADER FILE
#include "GeoPrimitives/GeoPrimitives.h"
#include "TileMuonFillerTool.h"

// CALORIMETER INCLUDES
#include "TileEvent/TileCell.h"
#include "CaloEvent/CaloCellContainer.h"

// MONTE CARLO INCLUDES
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenEvent.h"

//xAOD tools
#include "xAODPrimitives/IsolationType.h"
using namespace xAOD;

// ADD TILEMUONTRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{
//=========================================================================
TileMuonFillerTool::TileMuonFillerTool (const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : D3PD::BlockFillerTool<xAOD::Muon> (type, name, parent),
  m_trackInCalo("TrackTools"),
  m_trackToVertexTool("Reco::TrackToVertex"){
//=========================================================================
// CONSTRUCTOR

    book().ignore(); // AVOID COVERITY WARNINGS 

    // TRACK EXTRAPOLATOR TOOL
    declareProperty("TrackTools", m_trackInCalo);
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
    ATH_MSG_DEBUG("TileMuonFillerTool::initialize()");
    CHECK( D3PD::BlockFillerTool<xAOD::Muon>::initialize() ); 

    // CHECK WHETHER STOREGATE CAN BE RETRIEVED
    StatusCode sc = StatusCode::SUCCESS; 

    sc = service("StoreGateSvc",m_storeGate);
    if(sc.isFailure()) {
        ATH_MSG_ERROR("StoreGate service not found !");
        return StatusCode::FAILURE;
    } // IF

    //THIS NEEDS TO MODIFY TO WORK ON xAODs
    // MONTE CARLO EVENT COLLECTION
    m_truthRetrieved = m_storeGate->retrieve(m_MCtruth,m_MCeventCollection);
    if(m_truthRetrieved.isFailure()) if(m_LevelOfDetails > 6) ATH_MSG_ERROR("NO TRUTH INFORMATION");

    // RETRIEVE CALORIMETER EXTRAPOLATION TOOLS
    if(m_LevelOfDetails > 3){
        if(m_trackInCalo.retrieve().isFailure() ) {
            ATH_MSG_ERROR(" Cannot retrieve extrapolator in calorimeter properly. ");
        } // IF
    } // IF

    if(m_LevelOfDetails > 3){
        if(m_trackToVertexTool.retrieve().isFailure() ) {
            ATH_MSG_ERROR(" Cannot retrieve vertex tool properly. ");
        } // IF
    } // IF


    return StatusCode::SUCCESS;
  } // TileMuonFillerTool::initialize

//====================================
StatusCode TileMuonFillerTool::book(){
//====================================
    // MUON PARAMETERS
    if(m_LevelOfDetails > 0){
        CHECK( addVariable("author",                           m_muonAuthor,                       "", m_defaultValue));
        CHECK( addVariable("type",                             m_muonType,                         "", m_defaultValue));
        CHECK( addVariable("quality",                          m_muonQuality,                      "", m_defaultValue));
        CHECK( addVariable("pT",                               m_pT,                               "", m_defaultValue));
        CHECK( addVariable("p",                                m_p,                                "", m_defaultValue));
        CHECK( addVariable("charge",                           m_charge,                           "", m_defaultValue));
        CHECK( addVariable("eta",                              m_eta,                              "", m_defaultValue));
        CHECK( addVariable("phi",                              m_phi,                              "", m_defaultValue));
    } // IF


    // MUON HITS IN SUBDETECTORS
    if(m_LevelOfDetails > 1){
        // HIT NUMBERS IN SUBDETECTORS
        CHECK( addVariable("numberOfInnermostPixelLayerHits",               m_numberOfBLayerHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfPixelHits",                m_numberOfPixelHits,                "", m_defaultValue));
        CHECK( addVariable("numberOfSCTHits",                  m_numberOfSCTHits,                  "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHits",                  m_numberOfTRTHits,                  "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHighThresholdHits",     m_numberOfTRTHighThresholdHits,     "", m_defaultValue));
        CHECK( addVariable("numberOfInnermostPixelLayerSharedHits",         m_numberOfBLayerSharedHits,         "", m_defaultValue));
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
        CHECK( addVariable("numberOfPixelDeadSensors",         m_numberOfPixelDeadSensors,         "", m_defaultValue));
        CHECK( addVariable("numberOfSCTDeadSensors",           m_numberOfSCTDeadSensors,           "", m_defaultValue));
    }

    CHECK (book2());

    return StatusCode::SUCCESS;
} // TileMuonFillerTool::book


StatusCode TileMuonFillerTool::book2()
{
    if(m_LevelOfDetails > 2){
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

        CHECK( addVariable("matchChi2",                         m_matchChi2,                         "", m_defaultValue));
        CHECK( addVariable("matchChi2OverDoF",                  m_matchChi2OverDoF,                  "", m_defaultValue));
        CHECK( addVariable("matchNumberDoF",                    m_matchNumberDoF,                    "", m_defaultValue));
        CHECK( addVariable("fitChi2",                           m_fitChi2,                           "", m_defaultValue));
        CHECK( addVariable("fitNumberDoF",                      m_fitNumberDoF,                      "", m_defaultValue));
        CHECK( addVariable("fitChi2OverDoF",                    m_fitChi2OverDoF,                    "", m_defaultValue));
        CHECK( addVariable("bestMatch",                         m_bestMatch,                         "", m_defaultValue));
        CHECK( addVariable("isCorrected",                       m_isCorrected,                       "", m_defaultValue));
    } // IF

    if(m_LevelOfDetails > 4){
        if(m_truthRetrieved.isSuccess() && m_MCtruth){
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
StatusCode TileMuonFillerTool::fill(const xAOD::Muon& p){
//===========================================================

    ATH_MSG_DEBUG("TileMuonFillerTool::fill()");

    // RETRIEVE APPROPRIATE POINTERS
    const xAOD::Muon* muonPointer       = &p;

    if(!muonPointer) { ATH_MSG_INFO("NULL POINTER"); return StatusCode::RECOVERABLE; }

    /// primaryTrackParticle() Returns a pointer (which should not usually be NULL, but might be if the muon has been stripped of information) to the
    /// primary TrackParticle corresponding to the MuonType of this muon.
    /// This is determined in the following order:
    ///  1. CombinedTrackParticle
    ///  2. InnerDetectorTrackParticle
    ///  3. MuonSpectrometerTrackParticle
    const xAOD::TrackParticle* muon_track = 0;
    switch(m_trackType)
    {
      case 0:  muon_track = muonPointer->trackParticle(xAOD::Muon::CombinedTrackParticle);          break;
      case 1:  muon_track = muonPointer->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);     break;
      case 2:  muon_track = muonPointer->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);  break;
      default: muon_track = muonPointer->primaryTrackParticle();     break;
    } // SWITCH

    if( !muon_track ){ return StatusCode::SUCCESS; }

    // MUON TRACK PARAMETERS
    if(m_LevelOfDetails > 0){
        *m_pT                               = muonPointer->pt();
        *m_p                                = muonPointer->p4().P();
        *m_charge                           = muonPointer->charge();
        *m_eta                              = muonPointer->eta();
        *m_phi                              = muonPointer->phi();
        *m_muonAuthor = muonPointer->author();
        *m_muonType = muonPointer->muonType();
        *m_muonQuality = muonPointer->quality();
    } // IF


    // STORE ISOLATION ENERGIES
    if(m_LevelOfDetails > 2){

        if ( !muonPointer->isolation((*m_etcone20), Iso::IsolationType::etcone20 ) ) ATH_MSG_WARNING("No etcone20 defined");
        if ( !muonPointer->isolation((*m_etcone30), Iso::IsolationType::etcone30 ) ) ATH_MSG_WARNING("No etcone30 defined");
        if ( !muonPointer->isolation((*m_etcone40), Iso::IsolationType::etcone40 ) ) ATH_MSG_WARNING("No etcone40 defined");
        if ( !muonPointer->isolation((*m_ptcone20), Iso::IsolationType::ptcone20 ) ) ATH_MSG_WARNING("No ptcone20 defined");
        if ( !muonPointer->isolation((*m_ptcone30), Iso::IsolationType::ptcone30 ) ) ATH_MSG_WARNING("No ptcone30 defined");
        if ( !muonPointer->isolation((*m_ptcone40), Iso::IsolationType::ptcone40 ) ) ATH_MSG_WARNING("No ptcone40 defined");
    } // IF

    if(m_LevelOfDetails > 1){

        muonPointer->summaryValue( *((uint8_t*)m_numberOfBLayerHits), xAOD::numberOfInnermostPixelLayerHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfPixelHits), xAOD::numberOfPixelHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfSCTHits), xAOD::numberOfSCTHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfTRTHits), xAOD::numberOfTRTHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfPixelDeadSensors), xAOD::numberOfPixelDeadSensors );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfSCTDeadSensors), xAOD::numberOfSCTDeadSensors );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfSCTHoles), xAOD::numberOfSCTHoles );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfTRTHighThresholdHits), xAOD::numberOfTRTHighThresholdHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfBLayerSharedHits), xAOD::numberOfInnermostPixelLayerSharedHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfPixelSharedHits), xAOD::numberOfPixelSharedHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfPixelHoles), xAOD::numberOfPixelHoles );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfSCTSharedHits), xAOD::numberOfSCTSharedHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfPixelHoles), xAOD::numberOfPixelHoles );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfSCTSharedHits), xAOD::numberOfSCTSharedHits );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfTRTOutliers), xAOD::numberOfTRTOutliers );
        muonPointer->summaryValue( *((uint8_t*)m_numberOfTRTHighThresholdOutliers), xAOD::numberOfTRTHighThresholdOutliers );

/*      //THIS NEEDS TO BE CONVERTED FOR xAODs AS IT IS ABOVE
        // NUMBER OF HITS IN SUBDETECTORS
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
*/
    }
   

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

/*      //THIST NEEDS TO MODIFY TO WORK ON xAODs
        *m_matchChi2                        = muonPointer->matchChi2();
        *m_matchChi2OverDoF                 = muonPointer->matchChi2OverDoF();
        *m_matchNumberDoF                   = muonPointer->matchNumberDoF();
        *m_fitChi2                          = muonPointer->fitChi2();
        *m_fitNumberDoF                     = muonPointer->fitNumberDoF();
        *m_fitChi2OverDoF                   = muonPointer->fitChi2OverDoF();
        *m_bestMatch                        = muonPointer->bestMatch();
        *m_isCorrected                      = muonPointer->isCorrected();
*/
    } // IF

/*  //THIS NEEDS TO MODIFY TO WORK ON xAODs
    if(m_LevelOfDetails > 4){
    // STORE MONTE CARLO TRUTH INFORMATION IF AVAILABLE
        if(m_truthRetrieved.isSuccess() && m_MCtruth){
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
*/
    return StatusCode::SUCCESS;
} // TileMuonFillerTool::fill

} // namespace D3PD
