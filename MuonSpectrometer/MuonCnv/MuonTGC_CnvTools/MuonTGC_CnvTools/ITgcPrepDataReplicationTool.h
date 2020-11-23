/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOL_ITGCPREPDATAREPLICATIONTOOL_H
#define MUONTGC_CNVTOOL_ITGCPREPDATAREPLICATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "CxxUtils/checker_macros.h"

namespace Muon {

  class ATLAS_NOT_THREAD_SAFE ITgcPrepDataReplicationTool : virtual public IAlgTool  {

  public:
    DeclareInterfaceID( ITgcPrepDataReplicationTool, 1, 0 );

    virtual StatusCode replicate() const = 0;
  }; 

} // end of namespace

#endif // MUONTGC_CNVTOOL_ITGCPREPDATAREPLICATIONTOOL_H 
