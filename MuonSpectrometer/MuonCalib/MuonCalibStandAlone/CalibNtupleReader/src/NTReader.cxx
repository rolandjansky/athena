/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleReader/NTReader.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"
#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTriggerTimeInfo.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawTriggerHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcCoin.h"
#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"
#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/CscCalibHitBase.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MuonCalibEventBase/RpcCalibHitBase.h"
#include "MuonCalibEventBase/TgcCalibHitBase.h"

#include "GeoPrimitives/GeoPrimitives.h"

/*
  Mainly this file contains methods to fill the classes in MuonCalibEventBase
  from data from a calib ntuple
 */

#include <iostream>
#include <iomanip>
namespace MuonCalib{

  NTReader::~NTReader() {
    clearEvent();
  }

  void NTReader::clearEvent() {
    delete m_event;
    m_event = 0;
  }

  const MuonCalibEvent& NTReader::getEvent( int entry ) {
    clearEvent();

    if(!fChain->GetEntry( entry )) {
      m_event = new MuonCalibEvent();
      return *m_event;
    }

    MuonCalibRawHitCollection *raw = buildRawColl();
    MuonCalibRawTriggerHitCollection *rawtrig = buildRawTriggerColl();

    const MuonCalibEventInfo info = buildEventInfo();

    MuonCalibTriggerTimeInfo *triggerTimeInfo = buildTriggerTimeInfo();
    
    MuonCalibEvent::MCPVec patternVec = buildPatternVec();

    m_event = new MuonCalibEvent( patternVec);
    
    m_event->setMuonCalibEventInfo(info);
    m_event->setMuonCalibRawHitCollection(raw);
    m_event->setMuonCalibRawTriggerHitCollection(rawtrig);

    if(m_storeTruth) {
      MuonCalibTruthCollection *truth = buildTruthColl();
      m_event->setMuonCalibTruthCollection(truth);
    }

    m_event->setMuonCalibEventInfo(info);
    m_event->setMuonCalibTriggerTimeInfo(triggerTimeInfo);

    return *m_event;
  }  //end NTReader::getEvent
  
  const MuonCalibEventInfo NTReader::buildEventInfo() {
    const MuonCalibEventInfo info(event_runNumber, event_eventNumber, event_timeStamp, event_lumiBlock, event_bcId, event_eventTag);
    return info;
  }

  MuonCalibTriggerTimeInfo* NTReader::buildTriggerTimeInfo() {
    MuonCalibTriggerTimeInfo trigTimeInfo(event_mbtsTimeDiff, event_larTimeDiff);
    MuonCalibTriggerTimeInfo *ttInfo = new MuonCalibTriggerTimeInfo(trigTimeInfo);
    return ttInfo;
  }

  MuonCalibTruthCollection* NTReader::buildTruthColl() {
    //---- Fill the truth information
    MuonCalibTruthCollection::TruthVec vec(0);
    for( Int_t i_truth = 0; i_truth < truth_nTruth; ++i_truth ){
      vec.push_back( getTruth(i_truth) );
    }

    MuonCalibTruthCollection *truth = new MuonCalibTruthCollection(vec);
    MuonCalibTruthCollection::MdtTruthVec mdtVec(0);

    for( Int_t i_mdt_truth = 0; i_mdt_truth < mdtTruth_nMdtTruthHit; ++i_mdt_truth){
     truth->addTruth( getMdtTruth(i_mdt_truth) );
    }
    for( Int_t i_rpc_truth = 0; i_rpc_truth < rpcTruth_nRpcTruthHit; ++i_rpc_truth){
     truth->addTruth( getRpcTruth(i_rpc_truth) );
    }      
    for( Int_t i_tgc_truth = 0; i_tgc_truth < tgcTruth_nTgcTruthHit; ++i_tgc_truth){
     truth->addTruth( getTgcTruth(i_tgc_truth) );
    }
    for( Int_t i_csc_truth = 0; i_csc_truth < cscTruth_nCscTruthHit; ++i_csc_truth){
     truth->addTruth( getCscTruth(i_csc_truth) );
    }
    return truth;
  }  //end NTReader::buildTruthColl

  MuonCalibRawHitCollection* NTReader::buildRawColl() {    
    //---- Fill non-assigned 'raw'hits (non-calibrated)
    MuonCalibRawHitCollection *raw = new MuonCalibRawHitCollection();
    for( Int_t i_hit = 0; i_hit < rawMdt_nRMdt; ++i_hit ){
      raw->addMuonCalibRawHit( getMuonCalibRawMdtHit(i_hit) );
    }

    for( Int_t i_hit = 0; i_hit < rawRpc_nRRpc; ++i_hit ){
      raw->addMuonCalibRawHit( getMuonCalibRawRpcHit(i_hit) );
    }

    for( Int_t i_hit = 0; i_hit < rawTgc_nRTgc_All_Bunch; ++i_hit ){
      raw->addMuonCalibRawHit( getMuonCalibRawTgcHit(i_hit) );
    }

    for( Int_t i_hit = 0; i_hit < rawTgcCoin_nRTgc_All; ++i_hit ){
      raw->addMuonCalibRawHit( getMuonCalibRawTgcCoin(i_hit) );
    }

    for( Int_t i_hit = 0; i_hit < rawCsc_nRCsc; ++i_hit ){
      raw->addMuonCalibRawHit( getMuonCalibRawCscHit(i_hit) );
    }
    return raw;
  }  //end NTReader::buildRawColl
  
