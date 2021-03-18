/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSectorLogic.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "TrigT1TGC/TGCHighPtBoard.h"
#include "TrigT1TGC/TGCHighPtBoardOut.h"
#include "TrigT1TGC/TGCHighPtChipOut.h"
#include "TrigT1TGC/TGCTMDB.h"
#include "TrigT1TGC/TGCTMDBOut.h"
#include "TrigT1TGC/TGCEIFICoincidenceMap.h"
#include "TrigT1TGC/TGCTileMuCoincidenceLUT.h"
#include "TrigT1TGC/TGCNSW.h"
#include "TrigT1TGC/NSWTrigOut.h"
#include "TrigT1TGC/TGCNSWCoincidenceMap.h"
#include "TrigT1TGC/TGCGoodMF.h"
#include "TrigT1TGC/Run2TileMuCoincidenceMap.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

//for Run3
#include "TrigT1TGC/TGCTrackSelectorOut.h"

#include <iostream>

namespace LVL1TGCTrigger {
  
TGCSectorLogic::TGCSectorLogic(TGCArguments* tgcargs, const TGCDatabaseManager* db,
                               TGCRegionType regionIn, int idIn)
 : m_bid(0),
   m_id(idIn),
   m_region(regionIn),
   m_NumberOfWireHighPtBoard(0), 
   m_SSCController(tgcargs,this), 
   m_matrix(tgcargs,this),
   m_pTMDB(0),
   m_preSelector(this), // for Run2 
   m_selector(this),//for Run2  
   m_selectorOut(0), //for Run2
   m_trackSelector(this),// for Run3
   m_wordTileMuon(0),
   m_wordInnerStation(0),
   m_stripHighPtBoard(0),
   m_stripHighPtChipOut(0),
   m_tgcArgs(tgcargs)
{
  m_sideId = (idIn / NumberOfModule) / NumberOfOctant;
  m_octantId = (idIn / NumberOfModule) % NumberOfOctant;
  m_moduleId = idIn % NumberOfModule;

  if (m_region==ENDCAP) {
    m_sectorId  = m_moduleId%3 + 2*(m_moduleId/3);
    m_sectorId += 6*m_octantId;
  } else {
    m_sectorId  = m_moduleId/3; 
    m_sectorId += 3*m_octantId;
  }

  m_nswSide   =   (tgcArgs()->NSWSideInfo().find("A")!=std::string::npos && m_sideId==0)
               || (tgcArgs()->NSWSideInfo().find("C")!=std::string::npos && m_sideId==1);

  m_SSCController.setRegion(regionIn);

  m_matrix.setSideId(m_sideId);
  m_matrix.setRPhiMap(db->getRPhiCoincidenceMap(m_sideId, m_octantId));
  m_mapEIFI = db->getEIFICoincidenceMap(m_sideId);

  m_useTileMu = tgcArgs()->TILE_MU() && (m_region==ENDCAP);
  if(tgcArgs()->useRun3Config()) {
    m_tileMuLUT = db->getTileMuCoincidenceLUT();
    m_useTileMu = (m_tileMuLUT != nullptr) && m_useTileMu;
  } else {
    m_mapRun2TileMu = db->getRun2TileMuCoincidenceMap();
    m_useTileMu = (m_mapRun2TileMu != nullptr) && m_useTileMu;
  }

  m_mapGoodMF = db->getGoodMFMap();
  m_mapNSW = db->getNSWCoincidenceMap(m_sideId, m_octantId, m_moduleId);

  m_trackSelectorOut.reset(new TGCTrackSelectorOut());//for Run3

  for(int i=0; i<MaxNumberOfWireHighPtBoard; i++){
      m_wireHighPtBoard[i] = 0;
      m_wireHighPtChipOut[i] = 0;
  }

  for(unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
    m_innerTrackletSlots[iSlot] = 0;
  }

  m_useEIFI  = tgcArgs()->USE_INNER() && (m_region==ENDCAP);
  if(m_mapEIFI == 0) m_useEIFI = false;
  m_useGoodMF = (m_mapGoodMF != nullptr);
  if(m_mapNSW == 0) tgcArgs()->set_USE_NSW(false);
}

TGCSectorLogic::~TGCSectorLogic()
{
  if(m_selectorOut) delete m_selectorOut;
}

void TGCSectorLogic::setTMDB(const TGCTMDB* tmdb)
{
  m_pTMDB = tmdb;
  if (m_pTMDB==0) m_useTileMu = false;
}

void TGCSectorLogic::setNSW(std::shared_ptr<const TGCNSW> nsw)
{
  m_nsw = nsw;
  if(m_nsw == 0) tgcArgs()->set_USE_NSW(false);
}

void TGCSectorLogic::setWireHighPtBoard(int port, TGCHighPtBoard* highPtBoard)
{
  m_wireHighPtBoard[port] = highPtBoard;
  m_NumberOfWireHighPtBoard +=1;
}

void TGCSectorLogic::setStripHighPtBoard(TGCHighPtBoard* highPtBoard)
{
  m_stripHighPtBoard = highPtBoard;	 
}

void TGCSectorLogic::getTrackSelectorOutput(std::shared_ptr<TGCTrackSelectorOut> &trackSelectorOut)const
{
  trackSelectorOut=m_trackSelectorOut;
}

void TGCSectorLogic::eraseSelectorOut()
{
  m_selectorOut=0;
}

void TGCSectorLogic::clockIn(const SG::ReadCondHandleKey<TGCTriggerData> readCondKey,
                             int bidIn)
{
  int SSCid, phiposInSSC;
  m_bid=bidIn;

  collectInput();

  TGCSSCControllerOut* SSCCOut =
    m_SSCController.distribute(m_wireHighPtChipOut,m_stripHighPtChipOut);
#ifdef TGCDEBUG
  SSCCOut->print();
#endif  
  deleteHPBOut();

  m_preSelector.init();

  for(SSCid=0; SSCid<getNumberOfSubSectorCluster(); SSCid+=1){
    TGCRPhiCoincidenceOut* coincidenceOut = 0;
    if(SSCCOut->hasHit(SSCid)){
      m_matrix.clear();
      m_matrix.setSSCId(SSCid);
      m_matrix.inputR(SSCCOut->getR(SSCid),SSCCOut->getDR(SSCid),SSCCOut->getPtR(SSCid));
      for(phiposInSSC = 0 ;phiposInSSC < MaxNumberOfPhiInSSC; phiposInSSC++){
        if(SSCCOut->hasHit(SSCid, phiposInSSC)){
          m_matrix.inputPhi(SSCCOut->getPhi(SSCid,phiposInSSC),
                          SSCCOut->getDPhi(SSCid,phiposInSSC),
                          SSCCOut->getPtPhi(SSCid,phiposInSSC));
        }
      }
      coincidenceOut = m_matrix.doCoincidence();
    }
    ////// for SSC has chamber boundary //////
    if(SSCCOut->hasHit(SSCid,true)){
      m_matrix.clear();
      m_matrix.setSSCId(SSCid);
      m_matrix.inputR(SSCCOut->getR(SSCid),SSCCOut->getDR(SSCid),SSCCOut->getPtR(SSCid));
      for(phiposInSSC = 0 ;phiposInSSC < MaxNumberOfPhiInSSC; phiposInSSC++){
        if(SSCCOut->hasHit(SSCid, phiposInSSC, true)){
          m_matrix.inputPhi(SSCCOut->getPhi(SSCid,phiposInSSC,true),
                          SSCCOut->getDPhi(SSCid,phiposInSSC,true),
                          SSCCOut->getPtPhi(SSCid,phiposInSSC,true));
        }
      }
      
      TGCRPhiCoincidenceOut* oredCoincidenceOut = m_matrix.doCoincidence();
      if (oredCoincidenceOut) {
	if(coincidenceOut) {
	  if (coincidenceOut->isSuperior(oredCoincidenceOut)) {
	    delete oredCoincidenceOut;
	  } else {
	    delete coincidenceOut;
	    coincidenceOut = oredCoincidenceOut;
	  }
	} else {
	  coincidenceOut = oredCoincidenceOut;
	}
      }
    }

    if(tgcArgs()->useRun3Config()){
      if(coincidenceOut){
        if (m_useGoodMF){
          bool isgoodMF;
          isgoodMF = m_mapGoodMF->test_GoodMF(m_moduleId,SSCid,coincidenceOut->getRoI());
          coincidenceOut->setGoodMFFlag(isgoodMF);
        }
      }
    }

    ////////////////////////////////////////////
    // do coincidence with Inner Tracklet of EIFI and/or TileMu

    if(!tgcArgs()->useRun3Config()){
      if (m_useEIFI){ 
	doInnerCoincidence(readCondKey, SSCid, coincidenceOut);
      }
    }
    else{
      doInnerCoincidenceRun3(SSCid, coincidenceOut);/*InnerCoincidence Algorithm for Run3 will be implemented;*/
    }


    if(coincidenceOut){
      if(tgcArgs()->useRun3Config()){m_trackSelector.input(coincidenceOut);}// TrackSelector for Run3
      else{m_preSelector.input(coincidenceOut);}// TrackSelector for Run2
      // coincidenceOut will be deleted 
      //  in m_preSelector.input() if coincidenceOut has no hit
      //  in m_preSelector.select() if if coincidenceOut has hit
    }
  }
  if(SSCCOut!=0) delete SSCCOut;
  SSCCOut=0;

  if(tgcArgs()->useRun3Config()){
    //Track selector in Run3. max 4 tracks ,which are send to MUCTPI, are selected. 
    m_trackSelector.select(m_trackSelectorOut);
  }
  else{
#ifdef TGCDEBUG
    m_preSelector.dumpInput();
#endif
  
    // get SLPreSelectorOut
    TGCSLPreSelectorOut* preSelectorOut = m_preSelector.select();
    // preSelectorOut will be deleted after m_selector.select() 

#ifdef TGCDEBUG
    preSelectorOut->print();
#endif

    // delete SLSelectorOut if exists
    if(m_selectorOut!=0) delete m_selectorOut;
    // create new  SLSelectorOut
    m_selectorOut = new TGCSLSelectorOut;

    if(preSelectorOut!=0){
      // select final canidates
      m_selector.select(preSelectorOut,m_selectorOut);
    
      // delete SLPreSelectorOut
      delete preSelectorOut;
      preSelectorOut=0;
    }
  }

#ifdef TGCDEBUG
  showResult(m_selectorOut);
#endif
  // EIFI trigger information is dumped when we have Endcap SL trigger
  /*
  if(m_region==ENDCAP && m_selectorOut!=0 && m_selectorOut->getNCandidate()) {
    for(unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
      std::cout << " innerTrackletSlots[" << iSlot << "]"; 
      if(m_innerTrackletSlots[iSlot]) {
	std::cout << " m_sideId " << m_innerTrackletSlots[iSlot]->getSideId() 
		  << " slotId " << m_innerTrackletSlots[iSlot]->getSlotId()
		  << " triggerBits ";
	for(unsigned int iRegion=0; iRegion<TGCInnerTrackletSlot::NUMBER_OF_REGIONS; iRegion++) {
	  for(unsigned int readout=0; readout<TGCInnerTrackletSlot::NUMBER_OF_READOUTS; readout++) {
	    for(unsigned int iBit=0; iBit<TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS; iBit++) {
	      std::cout << (m_innerTrackletSlots[iSlot]->getTriggerBit(iRegion,readout,iBit) ? 1 : 0);
	    }
	  }
	}
      } 
      std::cout << std::endl;
    }
  }
  */
}

void TGCSectorLogic::collectInput()
{
  int i;
  for( i = 0; i < m_SSCController.getNumberOfWireHighPtBoard(); i += 1) {
    m_wireHighPtChipOut[i] = m_wireHighPtBoard[i]->getOutput();
    m_wireHighPtBoard[i]->eraseOutput();
  }  

  m_stripHighPtChipOut = m_stripHighPtBoard->getOutput();
  m_stripHighPtBoard->eraseOutput();
}

void TGCSectorLogic::deleteHPBOut()
{
  if(m_stripHighPtChipOut!=0) delete m_stripHighPtChipOut;
  m_stripHighPtChipOut=0;

  int i;
  for( i = 0; i < m_SSCController.getNumberOfWireHighPtBoard(); i += 1) {
    if(m_wireHighPtChipOut[i]!=0) delete m_wireHighPtChipOut[i];
    m_wireHighPtChipOut[i]=0;
  }
}


void TGCSectorLogic::showResult(TGCSLSelectorOut* out)
{
  int i;
  std::cout<<"#SL O"<<" BID:"<<m_bid
	   <<" region:"<<((m_region==FORWARD) ? "FWD" : "END")
	   <<" SLid:"<<m_id<<" ";

  for( i=0; i<out->getNCandidate(); i+=1){
    std::cout<<"  "<<i<<" "
	     <<out->getPtLevel(i)<<" "
	     <<out->getR(i)<<" "
	     <<out->getPhi(i)<<std::endl;
  }
  std::cout<<std::endl;

}


TGCSectorLogic::TGCSectorLogic(const TGCSectorLogic& right)
 : m_bid(right.m_id), m_id(right.m_id),
   m_sectorId(right.m_sectorId), m_moduleId(right.m_moduleId),
   m_sideId(right.m_sideId), m_octantId(right.m_octantId),
   m_region(right.m_region),
   m_NumberOfWireHighPtBoard(right.m_NumberOfWireHighPtBoard),
   m_useEIFI(right.m_useEIFI), m_useTileMu(right.m_useTileMu),
   m_useGoodMF(right.m_useGoodMF), m_nswSide(right.m_nswSide),
   m_SSCController(right.tgcArgs(),this), 
   m_matrix(right.tgcArgs(),this),
   m_mapEIFI(right.m_mapEIFI),
   m_pTMDB(right.m_pTMDB),
   m_preSelector(this), m_selector(this),
   m_selectorOut(0),
   m_wordTileMuon(0), m_wordInnerStation(0),
   m_stripHighPtBoard(right.m_stripHighPtBoard), 
   m_stripHighPtChipOut(0),
   m_tgcArgs(right.m_tgcArgs)
{
  for(int i=0; i<MaxNumberOfWireHighPtBoard; i++){
      m_wireHighPtBoard[i] = 0;
      m_wireHighPtChipOut[i] = 0;
  }

  for( int i = 0; i < m_SSCController.getNumberOfWireHighPtBoard(); i += 1) {
    m_wireHighPtBoard[i]   = right.m_wireHighPtBoard[i];
    m_wireHighPtChipOut[i] = right.m_wireHighPtChipOut[i];
  }

  for (unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
    m_innerTrackletSlots[iSlot] = right.m_innerTrackletSlots[iSlot];
  }
}

TGCSectorLogic&
TGCSectorLogic::operator=(const TGCSectorLogic& right)
{
  if ( this != &right ) {
    m_bid =right.m_bid;
    m_id  =right.m_id;
    m_sectorId=right.m_sectorId;
    m_moduleId=right.m_moduleId;
    m_sideId=right.m_sideId;
    m_octantId=right.m_octantId;
    m_region=right.m_region;
    m_NumberOfWireHighPtBoard=right.m_NumberOfWireHighPtBoard;
    m_mapEIFI=right.m_mapEIFI;
    m_pTMDB=right.m_pTMDB;
    delete m_selectorOut;
    m_selectorOut=0;
    m_wordTileMuon=0;
    m_wordInnerStation=0;
    m_stripHighPtBoard=right.m_stripHighPtBoard;  
    m_stripHighPtChipOut=0;
    m_useEIFI=right.m_useEIFI;
    m_useTileMu=right.m_useTileMu;
    for( int i = 0; i < m_SSCController.getNumberOfWireHighPtBoard(); i += 1) {
      m_wireHighPtBoard[i]   = right.m_wireHighPtBoard[i];
      m_wireHighPtChipOut[i] = right.m_wireHighPtChipOut[i];
    }
    for (unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
      m_innerTrackletSlots[iSlot] = right.m_innerTrackletSlots[iSlot];
    }
  }
  return *this;
}


void TGCSectorLogic::dec2bin(int dec, char* binstr, int length)
{
  for (int i=0; i<length; i++){
    if((dec>>i) & 1)
      binstr[length-1-i] = '1';
    else
      binstr[length-1-i] = '0';
  }
  binstr[length] ='\0';
}

void TGCSectorLogic::setInnerTrackletSlots(const TGCInnerTrackletSlot* innerTrackletSlots[]) 
{
  for(unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
    m_innerTrackletSlots[iSlot] = innerTrackletSlots[iSlot];
  }
}

 
void TGCSectorLogic::doInnerCoincidence(const SG::ReadCondHandleKey<TGCTriggerData> readCondKey,
                                        int ssc,
					TGCRPhiCoincidenceOut* coincidenceOut)
{
  ////////////////////////////////////////////////////////////////////////////
  // This function is only called for Run 2.
  ////////////////////////////////////////////////////////////////////////////

  if (coincidenceOut ==0) return;

  int pt = coincidenceOut->getPtLevel();
  if (pt==0) return;

  SG::ReadCondHandle<TGCTriggerData> readHandle{readCondKey};
  const TGCTriggerData* readCdo{*readHandle};

  // Run-2 Tile-Muon coincidence uses CONDDB, and m_useTileMu is set to the active content.
  // (go into the scope only once, if the Tile-CW DB is not active)
  if(m_useTileMu) {
    m_useTileMu = readCdo->isActive(TGCTriggerData::CW_TILE);
  }

  // check if inner is used for the ptLevel
  bool validInner = (m_mapEIFI->getFlagPT(pt, ssc, m_sectorId) == 1);

  // check if TileMu is used for the ptLevel
  bool validTileMu = false;
  if (m_useTileMu)  validTileMu = (m_mapRun2TileMu->getFlagPT(pt, ssc, m_sectorId, m_sideId) == 1) ;
  
  int pos = 4*coincidenceOut->getR() +  coincidenceOut->getPhi();
  // check if inner is used for the roi 
  if (validInner) validInner = (m_mapEIFI->getFlagROI(pos, ssc, m_sectorId) == 1);
 
  // check if TileMu is used for the roi 
  if (validTileMu) validTileMu = (m_mapRun2TileMu->getFlagROI(pos, ssc, m_sectorId, m_sideId) == 1);
  // not use InnerStation if TileMu is used
  if (validTileMu) validInner = false;

  // Check hits of inner tracklet
  bool isHitInner = false;
  if (validInner) {
    for(unsigned int iSlot=0; (!isHitInner) && (iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR); iSlot++) {
      const TGCInnerTrackletSlot* hit = m_innerTrackletSlots[iSlot];
      
      for (size_t reg=0; (!isHitInner) && (reg< TGCInnerTrackletSlot::NUMBER_OF_REGIONS); reg++){
	// Wire    
	bool isHitWire = false;
        
	for (size_t bit=0; (!isHitWire) && (bit< TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS); bit++){
	  isHitWire =   m_mapEIFI->getTriggerBit(iSlot, ssc, m_sectorId, reg, TGCInnerTrackletSlot::WIRE, bit)
	    &&   hit->getTriggerBit(reg,TGCInnerTrackletSlot::WIRE,bit) ;
	}
	// Strip
	bool isHitStrip = false;
	for (size_t bit=0; (!isHitStrip) && (bit< TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS); bit++){
	  isHitStrip =  m_mapEIFI->getTriggerBit(iSlot, ssc, m_sectorId, reg, TGCInnerTrackletSlot::STRIP, bit)
	    && hit->getTriggerBit(reg,TGCInnerTrackletSlot::STRIP,bit);
	}
	isHitInner = isHitWire && isHitStrip;
      }
    }

    // m_wordInnerStation
    // FI[3:0] EI[3:0]
    m_wordInnerStation = 0;
    for(unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
      const TGCInnerTrackletSlot* hit = m_innerTrackletSlots[iSlot];
      for (size_t reg=0; reg< TGCInnerTrackletSlot::NUMBER_OF_REGIONS; reg++){ // reg = 0:FI, 1:EI
	bool isHitW = false;
	bool isHitS = false;
	for (size_t bit=0; bit< TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS; bit++){
	  isHitW  = isHitW ||  hit->getTriggerBit(reg,TGCInnerTrackletSlot::WIRE,bit);
	  isHitS  = isHitS || hit->getTriggerBit(reg,TGCInnerTrackletSlot::STRIP,bit);
	}
	if (isHitW && isHitS) {
	  m_wordInnerStation |= (2-reg)<<(iSlot*2);
	}
      }
    }
  }

  // Check hits of TileMu
  bool isHitTileMu = false;
  if (validTileMu) {

    m_wordTileMuon = 0;
    for(int mod=0; mod< LVL1TGC::Run2TileMuCoincidenceMap::N_Input_TileMuModule; mod++) {
      uint8_t maskTM = m_mapRun2TileMu->getTrigMask(mod, ssc, m_sectorId, m_sideId);
      const TGCTMDBOut* tm = m_pTMDB->getOutput(m_sideId, m_sectorId, mod);
      isHitTileMu = isHitTileMu || this->hitTileMu(maskTM, tm->getHit6(), tm->getHit56());

      if (tm->getHit6()>0)  m_wordTileMuon |= 0x02 << mod*2;
      if (tm->getHit56()>0) m_wordTileMuon |= 0x01 << mod*2;
    }
  } 
 
  if ( !validInner && !validTileMu ) return;  //OK
 
  if ( validInner   &&  isHitInner)  return; //OK
  
  if  (validTileMu  &&  isHitTileMu) return; //OK 

  // NO Trigger
  coincidenceOut->setInnerVeto(true);

  // decrease pt level to the highest pt without InnerrCoin

  bool innerVeto = tgcArgs()->INNER_VETO();
  if (tgcArgs()->USE_CONDDB()) {
    bool isActiveEifi = readCdo->isActive(TGCTriggerData::CW_EIFI);  
    innerVeto  = isActiveEifi && (m_region==ENDCAP);
  }

  coincidenceOut->clearHit(pt);

  while (innerVeto && validInner && (pt>1) ) {
    pt = pt-1;
    validInner = (m_mapEIFI->getFlagPT(pt, ssc, m_sectorId) == 1);
  }
  while (m_useTileMu && validTileMu && (pt>1) ){
    pt = pt-1;
    validTileMu = (m_mapRun2TileMu->getFlagPT(pt, ssc, m_sectorId, m_sideId) == 1) ;
  }
  coincidenceOut->setHit(pt);
 
}



