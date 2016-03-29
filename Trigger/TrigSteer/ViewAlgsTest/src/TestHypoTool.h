// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestHypoTool_h
#define ViewAlgs_TestHypoTool_h

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "./TestEDM.h"


static const InterfaceID IID_TestHypoTool("TestHypoTool", 1, 0);

class TestHypoTool : public AthAlgTool {
public:
  TestHypoTool(const std::string& type, const std::string& name,  const IInterface* parent);

  static const InterfaceID& interfaceID() {     
    return IID_TestHypoTool; 
  }

  StatusCode initialize();

  bool decision(const TestCluster *) const;

private:
  float m_etThreshold;
};


#endif
