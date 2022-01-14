/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSectorLogic.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "TrigT1TGC/TGCHighPtBoard.h"
#include "TrigT1TGC/TGCHighPtBoardOut.h"
#include "TrigT1TGC/TGCHighPtChipOut.h"
#include "TrigT1TGC/TGCRPhiCoincidenceOut.h"
#include "TrigT1TGC/TGCTMDB.h"
#include "TrigT1TGC/TGCTMDBOut.h"
#include "TrigT1TGC/TGCEIFICoincidenceMap.h"
#include "TrigT1TGC/TGCTileMuCoincidenceLUT.h"
#include "TrigT1TGC/TGCNSW.h"
#include "TrigT1TGC/NSWTrigOut.h"
#include "TrigT1TGC/TGCNSWCoincidenceMap.h"
#include "TrigT1TGC/TGCBIS78.h"
#include "TrigT1TGC/BIS78TrigOut.h"
#include "TrigT1TGC/TGCBIS78CoincidenceMap.h"
#include "TrigT1TGC/TGCGoodMF.h"
#include "TrigT1TGC/TGCTrackSelectorOut.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

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
   m_trackSelector(this),
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

  m_nswSide   =   (tgcArgs()->NSWSideInfo().find('A')!=std::string::npos && m_sideId==0)
               || (tgcArgs()->NSWSideInfo().find('C')!=std::string::npos && m_sideId==1);

  m_SSCController.setRegion(regionIn);

  m_matrix.setSideId(m_sideId);
  m_mapEIFI = db->getEIFICoincidenceMap(m_sideId);

  m_useTileMu = tgcArgs()->TILE_MU() && (m_region==ENDCAP);

  m_matrix.setCoincidenceLUT(db->getBigWheelCoincidenceLUT());
  m_tileMuLUT = db->getTileMuCoincidenceLUT();
  m_useTileMu = (m_tileMuLUT != nullptr) && m_useTileMu;

  m_mapGoodMF = db->getGoodMFMap();
  m_mapNSW = db->getNSWCoincidenceMap(m_sideId, m_octantId, m_moduleId);
  if(tgcArgs()->USE_BIS78()) m_mapBIS78 = db->getBIS78CoincidenceMap();

  m_trackSelectorOut.reset(new TGCTrackSelectorOut());

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
  if(m_mapBIS78 == 0) tgcArgs()->set_USE_BIS78(false);
}

TGCSectorLogic::~TGCSectorLogic()
{}

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

