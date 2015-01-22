/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "TRT_ElectronPidTools/TRT_LocalOccupancy.h"

// StoreGate, Athena, and Database stuff:
#include "Identifier/Identifier.h"

// Tracking:
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackInfo.h"

// Drift circles and TRT identifiers:
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRawData/TRT_RDO_Container.h"

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Math functions:
#include <cmath>

//STL includes
#include <sstream>


class TRT_ID;

namespace InDet
{
TRT_LocalOccupancy::TRT_LocalOccupancy(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_TRTHelper(0),
  m_TRTStrawStatusSummarySvc("TRT_StrawStatusSummarySvc", n)
{
 declareInterface<ITRT_LocalOccupancy>(this);
  //declareProperty("isData", m_DATA = true);
}

// =======================================================================
TRT_LocalOccupancy::~TRT_LocalOccupancy()
{
}

// =======================================================================
StatusCode TRT_LocalOccupancy::initialize()
{


   // The TRT helper: 
  StatusCode sc = detStore()->retrieve(m_TRTHelper, "TRT_ID");
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to retrieve TRT ID Helper." << endreq;
    return sc;       
  } else {
    msg(MSG::DEBUG) << "retrieved m_TRTHelper " << m_TRTHelper << endreq; 
  }

  // create arrays
  m_occ_total = new int[7];
  m_hit_total = new int[7];
  m_stw_total = new int[7];
  m_occ_local = new int*[10];
  m_hit_local = new int*[10];
  m_stw_local = new int*[10];
  for (int i=0; i<10; ++i){
    m_occ_local[i] = new int[32];
    m_hit_local[i] = new int[32];
    m_stw_local[i] = new int[32];
  }

  m_occ_local_wheel = new int*[34];
  m_hit_local_wheel = new int*[34];
  m_stw_local_wheel = new int*[34];
  for (int i=0; i<34; ++i){
    m_occ_local_wheel[i] = new int[32];
    m_hit_local_wheel[i] = new int[32];
    m_stw_local_wheel[i] = new int[32];
  }
  m_occ_local_straw = new int*[36];
  m_hit_local_straw = new int*[36];
  m_stw_local_straw = new int*[36];
  for (int i=0; i<36; ++i){
    m_occ_local_straw[i] = new int[32];
    m_hit_local_straw[i] = new int[32];
    m_stw_local_straw[i] = new int[32];
  }

  m_eventnumber = -1;
  resetOccArrays();
  ATH_MSG_INFO ("initialize() successful in " << name());

  return sc;
}




StatusCode TRT_LocalOccupancy::finalize()
{
  delete [] m_occ_total;
  delete [] m_occ_local;
  delete [] m_hit_total;
  delete [] m_hit_local;
  delete [] m_stw_total;
  delete [] m_stw_local;

  delete [] m_occ_local_wheel;
  delete [] m_hit_local_wheel;
  delete [] m_stw_local_wheel;
  delete [] m_occ_local_straw;
  delete [] m_hit_local_straw;
  delete [] m_stw_local_straw;

  ATH_MSG_INFO ("finalize() successful in " << name());
  return AlgTool::finalize();
}

// =======================================================================
StatusCode TRT_LocalOccupancy::BeginEvent(){
  /** Compute the local occupancies for the whole event. ONLY DO ONCE PER EVENT */
// =======================================================================

  
  // Check if initialization has been done:
  StatusCode sc = StatusCode::SUCCESS; 
  const EventInfo *eventInfo = 0;
  sc = evtStore()->retrieve(eventInfo);
  int eventnumber =eventInfo->event_ID()->event_number();
  if (m_eventnumber == eventnumber){
  	ATH_MSG_DEBUG("Event " << eventnumber << " already initialized");
	return sc;
  } 
  else m_eventnumber = eventnumber;


  // Get the container with the drift circles 
  const InDet::TRT_DriftCircleContainer* driftCircleContainer = 0;  
  ATH_MSG_DEBUG("BeginEvent() for tool: " << name());
  if ( evtStore()->contains<InDet::TRT_DriftCircleContainer>("TRT_DriftCircles") ) {
    sc = evtStore()->retrieve(driftCircleContainer, "TRT_DriftCircles");
    if (sc.isFailure() || !driftCircleContainer) 	ATH_MSG_WARNING("No TRT Drift Circles in StoreGate");
    else 						ATH_MSG_DEBUG   ("Found Drift Circles in StoreGate");
  }

  resetOccArrays();
  //printArrays(m_hitsdc_array, m_hitsdc_array_phi , m_hitsdc_array_det);

  // put # hits in vectors
  if (driftCircleContainer)
  for (InDet::TRT_DriftCircleContainer::const_iterator colIt = driftCircleContainer->begin(); colIt != driftCircleContainer->end(); ++colIt) {
    const InDet::TRT_DriftCircleCollection *colNext=(*colIt);
    if(!colNext) continue;
    // loop over DCs 
    DataVector<TRT_DriftCircle>::const_iterator p_rdo 		=    colNext->begin();
    DataVector<TRT_DriftCircle>::const_iterator p_rdo_end 	=    colNext->end();
    for(; p_rdo!=p_rdo_end; ++p_rdo){
      const TRT_DriftCircle* rdo = (*p_rdo);
      if(!rdo)        continue;
      // if (isMiddleBXOn(rdo->getWord())) { // check that there is at least one hit in middle 25 ns
      Identifier id = rdo->identify();
      int det = m_TRTHelper->barrel_ec(         id)     ;
      int lay = m_TRTHelper->layer_or_wheel(    id)     ;
      int phi = m_TRTHelper->phi_module(        id)     ;
      // int arraydet = detToArrayIndex(det);
      // int arraydetwheel = arrayIndexToWheelIndex(arraydet, lay);

      int i_total = findArrayTotalIndex(det, lay);
      int i_local = findArrayLocalIndex(det, lay);
      m_hit_total[0]            +=1;
      m_hit_total[i_total]      +=1;
      m_hit_local[i_local][phi] +=1;

      int i_local_wheel = findArrayLocalWheelIndex(det, lay);
      m_hit_local_wheel[i_local_wheel][phi] +=1;
      int strawlay = m_TRTHelper->straw_layer(id);
      int i_local_straw = findArrayLocalStrawIndex(det, lay, strawlay);
      m_hit_local_straw[i_local_straw][phi] +=1;
	//}
    }
  }
//  printArrays(m_hitsdc_array, m_hitsdc_array_phi , m_hitsdc_array_det);


/*
  // Get the container with the RDOs
  const TRT_RDO_Container* rdoContainer = 0; // container of all TRT hits
  if ( evtStore()->contains<TRT_RDO_Container>("TRT_RDOs") ) {
    sc = evtStore()->retrieve(rdoContainer, "TRT_RDOs");
    if (sc.isFailure() || !rdoContainer)    	ATH_MSG_WARNING ("No TRT RDO in StoreGate");
      else  					ATH_MSG_INFO    ("Found TRT RDOs in StoreGate");
  }
   
  // Loop the hits and fill the hits maps:
 std::map <std::string, int>hits_map;
 if (rdoContainer)
 for (TRT_RDO_Container::const_iterator rdoIt = rdoContainer->begin(); rdoIt != rdoContainer->end(); ++rdoIt) {
    const InDetRawDataCollection<TRT_RDORawData>* TRTCollection(*rdoIt);
    if (TRTCollection==0) continue;
    for (DataVector<TRT_RDORawData>::const_iterator trtIt = TRTCollection->begin(); trtIt != TRTCollection->end(); trtIt++) {
      Identifier id = (*trtIt)->identify();
      int det = m_TRTHelper->barrel_ec(		id)	;
      int lay = m_TRTHelper->layer_or_wheel(	id)	;
      int phi = m_TRTHelper->phi_module(	id)	;
    } 
  }
*/

  // count live straws
  for (std::vector<Identifier>::const_iterator it = m_TRTHelper->straw_layer_begin(); it != m_TRTHelper->straw_layer_end(); it++  ) {
   for (int i=0; i<=m_TRTHelper->straw_max( *it); i++) {
      Identifier id = m_TRTHelper->straw_id( *it, i);
      int det = m_TRTHelper->barrel_ec(		id)	;
      int lay = m_TRTHelper->layer_or_wheel(	id)	;
      int phi = m_TRTHelper->phi_module(	id)	;

      // Remove the bad straws, so occupancy is realistic
      bool status 		= m_TRTStrawStatusSummarySvc->get_status( id ); 
//      bool statusHT 		= m_TRTStrawStatusSummarySvc->get_statusHT( id );
      if ( status ) ATH_MSG_VERBOSE(" The sector " << det << " " << lay << " " << phi << " has status " << status);
      if (status) continue;
      int i_total = findArrayTotalIndex(det, lay);
      int i_local = findArrayLocalIndex(det, lay);
      m_stw_total[0]            +=1;
      m_stw_total[i_total]      +=1;
      m_stw_local[i_local][phi] +=1;

      int i_local_wheel = findArrayLocalWheelIndex(det, lay);
      m_stw_local_wheel[i_local_wheel][phi] +=1;
      int strawlay = m_TRTHelper->straw_layer(id);
      int i_local_straw = findArrayLocalStrawIndex(det, lay, strawlay);
      m_stw_local_straw[i_local_straw][phi] +=1;
    }
  }

  // Calculate Occs:
  for (int i=0; i<7; ++i) {
    float occ = 0;
    int hits  = m_hit_total[i];
    int stws  = m_stw_total[i];
    if (stws>0) occ = float(hits)*100/stws;
    m_occ_total[i] = int(occ);
  }
  for (int i=0; i<10; ++i) {
    for (int j=0; j<32; ++j) {
      float occ = 0;
      int hits  = m_hit_local[i][j];
      int stws  = m_stw_local[i][j];
      if (stws>0) occ = float(hits)*100/stws;
      m_occ_local[i][j] = int(occ);
    }
  }

  for (int i=0; i<34; ++i){
    for (int j=0; j<32; ++j){
      float occ = 0;
      int hits = m_hit_local_wheel[i][j];
      int stws = m_stw_local_wheel[i][j];
      if (stws>0) occ = float(hits)*100/stws;
      m_occ_local_wheel[i][j] = int(occ);
    }
  }
  for (int i=0; i<36; ++i){
    for (int j=0; j<32; ++j){
      float occ = 0;
      int hits = m_hit_local_straw[i][j];
      int stws = m_stw_local_straw[i][j];
      if (stws>0) occ = float(hits)*100/stws;
      m_occ_local_straw[i][j] = int(occ);
    }
  }
   
  ATH_MSG_DEBUG("Active straws: " << m_stw_total[0] << "\t total number of hits: " << m_hit_total[0] << "\t occ: " << m_occ_total[0] ); 
//  printArrays( m_occdc_array, m_occdc_array_phi, m_occdc_array_det );
  return sc;
}



/*
double TRT_LocalOccupancy::LocalOccupancy(const Trk::Track track ){
  ATH_MSG_DEBUG("Compute LocalOccupancy(const Trk::Track& track ) for tool: " << name());

// FIXME : OVERUSE OF MEMORY. The full array is initialized but the track only cross few areas... VECTORS must be smarter or tiny map here. shoyld be fast...
  float m_track_array          [4][14][32]     ;
  float m_track_array_phi      [4][32] ;
  float m_track_array_det      [4]             ;
  resetArrays(m_track_array, m_track_array_phi , m_track_array_det);

  const DataVector<const Trk::TrackStateOnSurface>* trackStates = track.trackStateOnSurfaces();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator	tsos		=trackStates->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator	tsosEnd		=trackStates->end();
  for (;tsos!=tsosEnd;++tsos) {
    const Trk::MeasurementBase* mesb=(*tsos)->measurementOnTrack();
    if (!mesb) continue;
    const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(mesb);
    if(!driftcircle)  continue;
    Identifier id=driftcircle->identify();
    int det = m_TRTHelper->barrel_ec(         id)     ;
    int lay = m_TRTHelper->layer_or_wheel(    id)     ;
    int phi = m_TRTHelper->phi_module(        id)     ;

    int arraydet = detToArrayIndex(det);
    m_track_array	[arraydet][lay][phi]	+=1;
    m_track_array_phi	[arraydet][phi]		+=1;
    m_track_array_det 	[arraydet]		+=1;	
  }

  double averageoccdc_array = 0;
  int	 nhits_array        = 0;

  double averageoccdc_array_det = 0;
  int	 nhits_array_det        = 0;

  double averageoccdc_array_phi = 0;
  int	 nhits_array_phi        = 0;

  for (int i = 0; i < 4; i++){
     float hits_array_det  = m_track_array_det[i] ; 
     if (hits_array_det<1) continue;
     float occ_det =  m_occdc_array_det [i] ;
     if(occ_det == 0 && hits_array_det > 10){
       ATH_MSG_WARNING("Occupancy is 0 for : " << i << " BUT THERE ARE HITS!!!: " << hits_array_det);
       continue;
     }	
     averageoccdc_array_det  += (occ_det*hits_array_det);
     nhits_array_det		+= hits_array_det;	

     for (int j = 0; j < 32; j++){

        float hits_array_phi  = m_track_array_phi[i][j] ; 
        if (hits_array_phi<1) continue;
        float occ_phi =  m_occdc_array_phi [i][j] ;
        if(occ_phi == 0 && hits_array_phi > 10){
          ATH_MSG_WARNING("Occupancy is 0 for : " << i << " BUT THERE ARE HITS!!!: " << hits_array_phi);
          continue;
        }	
        averageoccdc_array_phi  += (occ_phi*hits_array_phi);
        nhits_array_phi		+= hits_array_phi;	

        int lay = 14;
	if ( (i == 1) || (i == 2) ) lay = 3; // For barrels
        for (int k = 0; k < lay; k++){
		float hits_array  = m_track_array[i][k][j] ; 
		if (hits_array<1) continue;
		float occ =  m_occdc_array [i][k][j] ;
		if(occ == 0 && hits_array > 10){
			  ATH_MSG_WARNING("Occupancy is 0 for : " << i <<" " << k << " " << j << " BUT THERE ARE HITS!!!: " << hits_array);
			  continue;
		}	
		averageoccdc_array  	+= (occ*hits_array);
		nhits_array		+= hits_array;	
    		ATH_MSG_DEBUG("new track: " << i << " " << k << " " << j << "\t" << hits_array << "\t" << occ ); 
	}
     }
  }

  if (nhits_array>0)		averageoccdc_array 	= averageoccdc_array 	/ nhits_array;
  if (nhits_array_det>0)	averageoccdc_array_det 	= averageoccdc_array_det/ nhits_array_det;
  if (nhits_array_phi>0)	averageoccdc_array_phi 	= averageoccdc_array_phi/ nhits_array_phi;
  ATH_MSG_INFO("Compute LocalOccupancy(const Trk::Track& track ) for tool: " << averageoccdc_array << "\t"<< averageoccdc_array_phi << "\t" << averageoccdc_array_det << " is over" );
  // So far, the average occ taking det, lay and mod in account
  return averageoccdc_array;
}
*/

// ========================================================================
bool TRT_LocalOccupancy::isMiddleBXOn(unsigned int word) {
  //  return true;
  unsigned mask = 0x00010000;
  int i=0;
  for (i=0; i<8; ++i) {
    if (word & mask) return true;
    mask >>= 1;
  }
return false;
}
// ========================================================================

