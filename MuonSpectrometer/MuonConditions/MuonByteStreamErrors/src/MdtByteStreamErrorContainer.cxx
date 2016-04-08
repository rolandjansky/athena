/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamErrors/MdtByteStreamErrorContainer.h"

namespace Muon {
    

MdtByteStreamErrorContainer::MdtByteStreamErrorContainer( ) 
{}

MdtByteStreamErrorContainer::MdtByteStreamErrorContainer( const DataVector< std::pair < IdentifierHash, int > > & inputVector ) 
    :
    m_errorVector (inputVector) 
{}

MdtByteStreamErrorContainer::~MdtByteStreamErrorContainer( )
{} 

const DataVector<std::pair<IdentifierHash, int> >& 
MdtByteStreamErrorContainer::getAllErrors() const
{
  return m_errorVector;
}

void MdtByteStreamErrorContainer::addError(IdentifierHash idh, int type)
{

   std::pair<IdentifierHash, int>* err = new std::pair<IdentifierHash, int>(std::make_pair(idh,type));
   m_errorVector.push_back(err);

}


}
