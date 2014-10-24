/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_PARTICLEJETTAGBUILDER_H
#define EVENTTAGALGS_PARTICLEJETTAGBUILDER_H

/**
   Tag Builder for ParticleJet
*/

#include "JetMissingEtTagTools/JetMissingEtTagTool.h"

class StoreGateSvc;

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class ParticleJetTagBuilder : public Algorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  ParticleJetTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~ParticleJetTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  //! Attribute list name
  std::string m_attributeListName;

  //! number of particles
  int m_MAX_NUMBER;

  //! StoreGate service.
  StoreGateSvc* m_storeGateSvc;

  //! Jet and Missing Tag Tool
  ToolHandle<JetMetTagTool> m_jetMissingEtTagTool;

};

#endif