  void TRT_LocalOccupancy::resetOccArrays(){
    for (int i=0; i<7; ++i){
      m_occ_total[i]=0;
      m_hit_total[i]=0;
      m_stw_total[i]=0;
    }
    for (int i=0; i<10; ++i){
      for (int j=0; j<32; ++j){
	m_occ_local[i][j]=0;
    	m_hit_local[i][j]=0;
	m_stw_local[i][j]=0;
      }
    }

    for (int i=0; i<34; ++i){
      for (int j=0; j<32; ++j){
	m_occ_local_wheel[i][j]=0;
	m_hit_local_wheel[i][j]=0;
	m_stw_local_wheel[i][j]=0;
      }
    }
    for (int i=0; i<36; ++i){
      for (int j=0; j<32; ++j){
	m_occ_local_straw[i][j]=0;
	m_hit_local_straw[i][j]=0;
	m_stw_local_straw[i][j]=0;
      }
    }
  return;
  }


std::vector<float> TRT_LocalOccupancy::GlobalOccupancy( ){
  BeginEvent()						;
  std::vector<float> 	output				;
  output.push_back(	m_occ_total[0]  )	;	//	Whole TRT
  output.push_back(	m_occ_total[1]	)	;	//	Barrel  C
  output.push_back(	m_occ_total[2]	)	;	//	EndcapA C
  output.push_back(	m_occ_total[3]	)	;	//	EndcapB C
  output.push_back(	m_occ_total[4]	)	;	//	Barrel  A
  output.push_back(	m_occ_total[5]	)	;	//	EndcapA A
  output.push_back(	m_occ_total[6]	)	;	//	EndcapB A

  ATH_MSG_DEBUG("Compute Global Occupancy: whole TRT: "  << output.at(0) << "\t Barrel C: " <<  output.at(1) << "\t EndcapA C: " << output.at(2) << "\t EndcapB C: " << output.at(3) << "\t Barrel A: " << output.at(4) << "\t EndcapA A: " << output.at(5) << "\t EndcapB A: " << output.at(6));
  return output;
}

std::vector<float> TRT_LocalOccupancy::LocalOccupancy(const Trk::Track track ){
  ATH_MSG_DEBUG("Compute LocalOccupancy(const Trk::Track& track ) for tool: " << name());
  BeginEvent();
// FIXME : OVERUSE OF MEMORY. The full array is initialized but the track only cross few areas... VECTORS must be smarter or tiny map here. shoyld be fast...
  // float m_track_array          [4][14][32]     ;
  // float m_track_array_phi      [4][32] ;
  // float m_track_array_det      [4]             ;
  float m_track_total [7];      // total TRT, barrelC, endcapAC,endcapBC, barrelA, endcapAA, ECBA
  float m_track_local [10][32]; // same as above, but split barrel into 3 layers
  float m_track_mod   [34][32]; // same as above, but split endcaps into 14 wheels
  //  resetArrays(m_track_array, m_track_array_phi , m_track_array_det);
  resetArrays(m_track_total, m_track_local, m_track_mod);

  const DataVector<const Trk::TrackStateOnSurface>* trackStates = track.trackStateOnSurfaces();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator	tsos		=trackStates->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator	tsosEnd		=trackStates->end();
  for (;tsos!=tsosEnd;++tsos) {
    const Trk::MeasurementBase* mesb=(*tsos)->measurementOnTrack();
    if (!mesb) continue;
    const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(mesb);
    if(!driftcircle)  continue;
    Identifier id=driftcircle->identify();
    int det = m_TRTHelper->barrel_ec(         id)     ;
    int lay = m_TRTHelper->layer_or_wheel(    id)     ;
    int phi = m_TRTHelper->phi_module(        id)     ;

     // int arraydet = detToArrayIndex(det);
     // m_track_array_mod	[arraydet][lay][phi]	+=1;
     // m_track_array_phi	[arraydet][phi]		+=1;
     // m_track_array_det 	[arraydet]		+=1;	
    int i_total = findArrayTotalIndex(det, lay);
    int i_local = findArrayLocalIndex(det, lay);
    int i_local_wheel = findArrayLocalWheelIndex(det, lay);
    m_track_total[0]                  +=1;
    m_track_total[i_total]            +=1;
    m_track_local[i_local][phi]       +=1;
    m_track_mod[i_local_wheel][phi]   +=1;
  }

  // double averageoccdc_array = 0;
  // int	 nhits_array        = 0;

  // double averageoccdc_array_det = 0;
  // int	 nhits_array_det        = 0;

  // double averageoccdc_array_phi = 0;
  // int	 nhits_array_phi        = 0;
  // double averageoccdc_det = 0;
  // int	 nhits_det        = 0;
  // double averageoccdc_phi = 0;
  // int	 nhits_phi        = 0;
  // double averageoccdc_mod = 0;
  // int	 nhits_mod        = 0;
  double averageoccdc_total = 0;
  int	 nhits_total        = 0;
  double averageoccdc_local = 0;
  int	 nhits_local        = 0;
  double averageoccdc_mod   = 0;
  int	 nhits_mod          = 0;


  // for (int i = 0; i < 4; i++){
  for (int i=0; i<7; ++i){
     float hits_array_det  = m_track_total[i] ; 
     if (hits_array_det<1) continue;
     float occ_det =  (m_occ_total [i])*1.e-2 ;
     if(occ_det == 0 && hits_array_det/m_stw_total[i] > 0.01){
       ATH_MSG_WARNING("Occupancy is 0 for : " << i << " BUT THERE ARE HITS!!!: " << hits_array_det);
       continue;
     }	
     averageoccdc_total  += (occ_det*hits_array_det);
     nhits_total         += hits_array_det;	
  }

  for (int i=0; i<10; ++i){
     for (int j = 0; j < 32; j++){
        float hits_array_phi  = m_track_local[i][j] ; 
        if (hits_array_phi<1) continue;
        float occ_phi =  (m_occ_local [i][j])*1.e-2 ;
        if(occ_phi == 0 && hits_array_phi/m_stw_local[i][j] > 0.01){
          ATH_MSG_WARNING("Occupancy is 0 for : " << i << " " << j << " BUT THERE ARE HITS!!!: " << hits_array_phi);
          continue;
        }	
        averageoccdc_local  += (occ_phi*hits_array_phi);
        nhits_local         += hits_array_phi;	

	ATH_MSG_DEBUG("new track: " << i << " " << j << "\t" << hits_array_phi << "\t" << occ_phi ); 
     }
  }

  for (int i=0; i<34; ++i){
     for (int j = 0; j < 32; j++){
        float hits_array_mod  = m_track_mod[i][j] ; 
        if (hits_array_mod<1) continue;
        float occ_mod =  (m_occ_local_wheel [i][j])*1.e-2 ;
        if(occ_mod == 0 && hits_array_mod/m_stw_local_wheel[i][j] > 0.01){
          ATH_MSG_WARNING("Occupancy is 0 for : " << i << " " << j << " BUT THERE ARE HITS!!!: " << hits_array_mod);
          continue;
        }	
        averageoccdc_mod  += (occ_mod*hits_array_mod);
        nhits_mod         += hits_array_mod;	
	
	ATH_MSG_DEBUG("new track: " << i << " " << j << "\t" << hits_array_mod << "\t" << occ_mod ); 
     }
  }

  if (nhits_total>0)	averageoccdc_total 	= averageoccdc_total / nhits_total;
  if (nhits_local>0)	averageoccdc_local 	= averageoccdc_local / nhits_local;
  if (nhits_mod  >0)    averageoccdc_mod        = averageoccdc_mod   / nhits_mod  ;
  ATH_MSG_DEBUG("Compute LocalOccupancy(const Trk::Track& track ) for tool: " << averageoccdc_total << "\t"<< averageoccdc_local << "\t" <<averageoccdc_mod << " is over" );
  // So far, the average occ taking det, lay and mod in account
  std::vector<float> output;
  output.push_back(averageoccdc_total	);
  output.push_back(averageoccdc_local	);
  output.push_back(averageoccdc_mod	);

  return output;
}

	


