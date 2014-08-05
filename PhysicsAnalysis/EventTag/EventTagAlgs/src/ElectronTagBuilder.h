/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_ELECTRONTAGBUILDER_H
#define EVENTTAGALGS_ELECTRONTAGBUILDER_H

/**
   Tag Builder for Electron
*/

#include "ElectronPhotonTagTools/ElectronTagTool.h"

class StoreGateSvc;

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class ElectronTagBuilder : public Algorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  ElectronTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~ElectronTagBuilder();
  
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

  //! Electron Tag Builder Tool
  ToolHandle<ElectronTagTool> m_electronTagTool;

};

#endif
