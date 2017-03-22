/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOL_ITGCPREPDATAREPLICATIONTOOL_H
#define MUONTGC_CNVTOOL_ITGCPREPDATAREPLICATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITgcPrepDataReplicationTool("Muon::ITgcPrepDataReplicationTool", 1, 0);

namespace Muon {

  class ITgcPrepDataReplicationTool : virtual public IAlgTool  {

  public: 
    /** Provide interface ID of ITgcPrepDataReplicationTool */
    static const InterfaceID& interfaceID() { return IID_ITgcPrepDataReplicationTool; };

    virtual StatusCode replicate()=0;
  }; 

} // end of namespace

#endif // MUONTGC_CNVTOOL_ITGCPREPDATAREPLICATIONTOOL_H 