  ////////////////////////////////////////
  // Inner Coincidnece Algorithms on Run3
  ////////////////////////////////////////
  void TGCSectorLogic::doInnerCoincidenceRun3(int SSCId,  TGCRPhiCoincidenceOut* coincidenceOut){
    if (coincidenceOut ==0) return;
    
    int pt = coincidenceOut->getpT();
    if (pt==0) return;

    if(SSCId<=4 && m_region==ENDCAP){//3 detectors are used to inner coincidnece in SSC#0~4 in Endcap;  

      // WHICH INNER COINCIDENCE
      // select a inner station detector which is used in inner coincidence algorithm.
      //Defenation of innerDetectorNumber :: enum{EI=0,TILE,BIS78};

      /*int innerDetectorNumber = which_InnerCoincidence();*/ //this function will be implemented.

      // The below section is a tmporary method. It will be replaced with WICHINNER COINCIDENCE.  
      int pos = 4*coincidenceOut->getR() +  coincidenceOut->getPhi();
      // check if inner is used for the roi 
      bool validEI = (m_mapEIFI->getFlagROI(pos, coincidenceOut->getIdSSC(), m_sectorId) == 1);
      // check if TileMu is used for the roi 
      bool validTileMu = (m_tileMuLUT->getFlagROI(pos, coincidenceOut->getIdSSC(), m_sectorId, m_sideId) == 1);


      bool isEI=false;
      bool isTILE=false;
      if(m_useEIFI && validEI){isEI=doTGCEICoincidence(coincidenceOut);}
      if(m_useTileMu && validTileMu){isTILE=doTILECoincidence(coincidenceOut); }
      //if(m_useBIS && validBIS){isBIS=doTGCBISCoincidence(coincidenceOut); }

      coincidenceOut->setInnerCoincidenceFlag(isEI || isTILE || (!m_useEIFI && !validEI && !m_useTileMu && !validTileMu));
    }
    else{//  NSW or FI are used to inner coincidnece in SSC#5~18 in Endcap and Forward region 

      int pos = 4*coincidenceOut->getR() +  coincidenceOut->getPhi();
      bool validFI = (m_mapEIFI->getFlagROI(pos, coincidenceOut->getIdSSC(), m_sectorId) == 1);

      if(tgcArgs()->USE_NSW() && m_nswSide){ //this function will be implemented.There is a NSW on A-side only in early Run3;
	doTGCNSWCoincidence(coincidenceOut);
      }
      else if(m_nswSide && validFI){
	if(m_useEIFI){
	  coincidenceOut->setInnerCoincidenceFlag( doTGCFICoincidence(coincidenceOut) );
	}
      }
      else{coincidenceOut->setInnerCoincidenceFlag(true);}

    }



  }


