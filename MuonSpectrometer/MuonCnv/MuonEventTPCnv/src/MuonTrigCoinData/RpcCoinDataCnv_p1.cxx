/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
//  file:   RpcCoinDataCnv_p1.cxx
//* Takashi Kubota - June 30, 2008 */
//-----------------------------------------------------------------------------

#include "MuonTrigCoinData/RpcCoinData.h"
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinDataCnv_p1.h"
#include "CxxUtils/make_unique.h"

Muon::RpcCoinData
RpcCoinDataCnv_p1::
createRpcCoinData( const Muon::RpcCoinData_p1 *persObj,
                   const Identifier& id,
                   const MuonGM::RpcReadoutElement* detEl,
                   MsgStream &/*log*/ ) 
{
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_localPos; 
  localPos[Trk::locY] = 0.0;

  std::vector<Identifier> rdoList;
  rdoList.reserve(persObj->m_rdoList.size());
  unsigned int id32 = id.get_identifier32().get_compact(); // identify is filled in RpcCoinContainerCnv
  for (short x : persObj->m_rdoList)
    rdoList.emplace_back ((unsigned int) x+id32);

  auto cmat = CxxUtils::make_unique<Amg::MatrixX>(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);

  Muon::RpcCoinData data (id,
                          0, // collectionHash
                          localPos,
                          std::move(rdoList),
                          cmat.release(),
                          detEl,
                          persObj->m_time,
                          persObj->m_ambiguityFlag,
                          persObj->m_ijk,
                          persObj->m_threshold,
                          persObj->m_overlap,
                          persObj->m_parentCmId,
                          persObj->m_parentPadId,
                          persObj->m_parentSectorId,
                          persObj->m_lowPtCm
                          );
  return data;
}

void RpcCoinDataCnv_p1::
persToTrans( const Muon::RpcCoinData_p1 *persObj, Muon::RpcCoinData *transObj,MsgStream &log ) 
{
  *transObj = createRpcCoinData (persObj,
                                 transObj->identify(),
                                 transObj->detectorElement(),
                                 log);
}

void RpcCoinDataCnv_p1::
transToPers( const Muon::RpcCoinData *transObj, Muon::RpcCoinData_p1 *persObj, MsgStream & log)
{
  //log << MSG::DEBUG << "RpcCoinDataCnv_p1::transToPers" << endmsg;
  persObj->m_localPos         = transObj->localPosition()[Trk::locX];
  persObj->m_errorMat         = transObj->localCovariance()(0,0);
  
  // List of Id of the cluster
  persObj->m_rdoList.resize( transObj->rdoList().size() );
  if (!transObj->identify().get_identifier32().is_valid()) log << MSG::ERROR << "RpcCoinDataCnv_p1::transToPers - invalid trans id!!" << endmsg;
  unsigned int id32 = transObj->identify().get_identifier32().get_compact();
  // convert the list of ID saved for the cluster
  persObj->m_rdoList.clear();
  persObj->m_rdoList.reserve(transObj->rdoList().size() );
  for (const Identifier& id : transObj->rdoList())
    persObj->m_rdoList.push_back (id.get_identifier32().get_compact() - id32);
  
  // Data from Muon::RpcPrepData 
  persObj->m_time            = transObj->time();
  persObj->m_ambiguityFlag   = transObj->ambiguityFlag();

  // Data from RpcCoinData
  persObj->m_ijk             = transObj->ijk();
  persObj->m_threshold       = transObj->threshold();
  persObj->m_overlap         = transObj->overlap();
  persObj->m_parentCmId      = transObj->parentCmId();
  persObj->m_parentPadId     = transObj->parentPadId();      
  persObj->m_parentSectorId  = transObj->parentSectorId();      
  persObj->m_lowPtCm         = transObj->lowPtCm();
}


