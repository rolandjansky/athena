/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileTrackFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>, Archil Durglishvili <archil.durglishvili@cern.ch>
 * Date: Nov 2014
 *
 */

// INCLUDE HEADER FILE
#include "TileTrackFillerTool.h"

//xAOD tools
#include "xAODPrimitives/IsolationType.h"

// ADD TILETRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{
//=========================================================================
TileTrackFillerTool::TileTrackFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
: D3PD::BlockFillerTool<xAOD::TrackParticle>(type,name,parent),
m_trackToVertexTool("Reco::TrackToVertex"){
    //=========================================================================
    
    book().ignore(); // AVOID COVERITY WARNINGS
    declareProperty("LevelOfDetails",        m_LevelOfDetails);
    declareProperty("CellsName",             m_calocellContainerName="SelectedCells" );
    declareProperty("TrackTools", m_trackInCalo);
    declareProperty("TrackToVertexTool",     m_trackToVertexTool, "tool to map track to vertex");     
    declareProperty("TrackParType",          m_trkParType = 1);

} // TileTrackFillerTool::TileTrackFillerTool

//===============================================
StatusCode TileTrackFillerTool::initialize(){
    //===============================================
    // RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES

    ATH_MSG_DEBUG("TILETRACKERFILLERTOOL::INITIALIZE()");    

    // CHECK WHETHER BLOCKFILLERTOOL HAS BEEN INITIALIZED
    CHECK( D3PD::BlockFillerTool<xAOD::TrackParticle>::initialize());
    
    // RETRIEVE STOREGATE SERVICE
    CHECK( service("StoreGateSvc",m_storeGate));

    CHECK(m_trackInCalo.retrieve()); 

    CHECK(m_trackToVertexTool.retrieve()); 
   
    return StatusCode::SUCCESS;    
} // TileTrackFillerTool::initialize

