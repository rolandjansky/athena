/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_MISSINGETTAGBUILDER_H
#define EVENTTAGALGS_MISSINGETTAGBUILDER_H

/**
   Tag Builder for MissingET
*/

#include "JetMissingEtTagTools/JetMissingEtTagTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class MissingETTagBuilder : public AthAlgorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  MissingETTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~MissingETTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  //! Attribute list name
  std::string m_attributeListName;

  //! Jet and Missing Tag Tool
  ToolHandle<JetMetTagTool> m_jetMissingEtTagTool;
};

#endif
