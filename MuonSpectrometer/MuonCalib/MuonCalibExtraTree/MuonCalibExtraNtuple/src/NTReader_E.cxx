/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraNtuple/NTReader_E.h"
#include "CLHEP/Geometry/Point3D.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"
#include "MuonCalibExtraTreeEvent/MuonCalibMUCTPI.h"
#include "MuonCalibExtraTreeEvent/MuonCalibRpcSectorLogic.h"
#include "MuonCalibEventBase/MuonCalibRawRpcCoin.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHole_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraNtuple/NTReaderBase_E.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"

#include <iostream>

namespace MuonCalib {

  void NTReader_E::setdbgcout(const bool dbglvl){
    m_debug = dbglvl;
  }

  NTReader_E::~NTReader_E(){
    clearEvent_E();
  }

  void NTReader_E::clearEvent_E(){
    delete m_evente;
    m_evente=0;
  }

  const MuonCalibEvent_E& NTReader_E::getEvent( int i_event ){
  
    clearEvent_E();

    if(!fChain->GetEntry( i_event )) {
      m_evente = new MuonCalibEvent_E();
      return *m_evente;
    }

    MuonCalibRawHitCollection *raw = buildRawColl();
    const MuonCalibEventInfo info = buildEventInfo();
    MuonCalibEvent::MCPVec patternVec = buildPatternVec();

    m_evente = new MuonCalibEvent_E();
    m_evente->setMuonCalibPatternVec(patternVec);
    m_evente->setMuonCalibRawHitCollection(raw);
    
    if(m_storeTruth==true) {
      MuonCalibTruthCollection *truth = buildTruthColl();
      m_evente->setMuonCalibTruthCollection(truth);
    }

    m_evente->setMuonCalibEventInfo(info);

    for( int i_trig = 0 ; i_trig < ctp_nTriggerInfo ; ++ i_trig ){
      m_evente->addCtp( getCtpTbpHit(i_trig) );
    }
    if(muctpi_nHits>99) {
     muctpi_nHits = 99;
    }  
    for( int i_muctpi = 0 ; i_muctpi < muctpi_nHits ; ++ i_muctpi ){
      m_evente->addMUCTPI( getMUCTPIHit(i_muctpi) );
    }

    if(rpcSl_nLogics > 0){
      for( int i_rpcsl = 0 ; i_rpcsl < rpcSl_nLogics ; ++ i_rpcsl ){
	m_evente->addRpcSL( getRpcSLHit(i_rpcsl) );
      }
    }

    for( int i_rpcco = 0 ; i_rpcco < rawRpcCoin_nRpcCoin ; ++ i_rpcco ){
      m_evente->addRpcCoin( getRpcCoinHit(i_rpcco) );
    }

    for( int i_calo = 0 ; i_calo < calo_nHits ; ++ i_calo ){
      m_evente->addL1Calo( getL1CaloHit(i_calo) );
    }

    for( int i_mbts = 0 ; i_mbts < mbts_nHits ; ++ i_mbts ){
      m_evente->addMBTS( getMBTSHit(i_mbts) );
    }

    for( int i_pat = 0 ; i_pat < pat_nPatterns ; ++ i_pat ){
      m_evente->addPattern( getPhiPattern(i_pat) );
    }
    buildSegmentOnTrackMap();
    for( int i_trk = 0 ; i_trk < trk_nTracks ; ++ i_trk ){
      m_evente->addTrack( getTrack(i_trk) );
    }
    
    return *m_evente;
  }  //end NTReader_E::getEvent
  
  MuonCalibHit_E* NTReader_E::getPhiHit( int i_phi ) {
    MuonFixedId id( phi_id[i_phi] );
    Amg::Vector3D position( phi_posX[i_phi], phi_posY[i_phi], phi_posZ[i_phi] );
    double error = phi_error[i_phi];
    MuonCalibHit_E *hit = new MuonCalibHit_E(id, position,0.,error);
    return hit;
  }

  MuonCalibHit_E* NTReader_E::getTrackHit( int i_trkHit ) {
    MuonFixedId id( trkHit_id[i_trkHit] );
    Amg::Vector3D position( trkHit_posX[i_trkHit], trkHit_posY[i_trkHit], trkHit_posZ[i_trkHit] );
    double error = trkHit_error[i_trkHit];
    MuonCalibHit_E *hit = new MuonCalibHit_E(id, position, trkHit_driftRadius[i_trkHit], error, 
					     trkHit_resi[i_trkHit], trkHit_pull[i_trkHit], trkHit_type[i_trkHit]);
    return hit;
  }

