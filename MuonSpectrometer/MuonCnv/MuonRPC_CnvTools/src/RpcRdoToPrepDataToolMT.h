/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOLMT_H
#define MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOLMT_H

#include "RpcRdoToPrepDataToolCore.h"

namespace Muon {

class RpcRdoToPrepDataToolMT : virtual public RpcRdoToPrepDataToolCore {
public:
  RpcRdoToPrepDataToolMT( const std::string&, const std::string&, const IInterface* );
  virtual ~RpcRdoToPrepDataToolMT();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

protected:
  virtual StatusCode manageOutputContainers(bool& firstTimeInTheEvent) override;
};

}

#endif // !ATHEXJOBOPTIONS_CONCRETETOOL_H
