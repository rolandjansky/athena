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
    bid(0),
    id(idIn),
    sectorId(0), moduleId(0),
    sideId(0),   octantId(0),
    region(regionIn),
    NumberOfWireHighPtBoard(0), 
    SSCController(this), 
    matrix(this),
    mapInner(0),
    mapTileMu(0),
    pTMDB(0),
    preSelector(this), 
    selector(this), 
    selectorOut(0),
    wordTileMuon(0),
    wordInnerStation(0),
    stripHighPtBoard(0),
    stripHighPtChipOut(0),
    useInner(false),
    useTileMu(false)
{
  sideId = (idIn/NumberOfModule)/NumberOfOctant;
  octantId = (idIn/NumberOfModule)%NumberOfOctant;
  moduleId = idIn%NumberOfModule;
  if (region==ENDCAP) {
    sectorId  = moduleId%3 + 2*(moduleId/3);
    sectorId += 6*octantId;
  } else {
    sectorId  = moduleId/3; 
    sectorId += 3*octantId;
  }

  for(int i=0; i<MaxNumberOfWireHighPtBoard; i++){
      wireHighPtBoard[i] = 0;
      wireHighPtChipOut[i] = 0;
  }
  
  for(unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
    m_innerTrackletSlots[iSlot] = 0;
  }

  SSCController.setRegion(regionIn);

  useInner  = g_USE_INNER && (region==ENDCAP); 
  useTileMu = g_TILE_MU && (region==ENDCAP); 
}

TGCSectorLogic::~TGCSectorLogic()
{
  if(selectorOut) delete selectorOut;
}

void TGCSectorLogic::setRPhiMap(const TGCRPhiCoincidenceMap* map,
				const TGCInnerCoincidenceMap* mapI)
{
  matrix.setRPhiMap(map);
  mapInner = mapI;
  if (mapInner ==0) useInner = false;
}

void TGCSectorLogic::setInnerMap(const TGCInnerCoincidenceMap* mapI)
{
  mapInner = mapI;
  if (mapInner ==0) useInner = false;
}

void TGCSectorLogic::setTileMuMap(const TGCTMDB* tmdb,
				  const  TGCTileMuCoincidenceMap* mapTM)
{
  pTMDB = tmdb;
  mapTileMu = mapTM;
  if (mapTileMu ==0 || pTMDB==0) useTileMu = false;
}

void TGCSectorLogic::setWireHighPtBoard(int port, TGCHighPtBoard* highPtBoard)
{
  wireHighPtBoard[port] = highPtBoard;
  NumberOfWireHighPtBoard +=1;
}

void TGCSectorLogic::setStripHighPtBoard(TGCHighPtBoard* highPtBoard)
{
  stripHighPtBoard = highPtBoard;	 
}

TGCSLSelectorOut* TGCSectorLogic::getSelectorOutput() const
{
  return selectorOut;
}

void TGCSectorLogic::eraseSelectorOut()
{
  selectorOut=0;
}