  MuonCalibRawTriggerHitCollection* NTReader::buildRawTriggerColl() {    
    //---- Fill trigger hits
    MuonCalibRawTriggerHitCollection* rawtrig = new MuonCalibRawTriggerHitCollection();
    if(rpc_fchan > 0){
      for( Int_t i_hit = 0; i_hit < rpc_fchan; ++i_hit ){
	rawtrig->addMuonCalibRawTriggerHit( getMuonCalibRawRpcTriggerHit(i_hit) );
      }
    }
    return rawtrig;
  }
  
  MuonCalibEvent::MCPVec NTReader::buildPatternVec() {
    //---- Fill pattern/segment/hits information
    MuonCalibEvent::MCPVec patternVec;
    for( Int_t i_pattern = 0; i_pattern < pat_nPatterns; ++ i_pattern){
      patternVec.push_back( getPattern(i_pattern) ); 
    }
    
    //create a dummy pattern for region ntuples 
    bool dummy_pattern(false);	
    if (patternVec.size()==0 && seg_nSegments>0) {
      dummy_pattern=true;
      patternVec.push_back(new MuonCalibPattern());
    }
    m_ordered_segments.resize(seg_nSegments);
    for( Int_t i_segment = 0; i_segment < seg_nSegments; ++ i_segment ){
      MuonCalibSegment *segment = getSegment( i_segment );
      m_ordered_segments[i_segment]=segment;
      if(dummy_pattern) {
	patternVec[0]->addMuonSegment( segment );
      } else {
	for( Int_t i_pattern = 0; i_pattern < pat_nPatterns; ++i_pattern){
	  if( i_pattern == seg_patIndex[i_segment]) (patternVec[i_pattern])->addMuonSegment( segment );
	}
      }
    }
    return patternVec;
  }  //end NTReader::buildPatternVec

  const MuonCalibEvent& NTReader::getEvent_light( int entry ) {
    clearEvent();
    
    fChain->GetEntry( entry );  
    
    MuonCalibEvent::MCPVec patternVec = buildPatternVec();
    m_event = new MuonCalibEvent( patternVec);
    
    const MuonCalibEventInfo info = buildEventInfo();
    m_event->setMuonCalibEventInfo(info);

    MuonCalibTriggerTimeInfo *triggerTimeInfo = buildTriggerTimeInfo();
    m_event->setMuonCalibTriggerTimeInfo(triggerTimeInfo);

    return *m_event;
  }  //end NTReader::getEvent_light

  MuonCalibTruth* NTReader::getTruth( int i_truth ) {
    Amg::Vector3D position( truth_gPosX[i_truth], truth_gPosY[i_truth], truth_gPosZ[i_truth] );
    Amg::Vector3D momentum( truth_pX[i_truth],truth_pY[i_truth], truth_pZ[i_truth] );
    double kinEnergy( truth_kinEnergy[i_truth] );
    int PDGCode( truth_PDGCode[i_truth] );
    int barCode( truth_barCode[i_truth] );

    MuonCalibTruth *truth = new MuonCalibTruth( position, momentum, kinEnergy, PDGCode, barCode );
    return truth;
  }
  
  MuonCalibMdtTruthHit* NTReader::getMdtTruth( int i_truth ){
    MuonFixedId id( mdtTruth_id[i_truth] );
		    
    MuonCalibMdtTruthHit *hit = new MuonCalibMdtTruthHit();
    hit->setIdentifier(id);
    hit->setBarCode(mdtTruth_barCode[i_truth]);
    hit->setDriftRadius(mdtTruth_driftRadius[i_truth]);
    hit->setPositionAlongTube(mdtTruth_posAlongTube[i_truth]);
    return hit;
  }

  MuonCalibRpcTruthHit* NTReader::getRpcTruth( int i_truth ){
    MuonFixedId id( rpcTruth_id[i_truth] );		    
    MuonCalibRpcTruthHit *hit = new MuonCalibRpcTruthHit( id, rpcTruth_barCode[i_truth], tgcTruth_time[i_truth]);
    return hit;
  }

  MuonCalibTgcTruthHit* NTReader::getTgcTruth( int i_truth ){
    MuonFixedId id( tgcTruth_id[i_truth] );		    
    MuonCalibTgcTruthHit *hit = new MuonCalibTgcTruthHit( id, tgcTruth_barCode[i_truth], tgcTruth_time[i_truth]);
    return hit;
  }

