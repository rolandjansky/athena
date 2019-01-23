/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TGCSectorLogic.cxx,v 1.14 2009-03-15 18:07:55 isaya Exp $
#include "TrigT1TGC/TGCSectorLogic.hh"
#include "TrigT1TGC/TGCElectronicsSystem.hh"
#include "TrigT1TGC/TGCHighPtBoard.hh"
#include "TrigT1TGC/TGCHighPtBoardOut.hh"
#include "TrigT1TGC/TGCHighPtChipOut.hh"
#include "TrigT1TGC/TGCTMDB.h"
#include "TrigT1TGC/TGCTMDBOut.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

#include <iostream>

namespace LVL1TGCTrigger {
  
  extern bool g_USE_INNER;
  extern bool g_INNER_VETO;
  extern bool g_TILE_MU;
  extern bool g_USE_CONDDB;

TGCSectorLogic::TGCSectorLogic(TGCRegionType regionIn, int idIn):
    m_bid(0),
    m_id(idIn),
    m_sectorId(0), m_moduleId(0),
    m_sideId(0),   m_octantId(0),
    m_region(regionIn),
    m_NumberOfWireHighPtBoard(0), 
    m_SSCController(this), 
    m_matrix(this),
    m_mapInner(0),
    m_mapTileMu(0),
    m_pTMDB(0),
    m_preSelector(this), 
    m_selector(this), 
    m_selectorOut(0),
    m_wordTileMuon(0),
    m_wordInnerStation(0),
    m_stripHighPtBoard(0),
    m_stripHighPtChipOut(0),
    m_useInner(false),
    m_useTileMu(false)
{
  m_sideId = (idIn/NumberOfModule)/NumberOfOctant;
  m_octantId = (idIn/NumberOfModule)%NumberOfOctant;
  m_moduleId = idIn%NumberOfModule;
  if (m_region==ENDCAP) {
    m_sectorId  = m_moduleId%3 + 2*(m_moduleId/3);
    m_sectorId += 6*m_octantId;
  } else {
    m_sectorId  = m_moduleId/3; 
    m_sectorId += 3*m_octantId;
  }

  for(int i=0; i<MaxNumberOfWireHighPtBoard; i++){
      m_wireHighPtBoard[i] = 0;
      m_wireHighPtChipOut[i] = 0;
  }
  
  for(unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
    m_innerTrackletSlots[iSlot] = 0;
  }

  m_SSCController.setRegion(regionIn);

  m_useInner  = g_USE_INNER && (m_region==ENDCAP); 
  m_useTileMu = g_TILE_MU && (m_region==ENDCAP); 
}

TGCSectorLogic::~TGCSectorLogic()
{
  if(m_selectorOut) delete m_selectorOut;
}

void TGCSectorLogic::setRPhiMap(const TGCRPhiCoincidenceMap* map,
				const TGCInnerCoincidenceMap* mapI)
{
  m_matrix.setRPhiMap(map);
  m_mapInner = mapI;
  if (m_mapInner ==0) m_useInner = false;
}

void TGCSectorLogic::setInnerMap(const TGCInnerCoincidenceMap* mapI)
{
  m_mapInner = mapI;
  if (m_mapInner ==0) m_useInner = false;
}

void TGCSectorLogic::setTileMuMap(const TGCTMDB* tmdb,
				  const  TGCTileMuCoincidenceMap* mapTM)
{
  m_pTMDB = tmdb;
  m_mapTileMu = mapTM;
  if (m_mapTileMu ==0 || m_pTMDB==0) m_useTileMu = false;
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

TGCSLSelectorOut* TGCSectorLogic::getSelectorOutput() const
{
  return m_selectorOut;
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
    ////////////////////////////////////////////
    // do coincidence with Inner Tracklet and/or TileMu
    if (m_useInner) doInnerCoincidence(readCondKey, SSCid, coincidenceOut);

    if(coincidenceOut) m_preSelector.input(coincidenceOut);
      // coincidenceOut will be deleted 
      //  in m_preSelector.input() if coincidenceOut has no hit
      //  in m_preSelector.select() if if coincidenceOut has hit
  }
  if(SSCCOut!=0) delete SSCCOut;
  SSCCOut=0;

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


TGCSectorLogic::TGCSectorLogic(const TGCSectorLogic& right): 
     m_bid(right.m_id), m_id(right.m_id),
     m_sectorId(right.m_sectorId), m_moduleId(right.m_moduleId),
     m_sideId(right.m_sideId), m_octantId(right.m_octantId),
     m_region(right.m_region),
     m_NumberOfWireHighPtBoard(right.m_NumberOfWireHighPtBoard),
     m_SSCController(this), 
     m_matrix(this),
     m_mapInner(right.m_mapInner),
     m_mapTileMu(right.m_mapTileMu), m_pTMDB(right.m_pTMDB),
     m_preSelector(this), m_selector(this),
     m_selectorOut(0),
     m_wordTileMuon(0), m_wordInnerStation(0),
     m_stripHighPtBoard(right.m_stripHighPtBoard), 
     m_stripHighPtChipOut(0),
     m_useInner(right.m_useInner), m_useTileMu(right.m_useTileMu)
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
    m_mapInner=right.m_mapInner;
    m_mapTileMu=right.m_mapTileMu;
    m_pTMDB=right.m_pTMDB;
    delete m_selectorOut;
    m_selectorOut=0;
    m_wordTileMuon=0;
    m_wordInnerStation=0;
    m_stripHighPtBoard=right.m_stripHighPtBoard;  
    m_stripHighPtChipOut=0;
    m_useInner=right.m_useInner;
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
  if (coincidenceOut ==0) return;

  int pt = coincidenceOut->getPtLevel();
  if (pt==0) return;

  SG::ReadCondHandle<TGCTriggerData> readHandle{readCondKey};
  const TGCTriggerData* readCdo{*readHandle};

  if (g_USE_CONDDB) {
    bool isActiveTile = readCdo->isActive(TGCTriggerData::CW_TILE);
    m_useTileMu = isActiveTile && (m_region==ENDCAP);
  }

  // check if inner is used for the ptLevel
  bool validInner = (m_mapInner->getFlagPT(pt, ssc, m_sectorId) == 1);

  // check if TileMu is used for the ptLevel
  bool validTileMu = false;
  if (m_useTileMu)  validTileMu = (m_mapTileMu->getFlagPT(pt, ssc, m_sectorId, m_sideId) == 1) ;
  
  int pos = 4*coincidenceOut->getR() +  coincidenceOut->getPhi();
  // check if inner is used for the roi 
  if (validInner) validInner = (m_mapInner->getFlagROI(pos, ssc, m_sectorId) == 1);
 
  // check if TileMu is used for the roi 
  if (validTileMu) validTileMu = (m_mapTileMu->getFlagROI(pos, ssc, m_sectorId, m_sideId) == 1);
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
	  isHitWire =   m_mapInner->getTriggerBit(iSlot, ssc, m_sectorId, reg, TGCInnerTrackletSlot::WIRE, bit)
	    &&   hit->getTriggerBit(reg,TGCInnerTrackletSlot::WIRE,bit) ;
	}
	// Strip
	bool isHitStrip = false;
	for (size_t bit=0; (!isHitStrip) && (bit< TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS); bit++){
	  isHitStrip =  m_mapInner->getTriggerBit(iSlot, ssc, m_sectorId, reg, TGCInnerTrackletSlot::STRIP, bit)
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
    for ( int mod=0; mod< TGCTileMuCoincidenceMap::N_Input_TileMuModule; mod++){
      int maskTM =  m_mapTileMu->getMask(mod, ssc, m_sectorId, m_sideId);
      const TGCTMDBOut* tm = m_pTMDB->getOutput(m_sideId, m_sectorId, mod); 
      int  hit6      = tm->GetHit6();
      int  hit56     = tm->GetHit56(); 
      if          (maskTM == TGCTileMuCoincidenceMap::TM_D6_L) {
	isHitTileMu = isHitTileMu || (hit6==TGCTMDBOut::TM_LOW) || (hit6==TGCTMDBOut::TM_HIGH) ;
      } else  if  (maskTM == TGCTileMuCoincidenceMap::TM_D6_H) {
	isHitTileMu = isHitTileMu || (hit6==TGCTMDBOut::TM_HIGH) ;
      } else  if  (maskTM == TGCTileMuCoincidenceMap::TM_D56_L) {
	isHitTileMu = isHitTileMu || (hit56==TGCTMDBOut::TM_LOW) || (hit56==TGCTMDBOut::TM_HIGH) ;
      } else  if  (maskTM == TGCTileMuCoincidenceMap::TM_D56_H) {
	isHitTileMu = isHitTileMu || (hit56==TGCTMDBOut::TM_HIGH) ;
      } 
#ifdef TGCDEBUG
      std::cout<<"SL id=" << m_id 
	       <<" sector="<<m_sectorId
	       <<" TMDB mod=" << mod 
	       <<"  mask:"<< maskTM
	       <<"  hit56:"<< hit56
	       <<"  hit6:"<< hit6
	       << std::endl;
#endif
    }

    // wordTIleMuon
    m_wordTileMuon =0;
    for ( int mod=0; mod< TGCTileMuCoincidenceMap::N_Input_TileMuModule; mod++){
      const TGCTMDBOut* tm = m_pTMDB->getOutput(m_sideId, m_sectorId, mod); 
      if (tm->GetHit6()>0)  m_wordTileMuon |= 0x02 << mod*2;
      if (tm->GetHit56()>0) m_wordTileMuon |= 0x01 << mod*2;
    }
  } 
 
  if ( !validInner && !validTileMu ) return;  //OK
 
  if ( validInner   &&  isHitInner)  return; //OK
  
  if  (validTileMu  &&  isHitTileMu) return; //OK 

  // NO Trigger
  coincidenceOut->setInnerVeto(true);

  // decrease pt level to the highest pt without InnerrCoin
  
  bool innerVeto = g_INNER_VETO;
  if (g_USE_CONDDB) {
    bool isActiveEifi = readCdo->isActive(TGCTriggerData::CW_EIFI);  
    innerVeto  = isActiveEifi && (m_region==ENDCAP);
  }

  coincidenceOut->clearHit(pt);
  
  while (innerVeto && validInner && (pt>1) ) {
    pt = pt-1;
    validInner = (m_mapInner->getFlagPT(pt, ssc, m_sectorId) == 1);
  }
  while (m_useTileMu && validTileMu && (pt>1) ){
    pt = pt-1;
    validTileMu = (m_mapTileMu->getFlagPT(pt, ssc, m_sectorId, m_sideId) == 1) ;
  }
  coincidenceOut->setHit(pt);
 
}

} //end of namespace bracket
