/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMERRORS_MUONBYTESTREAMERRORCONTAINERDICT_H
#define MUONBYTESTREAMERRORS_MUONBYTESTREAMERRORCONTAINERDICT_H 
// containers
#include "MuonByteStreamErrors/RpcByteStreamErrorContainer.h"
#include "MuonByteStreamErrors/MdtByteStreamErrorContainer.h"
#include "MuonByteStreamErrors/CscByteStreamErrorContainer.h"
#include "MuonByteStreamErrors/TgcByteStreamErrorContainer.h"

namespace MuonByteStreamErrorsDict 
{
  struct tmp 
  {
    std::pair <IdentifierHash, int> m_mypair;
    std::vector <std::pair<IdentifierHash, int>* > m_myvector_ppair;
    DataVector <std::pair <IdentifierHash, int> > m_mydatavector;
  };
  
    
  
}


#endif // MUONBYTESTREAMERRORS_MUONBYTESTREAMERRORCONTAINERDICT_H
