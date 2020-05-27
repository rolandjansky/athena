/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOL_H
#define MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOL_H

#include "RpcRdoToPrepDataToolCore.h"

namespace Muon {

class RpcRdoToPrepDataTool : virtual public RpcRdoToPrepDataToolCore {
public:
  RpcRdoToPrepDataTool( const std::string&, const std::string&, const IInterface* );
  virtual ~RpcRdoToPrepDataTool()=default;
  virtual StatusCode initialize() override;

protected:
  virtual StatusCode manageOutputContainers(bool& firstTimeInTheEvent) override;
};

}

#endif // !ATHEXJOBOPTIONS_CONCRETETOOL_H
