/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_MISSINGETTAGBUILDER_H
#define EVENTTAGALGS_MISSINGETTAGBUILDER_H

/**
   Tag Builder for MissingET
*/

#include "JetMissingEtTagTools/JetMissingEtTagTool.h"

class StoreGateSvc;

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class MissingETTagBuilder : public Algorithm, public TagBuilderBase
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

  //! StoreGate service.
  StoreGateSvc* m_storeGateSvc;

  //! Jet and Missing Tag Tool
  ToolHandle<JetMissingEtTagTool> m_jetMissingEtTagTool;

};

#endif
