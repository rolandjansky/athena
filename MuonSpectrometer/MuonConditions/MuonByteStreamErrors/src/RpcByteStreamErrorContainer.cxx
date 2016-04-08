/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamErrors/RpcByteStreamErrorContainer.h"

namespace Muon {
    

RpcByteStreamErrorContainer::RpcByteStreamErrorContainer( ) 
{}

RpcByteStreamErrorContainer::RpcByteStreamErrorContainer( const DataVector< std::pair < IdentifierHash, int > > & inputVector ) 
    :
    m_errorVector (inputVector) 
{}

RpcByteStreamErrorContainer::~RpcByteStreamErrorContainer( )
{} 

const DataVector<std::pair<IdentifierHash, int> >& 
RpcByteStreamErrorContainer::getAllErrors() const
{
  return m_errorVector;
}

void RpcByteStreamErrorContainer::addError(IdentifierHash idh, int type)
{

   std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair(idh,type));
   m_errorVector.push_back(err);

}
void RpcByteStreamErrorContainer::clear()
{
    if (m_errorVector.size()!=0) m_errorVector.clear();
}
int RpcByteStreamErrorContainer::getError(IdentifierHash idh) const
{
  int errorCode=-1;
  if ( m_errorVector.size()==0) return errorCode;
  rpcErrorIterator myIt  = m_errorVector.begin();
  rpcErrorIterator myItE = m_errorVector.end();
  for (;myIt!=myItE;++myIt)
    {
      IdentifierHash myPadId = (*myIt)->first;
      if (myPadId==idh) {
        errorCode= (*myIt)->second;
        break;
      }
    }  
  
  return errorCode; 
}

std::vector<IdentifierHash> RpcByteStreamErrorContainer::getPadsWithError() const
{
  rpcErrorIterator myIt = m_errorVector.begin();
  rpcErrorIterator myItE = m_errorVector.end();
  std::vector<IdentifierHash> myList;
  for (;myIt!=myItE;++myIt)
    {
      IdentifierHash myPadId = (*myIt)->first;
      myList.push_back(myPadId);
    }
  return myList;
}


}
