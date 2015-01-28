/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUGIRLLOWBETATAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUGIRLLOWBETATAGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"

#include "MuGirlInterfaces/IMuGirlRecoTool.h"


namespace MuonCombined {

  class MuGirlLowBetaTagTool: public AthAlgTool, virtual public IMuonCombinedInDetExtensionTool
  {

  public:
    MuGirlLowBetaTagTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuGirlLowBetaTagTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

  };

}	// end of namespace

#endif


