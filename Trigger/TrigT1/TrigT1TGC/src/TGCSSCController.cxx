/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCSSCController.cxx,v 1.6 2009-03-15 18:07:55 isaya Exp $
#include "TrigT1TGC/TGCSSCController.hh"
#include "TrigT1TGC/TGCSSCControllerOut.hh"
#include "TrigT1TGC/TGCSectorLogic.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.hh"
#include "TrigT1TGC/TGCHighPtBoardOut.hh"
#include <iostream>

namespace LVL1TGCTrigger {

extern   bool g_SHPT_ORED;

  
//constructor 
TGCSSCController::TGCSSCController(const TGCSectorLogic* sL) 
  :m_sectorLogic(sL), m_NumberOfWireHighPtBoard(0),
   m_region(FORWARD)
{ 
} 

// distribute signals to appropriate r-phi coincidence matrix.
TGCSSCControllerOut* TGCSSCController::distribute(TGCHighPtChipOut* wire[], TGCHighPtChipOut* strip)
{
  TGCSSCControllerOut* out = new TGCSSCControllerOut(m_sectorLogic->getRegion());
  out->clear();

  for(int HPBid=0; HPBid < m_sectorLogic->getNumberOfWireHighPtBoard(); HPBid++){
    if(wire[HPBid]==0) continue; // NO wire HIT
    for(int chip=0; chip<NumberOfChip; chip+=1){
      for(int iCandidate=1; iCandidate>=0; iCandidate-=1){
	if(wire[HPBid]->getSel(chip,iCandidate)){
	  // If both candidates has same HitID, ignore 2nd candidate(iCandidate==1).
	  if(iCandidate==1 && (wire[HPBid]->getHitID(chip,1)) == (wire[HPBid]->getHitID(chip,0))) continue;
	  int SSCId = getSSCId(HPBid,chip,wire[HPBid]->getHitID(chip,iCandidate));
	  if(SSCId<m_sectorLogic->getNumberOfSubSectorCluster()){
	    out->setR(SSCId,wire[HPBid]->getPos(chip,iCandidate));
	    out->setDR(SSCId,wire[HPBid]->getDev(chip,iCandidate));
	    out->setPtR(SSCId,wire[HPBid]->getPt(chip,iCandidate));
	    out->setHitR(SSCId,true);
	  }else{
	    std::cerr << "internal error in TGCSSCController::distribute() bid=" << m_sectorLogic->getBid() << " ";
	    std::cerr << " SSCId="<<SSCId<<std::endl;
	  }
	}
      }
    }
  }

  if(strip==0) return out; 

  for(int chip=0; chip<NumberOfChip; chip+=1){
    for(int iCandidate=1; iCandidate>=0; iCandidate-=1){
      if( strip->getSel(chip,iCandidate) ){
	// If both candidates has same HitID, ignore 2nd candidate(iCandidate==1).
	if(iCandidate==1 && (strip->getHitID(chip,1)) == (strip->getHitID(chip,0))) continue;
	int chamberNo = getChamberNo(chip,strip->getHitID(chip,iCandidate));
	//if((chamberNo<0)||(chamberNo >= MaxNumberOfChamberInR)) continue;
	
	int PhiPosInSSC = getPhiPosInSSC(chip,strip->getHitID(chip,iCandidate));
	int iPhi = convertPhi(chip,
			      strip->getHitID(chip,iCandidate),
			      strip->getPos(chip,iCandidate) );
	out->setPhi(chamberNo, PhiPosInSSC, iPhi); 
	out->setDPhi(chamberNo, PhiPosInSSC,strip->getDev(chip,iCandidate));
	out->setPtPhi(chamberNo, PhiPosInSSC, strip->getPt(chip,iCandidate));
	out->setHitPhi(chamberNo, PhiPosInSSC, true);
       
	// chamber No3 and No4 can not be distiguished because of SHPT bug
	// input from No3 or No4 are duplicated in both chamber
        int oredChamber=-1;
        if (chamberNo==4) oredChamber= 3;
	if (chamberNo==3) oredChamber= 4;
        if (g_SHPT_ORED && (oredChamber>0)) {
	  if(!out->hasHitPhi(oredChamber, PhiPosInSSC)){
	    out->setPhi(oredChamber, PhiPosInSSC, iPhi); 
	    out->setDPhi(oredChamber, PhiPosInSSC,strip->getDev(chip,iCandidate));
	    out->setPtPhi(oredChamber, PhiPosInSSC, strip->getPt(chip,iCandidate));
	    out->setHitPhi(oredChamber, PhiPosInSSC, true);
	  }
	}

      }
    }
  }
  return out;
}

int TGCSSCController::getSSCId(int nHPB, int chip, int block) const
{
    if(m_region==Forward){
        return (block+chip*MaxNumberOfHPBData);
    }else{
        return (block+chip*MaxNumberOfHPBData+nHPB*MaxNumberOfHPBData*NumberOfChip-5);
    }
}

int TGCSSCController::convertPhi(int /* chip */, int block, int pos) const
{
  return (pos + (block%2) * 2);
}

int TGCSSCController::getChamberNo(int chip, int block) const
{
  if(m_region==Endcap){
    int chamber = ((block+chip*MaxNumberOfHPBData)/2);
    if(chamber==5) return (chamber-1);
    else if(chamber==4) return 0;
    else return chamber;
  }
  
  else return 0;
}

int TGCSSCController::getPhiPosInSSC(int /* chip */, int block) const
{
  return block%2;
}

} //end of namespace bracket
