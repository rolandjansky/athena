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
//#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"

// Drift circles and TRT identifiers:
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetRawData/TRT_RDO_Container.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Math functions:
#include <cmath>

//STL includes
#include <sstream>

#define PI 3.141592653589793238462643383

class TRT_ID;

namespace InDet
{
TRT_LocalOccupancy::TRT_LocalOccupancy(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_TRTHelper(0),
  m_TRTStrawStatusSummarySvc("TRT_StrawStatusSummarySvc", n),
  m_trt_rdo_location("TRT_RDOs")
{
 declareInterface<ITRT_LocalOccupancy>(this);
  //declareProperty("isData", m_DATA = true);
 declareProperty("TRT_RDOContainerName", m_trt_rdo_location);
 declareProperty("isTrigger", m_isTrigger = false);
 declareProperty("TRTStrawSummarySvc", m_TRTStrawStatusSummarySvc);

  // create arrays
  m_occ_total = new int[7];
  m_hit_total = new int[7];

  m_occ_local = new int*[6];
  m_hit_local = new int*[6];

  for (int i=0; i<6; ++i){
    m_occ_local[i] = new int[32];
    m_hit_local[i] = new int[32];
  }

  m_eventnumber = -1;


  resetOccArrays();
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

  ATH_MSG_INFO ("initialize() successful in " << name());

  return sc;
}

StatusCode TRT_LocalOccupancy::finalize()
{
  delete [] m_occ_total;
  delete [] m_hit_total;
  
  for (int i=0; i<6; ++i){
    delete [] m_occ_local[i];
    delete [] m_hit_local[i];
  }
  delete [] m_occ_local;
  delete [] m_hit_local;

  ATH_MSG_INFO ("finalize() successful in " << name());
  return AlgTool::finalize();
}

// =======================================================================
StatusCode TRT_LocalOccupancy::StartEvent(){
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
  m_eventnumber = eventnumber;
  ATH_MSG_DEBUG("StartEvent() for tool: " << name());
  resetOccArrays();

  // Get the container with the raw data objects
  const TRT_RDO_Container* p_trtRDOContainer;
  sc = evtStore()->retrieve(p_trtRDOContainer, m_trt_rdo_location);
  if (sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not find the TRT_RDO_Container " 
		   << m_trt_rdo_location );
    return sc;
  } 
  
  // put # hits in vectors
  TRT_RDO_Container::const_iterator RDO_collection_iter = p_trtRDOContainer->begin();
  TRT_RDO_Container::const_iterator RDO_collection_end  = p_trtRDOContainer->end();
  for ( ; RDO_collection_iter!= RDO_collection_end; ++RDO_collection_iter) {
    const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection(*RDO_collection_iter);
    if (!RDO_Collection) continue;
    DataVector<TRT_RDORawData>::const_iterator rdo,rb=RDO_Collection->begin(),re=RDO_Collection->end(); 
    if (rb!=re){
      
      for(rdo=rb; rdo!=re; ++rdo) {
	
	if(!*rdo)        continue;
	Identifier id = (*rdo)->identify();
	int det      = m_TRTHelper->barrel_ec(         id)     ;
	int lay      = m_TRTHelper->layer_or_wheel(    id)     ;
	int phi      = m_TRTHelper->phi_module(        id)     ;
	int i_total  = findArrayTotalIndex(det, lay);


	// -----
	// FIXME:
	// the next few lines are replicates of code in TRT_DriftCircleTool.cxx
	// should make a method in there to avoid cut and pasting code!!
	if((m_TRTStrawStatusSummarySvc->getStatus(id) != TRTCond::StrawStatus::Good)
	   || (m_TRTStrawStatusSummarySvc->getStatusPermanent(id))) continue;

	if (!passValidityGate((*rdo)->getWord(), 0.)) continue;
	// end fixme
	// -----
	
	// if (isMiddleBXOn(rdo->getWord())) { 
	
	m_hit_total[0]                        +=1;
	m_hit_total[i_total]                  +=1;
	m_hit_local[i_total-1][phi]           +=1;
	//} // if (isMiddleBXOn)
      }
      
    }
  }
  
  // count live straws
  m_stw_total 		=  m_TRTStrawStatusSummarySvc->getStwTotal()		;
  m_stw_local 		=  m_TRTStrawStatusSummarySvc->getStwLocal()		;

  // Calculate Occs:
  for (int i=0; i<7; ++i) {
    float occ = 0;
    int hits  = m_hit_total[i];
    int stws  = m_stw_total[i];
    if (stws>0) occ = float(hits*100)/stws;
    m_occ_total[i] = int(occ);
  }
  for (int i=0; i<6; ++i) {
    for (int j=0; j<32; ++j) {
      float occ = 0;
      int hits  = m_hit_local[i][j];
      int stws  = m_stw_local[i][j];
      if (stws>0) occ = float(hits*100)/stws;
      m_occ_local[i][j] = int(occ);
    }
  }
   
  ATH_MSG_DEBUG("Active straws: " << m_stw_total[0] << "\t total number of hits: " << m_hit_total[0] << "\t occ: " << m_occ_total[0] ); 
//  printArrays( m_occdc_array, m_occdc_array_phi, m_occdc_array_det );
  return sc;
}


std::vector<float> TRT_LocalOccupancy::GlobalOccupancy( ){
  std::vector<float> 	output				;
  if (m_isTrigger) {
    ATH_MSG_INFO("Cannot compute Global Occupancies in trigger environment! Returning empty vector");
    return output;
  }
  StartEvent()						;

  output.push_back(	m_occ_total[0]*1.e-2    )	;	//	Whole TRT
  output.push_back(	m_occ_total[1]*1.e-2	)	;	//	Barrel  C
  output.push_back(	m_occ_total[2]*1.e-2	)	;	//	EndcapA C
  output.push_back(	m_occ_total[3]*1.e-2	)	;	//	EndcapB C
  output.push_back(	m_occ_total[4]*1.e-2	)	;	//	Barrel  A
  output.push_back(	m_occ_total[5]*1.e-2	)	;	//	EndcapA A
  output.push_back(	m_occ_total[6]*1.e-2	)	;	//	EndcapB A

  ATH_MSG_DEBUG("Compute Global Occupancy: whole TRT: "  << output.at(0) << "\t Barrel C: " <<  output.at(1) << "\t EndcapA C: " << output.at(2) << "\t EndcapB C: " << output.at(3) << "\t Barrel A: " << output.at(4) << "\t EndcapA A: " << output.at(5) << "\t EndcapB A: " << output.at(6));
  return output;
}

float TRT_LocalOccupancy::LocalOccupancy(const Trk::Track& track ){
  ATH_MSG_DEBUG("Compute LocalOccupancy(const Trk::Track& track ) for tool: " << name());

  if (m_isTrigger) {
    StatusCode sc = StatusCode::SUCCESS; 
    const EventInfo *eventInfo = 0;
    sc = evtStore()->retrieve(eventInfo);
    int eventnumber =eventInfo->event_ID()->event_number();
    if (m_eventnumber != eventnumber){
      resetOccArrays();
      m_stw_local 		=  m_TRTStrawStatusSummarySvc->getStwLocal()		;
    } 
    m_eventnumber = eventnumber;
  }
  else StartEvent();

  // FIXME : OVERUSE OF MEMORY. The full array is initialized but the track only cross few areas... VECTORS must be smarter or tiny map here. shoyld be fast...
  int m_track_local[6][32]; // same as above, but split into 32 phi sectors
  for (int i=0; i<6; ++i){
    for (int j=0; j<32; ++j){
      m_track_local[i][j]=0;
    }
  }

  //  resetArrays(m_track_total, m_track_local, m_track_mod);
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

    int i_total = findArrayTotalIndex(det, lay);
    m_track_local[i_total-1][phi]     +=1;

  }
  if (m_isTrigger)   countHitsNearTrack(m_track_local);

