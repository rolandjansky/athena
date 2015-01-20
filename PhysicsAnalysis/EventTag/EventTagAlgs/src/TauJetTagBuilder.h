/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_TAUJETTAGBUILDER_H
#define EVENTTAGALGS_TAUJETTAGBUILDER_H

/**
   Tag Builder for TauJet
*/

#include "TauTagTools/TauJetTagTool.h"

class StoreGateSvc;

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class TauJetTagBuilder : public AthAlgorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  TauJetTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~TauJetTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  //! Attribute list name
  std::string m_attributeListName;

  //! number of particles
  int m_MAX_NUMBER;

  //! TauJet Tag Tool
  ToolHandle<TauJetTagTool> m_tauJetTagTool;
};

#endif