//=========================================
StatusCode TileTrackFillerTool::book(){
    //=========================================
    // ADDS VARIABLES TO THE D3PD TREE
    
    // OUTPUT MESSAGING SERVICE
    ATH_MSG_DEBUG("TileTrackFillerTool::book()");
    
    if (m_LevelOfDetails > 0){
       
        CHECK( addVariable("pt",                               m_pt,                               "", m_defaultValue));
        CHECK( addVariable("p",                                m_p,                                "", m_defaultValue));
      if(m_trkParType) 
      {
        CHECK( addVariable("eta_layer",                        m_eta                                                 ));
        CHECK( addVariable("phi_layer",                        m_phi                                                 ));
      } else
      {
        CHECK( addVariable("eta_sampling",                     m_eta                                                 ));
        CHECK( addVariable("phi_sampling",                     m_phi                                                 ));
      }

        CHECK( addVariable("numberOfPixelHits",                m_numberOfPixelHits,                "", m_defaultValue));
        CHECK( addVariable("numberOfSCTHits",                  m_numberOfSCTHits,                  "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHits",                  m_numberOfTRTHits,                  "", m_defaultValue));
        
        CHECK( addVariable("d0",                               m_d0,                               "", m_defaultValue));
        CHECK( addVariable("z0",                               m_z0,                               "", m_defaultValue));       
    } // IF
    
    if (m_LevelOfDetails > 1){
     
      if(m_trkParType)
      {
        CHECK( addVariable("x_layer",                        m_x                                                 ));
        CHECK( addVariable("y_layer",                        m_y                                                 ));
        CHECK( addVariable("z_layer",                        m_z                                                 ));
      } else
      {
        CHECK( addVariable("x_sampling",                     m_x                                                 ));
        CHECK( addVariable("y_sampling",                     m_y                                                 ));
        CHECK( addVariable("z_sampling",                     m_z                                                 ));
      }

        CHECK( addVariable("vtxX",                             m_vtxX,                             "", m_defaultValue));
        CHECK( addVariable("vtxY",                             m_vtxY,                             "", m_defaultValue));
        CHECK( addVariable("vtxZ",                             m_vtxZ,                             "", m_defaultValue));
        CHECK( addVariable("phi0",                             m_phi0,                             "", m_defaultValue));
        CHECK( addVariable("theta",                            m_theta,                            "", m_defaultValue));
        CHECK( addVariable("qOverP",                           m_qOverP,                           "", m_defaultValue));
    }

    if (m_LevelOfDetails > 2){
        CHECK( addVariable("nucone05",                         m_nucone05,                         "", m_defaultValue));
        CHECK( addVariable("nucone10",                         m_nucone10,                         "", m_defaultValue));
        CHECK( addVariable("nucone20",                         m_nucone20,                         "", m_defaultValue));
        CHECK( addVariable("nucone30",                         m_nucone30,                         "", m_defaultValue));
        CHECK( addVariable("nucone40",                         m_nucone40,                         "", m_defaultValue));

        CHECK( addVariable("ptcone10",                         m_ptcone10,                         "", m_defaultValue));
        CHECK( addVariable("ptcone20",                         m_ptcone20,                         "", m_defaultValue));
        CHECK( addVariable("ptcone30",                         m_ptcone30,                         "", m_defaultValue));
        CHECK( addVariable("ptcone40",                         m_ptcone40,                         "", m_defaultValue));
        CHECK( addVariable("ptcone45",                         m_ptcone45,                         "", m_defaultValue));
    }

    if (m_LevelOfDetails > 3){
        CHECK( addVariable("numberOfInnermostPixelLayerHits",               m_numberOfBLayerHits,               "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHighThresholdHits",     m_numberOfTRTHighThresholdHits,     "", m_defaultValue));
        CHECK( addVariable("numberOfInnermostPixelLayerSharedHits",         m_numberOfBLayerSharedHits,         "", m_defaultValue));
        CHECK( addVariable("numberOfPixelSharedHits",          m_numberOfPixelSharedHits,          "", m_defaultValue));
        CHECK( addVariable("numberOfPixelHoles",               m_numberOfPixelHoles,               "", m_defaultValue));
        CHECK( addVariable("numberOfSCTSharedHits",            m_numberOfSCTSharedHits,            "", m_defaultValue));
        CHECK( addVariable("numberOfSCTHoles",                 m_numberOfSCTHoles,                 "", m_defaultValue));
        CHECK( addVariable("numberOfTRTOutliers",              m_numberOfTRTOutliers,              "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHighThresholdOutliers", m_numberOfTRTHighThresholdOutliers, "", m_defaultValue));

        CHECK( addVariable("numberOfGangedPixels",             m_numberOfGangedPixels,             "", m_defaultValue));
        CHECK( addVariable("numberOfOutliersOnTrack",          m_numberOfOutliersOnTrack,          "", m_defaultValue));
        CHECK( addVariable("numberOfTrackSummaryTypes",        m_numberOfTrackSummaryTypes,        "", m_defaultValue));
    }

    return StatusCode::SUCCESS;
} // TileTrackFillerTool::book

