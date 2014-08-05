/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_MUONTAGBUILDER_H
#define EVENTTAGALGS_MUONTAGBUILDER_H

/**
   Tag Builder for Muon
*/

#include "MuonTagTools/MuonTagTool.h"

class StoreGateSvc;

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class MuonTagBuilder : public Algorithm, public TagBuilderBase
{
public:

  //! Standard constructor.
  MuonTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  //! Destructor.
  ~MuonTagBuilder();
  
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

  //!  Muon Tag Tool 
  ToolHandle<MuonTagTool> m_muonTagTool;

};

#endif
