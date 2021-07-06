/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUCALSTREAMERTOOL_H
#define  TRIGL2MUONSA_MUCALSTREAMERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
 
namespace TrigL2MuonSA {
  
  class MuCalStreamerTool: public AthAlgTool
  {
  public:
    
    MuCalStreamerTool(const std::string& type,
		      const std::string& name,
		      const IInterface*  parent);

  };
  
} // namespace TrigL2MuonSA

#endif  // 
