/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBMWPCContCnv_p1.h"
#include "TBEvent/TBMWPCCont.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBMWPCCont_p1.h"



void
TBMWPCContCnv_p1::persToTrans(const TBMWPCCont_p1* pers, 
                                     TBMWPCCont* trans, MsgStream &/*log*/) const
{

  const unsigned nTBMWPCs = pers->m_cPos.size();
  
  // clear the trans contained and reserve space for the new MWPCs
  trans->clear();
  trans->reserve(nTBMWPCs);

  // copy all the MWPCs from the pers to the trans
  for (unsigned nTBMWPCNow=0; nTBMWPCNow<nTBMWPCs; nTBMWPCNow++) {
    TBMWPC * MWPC = new TBMWPC();

    MWPC -> setCPos(         pers->m_cPos[nTBMWPCNow] );
    MWPC -> setCErr(         pers->m_cErr[nTBMWPCNow] );
    MWPC -> setXchambers(    pers->m_isX[nTBMWPCNow] );
    MWPC -> setClusterSizeC( pers->m_clusterSize_c[nTBMWPCNow] );




    // ------------------------------------------------------------------------------------------------
    // for some reason a stright forward definition of m_cPosOverflow as
    // std::vector < std::vector<bool> > m_cPosOverflow;
    // does not work. perhaps due to the fact that std::vector<bool> is nonconforming (not
    // really a vector of bool). instead we defined
    // std::vector < std::vector<unsigned> > m_cPosOverflow;
    // and now need to convert the bool constituents of the vectors to unsigned
    // ------------------------------------------------------------------------------------------------
    unsigned nBools = pers->m_cPosOverflow[nTBMWPCNow].size();  //length of the vector of bool in TBMWPC
    std::vector<bool>  cPosOverflowNow;               //defines length
    cPosOverflowNow.reserve(nBools);

    for (unsigned nBoolNow=0; nBoolNow<nBools; nBoolNow++) {
      if(pers->m_cPosOverflow[nTBMWPCNow][nBoolNow])
        cPosOverflowNow.push_back(true);
      else
        cPosOverflowNow.push_back(false);
    }
    MWPC -> setCPosOverflow( cPosOverflowNow );
    // ------------------------------------------------------------------------------------------------



    MWPC -> setDetectorName( pers->m_tbDetectorName[nTBMWPCNow] );
    MWPC -> setOverflow(     pers->m_overflow[nTBMWPCNow] );

    // fill the container with the MWPC object
    trans->push_back(MWPC);
  }

}


void
TBMWPCContCnv_p1::transToPers(const TBMWPCCont* trans, 
                                     TBMWPCCont_p1* pers, MsgStream &/*log*/) const
{

  const unsigned nTBMWPCs = trans->size();

  pers -> m_cPos.reserve(nTBMWPCs);
  pers -> m_cErr.reserve(nTBMWPCs);
  pers -> m_isX.reserve(nTBMWPCs);
  pers -> m_clusterSize_c.reserve(nTBMWPCs);
  pers -> m_cPosOverflow.reserve(nTBMWPCs);
  pers -> m_tbDetectorName.reserve(nTBMWPCs);
  pers -> m_overflow.reserve(nTBMWPCs);


  // iterators for the container
  TBMWPCCont::const_iterator MWPCIt   = trans->begin();
  TBMWPCCont::const_iterator MWPCIt_e = trans->end();

  // copy all the MWPCs from the trans to the pers
  for (; MWPCIt!=MWPCIt_e; ++MWPCIt) {
    const TBMWPC * MWPC = * MWPCIt;

    // fill in the MWPC properties
    pers -> m_cPos.push_back(           MWPC->getCPos() );
    pers -> m_cErr.push_back(           MWPC->getCErr() );
    pers -> m_isX.push_back(            MWPC->isX() );
    pers -> m_clusterSize_c.push_back(  MWPC->getClusterSizeC() );

    // ------------------------------------------------------------------------------------------------
    // for some reason a stright forward definition of m_cPosOverflow as
    // std::vector < std::vector<bool> > m_cPosOverflow;
    // does not work. perhaps due to the fact that std::vector<bool> is nonconforming (not
    // really a vector of bool variables, but some ). instead we defined
    // std::vector < std::vector<unsigned> > m_cPosOverflow;
    // and now need to convert the bool constituents of the vectors to unsigned
    // ------------------------------------------------------------------------------------------------
    unsigned nBools = MWPC->isCPosOverflow().size();    //length of the vector of bool in TBMWPC
    std::vector<unsigned>  cPosOverflowNow(nBools);   //defines length and initializes to zero

    for (unsigned nBoolNow=0; nBoolNow<nBools; nBoolNow++) {
      if(MWPC->isCPosOverflow()[nBoolNow])
        cPosOverflowNow[nBoolNow] = 1;
    }
    pers -> m_cPosOverflow.push_back(   cPosOverflowNow  );
    // ------------------------------------------------------------------------------------------------

    pers -> m_tbDetectorName.push_back( MWPC->getDetectorName() );
    pers -> m_overflow.push_back(       MWPC->isOverflow() );
  }

}