	//////////////////////////////////////////////////////////////////
	//
	//    Extra tools to deal with multid. Arrays
	//
	//////////////////////////////////////////////////////////////////


  // void TRT_LocalOccupancy::resetOccArrays(){
  //  for (int i = 0; i < 4; i++){
  //     m_occdc_array_det[i]=0;
  //    for (int j = 0; j < 14; j++){
  // 	m_occdc_array_phi[i][j]=0;
  //       for (int k = 0; k < 32; k++){
  // 		m_occdc_array[i][j][k]=0;
  //       }
  //    }
  //  } 
  // return;
  // }

  void TRT_LocalOccupancy::resetArrays(float array_total [7], float array_local[10][32], float array_mod[34][32]){
   // for (int i = 0; i < 4; i++){
   //    array_det[i]=0;
   //   for (int j = 0; j < 32; j++){
   // 	array_phi[i][j]=0;
   //     for (int k = 0; k < 14; k++){
   // 		array_all[i][k][j]=0;
   //      }
   //   }
   // } 
   for (int i = 0; i < 7; ++i){
      array_total[i]=0;
   }
   for (int i=0; i<10; ++i){
     for (int j=0; j<32; ++j){
       array_local[i][j]=0;
     }
   }
   for (int i=0; i<34; ++i){
     for (int j=0; j<32; ++j){
       array_mod[i][j]=0;
     }
   }
  return;
  }

