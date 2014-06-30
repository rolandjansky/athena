/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileTrackFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * Date: May 2013
 *
 */

// INCLUDE HEADER FILE
#include "TileTrackFillerTool.h"

// ADD TILETRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{
//=========================================================================
TileTrackFillerTool::TileTrackFillerTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
: D3PD::BlockFillerTool<Rec::TrackParticle>(type,name,parent),
m_track_iso_tool("TrackIsolationTool"),
m_trackToVertexTool("Reco::TrackToVertex"){
    //=========================================================================
    
    book().ignore(); // AVOID COVERITY WARNINGS
    declareProperty("track_iso_tool",        m_track_iso_tool );
    declareProperty("LevelOfDetails",        m_LevelOfDetails);
    declareProperty("CellsName",             m_calocellContainerName="SelectedCells" );
    declareProperty("ExtrapolateInCaloTool", m_inCalo);
    declareProperty("TrackToVertexTool",     m_trackToVertexTool, "tool to map track to vertex");
    
} // TileTrackFillerTool::TileTrackFillerTool

//===============================================
StatusCode TileTrackFillerTool::initialize(){
    //===============================================
    // RETRIEVES NECESSARY SERVICES AND TOOLS, INSTANTIATES GLOBAL VARIABLES
    
    // CHECK WHETHER BLOCKFILLERTOOL HAS BEEN INITIALIZED
    CHECK( D3PD::BlockFillerTool<Rec::TrackParticle>::initialize());
    
    // OUTPUT MESSAGING SERVICE
    ATH_MSG_DEBUG(" NOW IN TILETRACKERFILLERTOOL::INITIALIZE() ");
    
    // RETRIEVE STOREGATE SERVICE
    CHECK( service("StoreGateSvc",m_storeGate));
    CHECK(m_trackToVertexTool.retrieve());
    
    ATH_MSG_ERROR(" NOW IN TILETRACKERFILLERTOOL::INITIALIZE() ");
    
    return StatusCode::SUCCESS;
    
} // TileTrackFillerTool::initialize

//=========================================
StatusCode TileTrackFillerTool::book(){
    //=========================================
    // ADDS VARIABLES TO THE D3PD TREE
    
    // OUTPUT MESSAGING SERVICE
    ATH_MSG_DEBUG(" NOW IN TileTrackFillerTool::book() ");
    
    // EVENT FLAGS AND VARIABLES
    /*
    if (m_LevelOfDetails > 0){
        CHECK( addVariable("nucone05",                         m_nucone05,                         "", m_defaultValue));
        CHECK( addVariable("nucone10",                         m_nucone10,                         "", m_defaultValue));
        CHECK( addVariable("nucone20",                         m_nucone20,                         "", m_defaultValue));
        CHECK( addVariable("nucone30",                         m_nucone30,                         "", m_defaultValue));
        CHECK( addVariable("nucone40",                         m_nucone40,                         "", m_defaultValue));
        
        CHECK( addVariable("ptcone05",                         m_ptcone05,                         "", m_defaultValue));
        CHECK( addVariable("ptcone10",                         m_ptcone10,                         "", m_defaultValue));
        CHECK( addVariable("ptcone20",                         m_ptcone20,                         "", m_defaultValue));
        CHECK( addVariable("ptcone30",                         m_ptcone30,                         "", m_defaultValue));
        CHECK( addVariable("ptcone40",                         m_ptcone40,                         "", m_defaultValue));
        
        CHECK( addVariable("pt",                               m_pt,                               "", m_defaultValue));
        CHECK( addVariable("x_layer",                          m_x                             ));
        CHECK( addVariable("y_layer",                          m_y                             ));
        CHECK( addVariable("z_layer",                          m_z                             ));
        CHECK( addVariable("eta_layer",                        m_eta                           ));
        CHECK( addVariable("phi_layer",                        m_phi                           ));
        
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
        
        //CHECK( addVariable("numberOfFriendTracks", m_numberOfFriendTracks, "", m_defaultValue));
        CHECK( addVariable("numberOfGangedPixels",             m_numberOfGangedPixels,             "", m_defaultValue));
        CHECK( addVariable("numberOfOutliersOnTrack",          m_numberOfOutliersOnTrack,          "", m_defaultValue));
        CHECK( addVariable("numberOfTrackSummaryTypes",        m_numberOfTrackSummaryTypes,        "", m_defaultValue));
        
        CHECK( addVariable("vtxX",                             m_vtxX,                             "", m_defaultValue));
        CHECK( addVariable("vtxY",                             m_vtxY,                             "", m_defaultValue));
        CHECK( addVariable("vtxZ",                             m_vtxZ,                             "", m_defaultValue));
        CHECK( addVariable("d0",                               m_d0,                               "", m_defaultValue));
        CHECK( addVariable("z0",                               m_z0,                               "", m_defaultValue));
        CHECK( addVariable("phi0",                             m_phi0,                             "", m_defaultValue));
        CHECK( addVariable("theta",                            m_theta,                            "", m_defaultValue));
        CHECK( addVariable("qOverP",                           m_qOverP,                           "", m_defaultValue));
    } // IF
*/
if (m_LevelOfDetails > 0){
       
        
        CHECK( addVariable("pt",                               m_pt,                               "", m_defaultValue));
       
        CHECK( addVariable("eta_layer",                        m_eta                           ));
        CHECK( addVariable("phi_layer",                        m_phi                           ));
        
    
        CHECK( addVariable("numberOfPixelHits",                m_numberOfPixelHits,                "", m_defaultValue));
        CHECK( addVariable("numberOfSCTHits",                  m_numberOfSCTHits,                  "", m_defaultValue));
        CHECK( addVariable("numberOfTRTHits",                  m_numberOfTRTHits,                  "", m_defaultValue));
        
       
        CHECK( addVariable("d0",                               m_d0,                               "", m_defaultValue));
        CHECK( addVariable("z0",                               m_z0,                               "", m_defaultValue));
       
       
    } // IF
    
    return StatusCode::SUCCESS;
} // TileTrackFillerTool::book

