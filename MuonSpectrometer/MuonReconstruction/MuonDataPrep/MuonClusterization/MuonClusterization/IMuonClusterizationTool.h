/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONCLUSTERIZATIONTOOL_H
#define IMUONCLUSTERIZATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

static const InterfaceID IID_IMuonClusterizationTool ("Muon::IMuonClusterizationTool",1,0);

namespace Muon {


  class IMuonClusterizationTool : virtual public IAlgTool
  {
  public:
    virtual Muon::TgcPrepDataContainer*  cluster( const Muon::TgcPrepDataContainer& prdContainer ) = 0;
    virtual Muon::TgcPrepDataCollection* cluster( const Muon::TgcPrepDataCollection& col ) = 0;

    virtual Muon::RpcPrepDataContainer*  cluster( const Muon::RpcPrepDataContainer& prdContainer ) = 0;
    virtual Muon::RpcPrepDataCollection* cluster( const Muon::RpcPrepDataCollection& col ) = 0;

    /** access to tool interface */
    static const InterfaceID& interfaceID() { return IID_IMuonClusterizationTool; }

  };
}

#endif 