//================================================================
StatusCode TileTrackFillerTool::fill(const xAOD::TrackParticle& p){
    //================================================================
    // OUTPUT MESSAGING SERVICE
    ATH_MSG_DEBUG("TileTrackFillerTool::fill");
    
    // Get Track information
    const xAOD::TrackParticle* trackPointer = &p;
    
    if(!trackPointer) { ATH_MSG_INFO("NULL POINTER"); return StatusCode::RECOVERABLE; }

    
    // STORE TRACK PARAMETERS
    if(m_LevelOfDetails > 0){
        *m_pt = trackPointer->pt();
        *m_p = trackPointer->p4().P();

        std::vector< std::vector<double> > positions = (m_trkParType) ?  m_trackInCalo->getXYZEtaPhiPerLayer(trackPointer) :
                                                                         m_trackInCalo->getXYZEtaPhiPerSampling(trackPointer);
       
        if( (m_trkParType) ? (positions.size() == 11) : (positions.size() == 21) ){
            for(uint i=0;i<positions.size();++i){
              if(m_LevelOfDetails > 1)
              {
                m_x->push_back( (positions[i].size()!=5) ? -9999. : positions[i][0] );
                m_y->push_back( (positions[i].size()!=5) ? -9999. : positions[i][1] );
                m_z->push_back( (positions[i].size()!=5) ? -9999. : positions[i][2] );
              }
              m_eta->push_back( (positions[i].size()!=5) ? -9999. : positions[i][3] );
              m_phi->push_back( (positions[i].size()!=5) ? -9999. : positions[i][4] );
            }
	} // IF

        trackPointer->summaryValue( *((uint8_t*)m_numberOfPixelHits), xAOD::numberOfPixelHits );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfSCTHits), xAOD::numberOfSCTHits );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfTRTHits), xAOD::numberOfTRTHits );
  
        *m_d0                               = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::d0];
        *m_z0                               = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::d0];
    }

    if(m_LevelOfDetails > 1){
        *m_vtxX                             = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::x];
        *m_vtxY                             = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::y];
        *m_vtxZ                             = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::z];
        *m_phi0                             = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::phi0];
        *m_theta                            = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::theta];
        *m_qOverP                           = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::qOverP];
    }

    if(m_LevelOfDetails > 2){        

        std::vector<xAOD::Iso::IsolationType> ptcones = {xAOD::Iso::ptcone40,xAOD::Iso::ptcone30,
                                                         xAOD::Iso::ptcone20 };
        xAOD::TrackIsolation trackIsolation;
        if( !m_trackInCalo->trackIsolation(trackIsolation, *trackPointer, ptcones) )
        {
          ATH_MSG_WARNING("Calculation of TrackIsolation failed");
        }
        else
        {
          *m_ptcone40 = trackIsolation.ptcones[0];
          *m_ptcone30 = trackIsolation.ptcones[1];
          *m_ptcone20 = trackIsolation.ptcones[2];
        }

        /* THIS IS GOING TO BE USED
        std::vector<xAOD::Iso::IsolationType> nucones = { xAOD::Iso::neflowisol10,xAOD::Iso::neflowisol15,xAOD::Iso::neflowisol20,
                                                          xAOD::Iso::neflowisol25,xAOD::Iso::neflowisol30,xAOD::Iso::neflowisol35,
                                                          xAOD::Iso::neflowisol40,xAOD::Iso::neflowisol45 };
        xAOD::CaloIsolation caloIsolation;
        if( !m_trackInCalo->neutralEflowIsolation(caloIsolation, *trackPointer, nucones, xAOD::ICaloIsolationTool::Ecore) )
        { 
          ATH_MSG_WARNING("Calculation of CaloIsolation failed");
        }
        else
        { 
           ;
        }
        */
    } // IF
    
    if(m_LevelOfDetails > 3){
        trackPointer->summaryValue( *((uint8_t*)m_numberOfBLayerHits), xAOD::numberOfInnermostPixelLayerHits );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfTRTHighThresholdHits), xAOD::numberOfTRTHighThresholdHits );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfBLayerSharedHits), xAOD::numberOfInnermostPixelLayerSharedHits );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfPixelSharedHits), xAOD::numberOfPixelSharedHits );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfPixelHoles), xAOD::numberOfPixelHoles );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfSCTSharedHits), xAOD::numberOfSCTSharedHits );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfPixelHoles), xAOD::numberOfPixelHoles );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfSCTSharedHits), xAOD::numberOfSCTSharedHits );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfSCTHoles), xAOD::numberOfSCTHoles );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfTRTOutliers), xAOD::numberOfTRTOutliers );
        trackPointer->summaryValue( *((uint8_t*)m_numberOfTRTHighThresholdOutliers), xAOD::numberOfTRTHighThresholdOutliers );
	trackPointer->summaryValue( *((uint8_t*)m_numberOfGangedPixels), xAOD::numberOfGangedPixels);
        trackPointer->summaryValue( *((uint8_t*)m_numberOfOutliersOnTrack), xAOD::numberOfOutliersOnTrack);
    }

    return StatusCode::SUCCESS;
} // TileTrackFillerTool::fill
} // namespace D3PD
