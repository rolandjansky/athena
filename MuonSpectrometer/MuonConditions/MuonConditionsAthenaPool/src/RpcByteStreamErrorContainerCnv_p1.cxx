/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   copied from ChamberT0sCnv_p1.cxx
//-----------------------------------------------------------------------------

#include "MuonByteStreamErrors/RpcByteStreamErrorContainer.h"
#include "MuonConditionsAthenaPool/MuonByteStreamErrors/RpcByteStreamErrorContainerCnv_p1.h"
#include <utility>

void RpcByteStreamErrorContainerCnv_p1::
persToTrans( const Muon::RpcByteStreamErrorContainer_p1 *persObj, Muon::RpcByteStreamErrorContainer *transObj,MsgStream & log ) 
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "RpcByteStreamErrorContainerCnv_p1::persToTrans" << endmsg;
    transObj->clear();
    DataVector<std::pair<IdentifierHash, int> >::const_iterator it=persObj->m_errorVector.begin(), itEnd=persObj->m_errorVector.end();
    for (;it!=itEnd;++it) {
      transObj->addError ((*it)->first, (*it)->second);
    }
}

void RpcByteStreamErrorContainerCnv_p1::
transToPers( const Muon::RpcByteStreamErrorContainer *transObj, Muon::RpcByteStreamErrorContainer_p1 *persObj, MsgStream & log )
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "RpcByteStreamErrorContainerCnv_p1::transToPers" << endmsg;
    const DataVector<std::pair<IdentifierHash, int> >& errs =
      transObj->getAllErrors();
    persObj->m_errorVector.reserve( errs.size() );
    for (const std::pair<IdentifierHash, int>* p : errs) {
      persObj->m_errorVector.push_back( new std::pair<IdentifierHash, int>( p->first, p->second) );
    }
}


