/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamErrors/CscByteStreamErrorContainer.h"

namespace Muon {
    

CscByteStreamErrorContainer::CscByteStreamErrorContainer( ) 
{}

CscByteStreamErrorContainer::CscByteStreamErrorContainer( const DataVector< std::pair < IdentifierHash, int > > & inputVector ) 
    :
    m_errorVector (inputVector) 
{}

CscByteStreamErrorContainer::~CscByteStreamErrorContainer( )
{} 

const DataVector<std::pair<IdentifierHash, int> >& 
CscByteStreamErrorContainer::getAllErrors() const
{
  return m_errorVector;
}

void CscByteStreamErrorContainer::addError(IdentifierHash idh, int type)
{

   std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair(idh,type));
   m_errorVector.push_back(err);

}


}
