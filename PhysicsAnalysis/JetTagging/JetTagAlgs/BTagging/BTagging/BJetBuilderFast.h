/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BJETBUILDERFAST_H
#define BTAGGING_BJETBUILDERFAST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Analysis { 
  
  class ITagTool; 

  class BJetBuilderFast : public AthAlgorithm {

    public:

    BJetBuilderFast(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~BJetBuilderFast();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

   private:

    std::string m_JetCollectionName;
    std::string m_collectionAppendix ;

    ToolHandleArray< Analysis::ITagTool > m_bTagToolHandleArray;

  };
 }
#endif
