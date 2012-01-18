/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDITIONSATHENAPOOL_RPCBYTESTREAMERRORCONTAINER_P1_H
#define MUONCONDITIONSATHENAPOOL_RPCBYTESTREAMERRORCONTAINER_P1_H
#include <vector>
#include <stdint.h>
#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"

namespace Muon
{
    class RpcByteStreamErrorContainer_p1
    {
    public:
        RpcByteStreamErrorContainer_p1() {}
	DataVector<std::pair<IdentifierHash, int> >m_errorVector;
    };
}

#endif
