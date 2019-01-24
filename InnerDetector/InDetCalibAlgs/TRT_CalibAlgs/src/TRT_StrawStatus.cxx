/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_StrawStatus.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include "TRT_CalibAlgs/TRT_StrawStatus.h"
#include "AthContainers/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "xAODEventInfo/EventInfo.h"

#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TRT_ConditionsServices/ITRT_HWMappingSvc.h"
#include "TRT_ConditionsServices/ITRT_DCS_ConditionsSvc.h"

#include "VxVertex/VxContainer.h"

#include "TRT_TrackHoleSearch/TRTTrackHoleSearchTool.h"
#include "TrkParameters/TrackParameters.h"

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"

#include "StoreGate/ReadHandle.h"

int last_lumiBlock0=-99;

const size_t nBarrelStraws { 1642 };
const size_t nEndcapStraws { 3840 };
const size_t nAllStraws    { nBarrelStraws + nEndcapStraws };

const size_t nBarrelBoards { 9 };
const size_t nEndcapBoards { 20 };
const size_t nAllBoards    { nBarrelBoards + nEndcapBoards };

const size_t nBarrelChips  { 104 };
const size_t nEndcapChips  { 240 };
const size_t nAllChips     { nBarrelChips + nAllChips };

const size_t nBarrelPads   { 2 * nBarrelChips };
const size_t nEndcapPads   { 2 * nEndcapChips }; 
const size_t nAllPads      { 2 * nAllChips }; 

//================ Constructor =================================================

InDet::TRT_StrawStatus::TRT_StrawStatus(const std::string& name, ISvcLocator* pSvcLocator)
:
AthAlgorithm(name,pSvcLocator),
m_nEvents(0), m_runNumber(0),
m_accumulateHits(0),
m_TRTHelper(0),
m_mapSvc("TRT_HWMappingSvc",name),
m_DCSSvc("TRT_DCS_ConditionsSvc",name),
m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc", name), // use this service to retrieve barrel and end-cap straw number later on, as well as DTMROC,.. 
m_TRTStrawStatusSummarySvc("TRT_StrawStatusSummarySvc", name),
m_trt_hole_finder("TRTTrackHoleSearchTool"),
m_locR_cut(1.4),
m_fileName("TRT_StrawStatusOutput"),
m_skipBusyEvents(0), // for cosmics - reject events that are either showers or noise bursts
m_printDetailedInformation(0) // print the information on mapping as well as which straws are declared dead etc. 
{
    declareProperty("outputFileName", m_fileName);
    declareProperty("skipBusyEvents", m_skipBusyEvents);        
    declareProperty("trt_hole_finder",          m_trt_hole_finder);
    declareProperty("HWMapSvc", m_mapSvc);
    declareProperty("InDetTRT_DCS_ConditionsSvc",m_DCSSvc);
    declareProperty("locR_cut",                 m_locR_cut );
    declareProperty("printDetailedInformation", m_printDetailedInformation);

}

//================ Destructor =================================================

InDet::TRT_StrawStatus::~TRT_StrawStatus()
{}


//================ Initialisation =================================================