  MuonCalibCscTruthHit* NTReader::getCscTruth( int i_truth ){
    MuonFixedId id( cscTruth_id[i_truth] );		    
    MuonCalibCscTruthHit *hit = new MuonCalibCscTruthHit( id, cscTruth_barCode[i_truth], tgcTruth_time[i_truth]);
    return hit;
  }

  MuonCalibRawMdtHit* NTReader::getMuonCalibRawMdtHit( int i_hit ) {
    Amg::Vector3D locPos( rawMdt_posX[i_hit], rawMdt_posY[i_hit], rawMdt_posZ[i_hit] );
    Amg::Vector3D globPos( rawMdt_gPosX[i_hit], rawMdt_gPosY[i_hit], rawMdt_gPosZ[i_hit] );
    MuonFixedId id( rawMdt_id[i_hit] );

    MuonCalibRawMdtHit *hit = new MuonCalibRawMdtHit( id, locPos, globPos, rawMdt_occupancy[i_hit]);
    hit->setAdc( rawMdt_adc[i_hit] );
    hit->setTdc( rawMdt_tdc[i_hit] );
    hit->setDriftTime( rawMdt_t[i_hit] );
    hit->setDriftRadius( rawMdt_r[i_hit]);
    hit->setDriftRadiusError( rawMdt_dr[i_hit]);

    return hit;
  }
  
  MuonCalibRawRpcHit* NTReader::getMuonCalibRawRpcHit( int i_hit ) {
    Amg::Vector3D globPos( rawRpc_gPosX[i_hit], rawRpc_gPosY[i_hit], rawRpc_gPosZ[i_hit] );
    MuonFixedId id( rawRpc_id[i_hit] );
    MuonCalibRawRpcHit *hit = new MuonCalibRawRpcHit( id, globPos, rawRpc_occupancy[i_hit], rawRpc_t[i_hit], 
						      rawRpc_width[i_hit], rawRpc_length[i_hit] );
    return hit;
  }

  MuonCalibRawRpcTriggerHit* NTReader::getMuonCalibRawRpcTriggerHit( int i_hit ) {
    MuonCalibRawRpcTriggerHit *hit = new MuonCalibRawRpcTriggerHit( rpc_sector[i_hit],rpc_padId[i_hit],rpc_status[i_hit],rpc_ercode[i_hit],
								    rpc_cmaId[i_hit],rpc_fel1Id[i_hit],rpc_febcId[i_hit],
								    rpc_crc[i_hit],rpc_bcId[i_hit],rpc_ticks[i_hit],rpc_ijk[i_hit],
								    rpc_cmachan[i_hit],rpc_overlap[i_hit],rpc_threshold[i_hit] );
    return hit;
  }
  
  MuonCalibRawTgcHit* NTReader::getMuonCalibRawTgcHit( int i_hit ) {

    MuonCalibRawTgcHit *hit = 0;

    if(rawTgc_bcTag[i_hit]==1){
      int hit_prev = rawTgc_num_Prev[i_hit];
      Amg::Vector3D globPos( rawTgc_gPosX_Prev[hit_prev], rawTgc_gPosY_Prev[hit_prev], rawTgc_gPosZ_Prev[hit_prev] );
      MuonFixedId id( rawTgc_id_Prev[hit_prev] );
      hit = new MuonCalibRawTgcHit( id, globPos, rawTgc_occupancy_Prev[hit_prev], rawTgc_station_Prev[hit_prev], 
				    rawTgc_eta_Prev[hit_prev], rawTgc_phi_Prev[hit_prev], rawTgc_gasGap_Prev[hit_prev], 
				    rawTgc_isStrip_Prev[hit_prev], rawTgc_channel_Prev[hit_prev], rawTgc_bcTag[i_hit], 
				    rawTgc_width_Prev[hit_prev], rawTgc_shortWidth_Prev[hit_prev], rawTgc_longWidth_Prev[hit_prev], 
				    rawTgc_length_Prev[hit_prev] );
    } else if (rawTgc_bcTag[i_hit]==2){
      int hit_curr = rawTgc_num_Curr[i_hit];
      Amg::Vector3D globPos( rawTgc_gPosX[hit_curr], rawTgc_gPosY[hit_curr], rawTgc_gPosZ[hit_curr] );
      MuonFixedId id( rawTgc_id[hit_curr] );
      hit = new MuonCalibRawTgcHit( id, globPos, rawTgc_occupancy[hit_curr], rawTgc_station[hit_curr], 
				    rawTgc_eta[hit_curr], rawTgc_phi[hit_curr], rawTgc_gasGap[hit_curr], 
				    rawTgc_isStrip[hit_curr], rawTgc_channel[hit_curr], rawTgc_bcTag[i_hit], 
				    rawTgc_width[hit_curr], rawTgc_shortWidth[hit_curr], rawTgc_longWidth[hit_curr], 
				    rawTgc_length[hit_curr] );
    } else if (rawTgc_bcTag[i_hit]==3){
      int hit_next = rawTgc_num_Next[i_hit];
      Amg::Vector3D globPos( rawTgc_gPosX_Next[hit_next], rawTgc_gPosY_Next[hit_next], rawTgc_gPosZ_Next[hit_next] );
      MuonFixedId id( rawTgc_id_Next[hit_next] );
      hit = new MuonCalibRawTgcHit( id, globPos, rawTgc_occupancy_Next[hit_next], rawTgc_station_Next[hit_next], 
				    rawTgc_eta_Next[hit_next], rawTgc_phi_Next[hit_next], rawTgc_gasGap_Next[hit_next], 
				    rawTgc_isStrip_Next[hit_next], rawTgc_channel_Next[hit_next], rawTgc_bcTag[i_hit], 
				    rawTgc_width_Next[hit_next], rawTgc_shortWidth_Next[hit_next], rawTgc_longWidth_Next[hit_next], 
				    rawTgc_length_Next[hit_next] );
    }

    return hit;
  }  //end NTReader::getMuonCalibRawTgcHit