  void TGCSectorLogic::doTGCNSWCoincidence(TGCRPhiCoincidenceOut* coincidenceOut){

    int pt_EtaPhi=0,pt_EtaDtheta=0;

    //////// calculate pT //////

    std::shared_ptr<const NSWTrigOut> pNSWOut = m_nsw->getOutput(m_region,m_sideId,m_sectorId);
    pt_EtaPhi = m_mapNSW->TGCNSW_pTcalcu_EtaPhi(
						pNSWOut.get(),
						coincidenceOut->getRoI()
						);

    pt_EtaDtheta = m_mapNSW->TGCNSW_pTcalcu_EtaDtheta(
						      pNSWOut.get(),
						      coincidenceOut->getRoI()
						      );
    ///////  set flag  ////////
    if(pt_EtaPhi==0 && pt_EtaDtheta==0){
      coincidenceOut->setInnerCoincidenceFlag(false);
      return;
    }
    else{
      coincidenceOut->setInnerCoincidenceFlag(true);
    }


    //////  select lowest pT   ///// will be replaced with pT merger
    int nsw_pT;
    if(pt_EtaPhi==0 || pt_EtaDtheta==0){
      nsw_pT = pt_EtaPhi!=0 ? pt_EtaPhi :  pt_EtaDtheta;
    }
    else{
      nsw_pT = pt_EtaPhi<pt_EtaDtheta ? pt_EtaPhi :  pt_EtaDtheta;
    }

    if(nsw_pT>coincidenceOut->getpT()){
      return;
    }
    else{
      coincidenceOut->setpT(nsw_pT);
      return;
    }

  }



bool TGCSectorLogic::doTILECoincidence(TGCRPhiCoincidenceOut* coincidenceOut)
{
  /////////////////////////////////////////////////////////////////////////////
  // The function is called for Run-3 algorithim, but the Run-2 algorithm is 
  // temporarily used for instance.
  ////////////////////////////////////////////////////////////////////////////

  bool isHitTileMu=false;
  for(int mod=0; mod < LVL1TGC::TGCTileMuCoincidenceLUT::N_Input_TileMuModule; mod++) {
    uint8_t maskTM = (uint8_t)(m_tileMuLUT->getTrigMask(mod, coincidenceOut->getIdSSC(), m_sectorId, m_sideId));
    const TGCTMDBOut* tm = m_pTMDB->getOutput(m_sideId, m_sectorId, mod); 
    isHitTileMu = isHitTileMu || this->hitTileMu(maskTM, tm->getHit6(), tm->getHit56());
  }

  return isHitTileMu;
}


