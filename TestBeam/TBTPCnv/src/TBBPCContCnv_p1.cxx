/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBBPCContCnv_p1.h"
#include "TBEvent/TBBPCCont.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBBPCCont_p1.h"



void
TBBPCContCnv_p1::persToTrans(const TBBPCCont_p1* pers, 
                                     TBBPCCont* trans, MsgStream &/*log*/) const
{
  const unsigned nTBBPCs = pers->m_xPos.size();
  
  // clear the trans contained and reserve space for the new BPCs
  trans->clear();
  trans->reserve(nTBBPCs);

  // copy all the BPCs from the pers to the trans
  for (unsigned nTBBPCNow=0; nTBBPCNow<nTBBPCs; nTBBPCNow++) {
    TBBPC * BPC = new TBBPC();

    BPC -> setXPos(           pers->m_xPos[nTBBPCNow] );
    BPC -> setYPos(           pers->m_yPos[nTBBPCNow] );
    BPC -> setXErr(           pers->m_xErr[nTBBPCNow] );
    BPC -> setYErr(           pers->m_yErr[nTBBPCNow] );
    BPC -> setXPulse(         pers->m_xPulse[nTBBPCNow] );
    BPC -> setYPulse(         pers->m_yPulse[nTBBPCNow] );
    BPC -> setHitNbr(         pers->m_hitnumber[nTBBPCNow] );
    BPC -> setXPosOverflow(   pers->m_xPosOverflow[nTBBPCNow] );
    BPC -> setYPosOverflow(   pers->m_yPosOverflow[nTBBPCNow] );
    BPC -> setXPulseOverflow( pers->m_xPulseOverflow[nTBBPCNow] );
    BPC -> setYPulseOverflow( pers->m_yPulseOverflow[nTBBPCNow] );

    BPC -> setDetectorName(   pers->m_tbDetectorName[nTBBPCNow] );
    BPC -> setOverflow(       pers->m_overflow[nTBBPCNow] );

    // fill the container with the scint object
    trans->push_back(BPC);
//		log << MSG::DEBUG << "\033[34m" << "\t- pers->trans: BPC->getYPulse()        " << BPC->getYPulse() << "\033[0m" <<endmsg;
  }

}


void
TBBPCContCnv_p1::transToPers(const TBBPCCont* trans, 
                                     TBBPCCont_p1* pers, MsgStream &/*log*/) const
{

  const unsigned nTBBPCs = trans->size();

  pers -> m_xPos.reserve(nTBBPCs);
  pers -> m_yPos.reserve(nTBBPCs);
  pers -> m_xErr.reserve(nTBBPCs);
  pers -> m_yErr.reserve(nTBBPCs);
  pers -> m_xPulse.reserve(nTBBPCs);
  pers -> m_yPulse.reserve(nTBBPCs);
  pers -> m_hitnumber.reserve(nTBBPCs);
  pers -> m_xPosOverflow.reserve(nTBBPCs);
  pers -> m_yPosOverflow.reserve(nTBBPCs);
  pers -> m_xPulseOverflow.reserve(nTBBPCs);
  pers -> m_yPulseOverflow.reserve(nTBBPCs);
  pers -> m_overflowSetFlag.reserve(nTBBPCs);
  pers -> m_tbDetectorName.reserve(nTBBPCs);
  pers -> m_overflow.reserve(nTBBPCs);


  // iterators for the container
  TBBPCCont::const_iterator BPCIt   = trans->begin();
  TBBPCCont::const_iterator BPCIt_e = trans->end();

  // copy all the scints from the trans to the pers
  for (; BPCIt!=BPCIt_e; ++BPCIt) {
    const TBBPC * BPC = * BPCIt;

    // fill in the scint properties
    pers -> m_xPos.push_back(            BPC->getXPos() );
    pers -> m_yPos.push_back(            BPC->getYPos() );
    pers -> m_xErr.push_back(            BPC->getXErr() );
    pers -> m_yErr.push_back(            BPC->getYErr() );
    pers -> m_xPulse.push_back(          BPC->getXPulse() );
    pers -> m_yPulse.push_back(          BPC->getYPulse() );
    pers -> m_hitnumber.push_back(       BPC->getHitNbr() );
    pers -> m_xPosOverflow.push_back(    BPC->isXPosOverflow() );
    pers -> m_yPosOverflow.push_back(    BPC->isYPosOverflow() );
    pers -> m_xPulseOverflow.push_back(  BPC->isXPulseOverflow() );
    pers -> m_yPulseOverflow.push_back(  BPC->isYPulseOverflow() );

    pers -> m_overflowSetFlag.push_back( false );

    pers -> m_tbDetectorName.push_back(  BPC->getDetectorName() );
    pers -> m_overflow.push_back(        BPC->isOverflow() );
	}


}