  float  averageocc   = 0;
  int	 nhits        = 0;

  for (int i=0; i<6; ++i){
     for (int j = 0; j < 32; j++){

        float hits_array  = m_track_local[i][j] ; 
        if (hits_array<1) continue;
	float occ=0;
	occ =  (m_occ_local [i][j])*1.e-2 ;
	if (m_stw_local[i][j] != 0){
	  if(occ == 0 && float(hits_array)/m_stw_local[i][j] > 0.01){
	    ATH_MSG_DEBUG("Occupancy is 0 for : " << i << " " << j << " BUT THERE ARE HITS!!!: " << hits_array);
	    continue;
	  }
	}	
        averageocc  += (occ*hits_array);
        nhits       += hits_array;	
	
	ATH_MSG_DEBUG("new track: " << i << " " << j << "\t" << hits_array << "\t" << occ ); 
     }
  }
  if (nhits>0)	averageocc 	= averageocc / nhits;
  ATH_MSG_DEBUG("Compute LocalOccupancy(const Trk::Track& track ) for tool: " << averageocc << " is over" );

  return averageocc;
}


void  TRT_LocalOccupancy::countHitsNearTrack(int track_hit_array[6][32]){
    const TRT_RDO_Container* p_trtRDOContainer;
    StatusCode sc = evtStore()->retrieve(p_trtRDOContainer, m_trt_rdo_location);
    if (sc.isFailure() ) {
      ATH_MSG_DEBUG( "Could not find the TRT_RDO_Container " 
		     << m_trt_rdo_location );
      return;
    } 

    for (int i=0; i<6; ++i){
      for (int j=0; j<32; ++j){

	// we are only interested in filling regions through which track passed
	if (track_hit_array[i][j] < 1) continue;

	// if we already filled this region, skip it
	if (m_hit_local[i][j] > 0) continue;

	TRT_RDO_Container::const_iterator RDO_collection_iter = p_trtRDOContainer->begin();
	TRT_RDO_Container::const_iterator RDO_collection_end  = p_trtRDOContainer->end();
	for ( ; RDO_collection_iter!= RDO_collection_end; ++RDO_collection_iter) {
	  const InDetRawDataCollection<TRT_RDORawData>* RDO_Collection(*RDO_collection_iter);
	  if (!RDO_Collection) return;
	  if (RDO_Collection->size() != 0){
	    DataVector<TRT_RDORawData>::const_iterator r,rb=RDO_Collection->begin(),re=RDO_Collection->end(); 
	    
	    for(r=rb; r!=re; ++r) {
	      if (!*r)                                continue;
	      Identifier   rdo_id  = (*r)->identify    ()                          ;
	      
	      if((m_TRTStrawStatusSummarySvc->getStatus(rdo_id) != TRTCond::StrawStatus::Good)
		 || (m_TRTStrawStatusSummarySvc->getStatusPermanent(rdo_id))) {
		continue;
	      }
	      if ( !passValidityGate((*r)->getWord(), 0.) ) continue;
	      
	      int det      = m_TRTHelper->barrel_ec(         rdo_id)     ;
	      int lay      = m_TRTHelper->layer_or_wheel(    rdo_id)     ;
	      int phi      = m_TRTHelper->phi_module(        rdo_id)     ;
	      int i_total       = findArrayTotalIndex(det, lay)-1;

	      if (i_total != i || phi != j) continue; // only fill the one region [i][j]
	      
	      
	      m_hit_local[i_total][phi]           +=1;
	    }
	  }
	}
	int hits = m_hit_local[i][j];
	int stws = m_stw_local[i][j];
	m_occ_local[i][j] = int(hits*100) / stws;

      }
    }
    
    return;
}

  float TRT_LocalOccupancy::LocalOccupancy(const double t_eta, const double t_phi){
    // take eta, phi of track, RoI, ... what have you
    // return local occupancy in an appropriate region of the detector
    // size of region is:
    //   - 1 of 6 partitions (barrel, endcapA, endcapB, sides A & C)
    //   - 1 of 32 phi modules (in triangular shape of chips, not 'pie slices')
    ATH_MSG_DEBUG("LocalOccupancy(eta,phi)");
    StartEvent();

    int partition=mapEtaToPartition(t_eta);
    int phisector=mapPhiToPhisector(t_phi);

    if (partition > 5 || phisector > 31) {
      ATH_MSG_DEBUG("mapping failed ; returning global occ");
      return m_occ_total[0]*1.e-2 ;
    }

    float mapped_occ = m_occ_local[partition][phisector]*1.e-2;
    ATH_MSG_DEBUG("returning mapped occupancy");
    return mapped_occ;

  }


