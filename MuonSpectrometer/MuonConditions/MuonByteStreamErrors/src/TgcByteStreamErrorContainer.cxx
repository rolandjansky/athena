/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamErrors/TgcByteStreamErrorContainer.h"

namespace Muon {
    

TgcByteStreamErrorContainer::TgcByteStreamErrorContainer( ) 
{}

TgcByteStreamErrorContainer::TgcByteStreamErrorContainer( const DataVector< std::pair < IdentifierHash, int > > & inputVector ) 
    :
    m_errorVector (inputVector) 
{}

TgcByteStreamErrorContainer::~TgcByteStreamErrorContainer( )
{} 

const DataVector<std::pair<IdentifierHash, int> >& 
TgcByteStreamErrorContainer::getAllErrors() const
{
  return m_errorVector;
}

void TgcByteStreamErrorContainer::addError(IdentifierHash idh, int type)
{

   std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair(idh,type));
   m_errorVector.push_back(err);

}


}