StatusCode InDet::TRT_StrawStatus::initialize()
{

  m_accumulateHits = new ACCHITS_t;
  assert( (*m_accumulateHits)[0][0].size() == nAllStraws );
  clear();
  
  // Code entered here will be executed once at program start.
  // Initialize ReadHandleKey
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_tracksName.initialize());
  ATH_CHECK(m_vxContainerKey.initialize());
  ATH_CHECK( detStore()->retrieve(m_TRTHelper, "TRT_ID"));
  ATH_CHECK( m_TRTStrawNeighbourSvc.retrieve()) ;
  ATH_CHECK( m_trt_hole_finder.retrieve() );
  ATH_CHECK(m_updator.retrieve());
  ATH_CHECK( m_mapSvc.retrieve());
  ATH_CHECK( m_DCSSvc.retrieve());
  ATH_MSG_DEBUG( "initialize() successful in " << name() << ", retrieved: ..., locR_cut = " << m_locR_cut << " mm." );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::TRT_StrawStatus::finalize(){
    reportResults();
    if (m_printDetailedInformation) printDetailedInformation();
    // Code entered here will be executed once at the end of the program run.
    delete m_accumulateHits;
    return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode InDet::TRT_StrawStatus::execute(){
  
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
    StatusCode sc = StatusCode::SUCCESS;
    if (not eventInfo.isValid()) {
      ATH_MSG_ERROR( "Unable to retrieve Event Info " );
      return StatusCode::FAILURE;
    } 
    int runNumber = (int) eventInfo->runNumber();  
    if (runNumber != m_runNumber) {
      if (m_nEvents) { reportResults(); clear(); }
      m_runNumber = runNumber;
    } 
    int lumiBlock0 =eventInfo->lumiBlock();
    SG::ReadHandle<TRT_RDO_Container> rdoContainer(m_rdoContainerKey);
  
    if (not rdoContainer.isValid()) {
      ATH_MSG_ERROR( "no TRT_RDO container available " );
      return StatusCode::FAILURE;
    }
    SG::ReadHandle<DataVector<Trk::Track>> trkCollection(m_tracksName);
    if (not trkCollection.isValid()) {
      ATH_MSG_ERROR( "Could not find Tracks Collection: " << m_tracksName );
      return StatusCode::FAILURE;
    }   
    
    //================ Event selection  
    
    SG::ReadHandle<VxContainer> vxContainer(m_vxContainerKey);
    if (not vxContainer.isValid()) { 
      ATH_MSG_ERROR( "vertex container missing!" );
      sc = StatusCode::FAILURE;
    } else {
        int countVertices(0);
        for (VxContainer::const_iterator it = vxContainer->begin() ; it != vxContainer->end() ; ++it ) {
            if ( (*it)->vxTrackAtVertex()->size() >= 3 ) countVertices++;
        }
        if (countVertices < 1)  return StatusCode::FAILURE; 
    }
    
    if (m_skipBusyEvents) { // cosmic running
        int countRDOhitsInEvent(0);
        for (TRT_RDO_Container::const_iterator rdoIt = rdoContainer->begin(); rdoIt != rdoContainer->end(); ++rdoIt) {
            const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
            if (not TRTCollection) continue;
            for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); ++trtIt) {
                countRDOhitsInEvent++;
            }
        }
        if (countRDOhitsInEvent>100000) {
            ATH_MSG_INFO( "N RDO hits in event greater than 100000: " << countRDOhitsInEvent << ", exiting" );
            return sc;
        }
        
        if (trkCollection->size() > 10) {
            ATH_MSG_INFO( "N tracks greater than 10: " << trkCollection->size() << ", exiting" );
            return sc;
        }
    }
    
    //================ End event selection         
    
    //================ Loop over all tracks, accumulate hits on track, also find holes on track
    
    std::vector<Identifier> holeIdentifiers;
    std::vector<Identifier> holeIdentifiersWithHits; // holes on straws that have hits, it is just that the hit was not associalted to a track
    for ( DataVector<Trk::Track>::const_iterator trackIt = trkCollection->begin(); trackIt != trkCollection->end(); trackIt++ ) {
        const Trk::Track *track = *trackIt;
        //=== select track
        const Trk::Perigee* perigee = (*trackIt)->perigeeParameters();
        if ( not perigee  ) { ATH_MSG_ERROR( "Trk::Perigee missing" ); continue; }
        if ( std::fabs(perigee->pT())/CLHEP::GeV < 1. ) continue; // 1 GeV pT cut
        
        const DataVector<const Trk::TrackStateOnSurface>* trackStates = (**trackIt).trackStateOnSurfaces();
        if ( not trackStates  ) { ATH_MSG_ERROR( "Trk::TrackStateOnSurface empty" ); continue; }
        
        int n_pixel_hits(0), n_sct_hits(0), n_trt_hits(0);  // count hits, require minimal number of all hits 
        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); trackStatesIt++ ) {
            if ( *trackStatesIt == 0 ) { ATH_MSG_ERROR( "*trackStatesIt == 0" ); continue; }

            if ( !((*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement)) ) continue; // this skips outliers
            
            if ( dynamic_cast<const InDet::TRT_DriftCircleOnTrack*> ( (*trackStatesIt)->measurementOnTrack() )  ) n_trt_hits++;
            else if ( dynamic_cast<const InDet::SCT_ClusterOnTrack*>     ( (*trackStatesIt)->measurementOnTrack() )  ) n_sct_hits++;
            else if( dynamic_cast<const InDet::PixelClusterOnTrack*>    ( (*trackStatesIt)->measurementOnTrack() )  ) n_pixel_hits++;
        }           
        if (n_pixel_hits<2 || n_sct_hits < 6 || n_trt_hits<15) continue; // end count hits      
        
        //=== end select track
        
        //=== loop over all hits on track, accumulate them
        
        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); trackStatesIt++ ) {
            
            if ( *trackStatesIt == 0 ) { ATH_MSG_ERROR( "*trackStatesIt == 0" ); continue; }

            if ( !((*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement)) ) continue; // this skips outliers

            const InDet::TRT_DriftCircleOnTrack *driftCircleOnTrack = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() );
            if ( not driftCircleOnTrack ) continue; // not TRT measurement - this way, keep both hits and outliers
            
			const Trk::TrackStateOnSurface& hit = **trackStatesIt;
			
			const Trk::TrackParameters* unbiased_track_parameters = m_updator->removeFromState( *(hit.trackParameters()), hit.measurementOnTrack()->localParameters(), hit.measurementOnTrack()->localCovariance());
			
			double unbiased_locR = unbiased_track_parameters->parameters()[Trk::locR];
			if ( fabs(unbiased_locR) > m_locR_cut ) continue; // same cut as the default hole search cut
    
            const InDet::TRT_DriftCircle *driftCircle = driftCircleOnTrack->prepRawData();
            if ( driftCircle == 0 ) { ATH_MSG_ERROR( "driftCircle == 0" ); continue; }
            
            Identifier id = driftCircle->identify();
            int index[6]; myStrawIndex(id, index); // side, layer, phi, straw_layer, straw_within_layer, straw_index
            (*m_accumulateHits)[(index[0]>0)?0:1][index[2]][index[5]][1]++; // accumulate hits on track
            if (driftCircle->highLevel()) (*m_accumulateHits)[(index[0]>0)?0:1][index[2]][index[5]][3]++; // accumulate hits on track
            
        } // end trackStatesIt loop
        
        // add holeIdentifiers - fill vector
        
        const DataVector<const Trk::TrackStateOnSurface>* holes = m_trt_hole_finder->getHolesOnTrack( *track );
        if ( holes==0 ) continue; // no holes found
        for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = holes->begin(); trackStatesIt != holes->end(); trackStatesIt++ ) {
            
            if ( !(*trackStatesIt)->type(   Trk::TrackStateOnSurface::Hole  )  ) { ATH_MSG_ERROR( "m_trt_hole_finder returned something that is not a hole" ); continue; }
            
            const Trk::TrackParameters* track_parameters = (*trackStatesIt)->trackParameters();
            if (!track_parameters) { ATH_MSG_WARNING( "m_trt_hole_finder track_parameters missing" ); continue; }
            
            Identifier id = track_parameters->associatedSurface().associatedDetectorElementIdentifier();
            if ( !(m_TRTHelper->is_trt(id)) ) { ATH_MSG_ERROR( "m_trt_hole_finder returned something that is not a TRT hole" ); continue; }

            // add se same 1.4 mm locR selection, in case it is not on by default 
            if ( std::fabs( track_parameters->parameters()[Trk::locR] ) > m_locR_cut ) continue;
    
            holeIdentifiers.push_back( id );
        } // end add holeIdentifiers
        
    } // end trackIt loop   
    
    //================ End loop over all tracks
    
    //================ Loop over all hits - it includes hits from dead straws that are masked off in drift circle creation
    
    for (TRT_RDO_Container::const_iterator rdoIt = rdoContainer->begin(); rdoIt != rdoContainer->end(); ++rdoIt) {
        const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
        if (TRTCollection==0) continue;
        for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); trtIt++) {
            Identifier id = (*trtIt)->identify();
            int index[6]; myStrawIndex(id, index); // side, layer, phi, straw_layer, straw_within_layer, straw_index
            (*m_accumulateHits)[(index[0]>0)?0:1][index[2]][index[5]][0]++; // accumulate all hits 
            if ((*trtIt)->highLevel()) (*m_accumulateHits)[(index[0]>0)?0:1][index[2]][index[5]][2]++; // accumulate TR hits
            
            if (std::find(holeIdentifiers.begin(), holeIdentifiers.end(), id) != holeIdentifiers.end())  // a hole was found on the same straw, but hits is there
                holeIdentifiersWithHits.push_back( id );            
        }  
    }
    
    //================ End loop over all hits 

    //================ End loop over all holes, each time also save whether the straw with a hole had a hit
    
    for (unsigned int i=0; i<holeIdentifiers.size(); i++) {
        
        Identifier id = holeIdentifiers[i];

        int index[6]; myStrawIndex(id, index); // side, layer, phi, straw_layer, straw_within_layer, straw_index
        
        (*m_accumulateHits)[(index[0]>0)?0:1][index[2]][index[5]][4]++;
        
        if (std::find(holeIdentifiersWithHits.begin(), holeIdentifiersWithHits.end(), id) != holeIdentifiersWithHits.end())
          (*m_accumulateHits)[(index[0]>0)?0:1][index[2]][index[5]][5]++;
    }
    
    //================ End loop over all hits 

    //===== searching for HV lines with voltage < 1490 V
    if (lumiBlock0 != last_lumiBlock0){
        float theValue;
        int chanNum;
        char fileName_mapping[300]; 
        snprintf(fileName_mapping, 299,"%s.%07d_Voltage_trips.txt", m_fileName.c_str(), m_runNumber);
        FILE *fmapping = fopen(fileName_mapping, "a");
        //StatusCode 
        sc = StatusCode::SUCCESS;//for compatibility with Rel 17
        
        //===== Loop over all HV lines
        for (chanNum=1;chanNum<1281;chanNum++){
            theValue = 9999.;
            std::string folderName = (chanNum <= 640)?"/TRT/DCS/HV/ENDCAPA":"/TRT/DCS/HV/ENDCAPC";
            short int old_chanNum = (chanNum<=640)?chanNum:(chanNum-640);
            sc = m_DCSSvc->getValue(folderName,old_chanNum,theValue);
            if (theValue<1490.){
                fprintf(fmapping,"Low voltage on line %i\n",chanNum);
            } else if (theValue == 9999.){ //if was not updated with last mDCSSvc->getValue(...)
                fprintf(fmapping,"Unknown voltage on line %i\n",chanNum);
            }
        }
        //~scc;//for compatibility with Rel 17
        fclose(fmapping);
    }

    m_nEvents++;
    last_lumiBlock0 = lumiBlock0;
    if (m_nEvents%1000==0 && msgLvl(MSG::DEBUG)) reportResults();
    return sc;
}