  bool TGCSectorLogic::doTGCEICoincidence(TGCRPhiCoincidenceOut* coincidenceOut){

    bool isHitInner=false;
    for(unsigned int iSlot=0;iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
      const TGCInnerTrackletSlot* hit = m_innerTrackletSlots[iSlot];
      
      for (size_t reg=0;reg< TGCInnerTrackletSlot::NUMBER_OF_REGIONS; reg++){

	// Wire    
	bool isHitWire = false;
	for (size_t bit=0; bit< TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS; bit++){
	  isHitWire =   m_mapEIFI->getTriggerBit(iSlot, coincidenceOut->getIdSSC(), m_sectorId, reg, TGCInnerTrackletSlot::WIRE, bit)
	    &&   hit->getTriggerBit(reg,TGCInnerTrackletSlot::WIRE,bit) ;
	  if(isHitWire){break;}
	}

	// Strip
	bool isHitStrip = false;
	for (size_t bit=0;bit< TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS; bit++){
	  isHitStrip =  m_mapEIFI->getTriggerBit(iSlot, coincidenceOut->getIdSSC(), m_sectorId, reg, TGCInnerTrackletSlot::STRIP, bit)
	    && hit->getTriggerBit(reg,TGCInnerTrackletSlot::STRIP,bit);
	  if(isHitStrip){break;}
	}

	isHitInner = isHitWire && isHitStrip;
	if(isHitInner){
	  return true;
	}
      }

    }

    return false;

  }