  void TRT_LocalOccupancy::printArrays(float array_all[][14][32], float array_phi[][32], float array_det[]){
   for (int i = 0; i < 4; i++){
     ATH_MSG_INFO(" Array print out: " << i << "\t" << array_det[i]);
      for (int j = 0; j < 32; j++){
        ATH_MSG_INFO(" Array print out: " << i << "\t" << j << "\t" << array_phi[i][j]);
        int lay = 14;
	if ( (i == 1) || (i == 2) ) lay = 3; // For barrels
        for (int k = 0; k < lay; k++){
        	ATH_MSG_INFO(" Array print out: " << i << "\t" << k  << "\t" << j << "\t" << array_all[i][k][j]);
       }
     }
   } 
  return;
  }


  int TRT_LocalOccupancy::detToArrayIndex (int det){
   int arrayindex = 0;
   if 		(det == -2) arrayindex = 0;
   else if 	(det == -1) arrayindex = 1;
   else if 	(det ==  1) arrayindex = 2;
   else if 	(det ==  2) arrayindex = 3;
   else 	{
	ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
   }
  return arrayindex;
  }

  int TRT_LocalOccupancy::ArrayIndexToDet (int arrayindex){
   int det = 0;
   if 		(arrayindex == 0) det = -2;
   else if 	(arrayindex == 1) det = -1;
   else if 	(arrayindex == 2) det =  1;
   else if 	(arrayindex == 3) det =  2;
   else 	{
	ATH_MSG_WARNING(" Array value  is: " << arrayindex << ", out of range 0, 1, 2, 3, so THIS IS NOT TRT!!!");
   }
   return det;
  }

