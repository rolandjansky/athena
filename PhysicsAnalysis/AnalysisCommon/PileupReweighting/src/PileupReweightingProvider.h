/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPREWEIGHTINGPROVIDER
#define PILEUPREWEIGHTINGPROVIDER

#include "PileupReweighting/IPileupReweightingTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace CP {

class PileupReweightingProvider : public AthAlgorithm {

     public:
      PileupReweightingProvider( const std::string& name, ISvcLocator* svcloc) : AthAlgorithm(name,svcloc),m_tool("PileupReweightingTool") {
         declareProperty("Tool",m_tool,"The configured PileupReweightingTool");
      }

      ~PileupReweightingProvider() { }

      virtual StatusCode initialize() { CHECK( m_tool.retrieve() ); return StatusCode::SUCCESS; }
      virtual StatusCode execute() { CHECK( m_tool->execute() ); return StatusCode::SUCCESS; }

   private:
      ToolHandle<IPileupReweightingTool> m_tool;


}; 

} //end of CP namespace

#endif