  bool TGCSectorLogic::doTGCFICoincidence(TGCRPhiCoincidenceOut* coincidenceOut){
   return doTGCEICoincidence(coincidenceOut);
  }


bool TGCSectorLogic::hitTileMu(const uint8_t& mask, const uint8_t& hit6, const uint8_t& hit56) const
{
  /*! returns whether TMDBOut satisfies the Tile-CW mask (Run-2 algo.)
   * @param[in] mask  Trigger mask per TMDB module (3 bits)
   * @param[in] hit6  TMDBOut for D6
   * @param[in] hit56 TMDBOut for D5+D6
   */
  switch(mask) {
    case LVL1TGC::Run2TileMuCoincidenceMap::TM_D6_L:
      return (hit6==TGCTMDBOut::TM_LOW || hit6==TGCTMDBOut::TM_HIGH);
      break;
    case LVL1TGC::Run2TileMuCoincidenceMap::TM_D6_H:
      return (hit6==TGCTMDBOut::TM_HIGH);
      break;
    case LVL1TGC::Run2TileMuCoincidenceMap::TM_D56_L:
      return (hit56==TGCTMDBOut::TM_LOW || hit56==TGCTMDBOut::TM_HIGH);
      break;
    case LVL1TGC::Run2TileMuCoincidenceMap::TM_D56_H:
      return (hit56==TGCTMDBOut::TM_HIGH);
      break;
    default:
      return false;
      break;
  }
  return true;
}

} //end of namespace bracket
