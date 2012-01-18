/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:   copied from ChamberT0sCnv_p1.cxx
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonByteStreamErrors/RpcByteStreamErrorContainer.h"
#undef private
#undef protected

#include "MuonConditionsAthenaPool/MuonByteStreamErrors/RpcByteStreamErrorContainerCnv_p1.h"
#include <utility>

void RpcByteStreamErrorContainerCnv_p1::
persToTrans( const Muon::RpcByteStreamErrorContainer_p1 *persObj, Muon::RpcByteStreamErrorContainer *transObj,MsgStream & log ) 
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "RpcByteStreamErrorContainerCnv_p1::persToTrans" << endreq;
    transObj->m_errorVector.clear();
    transObj->m_errorVector.reserve( persObj->m_errorVector.size() );
    DataVector<std::pair<IdentifierHash, int> >::const_iterator it=persObj->m_errorVector.begin(), itEnd=persObj->m_errorVector.end();
    for (;it!=itEnd;++it) {
      std::pair<IdentifierHash, int >* myPointer = new std::pair<IdentifierHash, int>(std::make_pair( (*it)->first, (*it)->second));
      transObj->m_errorVector.push_back( myPointer );
    }
}

void RpcByteStreamErrorContainerCnv_p1::
transToPers( const Muon::RpcByteStreamErrorContainer *transObj, Muon::RpcByteStreamErrorContainer_p1 *persObj, MsgStream & log )
{
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "RpcByteStreamErrorContainerCnv_p1::transToPers" << endreq;
    using namespace std;
    persObj->m_errorVector.reserve( transObj->m_errorVector.size() );
    DataVector<std::pair<IdentifierHash, int> >::const_iterator it=transObj->m_errorVector.begin(), itEnd=transObj->m_errorVector.end();
    for (;it!=itEnd;++it) {
      persObj->m_errorVector.push_back( new pair<IdentifierHash, int>(make_pair( (*it)->first, (*it)->second)) );
    }
}