  int TRT_LocalOccupancy::findArrayTotalIndex(const int det, const int lay){
    int arrayindex = 9; // to be reset below
    // NOTE: Below, arrayindex starts at 1 
    // because index 0 is filled with TOTAL value.
    if      (det == -1) arrayindex = 1; // barrel side C
    else if (det == -2) {               // endcap side C
      if (lay < 6)      arrayindex = 2; //   wheel A
      else              arrayindex = 3; //   wheel B
    }
    else if (det ==  1) arrayindex = 4; // barrel side A
    else if (det ==  2) {               // endcap side A
      if (lay < 6)      arrayindex = 5; //   wheel A
      else              arrayindex = 6; //   wheel B
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }
  int TRT_LocalOccupancy::findArrayLocalIndex(const int det, const int lay){
    int arrayindex = 9; // to be reset below
    if      (det == -1) {                // barrel side C
      if      (lay == 0) arrayindex = 0; // layer 0
      else if (lay == 1) arrayindex = 1; // layer 1
      else if (lay == 2) arrayindex = 2; // layer 2
    }
    else if (det == -2) {                // endcap side C
      if (lay < 6)       arrayindex = 3; //   wheel A
      else               arrayindex = 4; //   wheel B
    }
    else if (det ==  1) {                // barrel side A
      if      (lay == 0) arrayindex = 5; // layer 0
      else if (lay == 1) arrayindex = 6; // layer 1
      else if (lay == 2) arrayindex = 7; // layer 2
    }
    else if (det ==  2) {                // endcap side A
      if (lay < 6)       arrayindex = 8; //   wheel A
      else               arrayindex = 9; //   wheel B
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }
  int TRT_LocalOccupancy::findArrayLocalWheelIndex(const int det, const int lay){
    int arrayindex = 9; // to be reset below
    if      (det == -1) {                // barrel side C
      if      (lay == 0) arrayindex = 0; // layer 0
      else if (lay == 1) arrayindex = 1; // layer 1
      else if (lay == 2) arrayindex = 2; // layer 2
    }
    else if (det == -2) {                // endcap side C
      for (int i=0; i<14; ++i){
	if (lay==i) arrayindex=i+3;
      }
    }
    else if (det ==  1) {                // barrel side A
      if      (lay == 0) arrayindex = 17; // layer 0
      else if (lay == 1) arrayindex = 18; // layer 1
      else if (lay == 2) arrayindex = 19; // layer 2
    }
    else if (det ==  2) {                // endcap side A
      for (int i=0; i<14; ++i){
	if (lay==i) arrayindex=i+20;
      }
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }
  int TRT_LocalOccupancy::findArrayLocalStrawIndex(const int det, const int lay, const int strawlay){
    int arrayindex = 9; // to be reset below
    if      (det == -1) {                // barrel side C
      if      (lay == 0){                // layer 0
	if (strawlay < 9) arrayindex = 0; // short guys
	else              arrayindex = 1;
      }
      else if (lay == 1)  arrayindex = 2; // layer 1
      else if (lay == 2)  arrayindex = 3; // layer 2
    }
    else if (det == -2) {                // endcap side C
      for (int i=0; i<14; ++i){
	if (lay==i) arrayindex=i+4;
      }
    }
    else if (det ==  1) {                // barrel side A
      if      (lay == 0){                 // layer 0
	if (strawlay < 9) arrayindex = 18;
	else              arrayindex = 19;
      }
      else if (lay == 1)  arrayindex = 20; // layer 1
      else if (lay == 2)  arrayindex = 21; // layer 2
    }
    else if (det ==  2) {                // endcap side A
      for (int i=0; i<14; ++i){
	if (lay==i) arrayindex=i+22;
      }
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }

}// namespace close