void TGCSectorLogic::setBIS78(std::shared_ptr<const TGCBIS78> bis78)
{
  m_bis78 = bis78;
  if(m_bis78 == 0) tgcArgs()->set_USE_BIS78(false);
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

void TGCSectorLogic::clockIn(const SG::ReadCondHandleKey<TGCTriggerData> /* readCondKey */,
                             int bidIn, bool process)
{
  // skip to process if want. (e.g. no hit in TGC)
  if(!process) return;

  m_bid=bidIn;

  collectInput();

  TGCSSCControllerOut* SSCCOut =
    m_SSCController.distribute(m_wireHighPtChipOut,m_stripHighPtChipOut);
#ifdef TGCDEBUG
  SSCCOut->print();
#endif  
  deleteHPBOut();

  for(int SSCid=0; SSCid<getNumberOfSubSectorCluster(); SSCid+=1){
    TGCRPhiCoincidenceOut* coincidenceOut = 0;
    if(SSCCOut->hasHit(SSCid)){
      m_matrix.clear();
      m_matrix.setSSCId(SSCid);
      m_matrix.inputR(SSCCOut->getR(SSCid),SSCCOut->getDR(SSCid),SSCCOut->getPtR(SSCid));
      for(int phiposInSSC = 0 ;phiposInSSC < TGCSSCControllerOut::MaxNumberOfPhiInSSC; phiposInSSC++){
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
      for(int phiposInSSC = 0 ;phiposInSSC < TGCSSCControllerOut::MaxNumberOfPhiInSSC; phiposInSSC++){
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

    if (coincidenceOut) {
      if (m_useGoodMF) {
        bool isgoodMF = m_mapGoodMF->test_GoodMF(m_moduleId,SSCid,coincidenceOut->getRoI());
        coincidenceOut->setGoodMFFlag(isgoodMF);
      }
    }

    ////////////////////////////////////////////
    // do coincidence with Inner Tracklet of EI, NSW, Tile, and/or RPC-BIS78
    doInnerCoincidence(SSCid, coincidenceOut);

    if (coincidenceOut) {
      m_trackSelector.input(coincidenceOut);
    }
  }
  if(SSCCOut!=0) delete SSCCOut;
  SSCCOut=0;

  // Track selector chooses up to 4 track candidates to be sent to MUCTPI.
  m_trackSelector.select(m_trackSelectorOut);
}

void TGCSectorLogic::collectInput()
{
  for(int i = 0; i < m_SSCController.getNumberOfWireHighPtBoard(); i += 1) {
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


void TGCSectorLogic::showResult()
{
  std::cout<<"#SL O"<<" BID:"<<m_bid
	   <<" region:"<<((m_region==FORWARD) ? "FWD" : "END")
	   <<" SLid:"<<m_id<<" ";
}


TGCSectorLogic::TGCSectorLogic(const TGCSectorLogic& right)
 : m_bid(right.m_bid), m_id(right.m_id),
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

 
void TGCSectorLogic::doInnerCoincidence(int SSCId, TGCRPhiCoincidenceOut* coincidenceOut) {
  if (coincidenceOut == 0) return;

  int pt = coincidenceOut->getpT();
  if (pt==0) return;

  if(SSCId<=4 && m_region==ENDCAP){  //3 detectors are used to inner coincidnece in SSC#0~4 in Endcap;  

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
      bool validBIS78 = false;
      if(tgcArgs()->USE_BIS78() && m_sideId==0) validBIS78=(m_mapBIS78->getFlagROI(pos, coincidenceOut->getIdSSC(), m_sectorId, m_sideId) == 1); // A-side only

      bool isEI=false;
      bool isTILE=false;
      bool isBIS78=false;
      if(m_useEIFI && validEI){isEI=doTGCEICoincidence(coincidenceOut);}
      if(m_useTileMu && validTileMu){isTILE=doTILECoincidence(coincidenceOut); }
      if(validBIS78){isBIS78=doTGCBIS78Coincidence(coincidenceOut); }

      coincidenceOut->setInnerCoincidenceFlag(isEI || isTILE || isBIS78 || (!m_useEIFI && !validEI && !m_useTileMu && !validTileMu && !tgcArgs()->USE_BIS78() && !validBIS78));
  } else {
    //  NSW or FI are used to inner coincidnece in SSC#5~18 in Endcap and Forward region 
      int pos = 4*coincidenceOut->getR() +  coincidenceOut->getPhi();
      bool validFI = (m_mapEIFI->getFlagROI(pos, coincidenceOut->getIdSSC(), m_sectorId) == 1) && m_region==ENDCAP;

      if(tgcArgs()->USE_NSW() && m_nswSide){
	if(tgcArgs()->FORCE_NSW_COIN()){
	  coincidenceOut->setInnerCoincidenceFlag(true);
	}
	else{
	  doTGCNSWCoincidence(coincidenceOut);
	}
      }
      else if(!m_nswSide && validFI){
	if(m_useEIFI){
	  coincidenceOut->setInnerCoincidenceFlag( doTGCFICoincidence(coincidenceOut) );
	}
    } else {
      coincidenceOut->setInnerCoincidenceFlag(true);   // TBD
    }
  }
}


void TGCSectorLogic::doTGCNSWCoincidence(TGCRPhiCoincidenceOut* coincidenceOut){
  std::shared_ptr<const NSWTrigOut> pNSWOut = m_nsw->getOutput(m_region,m_sideId,m_sectorId);

  // for now, if there is a hit at NSW, turn on the inner coin flag
  coincidenceOut->setInnerCoincidenceFlag( pNSWOut->getNSWeta().size()>0 );
  return;

  // will implement NSW pT calculation later
}

bool TGCSectorLogic::doTGCBIS78Coincidence(TGCRPhiCoincidenceOut* coincidenceOut){
  std::shared_ptr<const BIS78TrigOut> pBIS78Out = m_bis78->getOutput(m_region,m_sectorId);
  if ( pBIS78Out.get() == 0 ) return false;
  int pt=0;
  pBIS78Out->print(); // just for test
    
  pt = m_mapBIS78->TGCBIS78_pt(pBIS78Out.get(), 	
                               coincidenceOut->getRoI());

  return (pt > 0);
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
    case LVL1TGC::TGCTileMuCoincidenceLUT::TM_D6_L:
      return (hit6==TGCTMDBOut::TM_LOW || hit6==TGCTMDBOut::TM_HIGH);
      break;
    case LVL1TGC::TGCTileMuCoincidenceLUT::TM_D6_H:
      return (hit6==TGCTMDBOut::TM_HIGH);
      break;
    case LVL1TGC::TGCTileMuCoincidenceLUT::TM_D56_L:
      return (hit56==TGCTMDBOut::TM_LOW || hit56==TGCTMDBOut::TM_HIGH);
      break;
    case LVL1TGC::TGCTileMuCoincidenceLUT::TM_D56_H:
      return (hit56==TGCTMDBOut::TM_HIGH);
      break;
    default:
      return false;
      break;
  }
  return true;
}

} //end of namespace bracket