// ========================================================================
bool TRT_LocalOccupancy::isMiddleBXOn(unsigned int word) {
  // check that there is at least one hit in middle 25 ns
  unsigned mask = 0x00010000;
  int i=0;
  for (i=0; i<8; ++i) {
    if (word & mask) return true;
    mask >>= 1;
  }
return false;
}

bool TRT_LocalOccupancy::passValidityGate(unsigned int word, float t0) {
  bool foundInterval = false;
  unsigned  mask = 0x02000000;
  int i = 0;
  while ( !foundInterval && (i < 24) ) {
    if (word & mask) {
      float thisTime = ((0.5+i)*3.125)-t0;
      //if (thisTime >= 14.0625 && thisTime <= 42.1875) foundInterval = true;
      if (thisTime >= 20.3125 && thisTime <= 54.6875) foundInterval = true;
    }
    mask >>= 1;
    if (i == 7 || i == 15) 
      mask >>= 1;
    i++;
  }
  return foundInterval;
}
// ========================================================================
	


	//////////////////////////////////////////////////////////////////
	//
	//    Extra tools to deal with multid. Arrays
	//
	//////////////////////////////////////////////////////////////////


  void TRT_LocalOccupancy::resetOccArrays(){
    for (int i=0; i<7; ++i){
      m_occ_total[i]=0;
      m_hit_total[i]=0;
    }
    for (int i=0; i<6; ++i){
      for (int j=0; j<32; ++j){
	m_occ_local[i][j]=0;
    	m_hit_local[i][j]=0;
      }
    }
  return;
  }

  void TRT_LocalOccupancy::resetArrays(float array_total [7], float array_local[6][32], float array_mod[34][32]){
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
   for (int i=0; i<6; ++i){
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

  void TRT_LocalOccupancy::printArrays(float array_total [7], float array_local[6][32], float array_mod[34][32]){
   for (int i = 0; i < 7; i++){
     ATH_MSG_INFO(" Array print out: " << i << "\t" << array_total[i]);
   }
   for (int i = 0; i < 6; ++i){
      for (int j = 0; j < 32; j++){
        ATH_MSG_INFO(" Array print out: " << i << "\t" << j << "\t" << array_local[i][j]);
      }
   }
   for (int i = 0; i < 34; ++i){
     for (int j = 0; j < 32; ++j){
       ATH_MSG_INFO(" Array print out: " << i << "\t" << j << "\t" << array_mod[i][j]);
       }
     }
  return;
  }

  int TRT_LocalOccupancy::findArrayTotalIndex(const int det, const int lay){
    int arrayindex = 0; // to be reset below
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

int TRT_LocalOccupancy::mapPhiToPhisector(const double t_phi){

  int phisector=33;
  // do phi selection
  // shift all phi to positive numbers
  float dphi = 0; // TBD
  
  double phi2pi = (t_phi > 0) ? t_phi : t_phi + 2*PI;

  phisector = int ( (phi2pi + dphi)*32./(2*PI) );
  return phisector;
}

int TRT_LocalOccupancy::mapEtaToPartition(const double t_eta){

  int partition=7;

  double abseta = fabs(t_eta);

  // do eta selection
  if      (                  abseta <= 0.90 ) partition = 0;
  else if ( abseta > 0.90 && abseta <= 1.55 ) partition = 1;
  else if ( abseta > 1.55 && abseta <= 2.00 ) partition = 2;
  else ATH_MSG_DEBUG("abs(eta) > 2.0 ; not in TRT!");

  if (t_eta>0.) partition += 3; // side A

  return partition;
}


}// namespace close