  MuonCalibRawTgcCoin* NTReader::getMuonCalibRawTgcCoin( int i_hit ) {
    
    MuonCalibRawTgcCoin *hit = 0;

    if(rawTgcCoin_type_bcTag[i_hit]==1){
      int tracklet_prev = rawTgcCoin_num_tracklet_Prev[i_hit];
      Amg::Vector3D globPosIn( rawTgcCoin_gPosInX_tracklet_Prev[tracklet_prev], rawTgcCoin_gPosInY_tracklet_Prev[tracklet_prev], rawTgcCoin_gPosInZ_tracklet_Prev[tracklet_prev] );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_tracklet_Prev[tracklet_prev], rawTgcCoin_gPosOutY_tracklet_Prev[tracklet_prev], rawTgcCoin_gPosOutZ_tracklet_Prev[tracklet_prev] );
      MuonFixedId id( rawTgcCoin_id_tracklet_Prev[tracklet_prev] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 0, rawTgcCoin_eta_tracklet_Prev[tracklet_prev], 
				     rawTgcCoin_phi_tracklet_Prev[tracklet_prev], rawTgcCoin_sector_tracklet_Prev[tracklet_prev],
				     rawTgcCoin_isForward_tracklet_Prev[tracklet_prev], rawTgcCoin_isStrip_tracklet_Prev[tracklet_prev], 
				     rawTgcCoin_trackletId_tracklet_Prev[tracklet_prev], -1, 
				     1, rawTgcCoin_widthIn_tracklet_Prev[tracklet_prev], rawTgcCoin_widthOut_tracklet_Prev[tracklet_prev], 0, 0,
				     rawTgcCoin_delta_tracklet_Prev[tracklet_prev], 0, 0, rawTgcCoin_sub_tracklet_Prev[tracklet_prev], 0);
    } else if (rawTgcCoin_type_bcTag[i_hit]==2){
      int highpt_prev = rawTgcCoin_num_highpt_Prev[i_hit];
      Amg::Vector3D globPosIn( rawTgcCoin_gPosInX_highpt_Prev[highpt_prev], rawTgcCoin_gPosInY_highpt_Prev[highpt_prev], rawTgcCoin_gPosInZ_highpt_Prev[highpt_prev] );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_highpt_Prev[highpt_prev], rawTgcCoin_gPosOutY_highpt_Prev[highpt_prev], rawTgcCoin_gPosOutZ_highpt_Prev[highpt_prev] );
      MuonFixedId id( rawTgcCoin_id_highpt_Prev[highpt_prev] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 1, rawTgcCoin_eta_highpt_Prev[highpt_prev], 
				     rawTgcCoin_phi_highpt_Prev[highpt_prev], rawTgcCoin_sector_highpt_Prev[highpt_prev],
				     rawTgcCoin_isForward_highpt_Prev[highpt_prev], rawTgcCoin_isStrip_highpt_Prev[highpt_prev], 
				     rawTgcCoin_trackletId_highpt_Prev[highpt_prev], -1, 
				     1, rawTgcCoin_widthIn_highpt_Prev[highpt_prev], rawTgcCoin_widthOut_highpt_Prev[highpt_prev], 0, 0,
				     rawTgcCoin_delta_highpt_Prev[highpt_prev], 0, 0, rawTgcCoin_sub_highpt_Prev[highpt_prev], 0);
    } else if (rawTgcCoin_type_bcTag[i_hit]==3){
      int sl_prev = rawTgcCoin_num_sl_Prev[i_hit];
      Amg::Vector3D globPosIn( 0.0, 0.0, 0.0 );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_sl_Prev[sl_prev], rawTgcCoin_gPosOutY_sl_Prev[sl_prev], rawTgcCoin_gPosOutZ_sl_Prev[sl_prev] );
      MuonFixedId id( rawTgcCoin_id_sl_Prev[sl_prev] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 2, rawTgcCoin_eta_sl_Prev[sl_prev], 
				     rawTgcCoin_phi_sl_Prev[sl_prev], rawTgcCoin_sector_sl_Prev[sl_prev],
				     rawTgcCoin_isForward_sl_Prev[sl_prev], 0, 
				     rawTgcCoin_trackletId_sl_Prev[sl_prev], rawTgcCoin_trackletIdStrip_sl_Prev[sl_prev], 
				     1, 0, 0, rawTgcCoin_widthR_sl_Prev[sl_prev], rawTgcCoin_widthPhi_sl_Prev[sl_prev],
				     0, rawTgcCoin_roi_sl_Prev[sl_prev], rawTgcCoin_pt_sl_Prev[sl_prev], 0, rawTgcCoin_isPositiveDeltaR_sl_Prev[sl_prev]);
    } else if (rawTgcCoin_type_bcTag[i_hit]==4){
      int tracklet_curr = rawTgcCoin_num_tracklet_Curr[i_hit];
      Amg::Vector3D globPosIn( rawTgcCoin_gPosInX_tracklet[tracklet_curr], rawTgcCoin_gPosInY_tracklet[tracklet_curr], rawTgcCoin_gPosInZ_tracklet[tracklet_curr] );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_tracklet[tracklet_curr], rawTgcCoin_gPosOutY_tracklet[tracklet_curr], rawTgcCoin_gPosOutZ_tracklet[tracklet_curr] );
      MuonFixedId id( rawTgcCoin_id_tracklet[tracklet_curr] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 0, rawTgcCoin_eta_tracklet[tracklet_curr], 
				     rawTgcCoin_phi_tracklet[tracklet_curr], rawTgcCoin_sector_tracklet[tracklet_curr],
				     rawTgcCoin_isForward_tracklet[tracklet_curr], rawTgcCoin_isStrip_tracklet[tracklet_curr], 
				     rawTgcCoin_trackletId_tracklet[tracklet_curr], -1, 
				     2, rawTgcCoin_widthIn_tracklet[tracklet_curr], rawTgcCoin_widthOut_tracklet[tracklet_curr], 0, 0,
				     rawTgcCoin_delta_tracklet[tracklet_curr], 0, 0, rawTgcCoin_sub_tracklet[tracklet_curr], 0);
    } else if (rawTgcCoin_type_bcTag[i_hit]==5){
      int highpt_curr = rawTgcCoin_num_highpt_Curr[i_hit];
      Amg::Vector3D globPosIn( rawTgcCoin_gPosInX_highpt[highpt_curr], rawTgcCoin_gPosInY_highpt[highpt_curr], rawTgcCoin_gPosInZ_highpt[highpt_curr] );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_highpt[highpt_curr], rawTgcCoin_gPosOutY_highpt[highpt_curr], rawTgcCoin_gPosOutZ_highpt[highpt_curr] );
      MuonFixedId id( rawTgcCoin_id_highpt[highpt_curr] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 1, rawTgcCoin_eta_highpt[highpt_curr], 
				     rawTgcCoin_phi_highpt[highpt_curr], rawTgcCoin_sector_highpt[highpt_curr],
				     rawTgcCoin_isForward_highpt[highpt_curr], rawTgcCoin_isStrip_highpt[highpt_curr], 
				     rawTgcCoin_trackletId_highpt[highpt_curr], -1, 
				     2, rawTgcCoin_widthIn_highpt[highpt_curr], rawTgcCoin_widthOut_highpt[highpt_curr], 0, 0,
				     rawTgcCoin_delta_highpt[highpt_curr], 0, 0, rawTgcCoin_sub_highpt[highpt_curr], 0);
    } else if (rawTgcCoin_type_bcTag[i_hit]==6){
      int sl_curr = rawTgcCoin_num_sl_Curr[i_hit];
      Amg::Vector3D globPosIn( 0.0, 0.0, 0.0 );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_sl[sl_curr], rawTgcCoin_gPosOutY_sl[sl_curr], rawTgcCoin_gPosOutZ_sl[sl_curr] );
      MuonFixedId id( rawTgcCoin_id_sl[sl_curr] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 2, rawTgcCoin_eta_sl[sl_curr], 
				     rawTgcCoin_phi_sl[sl_curr], rawTgcCoin_sector_sl[sl_curr],
				     rawTgcCoin_isForward_sl[sl_curr], 0, 
				     rawTgcCoin_trackletId_sl[sl_curr], rawTgcCoin_trackletIdStrip_sl[sl_curr], 
				     2, 0, 0, rawTgcCoin_widthR_sl[sl_curr], rawTgcCoin_widthPhi_sl[sl_curr],
				     0, rawTgcCoin_roi_sl[sl_curr], rawTgcCoin_pt_sl[sl_curr], 0, rawTgcCoin_isPositiveDeltaR_sl[sl_curr]);
    } else if (rawTgcCoin_type_bcTag[i_hit]==7){
      int tracklet_next = rawTgcCoin_num_tracklet_Next[i_hit];
      Amg::Vector3D globPosIn( rawTgcCoin_gPosInX_tracklet_Next[tracklet_next], rawTgcCoin_gPosInY_tracklet_Next[tracklet_next], rawTgcCoin_gPosInZ_tracklet_Next[tracklet_next] );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_tracklet_Next[tracklet_next], rawTgcCoin_gPosOutY_tracklet_Next[tracklet_next], rawTgcCoin_gPosOutZ_tracklet_Next[tracklet_next] );
      MuonFixedId id( rawTgcCoin_id_tracklet_Next[tracklet_next] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 0, rawTgcCoin_eta_tracklet_Next[tracklet_next], 
				     rawTgcCoin_phi_tracklet_Next[tracklet_next], rawTgcCoin_sector_tracklet_Next[tracklet_next],
				     rawTgcCoin_isForward_tracklet_Next[tracklet_next], rawTgcCoin_isStrip_tracklet_Next[tracklet_next], 
				     rawTgcCoin_trackletId_tracklet_Next[tracklet_next], -1, 
				     3, rawTgcCoin_widthIn_tracklet_Next[tracklet_next], rawTgcCoin_widthOut_tracklet_Next[tracklet_next], 0, 0,
				     rawTgcCoin_delta_tracklet_Next[tracklet_next], 0, 0, rawTgcCoin_sub_tracklet_Next[tracklet_next], 0);
    } else if (rawTgcCoin_type_bcTag[i_hit]==8){
      int highpt_next = rawTgcCoin_num_highpt_Next[i_hit];
      Amg::Vector3D globPosIn( rawTgcCoin_gPosInX_highpt_Next[highpt_next], rawTgcCoin_gPosInY_highpt_Next[highpt_next], rawTgcCoin_gPosInZ_highpt_Next[highpt_next] );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_highpt_Next[highpt_next], rawTgcCoin_gPosOutY_highpt_Next[highpt_next], rawTgcCoin_gPosOutZ_highpt_Next[highpt_next] );
      MuonFixedId id( rawTgcCoin_id_highpt_Next[highpt_next] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 1, rawTgcCoin_eta_highpt_Next[highpt_next], 
				     rawTgcCoin_phi_highpt_Next[highpt_next], rawTgcCoin_sector_highpt_Next[highpt_next],
				     rawTgcCoin_isForward_highpt_Next[highpt_next], rawTgcCoin_isStrip_highpt_Next[highpt_next], 
				     rawTgcCoin_trackletId_highpt_Next[highpt_next], -1, 
				     3, rawTgcCoin_widthIn_highpt_Next[highpt_next], rawTgcCoin_widthOut_highpt_Next[highpt_next], 0, 0,
				     rawTgcCoin_delta_highpt_Next[highpt_next], 0, 0, rawTgcCoin_sub_highpt_Next[highpt_next], 0);
    } else if (rawTgcCoin_type_bcTag[i_hit]==9){
      int sl_next = rawTgcCoin_num_sl_Next[i_hit];
      Amg::Vector3D globPosIn( 0.0, 0.0, 0.0 );
      Amg::Vector3D globPosOut( rawTgcCoin_gPosOutX_sl_Next[sl_next], rawTgcCoin_gPosOutY_sl_Next[sl_next], rawTgcCoin_gPosOutZ_sl_Next[sl_next] );
      MuonFixedId id( rawTgcCoin_id_sl_Next[sl_next] );
      hit = new MuonCalibRawTgcCoin( id, globPosIn, globPosOut, 2, rawTgcCoin_eta_sl_Next[sl_next], 
				     rawTgcCoin_phi_sl_Next[sl_next], rawTgcCoin_sector_sl_Next[sl_next],
				     rawTgcCoin_isForward_sl_Next[sl_next], 0, 
				     rawTgcCoin_trackletId_sl_Next[sl_next], rawTgcCoin_trackletIdStrip_sl_Next[sl_next], 
				     3, 0, 0, rawTgcCoin_widthR_sl_Next[sl_next], rawTgcCoin_widthPhi_sl_Next[sl_next],
				     0, rawTgcCoin_roi_sl_Next[sl_next], rawTgcCoin_pt_sl_Next[sl_next], 0, rawTgcCoin_isPositiveDeltaR_sl_Next[sl_next]);
    }

    return hit;
  }  //end NTReader::getMuonCalibRawTgcCoin

  MuonCalibRawCscHit* NTReader::getMuonCalibRawCscHit( int i_hit ) {
    Amg::Vector3D globPos( rawCsc_gPosX[i_hit], rawCsc_gPosY[i_hit], rawCsc_gPosZ[i_hit] );
    MuonFixedId id( rawCsc_id[i_hit] );
    MuonCalibRawCscHit *hit = new MuonCalibRawCscHit( id, globPos, rawCsc_occupancy[i_hit], 
						      rawCsc_t[i_hit], rawCsc_width[i_hit], rawCsc_charge[i_hit] );
    return hit;
  }

  MuonCalibPattern* NTReader::getPattern( int i_pattern ) {
    MuonCalibPattern *MCP = new MuonCalibPattern( pat_chi2[i_pattern] , pat_z0[i_pattern],
						  pat_r0[i_pattern], pat_invP[i_pattern],
						  pat_phi[i_pattern], pat_theta[i_pattern], 
						  pat_nmdt[i_pattern], pat_nrpc[i_pattern],
						  pat_ntgc[i_pattern], pat_ncsc[i_pattern] );
    return MCP;
  }
  
  MuonCalibSegment* NTReader::getSegment( int i_segment ) {
    Amg::Vector3D seg_localPos(seg_posX[i_segment], seg_posY[i_segment], seg_posZ[i_segment]);
    Amg::Vector3D seg_localDir(seg_dirX[i_segment], seg_dirY[i_segment], seg_dirZ[i_segment]);
    
    double phi   = seg_transPhi[i_segment];
    double theta = seg_transTheta[i_segment];
    double psi   = seg_transPsi[i_segment];    

    Amg::Translation3D  translation( seg_transX[i_segment], seg_transY[i_segment], seg_transZ[i_segment] );
    
    double sinPhi   = std::sin( phi   );
    double cosPhi   = std::cos( phi   );
    double sinTheta = std::sin( theta );
    double cosTheta = std::cos( theta );
    double sinPsi   = std::sin( psi   );
    double cosPsi   = std::cos( psi   );

    double rxx = cosTheta*cosPhi;  
    double rxy = cosTheta*sinPhi;
    double rxz = -sinTheta;   
      
    double ryx = sinPsi*sinTheta*cosPhi - cosPsi*sinPhi;
    double ryy = sinPsi*sinTheta*sinPhi + cosPsi*cosPhi;
    double ryz = cosTheta*sinPsi;
      
    double rzx = cosPsi*sinTheta*cosPhi + sinPsi*sinPhi;
    double rzy = cosPsi*sinTheta*sinPhi - sinPsi*cosPhi;
    double rzz = cosTheta*cosPsi;

    Amg::Vector3D colx( rxx ,ryx, rzx );
    Amg::Vector3D coly( rxy, ryy, rzy );
    Amg::Vector3D colz( rxz, ryz, rzz );

    Amg::RotationMatrix3D rotation;
    rotation.col(0)=colx;
    rotation.col(1)=coly;
    rotation.col(2)=colz;
   
    Amg::Transform3D loc2Glob;

    loc2Glob = translation * rotation;
    Double_t seg_chisq = seg_chi2[i_segment]; 
    MuonCalibSegment* MCS = new MuonCalibSegment(seg_chisq, seg_localPos, seg_localDir, loc2Glob, seg_quality[i_segment]); 

    MCS->setAuthor(seg_author[i_segment]); 
    MCS->setFittedT0(seg_fittedT0[i_segment]);

    if( (seg_gPosZ[i_segment]- MCS->globalPosition().z()>0.1 ) || (seg_gPosY[i_segment]-MCS->globalPosition().y()>0.1) 
	|| (seg_gPosX[i_segment]- MCS->globalPosition().x()>0.1) ){
    }
    
    for( Int_t i_hit = 0; i_hit < mdt_nMdt; ++i_hit){
      if( mdt_segIndex[i_hit] == i_segment)  {
 	MCS->addHitOnTrack( getMdtHit(i_hit) );
      }
    }
    for( Int_t i_hit = 0; i_hit < csc_nCsc; ++i_hit){
      if( csc_segIndex[i_hit] == i_segment)  {
 	MCS->addHitOnTrack( getCscHit(i_hit) );
      }
    }

    if(rpcOs_nRpcHits > 0){
      for( Int_t i_hit = 0; i_hit < rpcOs_nRpcHits; ++i_hit){
	if( rpcOs_segIndex[i_hit] == i_segment)  MCS->addHitOnTrack( getRpcOsHit(i_hit) );
      }
    } else {
      for( Int_t i_hit = 0; i_hit < rpc_nRpcHits; ++i_hit){
	if( rpc_segIndex[i_hit] == i_segment)  MCS->addHitOnTrack( getRpcHit(i_hit) );
      }
    }

    for( Int_t i_hit = 0; i_hit < tgc_nTgc; ++i_hit){
      if( tgc_segIndex[i_hit] == i_segment)  MCS->addHitOnTrack( getTgcHit(i_hit) );
    }

    return MCS;
  }  //end NTReader::getSegment

  MdtCalibHitBase* NTReader::getMdtHit( int i_hit) {

    Amg::Vector3D mdt_localPos(mdt_posX[i_hit], mdt_posY[i_hit], mdt_posZ[i_hit]); //Corrected for ntuple Y<->Z swap
    Amg::Vector3D mdt_globPos(mdt_gPosX[i_hit], mdt_gPosY[i_hit], mdt_gPosZ[i_hit]);
    MdtCalibHitBase *hit = new MdtCalibHitBase(mdt_tdc[i_hit], mdt_adc[i_hit], mdt_globPos, mdt_localPos);
    MuonFixedId id( mdt_id[i_hit] );

    hit->setIdentifier( id );
    hit->setDriftTime( mdt_t[i_hit] );
    hit->setDriftRadius( mdt_r[i_hit], mdt_dr[i_hit] );
    hit->setDistanceToTrack( mdt_rTrk[i_hit], mdt_drTrk[i_hit] );
    hit->setSlewingTime( mdt_slewTime[i_hit] );
    hit->setBFieldTime( mdt_lorTime[i_hit] );
    hit->setPropagationTime( mdt_propTime[i_hit] );
    hit->setTimeOfFlight( mdt_tof[i_hit] );
    hit->setDistanceToReadout( mdt_distRO[i_hit] );
    hit->setBFieldPerp( mdt_bFieldPerp[i_hit] );
    hit->setBFieldPara( mdt_bFieldPara[i_hit] );
    hit->setTemperature( mdt_temperature[i_hit] );
    hit->setProjSag( mdt_projSag[i_hit] );
    hit->setTubeT0( mdt_tube_t0[i_hit] );
    hit->setTubeAdcCal( mdt_tube_adccal[i_hit] );
    hit->setLocXtwin( mdt_xtwin[i_hit] );
    hit->setSegmentT0Applied( mdt_segmentT0Applied[i_hit] );
    return hit;
  }  //end NTReader::getMdtHit

  RpcCalibHitBase* NTReader::getRpcHit( int i_hit) {
    const Amg::Vector3D rpc_localPos(rpc_posX[i_hit], rpc_posY[i_hit], rpc_posZ[i_hit]);
    const Amg::Vector3D rpc_globPos(rpc_gPosX[i_hit], rpc_gPosY[i_hit], rpc_gPosZ[i_hit]);
    MuonFixedId id( rpc_id[i_hit] );
    RpcCalibHitBase *hit = new RpcCalibHitBase(rpc_nStrips[i_hit], rpc_stripWidth[i_hit], 
					       rpc_stripLength[i_hit], rpc_time[i_hit],
					       rpc_error[i_hit], rpc_globPos, rpc_localPos);
    hit->setIdentifier( id );
    hit->setDistanceToRO( rpc_distanceToRO[i_hit] );
    return hit;
  }

  RpcCalibHitBase* NTReader::getRpcOsHit( int i_hit) {
    const Amg::Vector3D rpcOs_localPos(rpcOs_posX[i_hit], rpcOs_posY[i_hit], rpcOs_posZ[i_hit]);
    const Amg::Vector3D rpcOs_globPos(rpcOs_gPosX[i_hit], rpcOs_gPosY[i_hit], rpcOs_gPosZ[i_hit]);
    MuonFixedId id( rpcOs_id[i_hit] );
    
    RpcCalibHitBase *hit = new RpcCalibHitBase(rpcOs_nStrips[i_hit], rpcOs_stripWidth[i_hit], 
					       rpcOs_stripLength[i_hit], rpcOs_time[i_hit], 
					       rpcOs_error[i_hit], rpcOs_globPos, rpcOs_localPos); 
    hit->setIdentifier( id );
    hit->setDistanceToRO( rpcOs_distanceToRO[i_hit] );
    return hit;
  }

  CscCalibHitBase* NTReader::getCscHit( int i_hit) {
    const Amg::Vector3D csc_localPos(csc_posX[i_hit], csc_posY[i_hit], csc_posZ[i_hit]);
    const Amg::Vector3D csc_globPos(csc_gPosX[i_hit], csc_gPosY[i_hit], csc_gPosZ[i_hit]);
    MuonFixedId id( csc_id[i_hit] );
    
    CscCalibHitBase *hit = new CscCalibHitBase(csc_nStrips[i_hit], csc_stripWidth[i_hit], csc_charge[i_hit], 
					       csc_error[i_hit], csc_globPos, csc_localPos); 
    hit->setIdentifier( id );
    return hit;
  }

  TgcCalibHitBase* NTReader::getTgcHit( int i_hit) { 
    const Amg::Vector3D tgc_localPos(tgc_posX[i_hit], tgc_posY[i_hit], tgc_posZ[i_hit]);
    const Amg::Vector3D tgc_globPos(tgc_gPosX[i_hit], tgc_gPosY[i_hit], tgc_gPosZ[i_hit]);
    MuonFixedId id( tgc_id[i_hit] );
    
    TgcCalibHitBase *hit = new TgcCalibHitBase( tgc_nStrips[i_hit], tgc_stripWidth[i_hit], 
						tgc_error[i_hit], tgc_globPos, tgc_localPos );
    hit->setIdentifier( id ); 
    hit->setStripLength( tgc_stripLength[i_hit] );

    return hit;
  }

} //namespace MuonCalib