  MuonCalibHole_E* NTReader_E::getTrackHole( int i_trkHole ) {
    MuonFixedId id( trkHole_id[i_trkHole] );
    Amg::Vector3D position( trkHole_posX[i_trkHole], trkHole_posY[i_trkHole], trkHole_posZ[i_trkHole] );
    MuonCalibHole_E *hole = new MuonCalibHole_E(id, position);
    return hole;
  }

  MuonCalibTriggerInfo* NTReader_E::getCtpTbpHit( int i_trigHit ) {
    int theTbpBit    = ctp_tbp[i_trigHit];  
    int theTapBit    = ctp_tap[i_trigHit];  
    int theTavBit    = ctp_tav[i_trigHit];  
    int theBcIndex   = ctp_bcIndexTriggerItems[i_trigHit];  
    MuonCalibTriggerInfo *trig = new MuonCalibTriggerInfo(theTbpBit, theTapBit, theTavBit, theBcIndex);
    return trig;
  } 

  MuonCalibTriggerInfo* NTReader_E::getCtpHit( int i_trigHit ) {
    int theTypeValue = ctp_type[i_trigHit];
    double theDelayValue = ctp_delay[i_trigHit];
    MuonCalibTriggerType theTrigType = (MuonCalibTriggerType)theTypeValue;
    
    MuonCalibTriggerInfo *trig = new MuonCalibTriggerInfo(theTrigType, theDelayValue);
    return trig;
  } 

  MuonCalibMUCTPI* NTReader_E::getMUCTPIHit( int i_trigMUCTPI ) {
    MuonCalibMUCTPI *muctpi = new MuonCalibMUCTPI(muctpi_roiWord[i_trigMUCTPI], muctpi_bcID[i_trigMUCTPI], 
						  muctpi_sysID[i_trigMUCTPI], muctpi_subsysID[i_trigMUCTPI], 
						  muctpi_sectorID[i_trigMUCTPI], muctpi_thresholdNumber[i_trigMUCTPI], 
						  muctpi_thresholdValue[i_trigMUCTPI], muctpi_roINumber[i_trigMUCTPI], 
						  muctpi_overlap[i_trigMUCTPI], muctpi_firstCandidate[i_trigMUCTPI], 
						  muctpi_sectorOverflow[i_trigMUCTPI], muctpi_padOverflow[i_trigMUCTPI], 
						  muctpi_phi[i_trigMUCTPI], muctpi_eta[i_trigMUCTPI]);
    return muctpi;
  } 
  
  MuonCalibRpcSectorLogic* NTReader_E::getRpcSLHit( int i_rpcSL ) {
    MuonCalibRpcSectorLogic *rpcsl = new MuonCalibRpcSectorLogic(rpcSl_sectorId[i_rpcSL],  rpcSl_fel1Id[i_rpcSL],  
								 rpcSl_bcid[i_rpcSL],  rpcSl_errorCode[i_rpcSL],  
								 rpcSl_crc[i_rpcSL],  rpcSl_hasMoreThan2TriggerCand[i_rpcSL],  
								 rpcSl_slIndex[i_rpcSL], rpcSl_triggerRates[i_rpcSL], 
								 rpcSl_counters[i_rpcSL], rpcSl_slIndex2[i_rpcSL],  
								 rpcSl_rowinBcid[i_rpcSL],  rpcSl_padid[i_rpcSL],  
								 rpcSl_ptid[i_rpcSL],  rpcSl_roi[i_rpcSL],  
								 rpcSl_outerPlane[i_rpcSL],  rpcSl_overlapPhi[i_rpcSL],  
								 rpcSl_overlapEta[i_rpcSL],  rpcSl_triggerBcid[i_rpcSL],
								 rpcSl_isInput[i_rpcSL] ); 
    return rpcsl;
  }

  MuonCalibRawRpcCoin* NTReader_E::getRpcCoinHit( int i_rpcCoin ) {
    MuonFixedId id( rawRpcCoin_id[i_rpcCoin] );
    Amg::Vector3D position( rawRpcCoin_gPosX[i_rpcCoin], rawRpcCoin_gPosY[i_rpcCoin], rawRpcCoin_gPosZ[i_rpcCoin] ); 
    MuonCalibRawRpcHit theRpcHit( id, position, rawRpcCoin_occupancy[i_rpcCoin], rawRpcCoin_t[i_rpcCoin], rawRpcCoin_width[i_rpcCoin] );
    MuonCalibRawRpcCoin *rpcco = new MuonCalibRawRpcCoin( theRpcHit, rawRpcCoin_ijk[i_rpcCoin], 
							  rawRpcCoin_threshold[i_rpcCoin], rawRpcCoin_overlap[i_rpcCoin], 
							  rawRpcCoin_parentCmId[i_rpcCoin], rawRpcCoin_parentPadId[i_rpcCoin],
							  rawRpcCoin_parentSectorId[i_rpcCoin], rawRpcCoin_lowPtCm[i_rpcCoin] ); 
    return rpcco;
  }

