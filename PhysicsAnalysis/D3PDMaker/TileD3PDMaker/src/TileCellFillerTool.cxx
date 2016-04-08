/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * Description: CaloCell filler.
 *
 * Created in September 2012
 */

// INCLUDE HEADER
#include "TileCellFillerTool.h"

// xAOD tools
#include "xAODPrimitives/IsolationType.h"
using namespace xAOD;


// ADD TILEMUONCELLFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{

//=======================================================================================================================
TileCellFillerTool::TileCellFillerTool(const std::string& type,const std::string& name,const IInterface* parent):
BlockFillerTool<CaloCell> (type, name, parent),
m_trackInCalo("TrackInCaloTools/extrapolMuonInCaloTool"){
//=======================================================================================================================

    // TRACK EXTRAPOLATOR TOOL
    declareProperty("TrackTools", m_trackInCalo);
    declareProperty("LevelOfDetails",        m_LevelOfDetails = 1);
    declareProperty("TryStandAloneMuons",    m_standalone = "no");
    declareProperty("FilterConfiguration",   m_filterConfiguration = 0);
    declareProperty("TrackType",             m_trackType = 0);
    declareProperty("MuonContainerName",     m_muonContainerName="Muons");

    book().ignore(); // AVOID COVERITY WARNINGS

} // TileCellFillerTool::TileCellFillerTool


//==============================================
StatusCode TileCellFillerTool::initialize(){
//==============================================

    ATH_MSG_DEBUG("TileCellFillerTool::initialize()");

    IToolSvc* toolsvc;
    CHECK(service("ToolSvc",toolsvc));

    if(m_LevelOfDetails > 1){
        // RETRIEVE CALORIMETER EXTRAPOLATOR TOOLS
        CHECK(m_trackInCalo.retrieve());
    } // IF

    // RETRIEVE STOREGATE AND SERVICES
    CHECK(service("StoreGateSvc",m_storeGate));
    if(m_LevelOfDetails > 2){
        CHECK(detStore()->retrieve(m_tileID));
        CHECK(detStore()->retrieve(m_tileHWID));
    } // IF

    return StatusCode::SUCCESS;

} // TileCellFillerTool::initialize

//============================================
StatusCode TileCellFillerTool::finalize(){
//============================================

    ATH_MSG_DEBUG("TileCellFillerTool::finalize()");
    return StatusCode::SUCCESS;

} // TileCellFillerTool::finalize

