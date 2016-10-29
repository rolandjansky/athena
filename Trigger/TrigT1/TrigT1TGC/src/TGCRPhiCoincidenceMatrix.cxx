/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCRPhiCoincidenceMatrix.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceOut.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceMap.hh"
#include "TrigT1TGC/TGCSectorLogic.hh"
#include "TrigT1TGC/TGCCoincidence.hh"
#include <iostream>
#include <cstdlib>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

namespace LVL1TGCTrigger {

extern bool g_OUTCOINCIDENCE;
extern TGCCoincidences * g_TGCCOIN;
extern bool g_DEBUGLEVEL;

void TGCRPhiCoincidenceMatrix::inputR(int rIn, int dRIn, int ptRIn)
{
  r=rIn;
  dR=dRIn;
  ptR=ptRIn;
  
#ifdef TGCDEBUG
  std::cout <<"LVL1TGCTrigger::TGCRPhiCoincidenceMatrix  "
    	    <<"inputR r=" <<r <<" dR=" <<dR <<" H/L=" <<ptR <<std::endl;
#endif
}

void TGCRPhiCoincidenceMatrix::inputPhi(int phiIn, int dPhiIn, int ptPhiIn)
{
  if(nPhiHit<MaxNPhiHit){
    phi[nPhiHit]=phiIn;
    dPhi[nPhiHit]=dPhiIn;
    ptPhi[nPhiHit]=ptPhiIn;

#ifdef TGCDEBUG
  std::cout <<"LVL1TGCTrigger::TGCRPhiCoincidenceMatrix  "
            << "inputPhi phi" << nPhiHit << "="<< phi[nPhiHit] 
	    << " dPhi=" << dPhi[nPhiHit] << " H/L=" << ptPhi[nPhiHit] 
	    << std::endl;
#endif
    
    nPhiHit++;
  }
}

void TGCRPhiCoincidenceMatrix::clear()
{
  SSCId=0;
  r=dR=ptR=0;

  nPhiHit=0;
  int i;
  for( i=0; i<MaxNPhiHit; i+=1)
    phi[i]=dPhi[i]=ptPhi[i]=0;
}

TGCRPhiCoincidenceOut* TGCRPhiCoincidenceMatrix::doCoincidence()
{
  TGCRPhiCoincidenceOut* out = new TGCRPhiCoincidenceOut;
  out->clear();

  if(nPhiHit ==0)  return out;

  out->setIdSSC(SSCId);

  int j0 = -1;
  int ptMax=-1;
  for( int j=nPhiHit-1; j>=0; j-=1){     // left half-SSC has priority when both output same pT
    int subsector;
    int ptOut = -99;
    if(sectorLogic->getRegion()==Endcap){
      subsector = 4*(2*SSCId+r-1)+phi[j];
    } else {
      subsector = 4*(2*SSCId+r)+phi[j];
    }
    
    int type = map->getMapType(ptR, ptPhi[j]);
    for( int pt=NumberOfPtLevel-1; pt>=0; pt-=1){
      if(map->test(sectorLogic->getOctantID(),sectorLogic->getModuleID(),subsector,
		   type, pt,
		   dR,dPhi[j])) {
	ptOut = pt;
	break;
      }
    } // loop pt
      
    if (g_OUTCOINCIDENCE) {
      TGCCoincidence * coin
	= new TGCCoincidence(sectorLogic->getBid(), sectorLogic->getId(), sectorLogic->getModuleID(), 
			     sectorLogic->getRegion(), SSCId, r, phi[j], subsector, 
			     ptR, dR, ptPhi[j], dPhi[j], ptOut);
      g_TGCCOIN->push_back(coin);
    }

    // Trigger Out
    if( ptOut >= ptMax ){
      ptMax = ptOut;
      out->clear();    
      out->setIdSSC(SSCId);
      out->setHit(ptMax+1);   
      out->setR(r);
      out->setPhi(phi[j]);
      out->setDR(dR);
      out->setDPhi(dPhi[j]);
      j0 = j;
    }
  }

  if (g_DEBUGLEVEL){
    IMessageSvc* msgSvc = 0;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    if (svcLocator->service("MessageSvc", msgSvc) != StatusCode::FAILURE) {
      MsgStream m_log(msgSvc, "LVL1TGCTrigger::TGCRPhiCoincidenceMatrix");
      if (j0>0) {
	m_log << MSG::DEBUG << " Trigger Out : "
	      << " pt =" << ptMax+1 << " R=" << r << " Phi=" << phi[j0]
	      << " ptR=" << ptR << " dR=" << dR 
	      << " ptPhi=" << ptPhi[j0] << " dPhi=" << dPhi[j0] 
	      << endmsg;
      } else {
	m_log << MSG::DEBUG << "NO Trigger Out : " << endmsg;
      }
    }
  }
  
  //matrixOut = out;  
  return out;
}

void TGCRPhiCoincidenceMatrix::setRPhiMap(const TGCRPhiCoincidenceMap* map)
{
  this->map = map;
}

TGCRPhiCoincidenceMatrix::TGCRPhiCoincidenceMatrix(const TGCSectorLogic* sL) 
  : sectorLogic(sL),
    matrixOut(0), map(0),
    nPhiHit(0), SSCId(0), r(0), dR(0), ptR(0)
{
  for (int i=0; i<MaxNPhiHit; i++) {
    phi[i]=0;
    dPhi[i]=0;
    ptPhi[i]=0;
  }
}

TGCRPhiCoincidenceMatrix::~TGCRPhiCoincidenceMatrix()
{
  matrixOut=0;
  map = 0;
}

TGCRPhiCoincidenceMatrix& TGCRPhiCoincidenceMatrix::operator=(const TGCRPhiCoincidenceMatrix& right)
{
  if (this != &right){
    sectorLogic = right.sectorLogic;
    delete matrixOut;
    matrixOut =0;
    delete map;
    map = new TGCRPhiCoincidenceMap(*(right.map));
    nPhiHit = 0;
    SSCId   = 0;
    r       = 0;
    dR      = 0;
    ptR     = 0;
    for (int i=0; i<MaxNPhiHit; i++) {
      phi[i]=0;
      dPhi[i]=0;
      ptPhi[i]=0;
    }
  }
  return *this;
}

TGCRPhiCoincidenceMatrix::TGCRPhiCoincidenceMatrix(const TGCRPhiCoincidenceMatrix& right)
  : sectorLogic(0),
    matrixOut(0), map(0),
    nPhiHit(0), SSCId(0), r(0), dR(0), ptR(0)
{
  *this = right;  
}

} //end of namespace bracket