//============================================================================================

void InDet::TRT_StrawStatus::clear() {
    m_nEvents = 0;
    *m_accumulateHits = {};
    return;
}

void InDet::TRT_StrawStatus::reportResults() {
    ATH_MSG_INFO( "InDet::TRT_StrawStatus::reportResults() for " << m_nEvents << " events." );
    char fileName[300]; 
    snprintf(fileName, 299,"%s.%07d_newFormat.txt", m_fileName.c_str(), m_runNumber);
    FILE *f = fopen(fileName, "w");
    fprintf(f, "%d %d %d %d %d %d %d %d %d \n", 0, 0, 0, 0, 0, 0, 0, 0, m_nEvents);
    for (size_t i=0; i<2; i++) for (size_t j=0; j<32; j++) for (size_t k=0; k<nAllStraws; k++) {
        int side = (i>0)?-1:1;
        if (k>=1642) side *= 2;
        fprintf(f, "%d %zu %zu", side, j, k);
        for (int m=0; m<6; m++) fprintf(f, " %d", (*m_accumulateHits)[i][j][k][m]);
        fprintf(f, "\n");   
    }
    fclose(f);
    return;
}  

void InDet::TRT_StrawStatus::printDetailedInformation() {
    ATH_MSG_INFO( "InDet::TRT_StrawStatus::printDetailedInformation() " );
    char fileName[300]; 
    snprintf(fileName, 299,"%s.%07d_printDetailedInformation.txt", m_fileName.c_str(), m_runNumber);
    FILE *f = fopen(fileName, "w");
    for (std::vector<Identifier>::const_iterator it = m_TRTHelper->straw_layer_begin(); it != m_TRTHelper->straw_layer_end(); it++  ) {
        for (int i=0; i<=m_TRTHelper->straw_max( *it); i++) {
            Identifier id = m_TRTHelper->straw_id( *it, i);
            int index[6];
            myStrawIndex(id, index);
            int chip, HVpad;    
            m_TRTStrawNeighbourSvc->getChip(id, chip);
            m_TRTStrawNeighbourSvc->getPad(id, HVpad);
            static int printStatusCount(0);
            if (!printStatusCount) {
                ATH_MSG_INFO( "if the code crashes on the next line, there is a problem with m_TRTStrawStatusSummarySvc not being loaded " );
                ATH_MSG_INFO( "in that case, running with reco turned on normally solves the problem, know of no better solution at the moment" );
                ATH_MSG_INFO( "if you do not need the detailed print information, you can also just set printDetailedInformation to 0 to avoid this crash" ); 
                printStatusCount++;
            }
            int status = m_TRTStrawStatusSummarySvc->get_status( id );  
            int statusTemporary = m_TRTStrawStatusSummarySvc->getStatus( id );
            int statusPermanent = m_TRTStrawStatusSummarySvc->getStatusPermanent( id ); 
            for (int j=0; j<6; j++) fprintf(f, "%d ", index[j]);
            fprintf(f, "%d %d %d %d %d\n", chip, HVpad, status, statusTemporary, statusPermanent);
        }
    }           
    fclose(f);
    return; 
}

