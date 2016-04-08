///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PDMAKERREADER_D3PDMAKERREADERDICT_H
#define D3PDMAKERREADER_D3PDMAKERREADERDICT_H 1

#include <vector>
#include <set>

#include "D3PDMakerUtils/ObjectMetadata.h"
#include "src/RootObjectMetadata.h"

namespace D3PDMakerReaderDict {
  struct tmp {
    std::set<D3PD::ObjectMetadata::Variable> m_set;
    std::vector<D3PD::ObjectMetadata::Variable> m_vector;
  };

  std::vector<D3PD::ObjectMetadata::Variable> 
    to_vector(const std::set<D3PD::ObjectMetadata::Variable>& v)
    { return std::vector<D3PD::ObjectMetadata::Variable>(v.begin(), v.end()); }

}
#endif // not  D3PDMAKERREADER_D3PDMAKERREADERDICT_H