//================================================================
StatusCode TileTrackFillerTool::fill(const Rec::TrackParticle& p){
    //================================================================
    // OUTPUT MESSAGING SERVICE
    ATH_MSG_DEBUG("in TileTrackFillerTool::fill");
    
    // Get Track information
    const Rec::TrackParticle* trackPointer = &p;
    
    if(!trackPointer) ATH_MSG_ERROR("NULL POINTER");
    
    // STORE TRACK PARAMETERS
    if(m_LevelOfDetails > 0){
        *m_pt = trackPointer->pt();
        
	//  *m_numberOfBLayerHits               = trackPointer->trackSummary()->get(Trk::numberOfBLayerHits);
        *m_numberOfPixelHits                = trackPointer->trackSummary()->get(Trk::numberOfPixelHits);
        *m_numberOfSCTHits                  = trackPointer->trackSummary()->get(Trk::numberOfSCTHits);
        *m_numberOfTRTHits                  = trackPointer->trackSummary()->get(Trk::numberOfTRTHits);
	//   *m_numberOfTRTHighThresholdHits     = trackPointer->trackSummary()->get(Trk::numberOfTRTHighThresholdHits);
	// *m_numberOfBLayerSharedHits         = trackPointer->trackSummary()->get(Trk::numberOfBLayerSharedHits);
	// *m_numberOfPixelSharedHits          = trackPointer->trackSummary()->get(Trk::numberOfPixelSharedHits);
	//  *m_numberOfPixelHoles               = trackPointer->trackSummary()->get(Trk::numberOfPixelHoles);
	// *m_numberOfSCTSharedHits            = trackPointer->trackSummary()->get(Trk::numberOfSCTSharedHits);
	// *m_numberOfSCTHoles                 = trackPointer->trackSummary()->get(Trk::numberOfSCTHoles);
	// *m_numberOfTRTOutliers              = trackPointer->trackSummary()->get(Trk::numberOfTRTOutliers);
	// *m_numberOfTRTHighThresholdOutliers = trackPointer->trackSummary()->get(Trk::numberOfTRTHighThresholdOutliers);
        
	// *m_vtxX                             = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::x];
	// *m_vtxY                             = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::y];
	// *m_vtxZ                             = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::z];
        *m_d0                               = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::d0];
        *m_z0                               = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::z0];
	// *m_phi0                             = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::phi0];
	// *m_theta                            = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::theta];
	// *m_qOverP                           = m_trackToVertexTool->perigeeAtVertex(*trackPointer)->parameters()[Trk::qOverP];
        /*
        *m_numberOfGangedPixels = trackPointer->trackSummary()->get(Trk::numberOfGangedPixels);
        *m_numberOfOutliersOnTrack = trackPointer->trackSummary()->get(Trk::numberOfOutliersOnTrack);
        
        *m_nucone05 = m_track_iso_tool->trackIsolationNumberOfTrack(trackPointer, 0.05);
        *m_nucone10 = m_track_iso_tool->trackIsolationNumberOfTrack(trackPointer, 0.1);
        *m_nucone20 = m_track_iso_tool->trackIsolationNumberOfTrack(trackPointer, 0.2);
        *m_nucone30 = m_track_iso_tool->trackIsolationNumberOfTrack(trackPointer, 0.3);
        *m_nucone40 = m_track_iso_tool->trackIsolationNumberOfTrack(trackPointer, 0.4);
        
        *m_ptcone05 = m_track_iso_tool->trackIsolationEnergy(trackPointer, 0.05);
        *m_ptcone10 = m_track_iso_tool->trackIsolationEnergy(trackPointer, 0.1);
        *m_ptcone20 = m_track_iso_tool->trackIsolationEnergy(trackPointer, 0.2);
        *m_ptcone30 = m_track_iso_tool->trackIsolationEnergy(trackPointer, 0.3);
        *m_ptcone40 = m_track_iso_tool->trackIsolationEnergy(trackPointer, 0.4);
	*/
    } // IF
    
    // STORE LAYER INFORMATION
    // if(m_LevelOfDetails > 5){
    if(m_LevelOfDetails > 0){
        std::vector< std::vector<double> > positions = m_inCalo->getXYZEtaPhiPerLayer(trackPointer);
        if(positions.size() == 11){
            for(uint i=0;i<positions.size();++i){
	      // m_x->push_back(positions[i][0] ? positions[i][0] : -9999.);
              //  m_y->push_back(positions[i][1] ? positions[i][1] : -9999.);
	      // m_z->push_back(positions[i][2] ? positions[i][2] : -9999.);
                m_eta->push_back(positions[i][3] ? positions[i][3] : -9999.);
                m_phi->push_back(positions[i][4] ? positions[i][4] : -9999.);
            }
        } // IF
    }
    
    return StatusCode::SUCCESS;
} // TileTrackFillerTool::fill
} // namespace D3PD