  MuonCalibCaloHit* NTReader_E::getMBTSHit( int i_trkHit ) {
    int id = mbts_id[i_trkHit];
    Amg::Vector3D position( mbts_posX[i_trkHit], mbts_posY[i_trkHit], mbts_posZ[i_trkHit] );
    MuonCalibCaloHit *mbts  = new MuonCalibCaloHit(id, position,mbts_time[i_trkHit],mbts_charge[i_trkHit]);
    return mbts;
  } 

  MuonCalibCaloHit* NTReader_E::getL1CaloHit( int i_trkHit ) {
    int id = calo_id[i_trkHit];
    Amg::Vector3D position( calo_posX[i_trkHit], calo_posY[i_trkHit], calo_posZ[i_trkHit] );
    MuonCalibCaloHit *calo = new MuonCalibCaloHit(id, position,calo_time[i_trkHit],calo_charge[i_trkHit]);
    return calo;
  } 

  MuonCalibTrack_E* NTReader_E::getPhiPattern( int i_pat ){

    double r0  = NTReader::pat_r0[i_pat];
    double phi = NTReader::pat_phi[i_pat];
    double x0  = r0*std::sin(phi);
    double y0  = -r0*std::cos(phi);
    
    MuonCalibTrack_E* pattern = new MuonCalibTrack_E( x0, y0, NTReader::pat_z0[i_pat], NTReader::pat_phi[i_pat],
						      NTReader::pat_theta[i_pat], 
                                                      NTReader::pat_invP[i_pat],0,   
                                                      -999.,-999.,-999.,-999.,-999.,  
                                                      -999.,-999.,-999.,-999.,  
                                                      -999.,-999.,-999.,  
                                                      -999.,-999.,  
                                                      -999., 0.,0);   
    for( int i_phi = 0 ; i_phi < phi_nPhiHits ; ++i_phi ){
      if( phi_phiPatIndex[i_phi] == i_pat ) {
	pattern->addHit( getPhiHit(i_phi) ) ;
      }
    } 
    return pattern;  
  }

  MuonCalibTrack_E* NTReader_E::getTrack( int i_trk ){
    MuonCalibTrack_E* track = new MuonCalibTrack_E(  trk_x0[i_trk], trk_y0[i_trk], trk_z0[i_trk], 
						     trk_phi[i_trk], trk_theta[i_trk], trk_qOverP[i_trk],trk_author[i_trk], 
						     trk_cov00[i_trk],trk_cov01[i_trk],trk_cov02[i_trk],trk_cov03[i_trk],trk_cov04[i_trk],
						     trk_cov11[i_trk],trk_cov12[i_trk],trk_cov13[i_trk],trk_cov14[i_trk],
						     trk_cov22[i_trk],trk_cov23[i_trk],trk_cov24[i_trk],
						     trk_cov33[i_trk],trk_cov34[i_trk],
						     trk_cov44[i_trk],
						     trk_chi2[i_trk],trk_ndof[i_trk]);   
    for( int i_trkHit = 0 ; i_trkHit < trkHit_nHits ; ++i_trkHit ){
      if( trkHit_trackIndex[i_trkHit] == i_trk ) {
	track->addHit( getTrackHit(i_trkHit) ) ;
      }
    }
    for( int i_trkHole = 0 ; i_trkHole < trkHole_nHoles ; ++i_trkHole ){
      if( trkHole_trackIndex[i_trkHole] == i_trk ) {
	track->addHole( getTrackHole(i_trkHole) ) ;
      }
    }
    std::map<int, std::list<int> >::const_iterator it=m_smap.find(i_trk);
    if(it!=m_smap.end()) {
      for (std::list<int> :: const_iterator it2=it->second.begin(); it2!=it->second.end(); it2++) {
	track->addSegmentOnTrack(m_ordered_segments[*it2]);
      }
    }

    return track;  
  }  //end NTReader_E::getTrack

  void NTReader_E::buildSegmentOnTrackMap() {
    m_smap.clear();
    if(!hasTrkSegs)
      return;
    for(int i=0; i<trkSeg_nTrkSegs; i++) {
      m_smap[trkSeg_trkIndex[i]].push_back(trkSeg_segIndex[i]);
    }
  }

} //namespace MuonCalib
