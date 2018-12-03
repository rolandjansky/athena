/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_ANALYSISTAGBUILDER_H
#define EVENTTAGALGS_ANALYSISTAGBUILDER_H

/**
   Tag Builder for Analysis specific information such from physics groups
*/

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetMissingEtTagTools/JetMissingEtIdentificationTagTool.h"

class StoreGateSvc;

class AnalysisTagBuilder : public AthAlgorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  AnalysisTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~AnalysisTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  //! Attribute list name
  std::string m_attributeListName;

  //! Analysis Specific tools
  ToolHandle<JetMissingEtIdentificationTagTool> m_jetMissingEtTagTool;

  //! number analysis words
  int m_nJetMissingEt;

};

#endif