void TGCSectorLogic::clockIn(const SG::ReadCondHandleKey<TGCTriggerData> readCondKey,
                             int bidIn)
{
  int SSCid, phiposInSSC;
  bid=bidIn;

  collectInput();

  TGCSSCControllerOut* SSCCOut =
    SSCController.distribute(wireHighPtChipOut,stripHighPtChipOut);
#ifdef TGCDEBUG
  SSCCOut->print();
#endif  
  deleteHPBOut();

  preSelector.init();

  for(SSCid=0; SSCid<getNumberOfSubSectorCluster(); SSCid+=1){
    TGCRPhiCoincidenceOut* coincidenceOut = 0;
    if(SSCCOut->hasHit(SSCid)){
      matrix.clear();
      matrix.setSSCId(SSCid);
      matrix.inputR(SSCCOut->getR(SSCid),SSCCOut->getDR(SSCid),SSCCOut->getPtR(SSCid));
      for(phiposInSSC = 0 ;phiposInSSC < MaxNumberOfPhiInSSC; phiposInSSC++){
        if(SSCCOut->hasHit(SSCid, phiposInSSC)){
          matrix.inputPhi(SSCCOut->getPhi(SSCid,phiposInSSC),
                          SSCCOut->getDPhi(SSCid,phiposInSSC),
                          SSCCOut->getPtPhi(SSCid,phiposInSSC));
        }
      }
      coincidenceOut = matrix.doCoincidence();
    }
    ////// for SSC has chamber boundary //////
    if(SSCCOut->hasHit(SSCid,true)){
      matrix.clear();
      matrix.setSSCId(SSCid);
      matrix.inputR(SSCCOut->getR(SSCid),SSCCOut->getDR(SSCid),SSCCOut->getPtR(SSCid));
      for(phiposInSSC = 0 ;phiposInSSC < MaxNumberOfPhiInSSC; phiposInSSC++){
        if(SSCCOut->hasHit(SSCid, phiposInSSC, true)){
          matrix.inputPhi(SSCCOut->getPhi(SSCid,phiposInSSC,true),
                          SSCCOut->getDPhi(SSCid,phiposInSSC,true),
                          SSCCOut->getPtPhi(SSCid,phiposInSSC,true));
        }
      }
      
      TGCRPhiCoincidenceOut* oredCoincidenceOut = matrix.doCoincidence();
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
    if (useInner) doInnerCoincidence(readCondKey, SSCid, coincidenceOut);

    if(coincidenceOut) preSelector.input(coincidenceOut);
      // coincidenceOut will be deleted 
      //  in preSelector.input() if coincidenceOut has no hit
      //  in preSelector.select() if if coincidenceOut has hit
  }
  if(SSCCOut!=0) delete SSCCOut;
  SSCCOut=0;

#ifdef TGCDEBUG
  preSelector.dumpInput();
#endif
  
  // get SLPreSelectorOut
  TGCSLPreSelectorOut* preSelectorOut = preSelector.select();
   // preSelectorOut will be deleted after selector.select() 

#ifdef TGCDEBUG
  preSelectorOut->print();
#endif

  // delete SLSelectorOut if exists
  if(selectorOut!=0) delete selectorOut;
  // create new  SLSelectorOut
  selectorOut = new TGCSLSelectorOut;

  if(preSelectorOut!=0){
    // select final canidates
    selector.select(preSelectorOut,selectorOut);
    
    // delete SLPreSelectorOut
    delete preSelectorOut;
    preSelectorOut=0;
  }

#ifdef TGCDEBUG
  showResult(selectorOut);
#endif
  // EIFI trigger information is dumped when we have Endcap SL trigger
  /*
  if(region==ENDCAP && selectorOut!=0 && selectorOut->getNCandidate()) {
    for(unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
      std::cout << " innerTrackletSlots[" << iSlot << "]"; 
      if(m_innerTrackletSlots[iSlot]) {
	std::cout << " sideId " << m_innerTrackletSlots[iSlot]->getSideId() 
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
  for( i = 0; i < SSCController.getNumberOfWireHighPtBoard(); i += 1) {
    wireHighPtChipOut[i] = wireHighPtBoard[i]->getOutput();
    wireHighPtBoard[i]->eraseOutput();
  }  

  stripHighPtChipOut = stripHighPtBoard->getOutput();
  stripHighPtBoard->eraseOutput();
}

void TGCSectorLogic::deleteHPBOut()
{
  if(stripHighPtChipOut!=0) delete stripHighPtChipOut;
  stripHighPtChipOut=0;

  int i;
  for( i = 0; i < SSCController.getNumberOfWireHighPtBoard(); i += 1) {
    if(wireHighPtChipOut[i]!=0) delete wireHighPtChipOut[i];
    wireHighPtChipOut[i]=0;
  }
}


void TGCSectorLogic::showResult(TGCSLSelectorOut* out)
{
  int i;
  std::cout<<"#SL O"<<" BID:"<<bid
	   <<" region:"<<((region==FORWARD) ? "FWD" : "END")
	   <<" SLid:"<<id<<" ";

  for( i=0; i<out->getNCandidate(); i+=1){
    std::cout<<"  "<<i<<" "
	     <<out->getPtLevel(i)<<" "
	     <<out->getR(i)<<" "
	     <<out->getPhi(i)<<std::endl;
  }
  std::cout<<std::endl;

}


TGCSectorLogic::TGCSectorLogic(const TGCSectorLogic& right): 
     bid(right.id), id(right.id),
     sectorId(right.sectorId), moduleId(right.moduleId),
     sideId(right.sideId), octantId(right.octantId),
     region(right.region),
     NumberOfWireHighPtBoard(right.NumberOfWireHighPtBoard),
     SSCController(this), 
     matrix(this),
     mapInner(right.mapInner),
     mapTileMu(right.mapTileMu), pTMDB(right.pTMDB),
     preSelector(this), selector(this),
     selectorOut(0),
     wordTileMuon(0), wordInnerStation(0),
     stripHighPtBoard(right.stripHighPtBoard), 
     stripHighPtChipOut(0),
     useInner(right.useInner), useTileMu(right.useTileMu)
{
  for(int i=0; i<MaxNumberOfWireHighPtBoard; i++){
      wireHighPtBoard[i] = 0;
      wireHighPtChipOut[i] = 0;
  }

  for( int i = 0; i < SSCController.getNumberOfWireHighPtBoard(); i += 1) {
    wireHighPtBoard[i]   = right.wireHighPtBoard[i];
    wireHighPtChipOut[i] = right.wireHighPtChipOut[i];
  }

  for (unsigned int iSlot=0; iSlot<TGCInnerTrackletSlotHolder::NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
    m_innerTrackletSlots[iSlot] = right.m_innerTrackletSlots[iSlot];
  }
}

TGCSectorLogic&
TGCSectorLogic::operator=(const TGCSectorLogic& right)
{
  if ( this != &right ) {
    bid =right.id;
    id  =right.id;
    sectorId=right.sectorId;
    moduleId=right.moduleId;
    sideId=right.sideId;
    octantId=right.octantId;
    region=right.region;
    NumberOfWireHighPtBoard=right.NumberOfWireHighPtBoard;
    mapInner=right.mapInner;
    mapTileMu=right.mapTileMu;
    pTMDB=right.pTMDB;
    delete selectorOut;
    selectorOut=0;
    wordTileMuon=0;
    wordInnerStation=0;
    stripHighPtBoard=right.stripHighPtBoard;  
    stripHighPtChipOut=0;
    useInner=right.useInner;
    useTileMu=right.useTileMu;
    for( int i = 0; i < SSCController.getNumberOfWireHighPtBoard(); i += 1) {
      wireHighPtBoard[i]   = right.wireHighPtBoard[i];
      wireHighPtChipOut[i] = right.wireHighPtChipOut[i];
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
    useTileMu = isActiveTile && (region==ENDCAP);
  }

  // check if inner is used for the ptLevel
  bool validInner = (mapInner->getFlagPT(pt, ssc, sectorId) == 1);

  // check if TileMu is used for the ptLevel
  bool validTileMu = false;
  if (useTileMu)  validTileMu = (mapTileMu->getFlagPT(pt, ssc, sectorId, sideId) == 1) ;
  
  int pos = 4*coincidenceOut->getR() +  coincidenceOut->getPhi();
  // check if inner is used for the roi 
  if (validInner) validInner = (mapInner->getFlagROI(pos, ssc, sectorId) == 1);
 
  // check if TileMu is used for the roi 
  if (validTileMu) validTileMu = (mapTileMu->getFlagROI(pos, ssc, sectorId, sideId) == 1);
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
	  isHitWire =   mapInner->getTriggerBit(iSlot, ssc, sectorId, reg, TGCInnerTrackletSlot::WIRE, bit)
	    &&   hit->getTriggerBit(reg,TGCInnerTrackletSlot::WIRE,bit) ;
	}
	// Strip
	bool isHitStrip = false;
	for (size_t bit=0; (!isHitStrip) && (bit< TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS); bit++){
	  isHitStrip =  mapInner->getTriggerBit(iSlot, ssc, sectorId, reg, TGCInnerTrackletSlot::STRIP, bit)
	    && hit->getTriggerBit(reg,TGCInnerTrackletSlot::STRIP,bit);
	}
	isHitInner = isHitWire && isHitStrip;
      }
    }

    // wordInnerStation
    // FI[3:0] EI[3:0]
    wordInnerStation = 0;
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
	  wordInnerStation |= (2-reg)<<(iSlot*2);
	}
      }
    }
  }

  // Check hits of TileMu
  bool isHitTileMu = false;
  if (validTileMu) {
    for ( int mod=0; mod< TGCTileMuCoincidenceMap::N_Input_TileMuModule; mod++){
      int maskTM =  mapTileMu->getMask(mod, ssc, sectorId, sideId);
      const TGCTMDBOut* tm = pTMDB->getOutput(sideId, sectorId, mod); 
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
     std::cout<<"SL id=" << id 
	       <<" sector="<<sectorId
	       <<" TMDB mod=" << mod 
	       <<"  mask:"<< maskTM
	       <<"  hit56:"<< hit56
	       <<"  hit6:"<< hit6
	       << std::endl;
#endif
    }

    // wordTIleMuon
    wordTileMuon =0;
    for ( int mod=0; mod< TGCTileMuCoincidenceMap::N_Input_TileMuModule; mod++){
      const TGCTMDBOut* tm = pTMDB->getOutput(sideId, sectorId, mod); 
      if (tm->GetHit6()>0)  wordTileMuon |= 0x02 << mod*2;
      if (tm->GetHit56()>0) wordTileMuon |= 0x01 << mod*2;
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
    innerVeto  = isActiveEifi && (region==ENDCAP);
  }

  coincidenceOut->clearHit(pt);
  
  while (innerVeto && validInner && (pt>1) ) {
    pt = pt-1;
    validInner = (mapInner->getFlagPT(pt, ssc, sectorId) == 1);
  }
  while (useTileMu && validTileMu && (pt>1) ){
    pt = pt-1;
    validTileMu = (mapTileMu->getFlagPT(pt, ssc, sectorId, sideId) == 1) ;
  }
  coincidenceOut->setHit(pt);
 
}

} //end of namespace bracket
