/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
//  file:   RpcCoinDataCnv_p1.cxx
//* Takashi Kubota - June 30, 2008 */
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonTrigCoinData/RpcCoinData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinDataCnv_p1.h"

void RpcCoinDataCnv_p1::
persToTrans( const Muon::RpcCoinData_p1 *persObj, Muon::RpcCoinData *transObj,MsgStream &/*log*/ ) 
{
   //log << MSG::DEBUG << "RpcCoinDataCnv_p1::persToTrans" << endreq;

  // Trk::PrepRawData

  // Fill localposition
  transObj->m_localPos[Trk::locX] = persObj->m_localPos; 
  transObj->m_localPos[Trk::locY] = 0.0;
  
  // Error Matrix
  Amg::MatrixX* cmat = new  Amg::MatrixX(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);
  transObj->m_localCovariance     = cmat;
  
    //RDO list
  // List of Id of the cluster
  transObj->m_rdoList.resize( persObj->m_rdoList.size() );

  unsigned int id32 = transObj->identify().get_identifier32().get_compact(); // identify is filled in RpcCoinContainerCnv
  std::vector<Identifier>::iterator tit = transObj->m_rdoList.begin();
  for (std::vector<short>::const_iterator it=persObj->m_rdoList.begin(); it != persObj->m_rdoList.end(); it++) {
    *tit = Identifier((unsigned int) *it+id32);
    tit++;
  }    


  // Data from Muon::RpcPrepData 
  transObj->m_time            = persObj->m_time; 
  transObj->m_ambiguityFlag   = persObj->m_ambiguityFlag;      

  // Data from RpcCoinData
  transObj->m_ijk             = persObj->m_ijk;      
  transObj->m_threshold       = persObj->m_threshold;      
  transObj->m_overlap         = persObj->m_overlap;      
  transObj->m_parentCmId      = persObj->m_parentCmId;      
  transObj->m_parentPadId     = persObj->m_parentPadId;      
  transObj->m_parentSectorId  = persObj->m_parentSectorId;      
  transObj->m_lowPtCm         = persObj->m_lowPtCm;      
}

void RpcCoinDataCnv_p1::
transToPers( const Muon::RpcCoinData *transObj, Muon::RpcCoinData_p1 *persObj, MsgStream & log)
{
   //log << MSG::DEBUG << "RpcCoinDataCnv_p1::transToPers" << endreq;
  persObj->m_localPos         = transObj->m_localPos[Trk::locX];
  persObj->m_errorMat         = (*transObj->m_localCovariance)(0,0);
  
  // List of Id of the cluster
  persObj->m_rdoList.resize( transObj->m_rdoList.size() );
  if (!transObj->identify().get_identifier32().is_valid()) log << MSG::ERROR << "RpcCoinDataCnv_p1::transToPers - invalid trans id!!" << endreq;
  unsigned int id32 = transObj->identify().get_identifier32().get_compact();
  // convert the list of ID saved for the cluster
  persObj->m_rdoList.resize(transObj->m_rdoList.size() );
  std::vector<short>::iterator pit = persObj->m_rdoList.begin();
  for (std::vector<Identifier>::const_iterator it=transObj->m_rdoList.begin(); it != transObj->m_rdoList.end(); it++) {
      *pit = (short) ((*it).get_identifier32().get_compact() - id32);
    pit++;
  }
  
  // Data from Muon::RpcPrepData 
  persObj->m_time            = transObj->m_time; 
  persObj->m_ambiguityFlag   = transObj->m_ambiguityFlag;      

  // Data from RpcCoinData
  persObj->m_ijk             = transObj->m_ijk;      
  persObj->m_threshold       = transObj->m_threshold;      
  persObj->m_overlap         = transObj->m_overlap;      
  persObj->m_parentCmId      = transObj->m_parentCmId;      
  persObj->m_parentPadId     = transObj->m_parentPadId;      
  persObj->m_parentSectorId  = transObj->m_parentSectorId;      
  persObj->m_lowPtCm         = transObj->m_lowPtCm;      
}


