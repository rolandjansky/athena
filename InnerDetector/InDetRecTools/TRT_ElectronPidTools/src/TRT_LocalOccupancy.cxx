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
#include "InDetIdentifier/TRT_ID.h"

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

TRT_LocalOccupancy::~TRT_LocalOccupancy()
{
}

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

  m_eventnumber = -1;
  m_averageocc  = 0;
  resetOccArrays();
  ATH_MSG_INFO ("initialize() successful in " << name());

  return sc;
}




StatusCode TRT_LocalOccupancy::finalize()
{
  ATH_MSG_INFO ("finalize() successful in " << name());
  return AlgTool::finalize();
}

  /** Compute the local occupancies for the whole event. ONLY DO ONCE PER EVENT */
StatusCode TRT_LocalOccupancy::BeginEvent(){
  
  // Check if inic has beed done:
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

  float m_hitsdc_array          [4][14][32]     ;
  float m_hitsdc_array_phi      [4][32] ;
  float m_hitsdc_array_det      [4]             ;

  resetArrays(m_hitsdc_array, m_hitsdc_array_phi , m_hitsdc_array_det);
  //printArrays(m_hitsdc_array, m_hitsdc_array_phi , m_hitsdc_array_det);

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
      Identifier id = rdo->identify();
      int det = m_TRTHelper->barrel_ec(         id)     ;
      int lay = m_TRTHelper->layer_or_wheel(    id)     ;
      int phi = m_TRTHelper->phi_module(        id)     ;
      int arraydet = detToArrayIndex(det);
      m_hitsdc_array		[arraydet][lay][phi]	+=1;
      m_hitsdc_array_phi	[arraydet][phi]		+=1;
      m_hitsdc_array_det 	[arraydet]		+=1;	
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

  float m_strawsalive_array          [4][14][32]     ;
  float m_strawsalive_array_phi      [4][32] ;
  float m_strawsalive_array_det      [4]             ;
  resetArrays(m_strawsalive_array, m_strawsalive_array_phi , m_strawsalive_array_det);


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
      int arraydet = detToArrayIndex(det);
      m_strawsalive_array	[arraydet][lay][phi]	+=1;
      m_strawsalive_array_phi	[arraydet][phi]		+=1;
      m_strawsalive_array_det 	[arraydet]		+=1;	
    }
  }

  int countactivestraws 	= 0;  
  int counthits			= 0;
 
  resetOccArrays();
  // Calculate Occs:
   for (int i = 0; i < 4; i++){
      float ratio_det = 0.0;
      int hits_det   = m_hitsdc_array_det [i];
      int straws_det = m_strawsalive_array_det[i];
      if (straws_det>0) 	ratio_det = float(hits_det) / straws_det;
      m_occdc_array_det [i]= ratio_det; 

      for (int j = 0; j < 32; j++){
        float ratio_phi = 0.0;
	int hits_phi   = m_hitsdc_array_phi [i][j];
        int straws_phi = m_strawsalive_array_phi[i][j];
	if (straws_phi> 0) ratio_phi = float(hits_phi) / straws_phi;
	m_occdc_array_phi[i][j] = ratio_phi;

        for (int k = 0; k < 14; k++){
		float ratio = 0;
		int hits  =  m_hitsdc_array[i][k][j];
		int straws = m_strawsalive_array[i][k][j]	; 
		countactivestraws += straws	;
		counthits += hits		;
		if (straws > 0) ratio = float(hits) / straws;
		m_occdc_array [i][k][j]= ratio;
       }
     }
   } 

   
  m_averageocc  = float(counthits)/countactivestraws ;

  ATH_MSG_DEBUG("Active straws: " << countactivestraws << "\t total number of hits: " << counthits << "\t occ: " << float(counthits)/countactivestraws ); 
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
     if(occ_det == 0){
       ATH_MSG_WARNING("Occupancy is 0 for : " << i << " BUT THERE ARE HITS!!!: " << hits_array_det);
       continue;
     }	
     averageoccdc_array_det  += (occ_det*hits_array_det);
     nhits_array_det		+= hits_array_det;	

     for (int j = 0; j < 32; j++){

        float hits_array_phi  = m_track_array_phi[i][j] ; 
        if (hits_array_phi<1) continue;
        float occ_phi =  m_occdc_array_phi [i][j] ;
        if(occ_phi == 0){
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
		if(occ == 0){
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

std::vector<float> TRT_LocalOccupancy::GlobalOccupancy( ){


  BeginEvent()						;
  std::vector<float> 	output				;
  output.push_back(	m_averageocc    )		;	//	Whole TRT
  output.push_back(	m_occdc_array_det [0]	)	;	//	Endcap C	
  output.push_back(	m_occdc_array_det [1]	)	;	//	BarrelC
  output.push_back(	m_occdc_array_det [2]	)	;	//	Barrel A
  output.push_back(	m_occdc_array_det [3]	)	;	//	Endcap A

  ATH_MSG_DEBUG("Compute Global Occupancy: whole TRT: "  << output.at(0) << "\t Endcap C: " <<  output.at(1) << "\tBarrel C: " << output.at(2) << "\t Barrel A: " << output.at(3) << "\t Endcap A: " << output.at(4) );
  return output;

}

std::vector<float> TRT_LocalOccupancy::LocalOccupancy(const Trk::Track track ){
  ATH_MSG_DEBUG("Compute LocalOccupancy(const Trk::Track& track ) for tool: " << name());

  BeginEvent();
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
     if(occ_det == 0){
       ATH_MSG_WARNING("Occupancy is 0 for : " << i << " BUT THERE ARE HITS!!!: " << hits_array_det);
       continue;
     }	
     averageoccdc_array_det  += (occ_det*hits_array_det);
     nhits_array_det		+= hits_array_det;	

     for (int j = 0; j < 32; j++){

        float hits_array_phi  = m_track_array_phi[i][j] ; 
        if (hits_array_phi<1) continue;
        float occ_phi =  m_occdc_array_phi [i][j] ;
        if(occ_phi == 0){
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
		if(occ == 0){
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
  ATH_MSG_DEBUG("Compute LocalOccupancy(const Trk::Track& track ) for tool: " << averageoccdc_array << "\t"<< averageoccdc_array_phi << "\t" << averageoccdc_array_det << " is over" );
  // So far, the average occ taking det, lay and mod in account
  std::vector<float> output;
  output.push_back(m_averageocc    		);
  output.push_back(averageoccdc_array_det	);
  output.push_back(averageoccdc_array_phi	);
  output.push_back(averageoccdc_array		);

  return output;
}

	


	//////////////////////////////////////////////////////////////////
	//
	//    Extra tools to deal with multid. Arrays
	//
	//////////////////////////////////////////////////////////////////


  void TRT_LocalOccupancy::resetOccArrays(){
   for (int i = 0; i < 4; i++){
      m_occdc_array_det[i]=0;
     for (int j = 0; j < 14; j++){
	m_occdc_array_phi[i][j]=0;
        for (int k = 0; k < 32; k++){
		m_occdc_array[i][j][k]=0;
        }
     }
   } 
  return;
  }

  void TRT_LocalOccupancy::resetArrays(float array_all [][14][32], float array_phi[][32], float array_det[]){
   for (int i = 0; i < 4; i++){
      array_det[i]=0;
     for (int j = 0; j < 32; j++){
	array_phi[i][j]=0;
       for (int k = 0; k < 14; k++){
		array_all[i][k][j]=0;
        }
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
   int arrayindex = 10;
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


}// namespace close
