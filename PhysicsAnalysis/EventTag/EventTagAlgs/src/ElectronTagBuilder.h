/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_ELECTRONTAGBUILDER_H
#define EVENTTAGALGS_ELECTRONTAGBUILDER_H

/**
   Tag Builder for Electron
*/

#include "ElectronPhotonTagTools/ElectronTagTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class ElectronTagBuilder : public AthAlgorithm, public TagBuilderBase
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

  //! Electron Tag Builder Tool
  ToolHandle<ElectronTagTool> m_electronTagTool;
};

#endif
