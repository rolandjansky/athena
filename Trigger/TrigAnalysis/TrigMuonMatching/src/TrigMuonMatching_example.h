/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGMUONEFFICIENCY_EXAMPLE_H_
#define TRIGMUONEFFICIENCY_EXAMPLE_H_

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigMuonMatching/ITrigMuonMatching.h"

namespace Trig {

   class TrigMuonMatching_example : public AthAlgorithm {

   public:
    TrigMuonMatching_example( const std::string& name, ISvcLocator* svcLoc );
    
    virtual StatusCode initialize();

    virtual StatusCode execute();

   private:
    
    ToolHandle<ITrigMuonMatching> m_matchTool;
     
   }; // class TrigMuonMatching_example

} // namespace Trig

#endif 
