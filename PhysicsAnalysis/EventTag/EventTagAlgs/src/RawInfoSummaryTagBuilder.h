/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_DATAQUALITYTAGBUILDER_H
#define EVENTTAGALGS_DATAQUALITYTAGBUILDER_H

/**
   Tag Builder for Detector status information
*/

#include "EventTagUtils/RawInfoSummaryForTagTool.h"

class StoreGateSvc;

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class RawInfoSummaryTagBuilder : public AthAlgorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  RawInfoSummaryTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~RawInfoSummaryTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  //! Attribute list name
  std::string m_attributeListName;

  //! Detetor Status Tools
  ToolHandle<RawInfoSummaryForTagTool> m_tagTool;

};

#endif