//========================================
StatusCode TileCellFillerTool::book(){
//========================================
  if(m_LevelOfDetails > 0){
        // CELL ENERGY AND ETA/PHI
        CHECK( addVariable ("E",                     m_cellEnergy));
        CHECK( addVariable ("Et",                    m_cellEt));
        CHECK( addVariable ("eta",                   m_cellEta));
        CHECK( addVariable ("phi",                   m_cellPhi));
    } // IF

    if(m_LevelOfDetails > 1){       
        CHECK( addVariable ("gain",                  m_cellGain));
        CHECK( addVariable ("badCell",               m_badCell));
        CHECK( addVariable ("samplingID",            m_samplingID));
        CHECK( addVariable ("time",                  m_time));
        CHECK( addVariable ("quality",               m_quality));
	 CHECK( addVariable ("subcalo",               m_subcalo));
    } // IF

    if(m_LevelOfDetails > 2){
        // GLOBAL CELL POSITIONING AND QUALITY
        CHECK( addVariable ("sinTh",                 m_cellSinTh));
        CHECK( addVariable ("cosTh",                 m_cellCosTh));
        CHECK( addVariable ("cotTh",                 m_cellCotTh));
        CHECK( addVariable ("x",                     m_cellX));
        CHECK( addVariable ("y",                     m_cellY));
        CHECK( addVariable ("z",                     m_cellZ));
       
        // CELL DIMENSIONS
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
    
    if(m_LevelOfDetails > 3){
        // PMT INFORMATION
        CHECK( addVariable ("PMT1_energy",           m_pmt1Energy,    "",0.));
        CHECK( addVariable ("PMT1_time",             m_pmt1Time,      "",0.));
        CHECK( addVariable ("PMT1_quality",          m_pmt1Quality,   "",0));
        CHECK( addVariable ("PMT1_qbit",             m_pmt1Qbit,      "",0));
        CHECK( addVariable ("PMT1_bad",              m_pmt1Bad,       "",false));
        CHECK( addVariable ("PMT1_gain",             m_pmt1Gain,      "",0));
        CHECK( addVariable ("PMT1_ros",              m_pmt1Ros,       "",-1));
        CHECK( addVariable ("PMT1_drawer",           m_pmt1Drawer,    "",-1));
        CHECK( addVariable ("PMT1_channel",          m_pmt1Channel,   "",-1));

        CHECK( addVariable ("PMT2_energy",           m_pmt2Energy,    "",0.));
        CHECK( addVariable ("PMT2_time",             m_pmt2Time,      "",0.));
        CHECK( addVariable ("PMT2_quality",          m_pmt2Quality,   "",0));
        CHECK( addVariable ("PMT2_qbit",             m_pmt2Qbit,      "",0));
        CHECK( addVariable ("PMT2_bad",              m_pmt2Bad,       "",false));
        CHECK( addVariable ("PMT2_gain",             m_pmt2Gain,      "",0));
        CHECK( addVariable ("PMT2_ros",              m_pmt2Ros,       "",-1));
        CHECK( addVariable ("PMT2_drawer",           m_pmt2Drawer,    "",-1));
        CHECK( addVariable ("PMT2_channel",          m_pmt2Channel,   "",-1));
    } // IF

    if(m_LevelOfDetails > 4){
        // PATH LENGTH AND CELL RESPONSE
        CHECK( addVariable ("muon_N",                m_muN,   "", 0));
        CHECK( addVariable ("muon_dx",               m_dx));
        CHECK( addVariable ("muon_dEdx",             m_dEdx));
        CHECK( addVariable ("muon_type",             m_mutype));
        CHECK( addVariable ("muon_author",           m_muonAuthor));
        CHECK( addVariable ("muon_ptcone40",         m_muptcone40));
        CHECK( addVariable ("muon_larcone40",        m_mularcone40));
        CHECK( addVariable ("muon_p",                m_mup));
        CHECK( addVariable ("muon_Nsiliconhits",     m_muNsiliconhits));
        CHECK( addVariable ("muon_x",                m_muonX));
        CHECK( addVariable ("muon_y",                m_muonY));
        CHECK( addVariable ("muon_z",                m_muonZ));
        CHECK( addVariable ("muon_phi",              m_muonPhi));
        CHECK( addVariable ("muon_eta",              m_muonEta));
        CHECK( addVariable ("to_muon_dx",            m_to_muon_dx));
        CHECK( addVariable ("to_muon_dy",            m_to_muon_dy));
        CHECK( addVariable ("to_muon_dz",            m_to_muon_dz));
        CHECK( addVariable ("to_muon_dPhi",          m_to_muon_dPhi));
        CHECK( addVariable ("to_muon_dEta",          m_to_muon_dEta));
    } // IF
    
    return StatusCode::SUCCESS;
} // TileCellFillerTool::book

//==========================================================
StatusCode TileCellFillerTool::fill (const CaloCell& p){
//==========================================================
    ATH_MSG_DEBUG("TileCellFillerTool::fill()");
 
    const CaloDetDescrElement *dde = p.caloDDE();

    if(m_LevelOfDetails > 0){
        // DEPOSITED ENERGY
        *m_cellEnergy                  = p.energy();
        *m_cellEt                      = p.et();

        // CELL POSITION
        *m_cellEta                     = p.eta();
        *m_cellPhi                     = p.phi();
    } // IF
   
    if(m_LevelOfDetails > 1){
      	*m_cellGain                    = p.gain();
        *m_badCell                     = p.badcell();
        *m_time                        = p.time();
        *m_quality                     = p.provenance();
        *m_samplingID                  = dde->getSampling();
        *m_subcalo                     = dde->getSubCalo();
        
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

       
        // RAW CHANNEL INFORMATION
        const CaloCell* cell=&p;
        const TileCell* tilecell = dynamic_cast<const TileCell*> (cell);

        if(tilecell){
            *m_side                    = m_tileID->side(cell->ID());
            *m_section                 = m_tileID->section(cell->ID());
            *m_module                  = m_tileID->module(cell->ID());
            *m_tower                   = m_tileID->tower(cell->ID());
            *m_sample                  = m_tileID->sample(cell->ID());
            *m_eta_index               = m_trackInCalo->retrieveIndex(*m_samplingID,*m_cellEta);
	    
            if(m_LevelOfDetails > 3){
                *m_pmt1Energy          = tilecell->ene1();
                *m_pmt2Energy          = tilecell->ene2();
                *m_pmt1Time            = tilecell->time1();
                *m_pmt2Time            = tilecell->time2();
                *m_pmt1Quality         = tilecell->qual1();
                *m_pmt2Quality         = tilecell->qual2();
                *m_pmt1Qbit            = tilecell->qbit1();
                *m_pmt2Qbit            = tilecell->qbit2();
                *m_pmt1Bad             = tilecell->badch1();
                *m_pmt2Bad             = tilecell->badch2();
                *m_pmt1Gain            = tilecell->gain1();
                *m_pmt2Gain            = tilecell->gain2();

                *m_pmt1Ros             = m_tileHWID->ros(m_tileHWID->adc_id(dde->onl1(),tilecell->gain1()));
                *m_pmt2Ros             = m_tileHWID->ros(m_tileHWID->adc_id(dde->onl2(),tilecell->gain2()));
                *m_pmt1Drawer          = m_tileHWID->drawer(m_tileHWID->adc_id(dde->onl1(),tilecell->gain1()));
                *m_pmt2Drawer          = m_tileHWID->drawer(m_tileHWID->adc_id(dde->onl2(),tilecell->gain2()));
                *m_pmt1Channel         = m_tileHWID->channel(m_tileHWID->adc_id(dde->onl1(),tilecell->gain1()));
                *m_pmt2Channel         = m_tileHWID->channel(m_tileHWID->adc_id(dde->onl2(),tilecell->gain2()));
            } // IF
        } // ELSE
    } // IF
    
    if(m_LevelOfDetails > 4){
        // RETRIEVE APPROPRIATE POINTERS
        // GET HANDLE ON MUON CONTAINER
        const MUONCONTAINER* muons = 0;

        // MUONS
        CHECK( m_storeGate->retrieve( muons, m_muonContainerName ) );

        MUONCONTAINER::const_iterator muonItr = muons->begin();
        MUONCONTAINER::const_iterator muonEnd = muons->end();
        for(; muonItr != muonEnd; ++muonItr ){
          const MUON* muon = *muonItr;
          // MUON COUNTER AND TYPES
          *m_muN = *m_muN + 1;
          m_mutype->push_back(m_trackType); // NO MUON
          m_muonAuthor->push_back( muon->author() );
     
          /// primaryTrackParticle() Returns a pointer (which should not usually be NULL, but might be if the muon has been stripped of information) to the
          /// primary TrackParticle corresponding to the MuonType of this muon.
          /// This is determined in the following order:
          ///  1. CombinedTrackParticle
          ///  2. InnerDetectorTrackParticle
          ///  3. MuonSpectrometerTrackParticle
          const TRACK* muon_track = 0;
          switch(m_trackType)
          {
            case 0:  muon_track = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);          break;
            case 1:  muon_track = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);     break;
            case 2:  muon_track = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);  break;
            default: muon_track = muon->primaryTrackParticle();                                    break;
          }  // SWITCH
          if(!muon_track) continue;
      
          std::vector<double> coordinates = m_trackInCalo->getXYZEtaPhiInCellSampling(muon_track,&p);

          if(coordinates.size() == 5 )
          {
            // CALCULATE PATH LENGTHS USING EXTRAPOLATOR TOOL
            float path =  m_trackInCalo->getPathInsideCell(muon_track,&p);
            float ptcone40 = 0.;
            if (!muon->isolation(ptcone40, Iso::IsolationType::ptcone40 ))
              ptcone40 = 0;
            m_muptcone40    ->push_back(ptcone40);
            m_mularcone40   ->push_back(0.); //FIXME
            m_mup           ->push_back(muon->p4().P());

            uint8_t NumberOfPixelHits=0;
            uint8_t NumberOfSCTHits=0;
            if( !muon->summaryValue(NumberOfPixelHits,xAOD::numberOfPixelHits) )
            {
              ATH_MSG_INFO("Can not get the value of numberOfPixelHits");
            }
            if( !muon->summaryValue(NumberOfSCTHits,xAOD::numberOfSCTHits) )
            {
              ATH_MSG_INFO("Can not get the value of numberOfSCTHits");
            }

            m_muNsiliconhits->push_back( int(NumberOfPixelHits + NumberOfSCTHits) );

            m_muonX  ->push_back(coordinates[0]);
            m_muonY  ->push_back(coordinates[1]);
            m_muonZ  ->push_back(coordinates[2]);
            m_muonEta->push_back(coordinates[3]);
            m_muonPhi->push_back(coordinates[4]);

            m_to_muon_dx  ->push_back(*m_cellX   - coordinates[0]);
            m_to_muon_dy  ->push_back(*m_cellY   - coordinates[1]);
            m_to_muon_dz  ->push_back(*m_cellZ   - coordinates[2]);
            m_to_muon_dEta->push_back(*m_cellEta - coordinates[3]);
            m_to_muon_dPhi->push_back( KinematicUtils::deltaPhi(coordinates[4],*m_cellPhi) );

            m_dx->push_back(path);
            m_dEdx->push_back(path > 0 ? p.energy() / path : -1.);
          } // IF
        } // FOR
    } // IF
    
    return StatusCode::SUCCESS;
} // TileCellFillerTool::fill

} // namespace D3PD