void InDet::TRT_StrawStatus::myStrawIndex(Identifier id, int *index) {
    int side = m_TRTHelper->barrel_ec(id);
    int layerNumber = m_TRTHelper->layer_or_wheel(id);
    int strawLayerNumber = m_TRTHelper->straw_layer(id);
    int strawNumber = m_TRTHelper->straw(id);        
    int straw(0);
    
    const int numberOfStraws[74] = { 0, 15, 31, 47, 63, 79, 96, 113, 130, 147, 164, 182, 200, 218, 236, 254, 273, 292, 311, 329, // layer 0, 329 straws, strawlayers 0-18
        348, 368, 388, 408, 428, 448, 469, 490, 511, 532, 553, 575, 597, 619, 641, 663, 686, 709, 732, 755, 778, 802, 826, 849, // layer 1, 520 straws, strawLayers 0-23
        872, 896, 920, 944, 968, 993, 1018, 1043, 1068, 1093, 1119, 1145, 1171, 1197, 1223, 1250, 1277, 1304, 1331, 1358, 1386, 1414, 1442, 1470, 1498, 1527, 1556, 1585, 1614, 1642 }; // layer 2
    
    if (abs(side)==1) { // barrel unique straw number
        if (layerNumber==1) strawLayerNumber+= 19;
        else if (layerNumber==2) strawLayerNumber+= 43;
        straw = ( numberOfStraws[strawLayerNumber+1] - strawNumber -1 );
    } else { // end-cap unique straw number
        int board = layerNumber;
        if (board<6) { board *= 2; if (strawLayerNumber>7) board++; }
        else { board += 6; }
        straw = board * 192 + strawNumber * 8 + strawLayerNumber % 8 ;
        straw += 1642;          
    }
    index[0] = side;
    index[1] = layerNumber;
    index[2] = m_TRTHelper->phi_module(id);
    index[3] = strawLayerNumber;
    index[4] = strawNumber;
    index[5] = straw;       
    return;
}
