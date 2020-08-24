/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.h"
#include "TrigT1TGC/TGCRPhiCoincidenceOut.h"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.h"
#include "TrigT1TGC/TGCSectorLogic.h"
#include <iostream>
#include <cstdlib>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

namespace LVL1TGCTrigger {

void TGCRPhiCoincidenceMatrix::inputR(int rIn, int dRIn, int ptRIn)
{
  m_r=rIn;
  m_dR=dRIn;
  m_ptR=ptRIn;
  
#ifdef TGCDEBUG
  std::cout <<"LVL1TGCTrigger::TGCRPhiCoincidenceMatrix  "
    	    <<"inputR r=" <<m_r <<" dR=" <<m_dR <<" H/L=" <<m_ptR <<std::endl;
#endif
}

void TGCRPhiCoincidenceMatrix::inputPhi(int phiIn, int dPhiIn, int ptPhiIn)
{
  if(m_nPhiHit<MaxNPhiHit){
    m_phi[m_nPhiHit]=phiIn;
    m_dPhi[m_nPhiHit]=dPhiIn;
    m_ptPhi[m_nPhiHit]=ptPhiIn;

#ifdef TGCDEBUG
  std::cout <<"LVL1TGCTrigger::TGCRPhiCoincidenceMatrix  "
            << "inputPhi phi" << m_nPhiHit << "="<< m_phi[m_nPhiHit] 
	    << " dPhi=" << m_dPhi[m_nPhiHit] << " H/L=" << m_ptPhi[m_nPhiHit] 
	    << std::endl;
#endif
    
    m_nPhiHit++;
  }
}

void TGCRPhiCoincidenceMatrix::clear()
{
  m_SSCId=0;
  m_r=m_dR=m_ptR=0;

  m_nPhiHit=0;
  int i;
  for( i=0; i<MaxNPhiHit; i+=1)
    m_phi[i]=m_dPhi[i]=m_ptPhi[i]=0;
}

TGCRPhiCoincidenceOut* TGCRPhiCoincidenceMatrix::doCoincidence()
{
  TGCRPhiCoincidenceOut* out = new TGCRPhiCoincidenceOut;
  out->clear();

  if(m_nPhiHit ==0)  return out;

  out->setIdSSC(m_SSCId);

  int j0 = -1;
  int ptMax=-1;
  for( int j=m_nPhiHit-1; j>=0; j-=1){     // left half-SSC has priority when both output same pT
    int subsector;
    int ptOut = -99;
    int chargeOut = 2;
    int CoincidenceTypeOut=-1;
    bool isgoodMFOut=false;

    if(m_sectorLogic->getRegion()==Endcap){
      subsector = 4*(2*m_SSCId+m_r-1)+m_phi[j];
    } else {
      subsector = 4*(2*m_SSCId+m_r)+m_phi[j];
    }
    

    int type = m_map->getMapType(m_ptR, m_ptPhi[j]);
    // calculate pT of muon candidate
    if(tgcArgs()->useRun3Config()){
      //Algorithm for Run3
      int pt=m_map->test_Run3(m_sectorLogic->getOctantID(),m_sectorLogic->getModuleID(),
                            subsector,type,m_dR,m_dPhi[j]); // this function will be implemented. 
      ptOut = std::abs(pt);
      chargeOut = pt<0 ? 0:1;
      //isgoodMFOut : will be set.
      
      CoincidenceTypeOut=(type==0);
    }
    else{
      for( int pt=NumberOfPtLevel-1; pt>=0; pt-=1){
	if(m_map->test(m_sectorLogic->getOctantID(),m_sectorLogic->getModuleID(),subsector,
		       type, pt,
		       m_dR,m_dPhi[j])) {
	  ptOut = pt;
	  break;
	}
      } // loop pt
    }

    // Trigger Out
    if( ptOut >= ptMax ){
      ptMax = ptOut;
      out->clear();    
      out->setIdSSC(m_SSCId);
      if(!tgcArgs()->useRun3Config()){out->setHit(ptMax+1);}// for Run2 Algo
      else{out->setpT(ptMax);}// for Run3 Algo
      out->setR(m_r);
      out->setPhi(m_phi[j]);
      out->setDR(m_dR);
      out->setDPhi(m_dPhi[j]);
      out->setRoI(subsector);
      out->setCharge(chargeOut);
      out->setCoincidenceType(CoincidenceTypeOut);
      out->setGoodMFFlag(isgoodMFOut);
      j0 = j;
    }
  }

  if (tgcArgs()->MSGLEVEL() <= MSG::DEBUG){
    IMessageSvc* msgSvc = 0;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    if (svcLocator->service("MessageSvc", msgSvc) != StatusCode::FAILURE) {
      MsgStream log(msgSvc, "LVL1TGCTrigger::TGCRPhiCoincidenceMatrix");
      if (j0>0) {
	log << MSG::DEBUG << " Trigger Out : "
	      << " pt =" << ptMax+1 << " R=" << m_r << " Phi=" << m_phi[j0]
	      << " ptR=" << m_ptR << " dR=" << m_dR 
	      << " ptPhi=" << m_ptPhi[j0] << " dPhi=" << m_dPhi[j0] 
	      << endmsg;
      } else {
	log << MSG::DEBUG << "NO Trigger Out : " << endmsg;
      }
    }
  }
  
  //m_matrixOut = out;  
  return out;
}

void TGCRPhiCoincidenceMatrix::setRPhiMap(const TGCRPhiCoincidenceMap* map)
{
  this->m_map = map;
}

TGCRPhiCoincidenceMatrix::TGCRPhiCoincidenceMatrix(const TGCArguments* tgcargs,const TGCSectorLogic* sL)
  : m_sectorLogic(sL),
    m_matrixOut(0), m_map(0),
    m_nPhiHit(0), m_SSCId(0), m_r(0), m_dR(0), m_ptR(0), m_tgcArgs(tgcargs)
{
  for (int i=0; i<MaxNPhiHit; i++) {
    m_phi[i]=0;
    m_dPhi[i]=0;
    m_ptPhi[i]=0;
  }
}

TGCRPhiCoincidenceMatrix::~TGCRPhiCoincidenceMatrix()
{
  m_matrixOut=0;
  m_map = 0;
}

TGCRPhiCoincidenceMatrix& TGCRPhiCoincidenceMatrix::operator=(const TGCRPhiCoincidenceMatrix& right)
{
  if (this != &right){
    m_sectorLogic = right.m_sectorLogic;
    delete m_matrixOut;
    m_matrixOut =0;
    delete m_map;
    m_map = new TGCRPhiCoincidenceMap(*(right.m_map));
    m_nPhiHit = 0;
    m_SSCId   = 0;
    m_r       = 0;
    m_dR      = 0;
    m_ptR     = 0;
    for (int i=0; i<MaxNPhiHit; i++) {
      m_phi[i]=0;
      m_dPhi[i]=0;
      m_ptPhi[i]=0;
    }
  }
  return *this;
}

TGCRPhiCoincidenceMatrix::TGCRPhiCoincidenceMatrix(const TGCRPhiCoincidenceMatrix& right)
  : m_sectorLogic(0),
    m_matrixOut(0), m_map(0),
    m_nPhiHit(0), m_SSCId(0), m_r(0), m_dR(0), m_ptR(0)
{
  *this = right;  
}

} //end of namespace bracket
